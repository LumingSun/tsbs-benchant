// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.
/*-------------------------------------------------------------------------
 *
 * auth.h
 *	  Definitions for network authentication routines
 *
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/auth.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef AUTH_H
#define AUTH_H

#include "libpq-be.h"

extern int ClientAuthentication(Port *port);

void auth_failed(Port *port);
int sendAuthRequest(Port *port, AuthRequest areq);
char *recv_password_packet(Port *port);

#ifdef USE_SSL
int	CheckCertAuth(Port *port);
#endif

#endif   /* AUTH_H */
