#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <windows.h>
#include <commctrl.h>

#include "vlist.h"
#include "resource.h"

using namespace std;

//static HWND hList = NULL;

static HWND vlist_create_window(HWND hwndParent, int btnid);
static int vlist_append_head(HWND hList, char *heads[], int cnt);

HWND vlist_init(HWND hwnd,  char *heads[], int cnt, int btnid) {

  HWND hList = vlist_create_window(hwnd, btnid);
  
  vlist_append_head(hList, heads, cnt);

  return hList;
}


int vlist_append(HWND hList, char *data[], int cnt) {
  int itemcnt = TabCtrl_GetRowCount(hList);
  int itemidx = itemcnt;
  
  char itemname[256];
  SYSTEMTIME sys;
  GetLocalTime( &sys );
  sprintf(itemname, "%02d/%02d %02d:%02d:%02d",
	  sys.wMonth,  sys.wDay,
	  sys.wHour,  sys.wMinute,  sys.wSecond);
  //������Ŀ  
  LVITEM item1;  
  RtlZeroMemory(&item1, sizeof(LVITEM));

  //item1.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_INDENT;  
  item1.mask = LVIF_TEXT | LVIF_INDENT;  
  item1.pszText = itemname;
  item1.iItem = itemidx;//��Ŀ��  
  //item1.iImage = 0;//ͼƬ��  
  item1.iIndent = 0;  
  SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&item1);

  //��������
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
static int vlist_append_head(HWND hList, char  *heads[], int cnt) {
  //SetWindowTheme(listview1, L"Explorer", NULL);  

  //����ͼƬ�б� 
  //HIMAGELIST imglist1;  
  //imglist1 = ImageList_Create(20, 20, ILC_MASK, 1, 1);  
  //ImageList_AddIcon(imglist1, LoadIcon((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), TEXT("ICON_2")));  
  //ImageList_AddIcon(imglist1, LoadIcon((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), TEXT("ICON_1")));  
  //ListView_SetImageList(listview1, imglist1, LVSIL_SMALL);
  
  RECT rc;
  GetWindowRect(hList, &rc);
  int width = (rc.right -rc.left) / ( 1 + cnt);
  
  //������ͷ
  LVCOLUMN list1;  
  RtlZeroMemory(&list1, sizeof(LVCOLUMN));  
  list1.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;//����  
  list1.fmt = LVCFMT_LEFT;//�����  
  //list1.cx = 90;//�п�
  
  list1.cx = width-1;//�п�  
  int i = 0;
  
  list1.pszText = (LPSTR)"ʱ��";
  SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&list1);//������
  for (i = 0; i < cnt; i++) {
    list1.pszText = (LPSTR)(heads[i]);
    SendMessage(hList, LVM_INSERTCOLUMN, i+1, (LPARAM)&list1);//������  
  }

  /*
  i = 0;
  for (i = 0; i < cnt; i++) {
    Arg_t *a = heads + i;
    list1.pszText = (LPWSTR)a->name;  
    SendMessage(listview1, LVM_INSERTCOLUMN, 0, (LPARAM)&list1);//������  
  }	
  */
  return 0;
}

static HWND vlist_create_window(HWND hwndParent, int btnid) {
    //����listview  
  RECT rc;
  GetClientRect(hwndParent, &rc);

  RECT rcbtn;
  //GetClientRect(GetDlgItem(hwndParent, BTN_SCAN),&rcbtn);
  GetClientRect(GetDlgItem(hwndParent, btnid),&rcbtn);  
  
  
  int style = WS_CHILD | LVS_REPORT | LVS_SINGLESEL | WS_VISIBLE;

  HWND hh = CreateWindowEx(WS_EX_STATICEDGE, TEXT("SysListView32"), NULL,
			   style,
			   0, 0, rc.right - rc.left - (rcbtn.right - rcbtn.left), rc.bottom - rc.top-10, 
			   hwndParent, (HMENU)1, GetModuleHandle(NULL), NULL);

  ListView_SetExtendedListViewStyle(hh, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);//����listview��չ���  
  SendMessage(hh, WM_SETFONT, (WPARAM)GetStockObject(17), 0);

  
  return hh;
  
}


void vlist_clear(HWND hList) {
  //int nCols;
  //HWND hWndListView, hWndListViewHeader;
  
  //ɾ��������
  SendMessage(hList, LVM_DELETEALLITEMS, 0, 0);

  /*
  //�õ�ListView��Header����
  hWndListViewHeader = (HWND)SendMessage(hList, LVM_GETHEADER, 0, 0);
  
  //�õ��е���Ŀ
  nCols = SendMessage(hWndListViewHeader, HDM_GETITEMCOUNT, 0, 0);
    
  nCols--;
  //ɾ��������
  for(; nCols >= 0; nCols--)
    SendMessage(hList, LVM_DELETECOLUMN, nCols, 0);  
  */
}

HWND vlist_getwindow(HWND hList) {
  return hList;
}



int vlist_hitted_item(HWND hList, int x, int y) {
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


void vlist_getcommip(HWND hList, int subitem, char *ip, int len) {
  ListView_GetItemText(vlist_getwindow(hList), subitem, 6, ip, len);  
}
int vlist_getcommport(HWND hList, int subitem) {
  // char strport[128];
  //ListView_GetItemText(vlist_getwindow(), subitem, 7, strport, sizeof(strport));
  //int port = 5566
  //f (sscanf(strport, "%d", &port) != 1) {
  //port = 5566
  //}
  return 5566;

}
void vlist_get_commmac(HWND hList, int subitem, char *mac, int len) {
  ListView_GetItemText(vlist_getwindow(hList), subitem, 1, mac,  len);  
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
static void test_year_day_str(char *ydstr, int len) {

  time_t timer;
  struct tm *tblock;
  timer = time(NULL);
  tblock = localtime(&timer);

  char buff[128];
  memset(ydstr, 0, len);
  sprintf(buff, "%d��%d��%d��_%d02%02d%02d-By_%s.txt", tblock->tm_year+1900, tblock->tm_mon+1, tblock->tm_mday, tblock->tm_hour, tblock->tm_min, tblock->tm_sec, "Keven");
  
  strcpy(ydstr, buff);
  //MultiByteToWideChar(CP_ACP, 0, buff, strlen(buff), ydstr, len);
}

int vlist_export(HWND hList) {
  char filename[256];

  test_year_day_str(filename, sizeof(filename));

  char path[1024];
  sprintf(path, "./%s", filename);
  FILE *fp = fopen(path, "w");
  if (fp == NULL) {
    cout << "1111111" << "," << path << endl;
    return -1;
  }

  
  int itemcnt = ListView_GetItemCount(hList);
  int i = 0;
  cout << "itemcnt:" << itemcnt <<endl;
  for (i = 0; i < itemcnt; i++) {
    int j = 0;
    for (j = 0; j < 7; j++) {
      char buf[128];
      ListView_GetItemText(vlist_getwindow(hList), i,j, buf, sizeof(buf));
      fwrite(buf, strlen(buf), 1, fp);
      fwrite(" ", 1, 1, fp);
    }
    fwrite("\r\n", 2, 1, fp);
  }

  fclose(fp);
  return 0;
}
