// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.
/*-------------------------------------------------------------------------
 *
 * libpq.h
 *	  POSTGRES LIBPQ buffer structure definitions.
 *
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/libpq.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef LIBPQ_H
#define LIBPQ_H

#include <sys/types.h>
#include <netinet/in.h>

#include "lib/stringinfo.h"
#include "libpq-be.h"


typedef struct
{
	void		(*comm_reset) (Port *port);
	int			(*flush) (Port *port);
	int			(*flush_if_writable) (Port *port);
	bool		(*is_send_pending) (Port *port);
	int			(*putmessage) (Port *port, char msgtype, const char *s, size_t len);
	void		(*putmessage_noblock) (Port *port, char msgtype, const char *s, size_t len);
    void		(*startcopyout) (Port *port);
    void		(*endcopyout) (Port *port, bool errorAbort);
} PQcommMethods;

extern PGDLLIMPORT PQcommMethods *PqCommMethods;

#define pq_comm_reset(port) (PqCommMethods->comm_reset(port))
#define pq_flush(port) (PqCommMethods->flush(port))
#define pq_flush_if_writable(port) (PqCommMethods->flush_if_writable(port))
#define pq_is_send_pending(port) (PqCommMethods->is_send_pending(port))
#define pq_putmessage(port, msgtype, s, len) \
	(PqCommMethods->putmessage(port, msgtype, s, len))
#define pq_putmessage_noblock(port, msgtype, s, len) \
	(PqCommMethods->putmessage_noblock(port, msgtype, s, len))
#define pq_startcopyout(port) (PqCommMethods->startcopyout())
#define pq_endcopyout(port, errorAbort) (PqCommMethods->endcopyout(errorAbort))

/*
 * External functions.
 */

/*
 * prototypes for functions in pqcomm.c
 */
extern int StreamServerPort(int family, char *hostName,
				 unsigned short portNumber, char *unixSocketDir,
				 pgsocket ListenSocket[], int MaxListen);
extern int	StreamConnection(pgsocket server_fd, Port *port);
extern void StreamClose(pgsocket sock);
extern int  pq_init(Port *port);
extern int	pq_getbytes(Port *port, char *s, size_t len);
extern int	pq_getstring(Port *port, StringInfo s);
extern void pq_startmsgread(Port *port);
extern void pq_endmsgread(Port *port);
extern bool pq_is_reading_msg(Port *port);
extern int	pq_getmessage(Port *port, StringInfo s, int maxlen);
extern int	pq_getbyte(Port *port);
extern int	pq_peekbyte(Port *port);
extern int	pq_getbyte_if_available(Port *port, unsigned char *c);
extern int	pq_putbytes(Port *port, const char *s, size_t len);

/*
 * prototypes for functions in be-secure.c
 */
extern char *ssl_cert_file;
extern char *ssl_key_file;
extern char *ssl_ca_file;
extern char *ssl_crl_file;

extern int	(*pq_putmessage_hook) (char msgtype, const char *s, size_t len);
extern int	(*pq_flush_hook) (void);

extern int	secure_initialize(void);
extern bool secure_loaded_verify_locations(void);
extern void secure_destroy(void);
extern int	secure_open_server(Port *port);
extern void secure_close(Port *port);
extern int pq_readready(Port *port, int timeout_ms);
extern ssize_t secure_read(Port *port, void *ptr, size_t len);
extern ssize_t secure_write(Port *port, void *ptr, size_t len);
extern ssize_t secure_raw_read(Port *port, void *ptr, size_t len);
extern ssize_t secure_raw_write(Port *port, const void *ptr, size_t len);

extern bool ssl_loaded_verify_locations;

/* GUCs */
extern char *SSLCipherSuites;
extern char *SSLECDHCurve;
extern bool SSLPreferServerCiphers;

void initSecure(const char *sslCertFile, const char *sslKeyFile, const char *sslCaFile,
               const char *sslCiphers, const char *sslEcdhCurve, bool sslPreferServerCiphers);

#endif   /* LIBPQ_H */
