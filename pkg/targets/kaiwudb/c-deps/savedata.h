//
// Created by hxy on 5/23/23.
//

#ifndef SAVEDATA_SAVEDATA_H
#define SAVEDATA_SAVEDATA_H
#ifdef __cplusplus
extern "C" {
#endif
int SetPortal(const char* tenant_name, const char* portal_name);

int Connect(const char* url, const char* user, const char* passwd);

int GetErrObject();

int GetCurPortal();

int GetDevice(const char* device_name);

int CloseKConn();

int SaveData(char* data);

#ifdef __cplusplus
}
#endif
#endif
