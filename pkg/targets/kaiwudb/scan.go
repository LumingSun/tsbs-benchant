package kaiwudb

import (
	"bytes"
	"hash/fnv"
	"sync"

	"github.com/benchant/tsbs/pkg/data"
	"github.com/benchant/tsbs/pkg/targets"
)

type Node2chan struct {
	chans []int
	idx   int
}

// indexer is used to consistently send the same hostnames to the same worker
type indexer struct {
	buffer        *bytes.Buffer
	prefix        []byte
	partitions    int
	hashEndGroups []uint32
	tmp           map[string]uint
	numChan       int
	node2Chan     []Node2chan
	Nodes         int
}

func (i *indexer) GetIndex(item data.LoadedPoint) uint {
	p := item.Data.(*point)
	var nodeID uint
	if p.sqlType != InsertMetric {
		return 0
	}
	nodeID, exist := i.tmp[p.tag]
	if !exist && i.Nodes > 1 {
		pt := make([]byte, 31)
		copy(pt, []byte(p.tag))

		hashPartitionNum := 10 * i.Nodes // partitionBalanceNumber * int64(len(nodeStatus.Nodes))
		hashPartitionSize := 65535 / hashPartitionNum
		fnv32 := fnv.New32()
		_, err := fnv32.Write(pt)
		if err != nil {
			return 0
		}
		hash := fnv32.Sum32() % 65535
		hashPartitionID := hash / uint32(hashPartitionSize)
		if hashPartitionID == uint32(hashPartitionNum) {
			hashPartitionID--
		}
		i.tmp[p.tag] = uint(hashPartitionID % uint32(i.Nodes))
		nodeID = uint(hashPartitionID % uint32(i.Nodes))
	} else {
		nodeID = 0
	}

	targetChans := &(i.node2Chan[nodeID])
	index := uint(targetChans.chans[targetChans.idx])
	targetChans.idx++
	if targetChans.idx == len(targetChans.chans) {
		targetChans.idx = 0
	}
	return index
	//return i.tmp[p.device]
}

// point is a single row of data keyed by which superTable it belongs
type point struct {
	sqlType    byte
	table      string
	device     string
	tag        string
	fieldCount int
	sql        string
	cols       string
}

var GlobalTable = sync.Map{}

type hypertableArr struct {
	createSql   []*point
	m           map[string][]string
	devices     map[string]bool
	totalMetric uint64
	cnt         uint
}

func (ha *hypertableArr) Len() uint {
	return ha.cnt
}

func (ha *hypertableArr) Append(item data.LoadedPoint) {
	that := item.Data.(*point)
	if that.sqlType == InsertMetric {
		// ha.m[that.table+":"+that.device] = append(ha.m[that.table+":"+that.device], that.sql)
		ha.m[that.table] = append(ha.m[that.table], that.sql)
		ha.devices[that.device] = true
		ha.totalMetric += uint64(that.fieldCount)
		ha.cnt++
	} else {
		ha.createSql = append(ha.createSql, that)
	}
}

func (ha *hypertableArr) Reset() {
	ha.m = map[string][]string{}
	ha.devices = map[string]bool{}
	ha.cnt = 0
	ha.createSql = ha.createSql[:0]
}

type factory struct{}

func (f *factory) New() targets.Batch {
	return &hypertableArr{
		m:       map[string][]string{},
		devices: map[string]bool{},
		cnt:     0,
	}
}
