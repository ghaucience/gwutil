#ifndef __VLIST_H_
#define __VLIST_H_

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

  int vlist_init(HWND hwnd, char *heads[], int cnt);
  int vlist_append(char *data[], int cnt);
  void vlist_clear();
  HWND vlist_getwindow();
  int vlist_hitted_item(int x, int y);
  void vlist_getcommip(int subitem, char *ip, int len);
  int vlist_getcommport(int subitem);
  
  int vlist_export();
#ifdef __cplusplus
}
#endif
#endif
