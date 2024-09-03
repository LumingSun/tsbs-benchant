package pgx_test

import (
	"context"
	"fmt"
	"net"
	"strings"
	"testing"

	"github.com/jackc/pgx/v5"
)

func TestPgxKaiwuDb(t *testing.T) {
	var (
		host          string = "127.0.0.1"
		port          int32  = 26257
		user          string = "root"
		defaultDbName string = "defaultdb"
		tenant        string = "t2"
		portal        string = "p2"
		device        string = "d2"
		boHost        string = "127.0.0.1"
		boPort        int32  = 9091
	)

	// Step1: connect to znbase to get parameters
	url := fmt.Sprintf("dbname=%s host=%s port=%d user=%s mode=3 password=0", defaultDbName, host, port, user)
	conn, err := pgx.Connect(context.Background(), url)
	if err != nil {
		panic(fmt.Sprintf("connnect to database failed,err :%s", err))
	}

	// 创建租户tenant
	sql := fmt.Sprintf("create tenant %s;", tenant)
	_, err = conn.Exec(context.Background(), sql)
	if err != nil && !strings.Contains(err.Error(), "already exists") {
		panic(fmt.Sprintf("kaiwudb2.0 create tenant failed,err :%s", err))
	}

	// 创建portal
	sql = fmt.Sprintf("create portal %s.%s;", tenant, portal)
	_, err = conn.Exec(context.Background(), sql)
	if err != nil && !strings.Contains(err.Error(), "already exists") {
		panic(fmt.Sprintf("kaiwudb2.0 create portal failed,err :%s", err))
	}

	// 创建device
	sql = fmt.Sprintf("create device %s.%s.%s (VIN char(17),value1 float,value2 float,value3 float,value4 float,value5 float)", tenant, portal, device)
	_, err = conn.Exec(context.Background(), sql)
	if err != nil && !strings.Contains(err.Error(), "already exists") {
		panic(fmt.Sprintf("kaiwudb2.0 create table failed,err :%s", err))
	}

	// 由于savedata暂时未调通使用，需进入ksql内写入数据或者连接bo的9091端口load导入数据

	// 连接bo高速写入端口9091写入数据
	loadAddr := fmt.Sprintf("%s:%d", boHost, boPort)
	boConn, err := net.Dial("tcp", fmt.Sprintf("%s", loadAddr))
	boDatabase := fmt.Sprintf("%s_%s", tenant, portal)
	boTable := fmt.Sprintf("%s_%s", device, device)
	ts := "1665737114870"
	csv := fmt.Sprintf("csv%c"+boDatabase+"."+boTable+"\n", 'i')
	strTableValue := fmt.Sprintf("%s,LSVNV2182E2100001, 3, 1360, 154673, 7101, 935\n%s,LSVNV2182E2100002, 3, 1360, 154673, 7101, 935\n", ts, ts)
	csvSQL := fmt.Sprintf("%s%s", csv, strTableValue)
	_, err = boConn.Write([]byte(csvSQL))
	if err != nil {
		panic(fmt.Sprintf("kaiwudb2.0 load csv data failed,err :%s", err))
	}

	// 查询
	sql = fmt.Sprintf("select value1 unionfrom %s.%s.*", tenant, portal)
	result, err := conn.Exec(context.Background(), sql)
	if err != nil {
		panic(fmt.Sprintf("kaiwudb2.0 create table failed,err :%s", err))
	}
	fmt.Printf("%d", result.RowsAffected())

	err = conn.Close(context.Background())
	if err != nil {
		panic(fmt.Sprintf("kaiwudb2.0 close failed,err :%s", err))
	}

	// savedata暂时未能使用，连接mode=3会连接错误，mode=1时，执行错误；
	// ksql内mode=1时，无法执行，需要mode=3
	sql = fmt.Sprintf(":savedata %s.%s.%s (%s, 597778307: 'LSVNV2182E2100000', 3, 1360, 154673, 7101, 935)", tenant, portal, device, device)
	_, err = conn.Exec(context.Background(), sql)
	if err != nil {
		panic(fmt.Sprintf("kaiwudb2.0 insert data failed,err :%s", err))
	}

	sql = fmt.Sprintf(":getlatestdata %s.%s.%s.%s", tenant, portal, device, "value1")
	_, err = conn.Exec(context.Background(), sql)
	if err != nil {
		panic(fmt.Sprintf("kaiwudb2.0 insert data failed,err :%s", err))
	}

	// // Step 2: get zdpConHandle
	// connHandle := conn.PgConn().Frontend().AEParameter("conn_id")
	// aeToken := conn.PgConn().Frontend().AEParameter("token")
	// // get assistant engine URL
	// tsUrl := conn.PgConn().Frontend().AEParameter("ts_url")

	// delim := ":"
	// arr := strings.Split(tsUrl, delim)
	// tsIp := arr[0]
	// tsPort := arr[1]

	// // Step 3: connection to assistant engine
	// connInfo := fmt.Sprintf("host=%s port=%s user=%s conn_id=%s password=%s tenant=%s portal=%s", tsIp, tsPort, user, connHandle, aeToken, tenant, portal)
	// conn2, err := pgx.Connect(context.Background(), connInfo)
	// if err != nil {
	// 	panic(fmt.Sprintf("connnect to database failed,err :%s", err))
	// }
}
