// Copyright (c) 2020-present,  INSPUR Co, Ltd.  All rights reserved.

typedef struct aeParameter {
  struct aeParameter *next; /* list link */
  char *key;                /* parameter name */
  char *value;              /* parameter value */
  int oid;
  /* Note: name and value are stored in same malloc block as struct is */
} aeParameters;

void termAEParamers(aeParameters *param);
