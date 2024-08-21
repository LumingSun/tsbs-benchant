package main

import (
	"context"
	"fmt"
	"log"
	"time"

	"github.com/benchant/tsbs/internal/utils"
	"github.com/benchant/tsbs/pkg/query"
	"github.com/benchant/tsbs/pkg/targets/kaiwudb/commonpool"
	"github.com/blagojts/viper"
	"github.com/spf13/pflag"
)

var (
	user   string
	pass   string
	host   string
	port   int
	runner *query.BenchmarkRunner
)

func init() {
	var config query.BenchmarkRunnerConfig
	config.AddToFlagSet(pflag.CommandLine)

	pflag.String("user", "root", "User to connect to kaiwudb")
	pflag.String("pass", "", "Password for the user connecting to kaiwudb")
	pflag.String("host", "", "kaiwudb host")
	pflag.Int("port", 26257, "kaiwudb Port")
	pflag.Parse()
	err := utils.SetupConfigFile()

	if err != nil {
		panic(fmt.Errorf("fatal error config file: %s", err))
	}
	if err := viper.Unmarshal(&config); err != nil {
		panic(fmt.Errorf("unable to decode config: %s", err))
	}
	user = viper.GetString("user")
	pass = viper.GetString("pass")
	host = viper.GetString("host")
	port = viper.GetInt("port")
	runner = query.NewBenchmarkRunner(config)
}
func main() {
	runner.Run(&query.KaiwudbPool, newProcessor)
}

type queryExecutorOptions struct {
	debug         bool
	printResponse bool
}

type processor struct {
	db   *commonpool.Conn
	opts *queryExecutorOptions
}

func (p *processor) Init(workerNum int) {
	db, err := commonpool.GetConnection(user, pass, host, port)
	if err != nil {
		panic(err)
	}
	p.db = db
	p.opts = &queryExecutorOptions{
		debug:         runner.DebugLevel() > 0,
		printResponse: runner.DoPrintResponses(),
	}
	//ctx := context.Background()
	//_, err = p.db.Connection.Exec(ctx, "set cluster setting sql.complex_query.enabled=true;")
	//if err != nil {
	//	panic(err)
	//}
}

func (p *processor) ProcessQuery(q query.Query, _ bool) ([]*query.Stat, error) {
	tq := q.(*query.Kaiwudb)

	start := time.Now()
	qry := string(tq.SqlQuery)
	if p.opts.debug {
		fmt.Println(qry)
	}

	ctx := context.Background()
	rows, err := p.db.Connection.Query(ctx, qry)
	if err != nil {
		log.Println("Error running query: '", qry, "'")
		return nil, err
	}

	if p.opts.debug {
		fmt.Println(qry)
	}

	if p.opts.printResponse {
		var max string
		var timestamp string
		for rows.Next() {
			if err = rows.Scan(&timestamp, &max); err == nil {
				fmt.Printf("%s %s\n", timestamp, max)
			} else {
				fmt.Printf("query error\n")
			}
		}
		fmt.Printf("-----------------")
	}
	for rows.Next() {

	}
	rows.Close()

	took := float64(time.Since(start).Nanoseconds()) / 1e6
	stat := query.GetStat()
	stat.Init(q.HumanLabelName(), took)

	return []*query.Stat{stat}, nil
}

func newProcessor() query.Processor { return &processor{} }
