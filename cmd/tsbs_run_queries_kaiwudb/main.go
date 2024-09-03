package main

import (
	"context"
	"encoding/json"
	"fmt"
	"log"
	"regexp"
	"strconv"
	"time"

	"github.com/jackc/pgx/v5/pgtype"

	"github.com/benchant/tsbs/internal/utils"
	"github.com/benchant/tsbs/pkg/query"
	"github.com/benchant/tsbs/pkg/targets/kaiwudb/commonpool"
	"github.com/blagojts/viper"
	"github.com/jackc/pgx/v5"
	"github.com/pkg/errors"
	"github.com/spf13/pflag"
)

var (
	user   string
	pass   string
	host   string
	port   int
	format int
	mode   int
	runner *query.BenchmarkRunner
)

func init() {
	var config query.BenchmarkRunnerConfig
	config.AddToFlagSet(pflag.CommandLine)

	pflag.String("user", "root", "User to connect to kaiwudb")
	pflag.String("pass", "", "Password for the user connecting to kaiwudb")
	pflag.String("host", "", "kaiwudb host")
	pflag.Int("port", 26257, "kaiwudb Port")
	pflag.Int("format", 0, "kaiwudb pg format")
	pflag.Int("mode", int(pgx.QueryExecModeSimpleProtocol), "kaiwudb pg mode")
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
	format = viper.GetInt("format")
	mode = viper.GetInt("mode")
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
	db, err := commonpool.GetConnection(user, pass, host, port, format)
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

	ctx := context.Background()

	var rows pgx.Rows
	var err error
	queryMode := pgx.QueryExecMode(mode)
	switch queryMode {
	case pgx.QueryExecModeCacheDescribe, pgx.QueryExecModeCacheStatement, pgx.QueryExecModeDescribeExec:
		// 定义正则表达式
		regex, err := regexp.Compile(`host_\d+|[^'](-)?[0-9]+\b`)
		if err != nil {
			panic(err)
		}

		// 使用正则表达式进行匹配
		matches := regex.FindAllString(qry, -1)
		// 使用正则表达式进行替换
		sql := regex.ReplaceAllString(qry, "?")
		if p.opts.debug {
			// 打印匹配结果
			fmt.Println("Matches:", matches)
			fmt.Println("sql:", sql)
		}

		s64, err := strconv.ParseInt(matches[1], 10, 64)
		e64, err := strconv.ParseInt(matches[2], 10, 64)
		s := pgtype.Timestamp{Time: time.UnixMilli(s64).UTC(), Valid: true}
		e := pgtype.Timestamp{Time: time.UnixMilli(e64).UTC(), Valid: true}

		rows, err = p.db.Connection.Query(ctx, sql, queryMode, matches[0], s, e)
	default:
		if p.opts.debug {
			fmt.Println(qry)
		}
		rows, err = p.db.Connection.Query(ctx, qry, queryMode)
	}

	if err != nil {
		log.Println("Error running query: '", qry, "'")
		return nil, err
	}

	if p.opts.printResponse {
		prettyPrintResponse(rows, qry)
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

func prettyPrintResponse(rows pgx.Rows, query string) {
	resp := make(map[string]interface{})
	resp["query"] = query
	resp["results"] = mapRows(rows)

	line, err := json.MarshalIndent(resp, "", "  ")
	if err != nil {
		panic(err)
	}

	fmt.Println(string(line) + "\n")
}

func mapRows(r pgx.Rows) []map[string]interface{} {
	rows := []map[string]interface{}{}
	cols := r.FieldDescriptions()
	for r.Next() {
		row := make(map[string]interface{})
		values := make([]interface{}, len(cols))
		for i := range values {
			values[i] = new(interface{})
		}

		err := r.Scan(values...)
		if err != nil {
			panic(errors.Wrap(err, "error while reading values"))
		}

		for i, column := range cols {
			row[column.Name] = *values[i].(*interface{})
		}
		rows = append(rows, row)
	}
	return rows
}
