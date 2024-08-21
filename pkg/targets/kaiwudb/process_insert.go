package kaiwudb

import (
	"bytes"
	"context"
	"fmt"
	"strings"
	"sync"

	"github.com/benchant/tsbs/pkg/targets"
	"github.com/benchant/tsbs/pkg/targets/kaiwudb/commonpool"
)

type syncCSI struct {
	m sync.Map //table:ctx
}

const Size1M = 1 * 1024 * 1024

type Ctx struct {
	c      context.Context
	cancel context.CancelFunc
}

var globalSCI = &syncCSI{}

type processorInsert struct {
	opts   *LoadingOptions
	dbName string
	sci    *syncCSI
	_db    *commonpool.Conn
	// _db1_0 *commonpool1_0.Conn
	wg  *sync.WaitGroup
	buf *bytes.Buffer
}

func newProcessorInsert(opts *LoadingOptions, dbName string) *processorInsert {
	return &processorInsert{opts: opts, dbName: dbName, sci: globalSCI, wg: &sync.WaitGroup{}, buf: &bytes.Buffer{}}
}

func (p *processorInsert) Init(proNum int, doLoad, _ bool) {
	if !doLoad {
		return
	}
	p.buf.Grow(Size1M)
	var err error
	l := len(p.opts.Port)
	idx := proNum % l

	p._db, err = commonpool.GetConnection(p.opts.User, p.opts.Pass, p.opts.Host[idx], p.opts.Port[idx])
	if err != nil {
		panic(err)
	}

	_, err = p._db.Connection.Exec(context.Background(), "use "+p.dbName)
	if err != nil {
		panic(err)
	}
}

func (p *processorInsert) ProcessBatch(b targets.Batch, doLoad bool) (metricCount, rowCount uint64) {
	batches := b.(*hypertableArr)
	rowCnt := uint64(0)
	metricCnt := batches.totalMetric
	if !doLoad {
		for _, sqls := range batches.m {
			rowCnt += uint64(len(sqls))
		}
		return metricCnt, rowCnt
	}
	p.buf.Reset()

	metricAndTagSql := map[string][]string{}
	// create table and insert first record including tags
	for _, row := range batches.createSql {
		switch row.sqlType {
		/** type 2 sample:
		 * 2,cpu,create table cpu (ts timestamp not null,usage_user int not null,usage_system int not null,
		 * usage_idle int not null,usage_nice int not null,usage_iowait int not null,usage_irq int not null,
		 * usage_softirq int not null,usage_steal int not null,usage_guest int not null,usage_guest_nice int not null)
		 * tags (hostname char(30) not null,region char(30),datacenter char(30),rack char(30),os char(30),arch char(30),
		 * team char(30),service char(30),service_version char(30),service_environment char(30)) primary tags (hostname)
		 */
		case CreateTable:
			c, cancel := context.WithCancel(context.Background())
			ctx := &Ctx{
				c:      c,
				cancel: cancel,
			}
			actual, _ := p.sci.m.LoadOrStore(row.table, ctx)

			// fmt.Println(row.sql)
			_, err := p._db.Connection.Exec(context.Background(), row.sql)
			if err != nil {
				panic(fmt.Sprintf("kaiwudb insert data failed,err :%s", err))
			}
			GlobalTable.Store(row.table, row.cols)
			actual.(*Ctx).cancel()

		/** type 3 sampe:
		 * 3,cpu,host_0,(1451606400000,58,2,24,61,22,63,6,44,80,38,'host_0','eu-west-1','eu-west-1c','87',
		 * 'Ubuntu16.04LTS','x64','NYC','18','1','production')
		 */
		case InsertMetricAndTag:
			c, cancel := context.WithCancel(context.Background())
			ctx := &Ctx{
				c:      c,
				cancel: cancel,
			}
			actual, _ := p.sci.m.LoadOrStore(row.device, ctx)

			//check if table created
			_, ok := GlobalTable.Load(row.table)
			if !ok {
				v, ok := p.sci.m.Load(row.table)
				if ok {
					<-v.(*Ctx).c.Done()

					sql := "insert into " + row.table + " values " + row.sql
					// fmt.Println(sql)
					_, err := p._db.Connection.Exec(context.Background(), sql)
					if err != nil {
						panic(fmt.Sprintf("kaiwudb insert data failed,err :%s", err))
					}

					metricAndTagSql[row.table] = append(metricAndTagSql[row.table], row.sql)
					GlobalTable.Store(row.device, nothing)
					actual.(*Ctx).cancel()
					continue
				}
				// wait for table created
				tableC, tableCancel := context.WithCancel(context.Background())
				tableCtx := &Ctx{
					c:      tableC,
					cancel: tableCancel,
				}
				tableActual, _ := p.sci.m.LoadOrStore(row.table, tableCtx)
				<-tableActual.(*Ctx).c.Done()
			}

			sql := "insert into " + row.table + " values " + row.sql
			// fmt.Println(sql)
			_, err := p._db.Connection.Exec(context.Background(), sql)
			if err != nil {
				panic(fmt.Sprintf("kaiwudb insert data failed,err :%s", err))
			}
			metricAndTagSql[row.table] = append(metricAndTagSql[row.table], row.sql)
			GlobalTable.Store(row.device, nothing)
			actual.(*Ctx).cancel()
		default:
			panic("impossible")
		}
	}

	// 同一张表的type 3数据合并执行，但是需要执行成功后再执行GlobalTable.Store(row.device, nothing)
	/*
		for tableName, sqls := range metricAndTagSql {
			sql := "insert into " + tableName + " values " + strings.Join(sqls, " , ")
			fmt.Println(sql)
			_, err := p._db.Connection.Exec(context.Background(), sql)
			if err != nil {
				panic(fmt.Sprintf("kaiwudb insert data failed,err :%s", err))
			}
		}
	*/

	// make sure first rerord inserted into devices
	p.buf.Reset()
	p.wg.Add(len(batches.devices))
	for deviceName := range batches.devices {
		device := deviceName
		// fmt.Println("device: ", device)
		go func() {
			defer p.wg.Done()
			_, ok := GlobalTable.Load(device)
			if ok {
				return
			}
			v, ok := p.sci.m.Load(device)
			if ok {
				<-v.(*Ctx).c.Done()
				return
			}
			c, cancel := context.WithCancel(context.Background())
			ctx := &Ctx{
				c:      c,
				cancel: cancel,
			}
			actual, _ := p.sci.m.LoadOrStore(device, ctx)
			<-actual.(*Ctx).c.Done()
			return
		}()
	}
	p.wg.Wait()

	/** type 1 sample:
	 * 1,cpu,host_0,11,(1451606400000,58,2,24,61,22,63,6,44,80,38,'host_0')
	 */
	for name, sqls := range batches.m {
		tableName := strings.Split(name, ":")[0]
		v, ok := GlobalTable.Load(tableName)
		if !ok {
			panic("table does not exists!")
		}

		cols := v.(string)

		rowCnt += uint64(len(sqls))
		p.buf.WriteString("insert into ")
		p.buf.WriteString(tableName + cols)
		p.buf.WriteString(" values")
		for i := 0; i < len(sqls); i++ {
			p.buf.WriteString(sqls[i])
			if i < len(sqls)-1 {
				p.buf.WriteString(" , ")
			}
		}
		sql := p.buf.String()
		// fmt.Println(sql)
		_, err := p._db.Connection.Exec(context.Background(), sql)
		if err != nil {
			panic(fmt.Sprintf("kaiwudb insert data failed,err :%s", err))
		}
		p.buf.Reset()
	}

	batches.Reset()
	return metricCnt, rowCnt
}

