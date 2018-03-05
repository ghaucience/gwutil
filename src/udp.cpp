#include "udp.h"
#include "util.h"

#pragma comment(lib,"ws2_32.lib")


int udp_init() {
  WSADATA wsa;  
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  {  
    udp_printf("WSAStartup failed\n");  
    return -1;  
  }    
  return 0;
}
void udp_uninit() {
  WSACleanup();  
}
SOCKET udp_open(int port) {
  //建立一个UDP的socket  
  SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  
  if (sock == SOCKET_ERROR)  {  
    udp_printf("create socket failed\n");  
    return INVALID_SOCKET;
  }

  bool bOpt = true;   
  setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));     
  
  //绑定地址信息  
  sockaddr_in serverAddr;  
  serverAddr.sin_family = AF_INET;  
  serverAddr.sin_port = htons(port);  
  serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);  
  
  bind(sock, (sockaddr*)&serverAddr, sizeof(sockaddr));  
        
  return sock;
}


int udp_close(SOCKET sock) {
  closesocket(sock);
  return 0;
}
int udp_recv(SOCKET sock, char *buffer, int len, int ms,  char *cliip, int *cliport) {
  fd_set readSet;
  FD_ZERO(&readSet);   
  FD_SET(sock, &readSet); // 将socketListen加入套接字集合中
  struct timeval tv = { ms / 1000, ms % 1000 };
  int result = select(0, &readSet, NULL, NULL, &tv);
  
  if (result == SOCKET_ERROR)   {  
    udp_printf("select() error\n");
    return -1;
  }
  if (result == 0) {
    return 0;
  }
  
  if (!FD_ISSET(sock, &readSet)) {
    return -2;
  }
  
  memset(buffer, 0, len);
  
  // 网络节点的信息，用来保存客户端的网络信息  
  sockaddr_in clientAddr;  
  memset(&clientAddr, 0, sizeof(sockaddr_in));  
  int clientAddrLen = sizeof(sockaddr);
  
  //接收客户端发来的数据  
  int ret = recvfrom(sock, buffer, len, 0,(sockaddr*) &clientAddr,&clientAddrLen );

  if (ret <= 0) {
    return -3;
  }
  buffer[ret] = 0;
  
  strcpy(cliip, inet_ntoa(clientAddr.sin_addr));
  *cliport = ntohs(clientAddr.sin_port);
  
  udp_printf("[recv msg] [%s] [%d]:\n", cliip, *cliport );
  util_viewbuf(buffer, ret);
  
  return ret;
}
int udp_send(SOCKET sock, char *buffer, int len, int ms, char *ip, int port) {
  // 发一个数据包返回给客户
  
  sockaddr_in addr = { 0 };  
  addr.sin_family = AF_INET;  
  addr.sin_port = htons(port);  
  addr.sin_addr.S_un.S_addr = inet_addr(ip);
  int ret = sendto(sock, buffer, len, 0, (SOCKADDR *)&addr, sizeof(SOCKADDR));  
  if (ret == 0)  {  
    udp_printf("send [%02x..] failed\n", buffer[0]&0xff);  
    return -1;  
  }
  
  udp_printf("[send msg][%s][%d]:\n", ip, port);
  util_viewbuf(buffer, len);
  
  return 0;
}


