// tsbs_run_queries_kwdb speed tests KWDB using requests from stdin or file
//
// It reads encoded Query objects from stdin or file, and makes concurrent requests
// to the provided KWDB endpoint.
package main

import (
	"database/sql"
	"fmt"
	"strings"
	"sync"
	"time"

	"github.com/blagojts/viper"
	_ "github.com/jackc/pgx/v4/stdlib"
	"github.com/spf13/pflag"
	"github.com/timescale/tsbs/internal/utils"
	"github.com/timescale/tsbs/pkg/query"
)

const pgxDriver = "pgx" // default driver

// Program option vars:
var (
	postgresConnect string
	hostList        []string
	user            string
	pass            string
	port            string
)

// Global vars:
var (
	runner *query.BenchmarkRunner
)

// Parse args:
func init() {
	var config query.BenchmarkRunnerConfig
	config.AddToFlagSet(pflag.CommandLine)

	pflag.String("postgres", "host=localhost user=postgres sslmode=disable",
		"String of additional PostgreSQL connection parameters, e.g., 'sslmode=disable'.")
	pflag.String("hosts", "localhost", "Comma separated list of KWDB hosts")
	pflag.String("user", "postgres", "User to connect to KWDB as")
	pflag.String("pass", "", "Password for the user connecting to KWDB")
	pflag.String("port", "5432", "Which port to connect to on the database host")

	pflag.Parse()

	err := utils.SetupConfigFile()

	if err != nil {
		panic(fmt.Errorf("fatal error config file: %s", err))
	}

	if err := viper.Unmarshal(&config); err != nil {
		panic(fmt.Errorf("unable to decode config: %s", err))
	}

	postgresConnect = viper.GetString("postgres")
	hosts := viper.GetString("hosts")
	user = viper.GetString("user")
	pass = viper.GetString("pass")
	port = viper.GetString("port")

	runner = query.NewBenchmarkRunner(config)

	// Parse comma separated string of hosts and put in a slice (for multi-node setups)
	for _, host := range strings.Split(hosts, ",") {
		hostList = append(hostList, host)
	}
}

// KWDBPool is a connection pool for KWDB
type KWDBPool struct {
	connections []*sql.DB
}

// NewKWDBPool creates a new connection pool for KWDB
func NewKWDBPool() *KWDBPool {
	pool := &KWDBPool{}
	pool.connections = make([]*sql.DB, len(hostList))

	// Use the first host for all connections for simplicity in this example
	// A more robust implementation might distribute connections or handle failures
	connectString := fmt.Sprintf("%s host=%s user=%s port=%s password=%s dbname=%s",
		postgresConnect, hostList[0], user, port, pass, runner.DatabaseName())

	for i := 0; i < len(hostList); i++ {
		db, err := sql.Open(pgxDriver, connectString)
		if err != nil {
			panic(err)
		}
		// TODO: Consider setting connection pool parameters like max open connections
		pool.connections[i] = db
	}

	return pool
}

// GetConnection returns a connection from the pool
func (p *KWDBPool) GetConnection(queryNumber int) *sql.DB {
	return p.connections[queryNumber%len(p.connections)]
}

// Close closes all connections in the pool
func (p *KWDBPool) Close() {
	for _, conn := range p.connections {
		if conn != nil {
			conn.Close()
		}
	}
}

// processor implements the query.Processor interface for KWDB
type processor struct {
	pool *KWDBPool
}

// newProcessor creates a new processor for KWDB
func newProcessor() query.Processor {
	return &processor{pool: NewKWDBPool()}
}

// Init initializes the processor
func (p *processor) Init(workerNum int) {
	// Initialization logic if needed, like warming up connections
}

// ProcessQuery processes a query for KWDB
func (p *processor) ProcessQuery(q query.Query, isWarm bool) ([]*query.Stat, error) {
	// Cast to the generic Query Stringer interface to get the SQL query
	qry := q.String()
	conn := p.pool.GetConnection(int(q.GetID())) // Use query ID for connection selection

	start := time.Now()
	rows, err := conn.Query(qry)
	if err != nil {
		return nil, fmt.Errorf("error executing query '%s': %v", qry, err)
	}

	// Process results - this depends heavily on the expected output of KWDB queries
	// For simplicity, we'll just count the rows for now
	rowCount := 0
	for rows.Next() {
		// TODO: Implement actual result scanning based on query type if needed
		rowCount++
	}
	err = rows.Err()
	if err != nil {
		rows.Close()
		return nil, fmt.Errorf("error iterating results for query '%s': %v", qry, err)
	}
	rows.Close()

	taken := float64(time.Since(start).Nanoseconds()) / 1e6 // milliseconds
	stat := query.NewStat(string(q.HumanLabelName()), q.GetID())
	stat.Init(q.HumanDescriptionName(), taken, uint64(rowCount), isWarm)

	return []*query.Stat{stat}, nil
}

// Close closes the processor's resources
func (p *processor) Close() {
	p.pool.Close()
}

// Simple implementation of query.Query for the pool
// In a real scenario, this might live in pkg/query/kwdb.go
// and potentially hold more specific query information.
type kwdbQuery struct {
	id      uint64
	humanLabel []byte
	humanDesc  []byte
	sqlQuery   []byte
}

func (kq *kwdbQuery) Release()                      { /* Optional: pooling logic */ }
func (kq *kwdbQuery) HumanLabelName() []byte        { return kq.humanLabel }
func (kq *kwdbQuery) HumanDescriptionName() []byte { return kq.humanDesc }
func (kq *kwdbQuery) GetID() uint64                 { return kq.id }
func (kq *kwdbQuery) SetID(id uint64)               { kq.id = id }
func (kq *kwdbQuery) String() string                { return string(kq.sqlQuery) }

func main() {
	// Initialize and run the benchmark
	queryPool := sync.Pool{New: func() interface{} {
		return &kwdbQuery{}
	}}
	runner.Run(&queryPool, newProcessor)
}

// processorFactory implements query.ProcessorFactory - REMOVED as not needed by runner.Run
// type processorFactory struct{}
//
// func (pf *processorFactory) New() query.Processor {
// 	return newProcessor()
// }