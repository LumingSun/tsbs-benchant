// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.
/*-------------------------------------------------------------------------
 *
 * pqformat.h
 *		Definitions for formatting and parsing frontend/backend messages
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/pqformat.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PQFORMAT_H
#define PQFORMAT_H

#include "lib/stringinfo.h"

extern int pq_beginmessage(StringInfo buf, char msgtype);
extern int pq_sendbyte(StringInfo buf, int byt);
extern int pq_sendbytes(StringInfo buf, const char *data, int datalen);
extern int pq_sendcountedtext(StringInfo buf, const char *str, int slen,
				   bool countincludesself);
extern int pq_sendtext(StringInfo buf, const char *str, int slen);
extern int pq_sendstring(StringInfo buf, const char *str);
extern int pq_send_ascii_string(StringInfo buf, const char *str);
extern int pq_sendint(StringInfo buf, int i, int b);
extern int pq_sendint64(StringInfo buf, int64 i);
extern int pq_sendfloat4(StringInfo buf, float4 f);
extern int pq_sendfloat8(StringInfo buf, float8 f);
extern void pq_endmessage(Port *port, StringInfo buf);

extern int pq_begintypsend(StringInfo buf);
extern bytea *pq_endtypsend(StringInfo buf);

extern void pq_puttextmessage(Port *port, char msgtype, const char *str);
extern void pq_putemptymessage(Port *port, char msgtype);

extern int	pq_getmsgbyte(StringInfo msg);
extern unsigned int pq_getmsgint(StringInfo msg, int b);
extern int64 pq_getmsgint64(StringInfo msg);
extern float4 pq_getmsgfloat4(StringInfo msg);
extern float8 pq_getmsgfloat8(StringInfo msg);
extern const char *pq_getmsgbytes(StringInfo msg, int datalen);
extern void pq_copymsgbytes(StringInfo msg, char *buf, int datalen);
extern char *pq_getmsgtext(StringInfo msg, int rawbytes, int *nbytes);
extern const char *pq_getmsgstring(StringInfo msg);
extern void pq_getmsgend(StringInfo msg);

#endif   /* PQFORMAT_H */
