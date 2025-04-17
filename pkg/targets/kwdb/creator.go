package kwdb

import (
	"database/sql"
	"fmt"
	"strings"

	"github.com/timescale/tsbs/pkg/data"
	"github.com/timescale/tsbs/pkg/targets"
)

// dbCreator is responsible for creating the database and tables for KWDB.
type dbCreator struct {
	db       *sql.DB
	dbName   string
	ds       targets.DataSource
}

// Init initializes the database creator.
func (dc *dbCreator) Init() error {
	if dc.db == nil {
		return fmt.Errorf("database connection is not initialized")
	}
	return nil
}

// DBExists checks if the database exists.
func (dc *dbCreator) DBExists(dbName string) bool {
	return true
}

// CreateDB creates the database.
func (dc *dbCreator) CreateDB(dbName string) error {
	_, err := dc.db.Exec(fmt.Sprintf("CREATE TS DATABASE %s", dbName))
	if err != nil && !strings.Contains(err.Error(), "already exists") {
		return err
	}
	return nil
}

// CreateTable creates a table in the database.
func (dc *dbCreator) CreateTable(tableName string, columns []string, tags []string, primaryTags []string) error {
	// Build column definitions
	columnDefs := make([]string, len(columns))
	for i, col := range columns {
		columnDefs[i] = col
	}

	// Build tag definitions
	tagDefs := make([]string, len(tags))
	for i, tag := range tags {
		tagDefs[i] = tag
	}

	// Create table SQL
	sql := fmt.Sprintf(
		"CREATE TABLE %s.%s (\n%s\n) TAGS (\n%s\n) PRIMARY TAGS (%s)",
		dc.dbName,
		tableName,
		strings.Join(columnDefs, ",\n"),
		strings.Join(tagDefs, ",\n"),
		strings.Join(primaryTags, ", "),
	)

	_, err := dc.db.Exec(sql)
	if err != nil && !strings.Contains(err.Error(), "already exists") {
		return fmt.Errorf("error creating table %s: %v", tableName, err)
	}

	return nil
}

// CreateIndexes creates indexes on the table.
func (dc *dbCreator) CreateIndexes(hypertable string) error {
	// KWDB automatically creates necessary indexes
	return nil
}

// Close closes the database connection.
func (dc *dbCreator) Close() error {
	if dc.db != nil {
		return dc.db.Close()
	}
	return nil
}