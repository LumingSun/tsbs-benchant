package kaiwudb

import (
	"bytes"
	"math"

	"github.com/benchant/tsbs/pkg/data/source"
	"github.com/benchant/tsbs/pkg/targets"
)

var (
	KAIWUINSERT     = "insert"
	KAIWUPREPARE    = "prepare"
	KAIWUPREPAREIOT = "prepareiot"
)

func NewBenchmark(dbName string, opts *LoadingOptions, dataSourceConfig *source.DataSourceConfig) (targets.Benchmark, error) {
	var ds targets.DataSource
	if dataSourceConfig.Type == source.FileDataSourceType {
		ds = newFileDataSource(dataSourceConfig.File.Location)
	} else {
		panic("not implement")
	}

	return &benchmark{
		opts:   opts,
		ds:     ds,
		dbName: dbName,
	}, nil
}

type benchmark struct {
	opts   *LoadingOptions
	ds     targets.DataSource
	dbName string
}

func (b *benchmark) GetDataSource() targets.DataSource {
	return b.ds
}

func (b *benchmark) GetBatchFactory() targets.BatchFactory {
	return &factory{}
}

func (b *benchmark) GetPointIndexer(maxPartitions uint) targets.PointIndexer {
	if maxPartitions > 1 {
		interval := uint32(math.MaxUint32 / maxPartitions)
		hashEndGroups := make([]uint32, maxPartitions)
		for i := 0; i < int(maxPartitions); i++ {
			if i == int(maxPartitions)-1 {
				hashEndGroups[i] = math.MaxUint32
			} else {
				hashEndGroups[i] = interval*uint32(i+1) - 1
			}
		}
		prefix := []byte("1." + b.dbName + ".")
		var ncs []Node2chan
		for n := 0; n < len(b.opts.Port); n++ {
			var nc Node2chan
			nc.idx = 0
			ncs = append(ncs, nc)
		}
		for w := 0; w < b.opts.Workers; w++ {
			ncs[w%len(b.opts.Port)].chans = append(ncs[w%len(b.opts.Port)].chans, w)
		}

		return &indexer{buffer: &bytes.Buffer{}, prefix: prefix, hashEndGroups: hashEndGroups,
			partitions: int(maxPartitions), tmp: map[string]uint{}, numChan: b.opts.Workers, node2Chan: ncs, Nodes: len(b.opts.Port)}
	}
	return &targets.ConstantIndexer{}
}

func (b *benchmark) GetProcessor() targets.Processor {
	return newProcessorInsert(b.opts, b.dbName)
}

func (b *benchmark) GetDBCreator() targets.DBCreator {
	return &dbCreator{opts: b.opts, ds: b.ds}
}
