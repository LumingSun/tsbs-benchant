package pgx

import (
	"context"
	"github.com/jackc/pgx/v5/pgconn"
	"regexp"
	"strings"
)

const (
	TargetUrl       = "target_url"
	ConnectionId    = "conn_id"
	Token           = "token"
	TsUrl           = "ts_url"
	SharedMemoryKey = "shm_key"
)

type KwConn struct {
	Kwbase          *Conn
	Ts              *Conn
	TargetUrl       string
	ConnectionId    string
	Token           string
	TsUrl           string
	SharedMemoryKey string
}

func getTargetTenantPortal(url string) string {
	tenantIndex := strings.Index(url, "tenant")
	if tenantIndex == -1 {
		return ""
	}
	tenantSubStr := url[tenantIndex:]
	tenantIndexEnd := strings.Index(tenantSubStr, " ")
	target := ""
	if tenantIndexEnd == -1 {
		target += " " + tenantSubStr
	} else {
		target += " " + tenantSubStr[:tenantIndexEnd]
	}

	portalIndex := strings.Index(url, "portal")
	if portalIndex == -1 {
		return ""
	}
	portalSubStr := url[portalIndex:]
	portalIndexEnd := strings.Index(portalSubStr, " ")
	if portalIndexEnd == -1 {
		target += " " + portalSubStr
	} else {
		target += " " + portalSubStr[:portalIndexEnd]
	}

	return target
}

func getMode(url string) string {
	regex := regexp.MustCompile("mode=(\\d+)")
	return regex.FindString(url)
}

func getUser(url string) string {
	regex := regexp.MustCompile("(?:^|\\W)user=([^\\s,]+)(?:$|\\W)")
	return regex.FindString(url)
}

func getApplicationName(url string) string {
	regex := regexp.MustCompile("(?:^|\\W)application_name=([^\\s,]+)(?:$|\\W)")
	return regex.FindString(url)
}

func getSSLParams(url string) string {
	ssl := ""
	sslStrs := []string{"sslmode", "sslcert", "sslkey", "sslrootcert"}
	for _, str := range sslStrs {
		sslIndex := strings.Index(url, str)
		if sslIndex == -1 {
			continue
		}
		sslSubStr := url[sslIndex:]
		sslIndexEnd := strings.Index(sslSubStr, " ")
		if sslIndexEnd == -1 {
			ssl += " " + sslSubStr
		} else {
			ssl += " " + sslSubStr[:sslIndexEnd]
		}
	}
	return ssl
}

func splitUrl(url string) (ip string, port string) {
	strs := strings.Split(url, ":")
	return strs[0], strs[1]
}

func KwConnect(ctx context.Context, connInfo string) (*KwConn, error) {
	// Step 1: connect to Kwbase
	kwConnect, err := Connect(ctx, connInfo)
	if err != nil {
		return nil, err
	}

	targetUrl := AEParameter(kwConnect, TargetUrl)
	// 如果targetUrl存在，说明需要网络中转
	if targetUrl != "" {
		err = kwConnect.Close(ctx)
		if err != nil {
			return nil, err
		}

		kwConnect, err = Connect(ctx, targetUrl)
		if err != nil {
			return nil, err
		}
	}

	// Step 2: get kwdbConHandle_
	connectionId := AEParameter(kwConnect, ConnectionId)
	// Step 3: connection to assistant engine
	token := AEParameter(kwConnect, Token)
	targetTenantPortal := ""

	if token != "" {
		targetTenantPortal = getTargetTenantPortal(connInfo)
	}
	tsUrl := AEParameter(kwConnect, TsUrl)

	var tsConnect *Conn
	sharedMemoryKey := ""
	if tsUrl != "" {
		ip, port := splitUrl(tsUrl)
		ssl := getSSLParams(connInfo)
		mode := getMode(connInfo)
		userInfo := getUser(connInfo)
		applicationName := getApplicationName(connInfo)
		tsConnInfo := "host=" + ip + " " +
			"port=" + port + " " +
			"conn_id=" + connectionId + " " +
			mode + " " +
			applicationName + " " +
			userInfo + " " +
			" password=" + token + " " +
			targetTenantPortal + " " +
			ssl

		tsConnect, err = Connect(ctx, tsConnInfo)
		if err != nil {
			return nil, err
		}
		// Step 4: get shared memory key and token
		sharedMemoryKey = AEParameter(kwConnect, SharedMemoryKey)
	}

	conn := KwConn{
		Kwbase:          kwConnect,
		Ts:              tsConnect,
		TargetUrl:       targetUrl,
		ConnectionId:    connectionId,
		Token:           token,
		TsUrl:           tsUrl,
		SharedMemoryKey: sharedMemoryKey,
	}
	// todo(wzk) step 5: get current instance
	// todo(wzk) step 6: get license

	return &conn, nil
}

func (c *KwConn) Exec(ctx context.Context, sql string, arguments ...any) (pgconn.CommandTag, error) {
	if sql[0] == ':' {
		// todo(wzk) ts assistant engine AeExec interface implement
		return c.Ts.Exec(ctx, sql, arguments)
	}
	// todo(wzk) ts master engine Exec interface implement
	return c.Kwbase.Exec(ctx, sql, arguments)
}