func (p *processorInsert) Close(doLoad bool) {
	if doLoad {
		p._db.Put()
		//p._db1_0.Put()
	}
}

// func (p *processorInsert) createDeviceAndAttribute(createSql []*point) int {
// 	var deviceNums int = 0
// 	for _, row := range createSql {
// 		deviceNums++
// 		switch row.sqlType {
// 		case CreateTemplateTable:
// 			c, cancel := context.WithCancel(context.Background())
// 			ctx := &Ctx{
// 				c:      c,
// 				cancel: cancel,
// 			}
// 			actual, _ := p.sci.m.LoadOrStore(row.table, ctx)
// 			sql := fmt.Sprintf("create table %s.%s %s", p.opts.DBName, row.table, row.sql)
// 			//fmt.Println(sql)
// 			_, err := p._db.Connection.Exec(ctx.c, sql)
// 			if err != nil && !strings.Contains(err.Error(), "already exists") {
// 				panic(fmt.Sprintf("kaiwudb create device failed,err :%s", err))
// 			}
// 			actual.(*Ctx).cancel()
// 		case CreateTable:
// 			c, cancel := context.WithCancel(context.Background())
// 			ctx := &Ctx{
// 				c:      c,
// 				cancel: cancel,
// 			}
// 			actual, _ := p.sci.m.LoadOrStore(row.device, ctx)
// 			sql := fmt.Sprintf("insert into %s.cpu values %s", p.dbName, row.sql)
// 			//fmt.Println(sql)
// 			_, err := p._db.Connection.Exec(context.Background(), sql)
// 			if err != nil {
// 				panic(fmt.Sprintf("kaiwudb insert data failed,err :%s", err))
// 			}
// 			actual.(*Ctx).cancel()
// 			continue
// 		default:
// 			panic("impossible")
// 		}
// 	}
// 	// fmt.Printf("device numbers : %d\n", deviceNums)
// 	return deviceNums
// }
