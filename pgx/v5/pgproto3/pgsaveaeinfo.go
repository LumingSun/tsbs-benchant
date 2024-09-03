package pgproto3

import (
	"encoding/binary"
	"github.com/jackc/pgx/v5/internal/pgio"
)

type AEParameterValue struct {
	value string
	oid   int32
}

// ae参数
type AEParameter struct {
	Parameter map[string]AEParameterValue
}

func NewAEParameter() *AEParameter {
	return &AEParameter{
		Parameter: map[string]AEParameterValue{},
	}
}

// Backend identifies this message as sendable by the PostgreSQL backend.
func (*AEParameter) Backend() {}

// Frontend identifies this message as sendable by a PostgreSQL frontend.
func (*AEParameter) Frontend() {}

// Decode decodes src into dst. src must contain the complete message with the exception of the initial 1 byte message
// type identifier and 4 byte message length.
// 解析并存储ae参数到AEParameter

func (dst *AEParameter) Decode(src []byte) error {

	if len(src) < 2 {
		return &invalidMessageFormatErr{messageType: "AEParameter"}
	}

	cursor := 0
	count, cursor := dst.getInt32(src, cursor)

	for i := int32(0); i < count; i++ {
		localCursor := cursor
		keyLength, localCursor := dst.getInt32(src, localCursor)
		key, localCursor := dst.getString(src, int(keyLength), localCursor)
		oid, localCursor := dst.getInt32(src, localCursor)
		valueLength, localCursor := dst.getInt32(src, localCursor)
		value, localCursor := dst.getString(src, int(valueLength), localCursor)
		dst.Parameter[key] = AEParameterValue{value, oid}
		cursor = localCursor
	}
	return nil
}

func (dst *AEParameter) getInt32(src []byte, cursor int) (int32, int) {
	resultBuf := src[cursor : cursor+4]
	cursor += 4
	result := int32(binary.BigEndian.Uint32(resultBuf))
	return result, cursor
}

func (dst *AEParameter) getString(src []byte, size int, cursor int) (string, int) {
	resultBuf := src[cursor : cursor+size]
	cursor += size + 1
	result := string(resultBuf)
	return result, cursor
}

// Encode encodes src into dst. dst will include the 1 byte message type identifier and the 4 byte message length.
func (src *AEParameter) Encode(dst []byte) []byte {
	dst = append(dst, 'e')
	dst = pgio.AppendInt32(dst, int32(4+len(src.Parameter)))
	return dst
}
