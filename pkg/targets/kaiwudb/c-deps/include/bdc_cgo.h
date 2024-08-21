/*
 * @author jiadx
 * @date 2023/3/4
 * @version 1.0
*/
#ifndef BDC_CGO_H
#define BDC_CGO_H
#ifdef __cplusplus
extern "C" {
#endif
int SetPortal(char* url, int url_len,
              char* tenant_name, int t_len,
              char* portal_name, int p_len,
              char* user, int u_len, char* passw, int pw_len,
              int worker_num);

int OpenKConn(char* dev_name, int d_len);

int CloseKConn();

int CreateDevice(char* dev_name, int d_len, int num);

int SaveData(char* pay_load, long pay_len, int batch);
#ifdef __cplusplus
}
#endif
#endif