// Package kwdb implements the query generation for KWDB.
package kwdb

import (
	"fmt"
	"strings"
	"time"

	"github.com/timescale/tsbs/cmd/tsbs_generate_queries/uses/devops"
	"github.com/timescale/tsbs/pkg/query"
)

// BaseGenerator defines the common functionality for KWDB query generators.
type BaseGenerator struct{}

// GenerateEmptyQuery returns an empty query.Query for reuse.
func (g *BaseGenerator) GenerateEmptyQuery() query.Query {
	return query.NewHTTP()
}

// fillInQuery fills the query struct with basic information.
func (g *BaseGenerator) fillInQuery(qi query.Query, humanLabel, humanDesc, sql string) {
	q := qi.(*query.HTTP)
	q.HumanLabel = []byte(humanLabel)
	q.HumanDescription = []byte(humanDesc)
	q.Method = []byte("GET") // Assuming GET for KWDB, adjust if needed
	q.Path = []byte(fmt.Sprintf("/query?db=benchmark&q=%s", sql)) // Example path, adjust based on KWDB API
	q.Body = nil
}

// NewDevops creates a new devops use case query generator.
func NewDevops(start, end time.Time, scale int) (query.QueryGenerator, error) {
	core, err := devops.NewCore(start, end, scale)
	if err != nil {
		return nil, err
	}

	generator := &Devops{ // Assuming a Devops struct specific to KWDB
		BaseGenerator: BaseGenerator{},
		Core:          core,
	}

	return generator, nil
}

// Devops handles generating queries for the devops use case.
type Devops struct {
	BaseGenerator
	Core devops.Core
}

// GroupByOrderByLimit implements the eponymous query generator.
func (d *Devops) GroupByOrderByLimit(q query.Query) {
	humanLabel := "KWDB simple agg query"
	humanDesc := "KWDB simple agg query: Find the 5 minutes with the highest avg CPU usage across all hosts."
	// KWDB specific SQL for GroupByOrderByLimit
	sql := "SELECT time_bucket('1 minute', time) AS minute, avg(usage_user) as mean_usage_user FROM cpu GROUP BY minute ORDER BY mean_usage_user DESC, minute DESC LIMIT 5"
	d.fillInQuery(q, humanLabel, humanDesc, sql)
}

// MaxAllCPU implements the eponymous query generator.
func (d *Devops) MaxAllCPU(q query.Query, nHosts int, duration time.Duration) {
	hostnames, err := d.Core.GetRandomHosts(nHosts)
	if err != nil {
		panic(fmt.Sprintf("error getting random hosts: %v", err))
	}
	humanLabel := devops.GetMaxAllLabel("KWDB", nHosts)
	humanDesc := fmt.Sprintf("%s: %s", humanLabel, devops.GetMaxAllLabelDescription(duration))

	metrics := devops.GetAllCPUMetrics()
	selectClauses := make([]string, len(metrics))
	for i, m := range metrics {
		selectClauses[i] = fmt.Sprintf("max(%s)", m)
	}

	// KWDB specific SQL for MaxAllCPU
	sql := fmt.Sprintf("SELECT time_bucket('1 hour', time) AS hour, %s FROM cpu WHERE hostname IN ('%s') AND time >= '%s' AND time < '%s' GROUP BY hour ORDER BY hour",
		strings.Join(selectClauses, ", "),
		strings.Join(hostnames, "', '"),
		d.Core.TimeStart.Format(time.RFC3339),
		d.Core.TimeStart.Add(duration).Format(time.RFC3339))
	d.fillInQuery(q, humanLabel, humanDesc, sql)
}

