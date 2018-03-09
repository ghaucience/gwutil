#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <windows.h>
#include <commctrl.h>

#include "vlist.h"
#include "resource.h"

using namespace std;

static HWND hList = NULL;

static HWND vlist_create_window(HWND hwndParent);
static int vlist_append_head(char *heads[], int cnt);

int vlist_init(HWND hwnd,  char *heads[], int cnt) {

  hList = vlist_create_window(hwnd);
  
  vlist_append_head(heads, cnt);
  return 0;
}


int vlist_append(char *data[], int cnt) {
  int itemcnt = TabCtrl_GetRowCount(hList);
  int itemidx = itemcnt;
  
  char itemname[256];
  SYSTEMTIME sys;
  GetLocalTime( &sys );
  sprintf(itemname, "%02d/%02d %02d:%02d:%02d",
	  sys.wMonth,  sys.wDay,
	  sys.wHour,  sys.wMinute,  sys.wSecond);
  //创建项目  
  LVITEM item1;  
  RtlZeroMemory(&item1, sizeof(LVITEM));

  //item1.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;  
  item1.mask = LVIF_TEXT | LVIF_INDENT;  
  item1.pszText = itemname;
  item1.iItem = itemidx;//项目号  
  //item1.iImage = 0;//图片号  
  item1.iIndent = 0;  
  SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&item1);

  //创建子项
  int i = 0;
  for (i = 0; i < cnt; i++) {  
    //item1.mask = LVIF_TEXT | LVIF_IMAGE;  
    item1.mask = LVIF_TEXT;  
    item1.iItem = itemidx;  
    item1.iSubItem = i+1;  
    item1.iImage = -1;  
    item1.pszText =(LPSTR)(data[i]);
    SendMessage(hList, LVM_SETITEM, 0, (LPARAM)&item1);
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int vlist_append_head(char  *heads[], int cnt) {
  //SetWindowTheme(listview1, L"Explorer", NULL);  

  //创建图片列表 
  //HIMAGELIST imglist1;  
  //imglist1 = ImageList_Create(20, 20, ILC_MASK, 1, 1);  
  //ImageList_AddIcon(imglist1, LoadIcon((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), TEXT("ICON_2")));  
  //ImageList_AddIcon(imglist1, LoadIcon((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), TEXT("ICON_1")));  
  //ListView_SetImageList(listview1, imglist1, LVSIL_SMALL);
  
  RECT rc;
  GetWindowRect(hList, &rc);
  int width = (rc.right -rc.left) / ( 1 + cnt);
  
  //创建列头
  LVCOLUMN list1;  
  RtlZeroMemory(&list1, sizeof(LVCOLUMN));  
  list1.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;//掩码  
  list1.fmt = LVCFMT_LEFT;//左对齐  
  //list1.cx = 90;//列宽
  
  list1.cx = width-1;//列宽  
  int i = 0;
  
  list1.pszText = (LPSTR)"时间";
  SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&list1);//创建列
  for (i = 0; i < cnt; i++) {
    list1.pszText = (LPSTR)(heads[i]);
    SendMessage(hList, LVM_INSERTCOLUMN, i+1, (LPARAM)&list1);//创建列  
  }

  /*
  i = 0;
  for (i = 0; i < cnt; i++) {
    Arg_t *a = heads + i;
    list1.pszText = (LPWSTR)a->name;  
    SendMessage(listview1, LVM_INSERTCOLUMN, 0, (LPARAM)&list1);//创建列  
  }	
  */
  return 0;
}

static HWND vlist_create_window(HWND hwndParent) {
    //创建listview  
  RECT rc;
  GetClientRect(hwndParent, &rc);

  RECT rcbtn;
  GetClientRect(GetDlgItem(hwndParent, BTN_SCAN),&rcbtn);
  
  
  int style = WS_CHILD | LVS_REPORT | LVS_SINGLESEL | WS_VISIBLE;

  HWND hh = CreateWindowEx(WS_EX_STATICEDGE, TEXT("SysListView32"), NULL,
			   style,
			   0, 0, rc.right - rc.left - (rcbtn.right - rcbtn.left), rc.bottom - rc.top-10, 
			   hwndParent, (HMENU)1, GetModuleHandle(NULL), NULL);

  ListView_SetExtendedListViewStyle(hh, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);//设置listview扩展风格  
  SendMessage(hh, WM_SETFONT, (WPARAM)GetStockObject(17), 0);

  
  return hh;
  
}


void vlist_clear() {
  //int nCols;
  //HWND hWndListView, hWndListViewHeader;
  
  //删除所有行
  SendMessage(hList, LVM_DELETEALLITEMS, 0, 0);

  /*
  //得到ListView的Header窗体
  hWndListViewHeader = (HWND)SendMessage(hList, LVM_GETHEADER, 0, 0);
  
  //得到列的数目
  nCols = SendMessage(hWndListViewHeader, HDM_GETITEMCOUNT, 0, 0);
    
  nCols--;
  //删除所有列
  for(; nCols >= 0; nCols--)
    SendMessage(hList, LVM_DELETECOLUMN, nCols, 0);  
  */
}

HWND vlist_getwindow() {
  return hList;
}



int vlist_hitted_item(int x, int y) {
  int itemcnt = ListView_GetItemCount(hList);
  int i = 0;
  for (i = 0; i < itemcnt; i++) {
    int state = ListView_GetItemState(hList, i, LVIS_SELECTED);
    if (state == 0) {
      continue;
    }
    RECT rc;
    ListView_GetItemRect(hList, i, &rc, LVIR_BOUNDS);
    if ((x >= rc.left && x <= rc.right) && (y >= rc.top && y <= rc.bottom)) {
      return i;
    }
  }
  return -1;
}


void vlist_getcommip(int subitem, char *ip, int len) {
  ListView_GetItemText(vlist_getwindow(), subitem, 6, ip, len);  
}
int vlist_getcommport(int subitem) {
  // char strport[128];
  //ListView_GetItemText(vlist_getwindow(), subitem, 7, strport, sizeof(strport));
  //int port = 5566
  //f (sscanf(strport, "%d", &port) != 1) {
  //port = 5566
  //}
  return 5566;
}
