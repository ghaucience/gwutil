#ifndef __UDP_H_
#define __UDP_H_


#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

 #ifdef __cplusplus
extern "C" {
#endif


  int udp_init();
  void udp_uninit();
  SOCKET udp_open(int port);
  int udp_close(SOCKET sock);
  int udp_recv(SOCKET sock, char *buffer, int len, int ms, char *cliip, int *cliport);
  int udp_send(SOCKET sock, char *buffer, int len, int ms, char *ip, int port);
  
  #define udp_printf(...) printf(##__VA_ARGS__)
  //#define udp_printf(...) MessageBox(NULL, ##__VA_ARGS__, "Error", MB_OK)

#ifdef __cplusplus
}
#endif
#endif