// GroupByTimeAndPrimaryTag implements the eponymous query generator.
func (d *Devops) GroupByTimeAndPrimaryTag(q query.Query, numMetrics int) {
	metrics, err := devops.GetCPUMetricsSlice(numMetrics)
	if err != nil {
		panic(err)
	}
	humanLabel := devops.GetDoubleGroupByLabel("KWDB", numMetrics)
	humanDesc := fmt.Sprintf("%s: %s", humanLabel, devops.GetDoubleGroupByDescription(devops.DoubleGroupByDuration))

	selectClauses := make([]string, len(metrics))
	for i, m := range metrics {
		selectClauses[i] = fmt.Sprintf("avg(%s)", m)
	}

	// KWDB specific SQL for GroupByTimeAndPrimaryTag
	sql := fmt.Sprintf("SELECT time_bucket('1 hour', time) AS hour, hostname, %s FROM cpu WHERE time >= '%s' AND time < '%s' GROUP BY hour, hostname ORDER BY hour, hostname",
		strings.Join(selectClauses, ", "),
		d.Core.TimeStart.Format(time.RFC3339),
		d.Core.TimeStart.Add(devops.DoubleGroupByDuration).Format(time.RFC3339))
	d.fillInQuery(q, humanLabel, humanDesc, sql)
}

// HighCPUForHosts implements the eponymous query generator.
func (d *Devops) HighCPUForHosts(q query.Query, nHosts int) {
	var hostClause string
	var err error
	if nHosts == 0 {
		// No specific host filter needed for all hosts
		hostClause = ""
	} else {
		var hostnames []string
		hostnames, err = d.Core.GetRandomHosts(nHosts)
		if err != nil {
			panic(fmt.Sprintf("error getting random hosts: %v", err))
		}
		hostClause = fmt.Sprintf("AND hostname IN ('%s')", strings.Join(hostnames, "', '"))
	}
	humanLabel, err := devops.GetHighCPULabel("KWDB", nHosts)
	if err != nil {
		panic(fmt.Sprintf("error getting label: %v", err))
	}
	humanDesc := fmt.Sprintf("%s: %s", humanLabel, devops.GetHighCPUDescription(devops.HighCPUDuration))

	// KWDB specific SQL for HighCPUForHosts
	sql := fmt.Sprintf("SELECT time, hostname, usage_user FROM cpu WHERE usage_user > 90.0 %s AND time >= '%s' AND time < '%s' ORDER BY time, hostname",
		hostClause,
		d.Core.TimeStart.Format(time.RFC3339),
		d.Core.TimeStart.Add(devops.HighCPUDuration).Format(time.RFC3339))
	d.fillInQuery(q, humanLabel, humanDesc, sql)
}

// LastPointPerHost implements the eponymous query generator.
func (d *Devops) LastPointPerHost(q query.Query) {
	humanLabel := "KWDB last point per host"
	humanDesc := humanLabel + ": Get the latest reading for every host" // Adjusted description
	// KWDB specific SQL for LastPointPerHost
	// Assuming 'cpu' table has all metrics. If not, adjust '*' accordingly.
	sql := "SELECT DISTINCT ON (hostname) * FROM cpu ORDER BY hostname, time DESC"
	d.fillInQuery(q, humanLabel, humanDesc, sql)
}

// GroupByTime implements the eponymous query generator.
func (d *Devops) GroupByTime(q query.Query, nHosts, numMetrics int, duration time.Duration) {
	hostnames, err := d.Core.GetRandomHosts(nHosts)
	if err != nil {
		panic(fmt.Sprintf("error getting random hosts: %v", err))
	}
	metrics, err := devops.GetCPUMetricsSlice(numMetrics)
	if err != nil {
		panic(err)
	}
	humanLabel := fmt.Sprintf("KWDB mean of %d metrics over %d hosts", numMetrics, nHosts)
	humanDesc := fmt.Sprintf("%s: %s", humanLabel, devops.GetGroupByTimeDescription(duration))

	selectClauses := make([]string, len(metrics))
	for i, m := range metrics {
		selectClauses[i] = fmt.Sprintf("avg(%s)", m)
	}

	// KWDB specific SQL for GroupByTime
	sql := fmt.Sprintf("SELECT time_bucket('1 minute', time) AS minute, %s FROM cpu WHERE hostname IN ('%s') AND time >= '%s' AND time < '%s' GROUP BY minute ORDER BY minute DESC",
		strings.Join(selectClauses, ", "),
		strings.Join(hostnames, "', '"),
		d.Core.TimeStart.Format(time.RFC3339),
		d.Core.TimeStart.Add(duration).Format(time.RFC3339))
	d.fillInQuery(q, humanLabel, humanDesc, sql)
}

// Add other query generation methods here...