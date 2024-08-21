package main

import (
	"fmt"

	kaiwudb "github.com/benchant/tsbs/pkg/targets/kaiwudb"

	"github.com/benchant/tsbs/internal/utils"
	"github.com/benchant/tsbs/load"
	"github.com/benchant/tsbs/pkg/data/source"
	"github.com/blagojts/viper"
	"github.com/spf13/pflag"
)

func initProgramOptions() (*kaiwudb.LoadingOptions, load.BenchmarkRunner, *load.BenchmarkRunnerConfig) {
	target := kaiwudb.NewTarget()
	loaderConf := load.BenchmarkRunnerConfig{}
	loaderConf.AddToFlagSet(pflag.CommandLine)
	target.TargetSpecificFlags("", pflag.CommandLine)
	pflag.Parse()
	err := utils.SetupConfigFile()

	if err != nil {
		panic(fmt.Errorf("fatal error config file: %s", err))
	}

	if err := viper.Unmarshal(&loaderConf); err != nil {
		panic(fmt.Errorf("unable to decode config: %s", err))
	}
	opts := kaiwudb.LoadingOptions{}
	viper.SetTypeByDefaultValue(true)
	opts.User = viper.GetString("user")
	opts.Pass = viper.GetString("pass")
	opts.Host = viper.GetStringSlice("mehost")
	opts.Port = viper.GetIntSlice("meport")
	opts.DBName = viper.GetString("db-name")
	opts.Case = viper.GetString("case")
	opts.Workers = viper.GetInt("workers")
	opts.DoCreate = viper.GetBool("do-create-db")
	opts.Preparesize = viper.GetInt("preparesize")
	//todo: kaiwudb连接可配参数
	loaderConf.HashWorkers = true
	loaderConf.NoFlowControl = true
	loaderConf.ChannelCapacity = 50
	loaderConf.DBName = opts.DBName
	loader := load.GetBenchmarkRunner(loaderConf)
	return &opts, loader, &loaderConf
}
func main() {
	opts, loader, loaderConf := initProgramOptions()
	benchmark, err := kaiwudb.NewBenchmark(loaderConf.DBName, opts, &source.DataSourceConfig{
		Type: source.FileDataSourceType,
		File: &source.FileDataSourceConfig{Location: loaderConf.FileName},
	})
	if err != nil {
		panic(err)
	}
	loader.RunBenchmark(benchmark)
}
