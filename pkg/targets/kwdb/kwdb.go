// Package kwdb implements the KWDB target for TSBS
package kwdb

import (
	"database/sql"
	"fmt"
	"strings"
	"time"

	"github.com/timescale/tsbs/pkg/data"
	"github.com/timescale/tsbs/pkg/data/source"
	"github.com/timescale/tsbs/pkg/targets"
)

// NewTarget creates a new KWDB target.
func NewTarget() targets.ImplementedTarget {
	return &Target{}
}

// Target is a KWDB implementation of the targets.ImplementedTarget interface
type Target struct {
}

// Benchmark is a KWDB implementation of the targets.Benchmark interface
type Benchmark struct {
	db        *sql.DB
	options   *LoadingOptions
	dbName    string
	tableName string
}

// LoadingOptions contains the options for loading data into KWDB
type LoadingOptions struct {
	Host            string
	Port            string
	User            string
	Pass            string
	ConnDB          string
	PostgresConnect string
	LogBatches      bool
}

// GetConnectString returns the connection string for KWDB
func (lo *LoadingOptions) GetConnectString(dbName string) string {
	connectString := lo.PostgresConnect
	if !strings.Contains(connectString, "host=") && lo.Host != "" {
		connectString = fmt.Sprintf("%s host=%s", connectString, lo.Host)
	}
	if !strings.Contains(connectString, "port=") && lo.Port != "" {
		connectString = fmt.Sprintf("%s port=%s", connectString, lo.Port)
	}
	if !strings.Contains(connectString, "user=") && lo.User != "" {
		connectString = fmt.Sprintf("%s user=%s", connectString, lo.User)
	}
	if !strings.Contains(connectString, "password=") && lo.Pass != "" {
		connectString = fmt.Sprintf("%s password=%s", connectString, lo.Pass)
	}
	if !strings.Contains(connectString, "dbname=") {
		connectString = fmt.Sprintf("%s dbname=%s", connectString, dbName)
	}
	return connectString
}

// NewBenchmark creates a new KWDB Benchmark
func NewBenchmark(dbName string, opts *LoadingOptions, dataSourceConfig *source.DataSourceConfig) (targets.Benchmark, error) {
	connectString := opts.GetConnectString(opts.ConnDB)
	db, err := sql.Open("pgx", connectString)
	if err != nil {
		return nil, err
	}

	// Create the database if it does not exist
	_, err = db.Exec(fmt.Sprintf("CREATE TS DATABASE %s", dbName))
	if err != nil {
		// Ignore error if database already exists
		if !strings.Contains(err.Error(), "already exists") {
			return nil, err
		}
	}

	// Connect to the benchmark database
	db.Close()
	connectString = opts.GetConnectString(dbName)
	db, err = sql.Open("pgx", connectString)
	if err != nil {
		return nil, err
	}

	return &Benchmark{
		db:      db,
		options: opts,
		dbName:  dbName,
	}, nil
}

// GetDataSource returns the data source for the benchmark
func (b *Benchmark) GetDataSource() targets.DataSource {
	return nil
}

// GetBatchFactory returns the batch factory for the benchmark
func (b *Benchmark) GetBatchFactory() targets.BatchFactory {
	return nil
}

// GetPointIndexer returns the point indexer for the benchmark
func (b *Benchmark) GetPointIndexer(maxPartitions uint) targets.PointIndexer {
	return &PointIndexer{}
}

// GetProcessor returns the processor for the benchmark
func (b *Benchmark) GetProcessor() targets.Processor {
	return nil
}

// GetDBName returns the name of the database
func (b *Benchmark) GetDBName() string {
	return b.dbName
}

// PointIndexer is used to index points in KWDB
type PointIndexer struct {
}

// GetIndex returns the index for a point
func (p *PointIndexer) GetIndex(point *data.Point) uint {
	return 0
}

// GetPartitionCount returns the number of partitions
func (p *PointIndexer) GetPartitionCount() uint {
	return 1
}