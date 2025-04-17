// tsbs_load_kwdb loads a KWDB instance with data from stdin.
//
// If the database exists beforehand, it will be *DROPPED*.
package main

import (
	"fmt"
	"sync"

	"github.com/blagojts/viper"
	"github.com/spf13/pflag"
	"github.com/timescale/tsbs/internal/utils"
	"github.com/timescale/tsbs/load"
	"github.com/timescale/tsbs/pkg/data/source"
	"github.com/timescale/tsbs/pkg/targets/kwdb"
	"github.com/jackc/pgx/v4/stdlib"
)

// KWDBLoadingOptions contains specific options for loading data into KWDB
type KWDBLoadingOptions struct {
	Host     string
	Port     string
	User     string
	Pass     string
	ConnDB   string
	PostgresConnect string
	LogBatches bool
}

// Parse args:
func initProgramOptions() (*KWDBLoadingOptions, load.BenchmarkRunner, *load.BenchmarkRunnerConfig) {
	loaderConf := load.BenchmarkRunnerConfig{}
	loaderConf.AddToFlagSet(pflag.CommandLine)

	// Add KWDB specific flags
	pflag.String("postgres", "host=localhost user=postgres sslmode=disable", "String of additional PostgreSQL connection parameters")
	pflag.String("host", "localhost", "Host to connect to")
	pflag.String("port", "5432", "Port to connect to")
	pflag.String("user", "postgres", "User to connect as")
	pflag.String("pass", "", "Password for user")
	pflag.String("admin-db-name", "postgres", "Database to connect to initially")
	pflag.Bool("log-batches", false, "Whether to time individual batches.")

	pflag.Parse()

	err := utils.SetupConfigFile()

	if err != nil {
		panic(fmt.Errorf("fatal error config file: %s", err))
	}

	if err := viper.Unmarshal(&loaderConf); err != nil {
		panic(fmt.Errorf("unable to decode config: %s", err))
	}

	opts := &KWDBLoadingOptions{}
	viper.SetTypeByDefaultValue(true)
	opts.PostgresConnect = viper.GetString("postgres")
	opts.Host = viper.GetString("host")
	opts.Port = viper.GetString("port")
	opts.User = viper.GetString("user")
	opts.Pass = viper.GetString("pass")
	opts.ConnDB = viper.GetString("admin-db-name")
	opts.LogBatches = viper.GetBool("log-batches")

	loader := load.GetBenchmarkRunner(loaderConf)
	return opts, loader, &loaderConf
}

func main() {
	opts, loader, loaderConf := initProgramOptions()

	// Initialize KWDB benchmark
	benchmark, err := kwdb.NewBenchmark(loaderConf.DBName, &kwdb.LoadingOptions{
		Host:            opts.Host,
		Port:            opts.Port,
		User:            opts.User,
		Pass:            opts.Pass,
		ConnDB:          opts.ConnDB,
		PostgresConnect: opts.PostgresConnect,
		LogBatches:      opts.LogBatches,
	}, &source.DataSourceConfig{
		Type: source.FileDataSourceType,
		File: &source.FileDataSourceConfig{Location: loaderConf.FileName},
	})

	if err != nil {
		panic(err)
	}

	// Run the benchmark
	loader.RunBenchmark(benchmark)
}