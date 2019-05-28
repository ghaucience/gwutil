#ifndef __VLIST_H_
#define __VLIST_H_

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

  HWND vlist_init(HWND hwnd, char *heads[], int cnt, int btnid);
  int vlist_append(HWND hList, char *data[], int cnt);
  void vlist_clear(HWND hList);
  HWND vlist_getwindow(HWND hList);
  int vlist_hitted_item(HWND hList, int x, int y);
  void vlist_getcommip(HWND hList, int subitem, char *ip, int len);
  int vlist_getcommport(HWND hList, int subitem);
  
  int vlist_export(HWND hList);
#ifdef __cplusplus
}
#endif
#endif
