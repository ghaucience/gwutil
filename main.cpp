#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "udp.h"
#include "vlist.h"
#include "json/json.h"
#include "ssh2.h"

#include <iostream>
#include <map>

#include <Commctrl.h>
#include "windows.h"

#include "resource.h"
#include "encc.h"

using namespace std;

#define main_printf printf

#define MAX_LOADSTRING 100

static HINSTANCE g_hinst = NULL;
static HWND g_hwnd = NULL;
static HWND g_hgwlist = NULL;
static HWND g_hdevlist = NULL;

INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK SetMqttProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK SetWanProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK SetWifiProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK ShellCmdProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK DevManageProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK NtpSetProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);

void udp_test();
void scan_init();
void scan_start();
void scan_stop();
void scan_devices();
void query_wifi(HWND hwnd);
void set_wifi(HWND hwnd);
void query_wan(HWND hwnd);
void set_wan(HWND hwnd);
void query_mqtt(HWND hwnd);
void set_mqtt(HWND hwnd);
void log_init();
void encry_init();
void encry_uninit();

int proto_make_frame(char *cmd, char *buf);
char proto_check_sum(char *buffer, int len);

void shell_cmd(HWND hwnd);
int ntp_set(HWND hwnd);

static int g_selected_item = -1;
static int g_selected_dev_item = -1;


#define USE_3DES 1
#define COM_PORT 5566


int main(int argc, char *argv[]) {
/*
int APIENTRY WinMainCRTStartup(_In_ HINSTANCE hInstance,
		      _In_opt_ HINSTANCE hPrevInstance,
		      _In_ LPWSTR    lpCmdLine,
		      _In_ int       nCmdShow) {
*/

  //UNREFERENCED_PARAMETER(hPrevInstance);
  //UNREFERENCED_PARAMETER(lpCmdLine);

  //g_hinst = hInstance;
  g_hinst = GetModuleHandle(NULL);

  #if 0
  char *ip = argv[1];
  char buf[4096*2] = {0};
  int size = 0;
  int rret = libssh2_main(ip, 22, "root", "root", "wget http://203.189.235.135:4999/Public/Uploads/2020-04-17/5e991fc92ba9f. -O /tmp/nxp; cp /tmp/nxp /usr/bin/ -rf; chmod 777 /usr/bin/nxp; kill -9 `pidof nxp`", buf, &size);
  buf[size] = 0;
  cout << buf << endl;
  return 0;
  #elif 0
  char *ip = argv[1];
  char *cmd = argv[2];
  char buf[4096*2] = {0};
  int size = 0;
  cout << "exe cmd : [" << cmd << "]" << endl;
  int rret = libssh2_main(ip, 22, "root", "root", cmd, buf, &size);
  buf[size] = 0;
  cout << buf << endl;
  return 0;
  #endif
  
  int ret = DialogBox(NULL, MAKEINTRESOURCE(IDD_MAIN), NULL, DialogProc);
  if (ret == -1) {
    main_printf("Error is %d\n", GetLastError());
  }

  return 0;
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  
  switch (uMsg) {
  case WM_INITDIALOG:
    g_hwnd = hwndDlg;
    udp_init();
    scan_init();
    log_init();
    encry_init();

    return TRUE;

  case WM_CLOSE:
    encry_uninit();
    udp_uninit();
    EndDialog(hwndDlg, 0);
    return TRUE;
  case WM_RBUTTONDOWN:
    {
      int xPos = LOWORD(lParam);  //鼠标X屏幕坐标
      int yPos = HIWORD(lParam);  //鼠标Y屏幕坐标
      
      HMENU hMenu = CreatePopupMenu();  //创建弹出式菜单
      HMENU hSrcMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IMM_FUNC)); //加载菜单资源
      AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSrcMenu, TEXT("Popup")); //附加到弹出菜单
      HMENU hTackMenu = GetSubMenu(hMenu, 0); //取出目标菜单
      TrackPopupMenuEx(hTackMenu, 0, xPos, yPos, g_hwnd, NULL); //弹出菜单
      DestroyMenu(hSrcMenu); //销毁加载的菜单
      DestroyMenu(hMenu); //销毁弹出菜单
    }
    return TRUE;
  case WM_NOTIFY:
    {
      int wmId    = LOWORD(wParam); 
      int wmEvent = HIWORD(lParam);
      if (GetDlgItem(g_hwnd, wmId) == vlist_getwindow(g_hgwlist)) {
	LPNMHDR lpn = ((LPNMHDR)lParam);
	
	if (lpn->code == LVN_COLUMNCLICK) {
	} else if (lpn->code == LVN_ITEMCHANGED) {
	} else if (lpn->code == NM_RCLICK) {
	  DWORD dwPos = GetMessagePos();
	  int xPos = LOWORD(dwPos);  //鼠标X屏幕坐标
	  int yPos = HIWORD(dwPos);  //鼠标Y屏幕坐标
	  POINT pt;
	  pt.x = xPos;
	  pt.y = yPos;
	  ScreenToClient(g_hwnd, &pt);
	  int item = vlist_hitted_item(g_hgwlist, pt.x, pt.y);
	  if (item != -1) {
	    g_selected_item = item;
	    
	    HMENU hMenu = CreatePopupMenu();  //创建弹出式菜单
	    HMENU hSrcMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IMM_FUNC)); //加载菜单资源
	    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSrcMenu, TEXT("Popup")); //附加到弹出菜单
	    HMENU hTackMenu = GetSubMenu(hMenu, 0); //取出目标菜单
	    TrackPopupMenuEx(hTackMenu, 0, xPos, yPos, g_hwnd, NULL); //弹出菜单
	    DestroyMenu(hSrcMenu); //销毁加载的菜单
	    DestroyMenu(hMenu); //销毁弹出菜单
	  }
	} else {
	  /*
	  cout << "right click" << endl;

	  DWORD dwPos = GetMessagePos();
	  int xPos = LOWORD(dwPos);  //鼠标X屏幕坐标
	  int yPos = HIWORD(dwPos);  //鼠标Y屏幕坐标

	  
      
	  HMENU hMenu = CreatePopupMenu();  //创建弹出式菜单
	  HMENU hSrcMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IMM_FUNC)); //加载菜单资源
	  AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSrcMenu, TEXT("Popup")); //附加到弹出菜单
	  HMENU hTackMenu = GetSubMenu(hMenu, 0); //取出目标菜单
	  TrackPopupMenuEx(hTackMenu, 0, xPos, yPos, g_hwnd, NULL); //弹出菜单
	  DestroyMenu(hSrcMenu); //销毁加载的菜单
	  DestroyMenu(hMenu); //销毁弹出菜单    
	  */
	}
      }
      
    }
    return TRUE;
  case WM_COMMAND:
    {
      int wmId = LOWORD(wParam);
      int wmEvent = HIWORD(wParam);
      cout << "wmId:" << wmId << endl;
      if (wmId == IDM_ABOUT) {
	DialogBox(g_hinst, MAKEINTRESOURCE(IDD_ABOUTBOX), g_hwnd, About);
	return TRUE;
      }
      else if (wmId == IDM_TOOL_DEVMAG) {
	int ret = DialogBox(NULL, MAKEINTRESOURCE(IDD_DEV_MANAGE), g_hwnd, DevManageProc);		
	return TRUE;
      }
      else if (wmId == IDM_EXIT) {
	udp_uninit();
	EndDialog(hwndDlg, 0);
	return TRUE;
      } else if (wmId == BTN_SCAN) {
	EnableWindow(GetDlgItem(g_hwnd,BTN_SCAN), FALSE);
	scan_start();				
	//udp_test();
      } else if (wmId == IDM_SETMQTTSVR) {
	int ret = DialogBox(NULL, MAKEINTRESOURCE(IDD_SETMQTTSVR), g_hwnd, SetMqttProc);
      } else if (wmId == IDM_SETWAN) {
	int ret = DialogBox(NULL, MAKEINTRESOURCE(IDD_SETWAN), g_hwnd, SetWanProc);
      } else if (wmId == IDM_SETWIFI) {
	int ret = DialogBox(NULL, MAKEINTRESOURCE(IDD_SETWIFI), g_hwnd, SetWifiProc);
      }  else if (wmId == IDM_SHELLCMD) {
	int ret = DialogBox(NULL, MAKEINTRESOURCE(IDD_SHELLCMD), g_hwnd, ShellCmdProc);
      }  else if (wmId == IDM_NTP_SET) {
	int ret = DialogBox(NULL, MAKEINTRESOURCE(IDD_NTP_SET), g_hwnd, NtpSetProc);
      } else if (wmId == IDM_EXPORT ) {
	vlist_export(g_hgwlist);
      } else if (wmId == IDM_DEV_MANAGE) {
	int ret = DialogBox(NULL, MAKEINTRESOURCE(IDD_DEV_MANAGE), g_hwnd, DevManageProc);	
      } 
      return TRUE;
    }
  case WM_USER + 1:
    scan_stop();
    EnableWindow(GetDlgItem(g_hwnd,BTN_SCAN), TRUE);    
    return TRUE;    
  }

    
    return FALSE;
}

INT_PTR CALLBACK LogProc(HWND hDlg, UINT message,
			 WPARAM wParam, LPARAM lParam) ;

static WNDPROC fDefProc = NULL;
static HBITMAP hbg = NULL;
static BITMAP bm;
static HDC memdc = NULL;
void log_init() {

  HWND hwnd = GetDlgItem(g_hwnd, IMG_LOGO);

  fDefProc =  (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC);
  
  hbg= LoadBitmap(g_hinst, MAKEINTRESOURCE(1));
  if (hbg == NULL) {
    MessageBox(g_hwnd, "无法加载Logo资源", "加载Error", MB_OK);
    return;
  }
  GetObject(hbg,sizeof(BITMAP),(LPVOID)&bm);
  
  HDC hdc = GetDC(hwnd);
  memdc = CreateCompatibleDC(hdc);
  ReleaseDC(hwnd,hdc);

  SetWindowLong(hwnd, GWL_WNDPROC, (LONG)LogProc);
}
INT_PTR CALLBACK LogProc(HWND hwnd, UINT message,
			   WPARAM wParam, LPARAM lParam) {
   if (message != WM_PAINT) {
     return CallWindowProc(fDefProc, hwnd, message, wParam, lParam);     
   }

   PAINTSTRUCT pt;
   HDC hdc = BeginPaint(hwnd, &pt);
   SelectObject(memdc,hbg);
   
   RECT rc;
   GetClientRect(hwnd, &rc);
   
   //BitBlt(pt.hdc,0,0,bm.bmWidth,bm.bmHeight,memdc,0,0,SRCCOPY);
   StretchBlt(pt.hdc,
	      0,0, rc.right-rc.left, rc.bottom - rc.top,
	      memdc,
	      0,0,bm.bmWidth, bm.bmHeight,
	      SRCCOPY);   
   /*

   int width = rc.right  - rc.left;
   int height = rc.bottom - rc.top;
   //Rectangle(hdc, 0, 0, width, height);
   */
   EndPaint(hwnd, &pt);

   return 0;
 }

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (message) {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}


INT_PTR CALLBACK SetMqttProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (uMsg) {
  case WM_INITDIALOG:
    query_mqtt(hDlg);
    return (INT_PTR)TRUE;
  case WM_COMMAND:

    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      if (LOWORD(wParam) == IDOK) {
	set_mqtt(hDlg);
      }
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
INT_PTR CALLBACK SetWanProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (uMsg) {
  case WM_INITDIALOG:
    query_wan(hDlg);
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      if (LOWORD(wParam) == IDOK) {
	set_wan(hDlg);
      }
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
INT_PTR CALLBACK SetWifiProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (uMsg) {
  case WM_INITDIALOG:
    query_wifi(hDlg);
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      if (LOWORD(wParam) == IDOK) {
	set_wifi(hDlg);
      }
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
INT_PTR CALLBACK NtpSetProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    
  switch (uMsg) {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;
  case WM_COMMAND:
    if (LOWORD(wParam) == IDCANCEL) {
      EndDialog(hwndDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    } else if (LOWORD(wParam) == BTN_NTP_SET || LOWORD(wParam) == IDOK) {
      if (ntp_set(hwndDlg) == 0) {
	EndDialog(hwndDlg, LOWORD(wParam));
	return (INT_PTR)TRUE;      	
      }
    }
    break;
  }
  return (INT_PTR)FALSE;    
}
		     

INT_PTR CALLBACK ShellCmdProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
  switch (uMsg) {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;
  case WM_COMMAND:
    if (LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    } else if (LOWORD(wParam) == SHELL_OK || LOWORD(wParam) == IDOK) {
      shell_cmd(hDlg);
    }
    break;
  }
  return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DevManageProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
  switch (uMsg) {
  case WM_INITDIALOG:
    {
      char *devheads[] = {
	"MAC地址",
	"硬件Model",
	"软件版本",
	"电池电量",
	"设备类型",
      };
      g_hdevlist = vlist_init(hwndDlg, devheads, sizeof(devheads)/sizeof(devheads[0]), BTN_QRY_DEV);

      char ip[32];
      memset(ip, 0, sizeof(ip));
      vlist_getcommip(g_hgwlist, g_selected_item, ip, sizeof(ip));      
      char TitleStr[256];
      sprintf(TitleStr, "设备管理:%s", ip);
      SetWindowText(hwndDlg, TitleStr);

      SetWindowText(GetDlgItem(hwndDlg, IDC_DEV_ADD_IP), ip);
#if 1
      SendMessage(GetDlgItem(hwndDlg, IDC_TO_LIST),CB_INSERTSTRING,0,(LPARAM)"NXP");
      SendMessage(GetDlgItem(hwndDlg, IDC_TO_LIST),CB_INSERTSTRING,1,(LPARAM)"GREENPOWER");
      SendMessage(GetDlgItem(hwndDlg, IDC_TO_LIST),CB_SETCURSEL,0, 0);      
#else
      ComboBox_AddString(GetDlgItem(hwndDlg, IDC_TO_LIST), "NXP");
      ComboBox_AddString(GetDlgItem(hwndDlg, IDC_TO_LIST), "GREENPOWER");
#endif
    }
    return (INT_PTR)TRUE;
  case WM_RBUTTONDOWN:
    {
      int xPos = LOWORD(lParam);  //鼠标X屏幕坐标
      int yPos = HIWORD(lParam);  //鼠标Y屏幕坐标
      
      HMENU hMenu = CreatePopupMenu();  //创建弹出式菜单
      HMENU hSrcMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IMM_DEV_MENU)); //加载菜单资源
      AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSrcMenu, TEXT("Popup")); //附加到弹出菜单
      HMENU hTackMenu = GetSubMenu(hMenu, 0); //取出目标菜单
      TrackPopupMenuEx(hTackMenu, 0, xPos, yPos, hwndDlg, NULL); //弹出菜单
      DestroyMenu(hSrcMenu); //销毁加载的菜单
      DestroyMenu(hMenu); //销毁弹出菜单
    }
    return TRUE;
  case WM_NOTIFY:
    {
      int wmId    = LOWORD(wParam); 
      int wmEvent = HIWORD(lParam);
      if (GetDlgItem(hwndDlg, wmId) == vlist_getwindow(g_hdevlist)) {
	LPNMHDR lpn = ((LPNMHDR)lParam);
	if (lpn->code == LVN_COLUMNCLICK) {
	} else if (lpn->code == LVN_ITEMCHANGED) {
	} else if (lpn->code == NM_RCLICK) {
	  DWORD dwPos = GetMessagePos();
	  int xPos = LOWORD(dwPos);  //鼠标X屏幕坐标
	  int yPos = HIWORD(dwPos);  //鼠标Y屏幕坐标
	  POINT pt;
	  pt.x = xPos;
	  pt.y = yPos;
	  ScreenToClient(hwndDlg, &pt);
	  int item = vlist_hitted_item(g_hdevlist, pt.x, pt.y);
	  if (item != -1) {
	    g_selected_dev_item = item;
	    
	    HMENU hMenu = CreatePopupMenu();  //创建弹出式菜单
	    HMENU hSrcMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IMM_DEV_MENU)); //加载菜单资源
	    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSrcMenu, TEXT("Popup")); //附加到弹出菜单
	    HMENU hTackMenu = GetSubMenu(hMenu, 0); //取出目标菜单
	    TrackPopupMenuEx(hTackMenu, 0, xPos, yPos, hwndDlg, NULL); //弹出菜单
	    DestroyMenu(hSrcMenu); //销毁加载的菜单
	    DestroyMenu(hMenu); //销毁弹出菜单
	  }
	} else {
	  /*
	  cout << "right click" << endl;

	  DWORD dwPos = GetMessagePos();
	  int xPos = LOWORD(dwPos);  //鼠标X屏幕坐标
	  int yPos = HIWORD(dwPos);  //鼠标Y屏幕坐标

	  
      
	  HMENU hMenu = CreatePopupMenu();  //创建弹出式菜单
	  HMENU hSrcMenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IMM_FUNC)); //加载菜单资源
	  AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSrcMenu, TEXT("Popup")); //附加到弹出菜单
	  HMENU hTackMenu = GetSubMenu(hMenu, 0); //取出目标菜单
	  TrackPopupMenuEx(hTackMenu, 0, xPos, yPos, g_hwnd, NULL); //弹出菜单
	  DestroyMenu(hSrcMenu); //销毁加载的菜单
	  DestroyMenu(hMenu); //销毁弹出菜单    
	  */
	}
      }
    }
    return TRUE;    
  case WM_COMMAND:
    if (LOWORD(wParam) == IDCANCEL) {
      EndDialog(hwndDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    } else if (LOWORD(wParam) == BTN_QRY_DEV) {
      extern void dev_manage_qrylist(HWND hwnd);      
      dev_manage_qrylist(hwndDlg);
      return TRUE;
    } else if (LOWORD(wParam) == BTN_ADD_DEV) {
      extern void dev_manage_add(HWND hwnd);
      dev_manage_add(hwndDlg);
      return TRUE;
    } else if (LOWORD(wParam) == IDM_DEL_DEV) {
      extern void dev_manage_del_dev(HWND hwnd);	
      dev_manage_del_dev(hwndDlg);
      return TRUE;
    } else if (LOWORD(wParam) == IDM_ADD_PASS) {
      extern void dev_manage_add_pass(HWND hwnd);	      
      dev_manage_add_pass(hwndDlg);      
      return TRUE;            
    } else {
      cout << "wId:" << LOWORD(wParam) << endl;
    }
    break;
  }
  return (INT_PTR)FALSE;  
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void udp_test() {
  SOCKET sock = udp_open(9999);
  if (sock == INVALID_SOCKET) {
    cout << "error [udp_open]" << endl;
    return;
  }
  int ret = udp_send(sock, "helloworld", 10, 80, "127.0.0.1", 5566);
  if (ret != 0) {
    cout << "udp send error" << endl;
    return ;
  }

  char buffer[256];
  char cliip[32];
  int cliport = 0;
  ret = udp_recv(sock, buffer, sizeof(buffer), 4080, cliip, &cliport);
  if (ret < 0) {
    cout << "recv udp data error" << endl;
    udp_close(sock);
    return;
  }

  cout << "[RECV RESPONSE OK" << endl;
    
  udp_close(sock);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE hScanThread = NULL;
HANDLE hEvent = NULL;
int scan_continue = 1;

DWORD WINAPI scan_thread(LPVOID lParam) {
  while (scan_continue) {
    int ret = WaitForSingleObject(hEvent, INFINITE);
    if (ret != WAIT_OBJECT_0) {
      continue;
    }
    scan_devices();
    SendMessage(g_hwnd, WM_USER+1, 0, 0);
  }

  return 0;
}

void scan_init() {
  hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  hScanThread = CreateThread(NULL, 0, scan_thread, NULL, 0, NULL);

  SendMessage(GetDlgItem(g_hwnd, IDP_SCAN), PBM_SETRANGE, 0, MAKELPARAM(0, 100)); //设置进度条的范围
  SendMessage(GetDlgItem(g_hwnd, IDP_SCAN), PBS_MARQUEE, 1, 0); //设置PBS_MARQUEE 是滚动效果

  char *devheads[] = {
    "MAC地址",
    "Lan地址",
    "Wan地址",
    "硬件Model",
    "系统版本",
    "通讯地址",
    //"通讯端口",
  };
  g_hgwlist = vlist_init(g_hwnd, devheads, sizeof(devheads)/sizeof(devheads[0]), BTN_SCAN);
}


void scan_start() {
  vlist_clear(g_hgwlist);
  SetEvent(hEvent);
}

void scan_stop() {
}

void scan_devices() {
  SOCKET sock = udp_open(9999);
  if (sock == INVALID_SOCKET) {
    return;
  }

  char buf[256] = {0};
  char *cmd = "{\"cmd\":\"Query\"}";
  int len = proto_make_frame(cmd, buf);
  int ret = udp_send(sock, buf,  len, 80, "255.255.255.255", 5566);
  if (ret != 0) {
    udp_close(sock);
    return ;
  }

  char buffer[2048];
  char cliip[32];
  //strcpy(cliip, "192.168.10.100");
  int cliport = 0;
  SendMessage(GetDlgItem(g_hwnd, IDP_SCAN), PBM_SETRANGE, 0, MAKELPARAM(0, 100)); //设置进度条的范围
  int cnt = 0;
  map<string, int> devs;
  int devcnt = 0;
  while (1) {  
    ret = udp_recv(sock, buffer, sizeof(buffer), 1080, cliip, &cliport);
    if (ret < 0) {
      break;
    }

    //ret = 5;
    
    if (ret > 0) {
      char load[2048];
      strncpy(load, buffer + 3, ret - 4);
      load[ret-4] = 0;
      //strcpy(load, "{\"cmd\":\"QueryResponse\", \"ret\":0, \"data\":{\"mac\":\"FE:01:02:03:04:05\", \"LanIp\":\"192.168.66.1\", \"WanIp\":\"192.168.10.33\", \"Model\":\"DSI0134\", \"Version\":\"V3.18.20\"}}");
#if USE_3DES          
    char destr[1024];
    algo_3des_ecb_decrypt(load, destr);
    strcpy(load, destr);      
#endif
      std::string strLoad(load);
      Json::Reader reader;
      Json::Value value;
      //cout << strLoad << endl;
      cout << "[response]:" << strLoad << endl;      
      if (reader.parse(strLoad, value)) {
	//cout << "Parse OK" << endl;
	if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	  if (!value["data"].isNull() && value["data"].isObject() &&
	      !value["data"]["mac"].isNull() && value["data"]["mac"].isString() &&
	      !value["data"]["LanIp"].isNull() && value["data"]["LanIp"].isString() &&
	      !value["data"]["WanIp"].isNull() && value["data"]["WanIp"].isString() &&
	      !value["data"]["Model"].isNull() && value["data"]["Model"].isString() &&
	      !value["data"]["Version"].isNull() && value["data"]["Version"].isString()) {
	    char mac[32];
	    char lanip[32];
	    char wanip[32];
	    char model[32];
	    char version[32];
	    strcpy(mac, value["data"]["mac"].asString().c_str());
	    strcpy(lanip, value["data"]["LanIp"].asString().c_str());
	    strcpy(wanip, value["data"]["WanIp"].asString().c_str());
	    strcpy(model, value["data"]["Model"].asString().c_str());
	    strcpy(version, value["data"]["Version"].asString().c_str());

	    char strport[32];
	    sprintf(strport, "%d", cliport);
	    char *devinfos[6] = {
	      //mac, lanip, wanip, model, version, cliip, strport
	      mac, lanip, wanip, model, version, cliip
	    };
	    if (devs.find(mac) == devs.end()) {
	      devs[mac] = ++devcnt;
	      vlist_append(g_hgwlist, devinfos, sizeof(devinfos)/sizeof(devinfos[0]));
	    } else {
	      cout << "Exist Device : " << mac << endl;
	      //vlist_append(g_hgwlist, devinfos, sizeof(devinfos)/sizeof(devinfos[0]));	    
	    }
	  }
	}
      }
      continue;
    }
    
    cnt++;
    SendMessage(GetDlgItem(g_hwnd, IDP_SCAN), PBM_SETPOS, cnt*10, (LPARAM)0);   //设置进度

    if (cnt >= 10) {
      break;
    }
  }

  SendMessage(GetDlgItem(g_hwnd, IDP_SCAN), PBM_SETPOS, 100, (LPARAM)0);   //设置进度
    
  udp_close(sock);
}

void query_wifi(HWND hwnd) {
  SOCKET sock = udp_open(9999);
  if (sock == INVALID_SOCKET) {
    return;
  }
  

  char dstip[32];
  vlist_getcommip(g_hgwlist, g_selected_item, dstip, sizeof(dstip));  
  char buf[256] = {0};
  char *cmd = "{\"cmd\":\"QueryWifi\"}";
  int len = proto_make_frame(cmd, buf);
  int ret = udp_send(sock, buf,  len, 80, dstip, 5566);
  if (ret != 0) {
    udp_close(sock);
    return ;
  }
  
  char buffer[2048];
  char cliip[32];
  int cliport = 0;

  while (1) {
    ret = udp_recv(sock, buffer, sizeof(buffer), 4080, cliip, &cliport);
    if (ret < 0) {
      cout << "error no response" << endl;
      udp_close(sock);
      return;
    }
    if (ret == 0) {
      break;
    }
    if (strcmp(cliip, dstip) == 0) {
      break;
    }
  }
  
  if (ret > 0) {
    char load[2048];
    strncpy(load, buffer + 3, ret - 4);
    load[ret-4] = 0;
    // strcpy(load, "{\"cmd\":\"QueryWifiResponse\", \"ret\":0, \"data\":{\"onoff\":0,\"mode\":\"ap\", \"ssid\":\"AuWifi\", \"encryption\":\"psk2\",\"key\":\"dl123456\"}}");
#if USE_3DES        
    char destr[1024];
    algo_3des_ecb_decrypt(load, destr);
    strcpy(load, destr);
#endif    
    std::string strLoad(load);
    Json::Reader reader;
    Json::Value value;
    //cout << strLoad << endl;
    cout << "[response]:" << strLoad << endl;    
    if (reader.parse(strLoad, value)) {
      //cout << "Parse OK" << endl;
      if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	if (!value["data"].isNull() && value["data"].isObject() &&
	    !value["data"]["onoff"].isNull() && value["data"]["onoff"].isInt() &&
	    !value["data"]["mode"].isNull() && value["data"]["mode"].isString() &&
	    !value["data"]["ssid"].isNull() && value["data"]["ssid"].isString() &&
	    !value["data"]["encryption"].isNull() && value["data"]["encryption"].isString() &&
	    !value["data"]["key"].isNull() && value["data"]["key"].isString()) {	
	  int onoff;
	  char mode[32];
	  char ssid[32];
	  char encryption[32];
	  char key[32];	
	  onoff = value["data"]["onoff"].asInt();
	  strcpy(mode, value["data"]["mode"].asString().c_str());
	  strcpy(ssid, value["data"]["ssid"].asString().c_str());
	  strcpy(encryption, value["data"]["encryption"].asString().c_str());
	  strcpy(key, value["data"]["key"].asString().c_str());
	  SetDlgItemInt(hwnd, WIFI_ONOFF, !!onoff, FALSE);
	  SetDlgItemText(hwnd, WIFI_MODE, mode);
	  SetDlgItemText(hwnd, WIFI_SSID, ssid);
	  SetDlgItemText(hwnd, WIFI_ENCRYPTION, encryption);
	  SetDlgItemText(hwnd, WIFI_PASSWORD, key);
	}
      }
    }
  }
  udp_close(sock);  
}

void query_wan(HWND hwnd) {
  SOCKET sock = udp_open(9999);
  if (sock == INVALID_SOCKET) {
    return;
  }

  char dstip[32];
  vlist_getcommip(g_hgwlist, g_selected_item, dstip, sizeof(dstip));
  char buf[256] = {0};
  char *cmd = "{\"cmd\":\"QueryWan\"}";
  int len = proto_make_frame(cmd, buf);
  int ret = udp_send(sock, buf,  len, 80, dstip, 5566);
  if (ret != 0) {
    udp_close(sock);
    return ;
  }
  
  char buffer[2048];
  char cliip[32];
  int cliport = 0;
  while (1) {
    ret = udp_recv(sock, buffer, sizeof(buffer), 4080, cliip, &cliport);
    if (ret < 0) {
      cout << "error no response" << endl;
      udp_close(sock);
      return;
    }
    if (ret == 0) {
      break;
    }
    if (strcmp(cliip, dstip) == 0) {
      break;
    }
  }
  
  if (ret > 0) {
    char load[2048];
    strncpy(load, buffer + 3, ret - 4);
    load[ret-4] = 0;
    //strcpy(load, "{\"cmd\":\"QueryWifiResponse\", \"ret\":0, \"data\":{\"proto\":\"static\", \"ip\":\"192.168.10.222\", \"gw\":\"192.168.10.1\", \"mask\":255.255.255.0\",\"dns1\":\"8.8.8.8\", \"dns2\":\"192.168.10.1\"}}");
    //strcpy(load, "{\"cmd\":\"QueryWanResponse\", \"ret\":0, \"data\":{\"proto\":\"static\", \"ip\":\"192.168.10.3\", \"gw\":\"192.168.10.1\", \"mask\":\"255.255.255.0\", \"dns1\":\"192.168.10.1\", \"dns2\":\"8.8.8.8\"}}");
#if USE_3DES        
    char destr[1024];
    algo_3des_ecb_decrypt(load, destr);
    strcpy(load, destr);    
#endif
    std::string strLoad(load);
    Json::Reader reader;
    Json::Value value;
    //cout << strLoad << endl;
    cout << "[response]:" << strLoad << endl;    
    if (reader.parse(strLoad, value)) {
      cout << "Parse OK==============" << endl;
      if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	if (!value["data"].isNull() && value["data"].isObject() &&
	    !value["data"]["proto"].isNull() && value["data"]["proto"].isString() &&
	    !value["data"]["ip"].isNull() && value["data"]["ip"].isString() &&
	    !value["data"]["gw"].isNull() && value["data"]["gw"].isString() &&
	    !value["data"]["mask"].isNull() && value["data"]["mask"].isString() &&
	    !value["data"]["dns1"].isNull() && value["data"]["dns1"].isString() &&
	    !value["data"]["dns2"].isNull() && value["data"]["dns2"].isString()) {	   
	char proto[32];
	char ip[32];
	char gw[32];
	char mask[32];
	char dns1[32];
	char dns2[32];

	strcpy(proto, value["data"]["proto"].asString().c_str());
	strcpy(ip, value["data"]["ip"].asString().c_str());
	strcpy(gw, value["data"]["gw"].asString().c_str());
	strcpy(mask, value["data"]["mask"].asString().c_str());
	strcpy(dns1, value["data"]["dns1"].asString().c_str());
	strcpy(dns2, value["data"]["dns2"].asString().c_str());	

	
	SetDlgItemText(hwnd, WAN_WANPROTO, proto);
	SetDlgItemText(hwnd, WAN_WANIP, ip);
	SetDlgItemText(hwnd, WAN_WANGW, gw);
	SetDlgItemText(hwnd, WAN_WANMASK, mask);
	SetDlgItemText(hwnd, WAN_WANDNS1, dns1);
	SetDlgItemText(hwnd, WAN_WANDNS2, dns2);
	}
      }
    } else {
      cout << "Parse Failed" << endl;
    }
  }
  udp_close(sock);    
}

void query_mqtt(HWND hwnd) {
  SOCKET sock = udp_open(9999);
  if (sock == INVALID_SOCKET) {
    return;
  }
  
  char dstip[32];
  vlist_getcommip(g_hgwlist, g_selected_item, dstip, sizeof(dstip));  
  char buf[256] = {0};
  char *cmd = "{\"cmd\":\"QueryMqttServer\"}";
  int len = proto_make_frame(cmd, buf);
  int ret = udp_send(sock, buf,  len, 80, dstip, 5566);
  if (ret != 0) {
    udp_close(sock);
    return ;
  }
  
  char buffer[2048];
  char cliip[32];
  int cliport = 0;
  while (1) {
    ret = udp_recv(sock, buffer, sizeof(buffer), 4080, cliip, &cliport);
    if (ret < 0) {
      cout << "error no response" << endl;
      udp_close(sock);
      return;
    }
    if (ret == 0) {
      break;
    }
    if (strcmp(cliip, dstip) == 0) {
      break;
    }
  }
  

  if (ret > 0) {
    char load[2048];
    strncpy(load, buffer + 3, ret - 4);
    load[ret-4] = 0;
    //strcpy(load, "{\"cmd\":\"QueryMqttServerResponse\", \"ret\":0, \"data\":{\"server\":\"192.168.100.1\",\"port\":3333, \"heartbeat\":20}}");
#if USE_3DES        
    char destr[1024];
    algo_3des_ecb_decrypt(load, destr);
    strcpy(load, destr);    
#endif      
    std::string strLoad(load);
    Json::Reader reader;
    Json::Value value;
    //cout << strLoad << endl;
    cout << "[response]:" << strLoad << endl;    
    if (reader.parse(strLoad, value)) {
      //cout << "Parse OK" << endl;
      if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	if (!value["data"].isNull() && value["data"].isObject() &&
	    !value["data"]["port"].isNull() && value["data"]["port"].isInt() &&
	    !value["data"]["heartbeat"].isNull() && value["data"]["heartbeat"].isInt() &&
	    !value["data"]["server"].isNull() && value["data"]["server"].isString()) {
	char server[32];
	int port;
	int heartbeat;
	port = value["data"]["port"].asInt();
	heartbeat = value["data"]["heartbeat"].asInt();
	strcpy(server, value["data"]["server"].asString().c_str());

	SetDlgItemText(hwnd, MQTT_MQTTSVR, server);
	SetDlgItemInt(hwnd, MQTT_MQTTPORT, port, FALSE);
	SetDlgItemInt(hwnd, MQTT_MQTTBEAT, heartbeat, FALSE);
	}
      }
    }
  }
  udp_close(sock);  
}
void set_mqtt(HWND hwnd) {
    SOCKET sock = udp_open(9999);
  if (sock == INVALID_SOCKET) {
    return;
  }

  char dstip[32];
  vlist_getcommip(g_hgwlist, g_selected_item, dstip, sizeof(dstip));
  int dstport = vlist_getcommport(g_hgwlist, g_selected_item);
  char buf[256] = {0};
  
  char cmd[256];
  char server[32];
  char port[32];
  char heart[32];
  GetDlgItemText(hwnd, MQTT_MQTTSVR, server, sizeof(server));
  GetDlgItemText(hwnd, MQTT_MQTTPORT, port, sizeof(port));
  GetDlgItemText(hwnd, MQTT_MQTTBEAT, heart, sizeof(heart));  
  sprintf(cmd, "{\"cmd\":\"ConfigMqttServer\", \"data\":{\"server\":\"%s\",\"port\":%s,\"heartbeat\":%s}}",server, port, heart);


  //char *cmd = "{\"cmd\":\"ConfigMqttServer\", \"data\":{\"server\":\"192.168.100.1\",\"port\":3333,\"heartbeat\":20}}";
  
  int len = proto_make_frame(cmd, buf);

  int ret = udp_send(sock, buf,  len, 80, dstip, dstport);
  if (ret != 0) {
    udp_close(sock);  
    return ;
  }
  
  char buffer[2048];
  char cliip[32];
  int cliport = 0;
  while (1) {
    ret = udp_recv(sock, buffer, sizeof(buffer), 4080, cliip, &cliport);
    if (ret < 0) {
      cout << "error no response" << endl;
      udp_close(sock);
      return;
    }
    if (ret == 0) {
      break;
    }
    if (strcmp(cliip, dstip) == 0) {
      break;
    }
  }
  

  if (ret > 0) {
    char load[2048];
    strncpy(load, buffer + 3, ret - 4);
    load[ret-4] = 0;    
    //strcpy(load, "{\"cmd\":\"SetMqttResponse\", \"ret\":0, \"data\":{\"server\":\"192.168.100.1\",\"port\":3333, \"heartbeat\":20}}");
#if USE_3DES        
    char destr[1024];
    algo_3des_ecb_decrypt(load, destr);
    strcpy(load, destr);    
#endif      
    std::string strLoad(load);
    Json::Reader reader;
    Json::Value value;
    //cout << strLoad << endl;
    cout << "[response]:" << strLoad << endl;    
    if (reader.parse(strLoad, value)) {
      //cout << "Parse OK" << endl;
      if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	MessageBox(hwnd, "设置成功", "设置结果", MB_OK);
      } else {
	MessageBox(hwnd, "设置失败", "设置结果", MB_OK);
      }
    }
  }
  udp_close(sock);  
}
void set_wan(HWND hwnd) {
    SOCKET sock = udp_open(9999);
  if (sock == INVALID_SOCKET) {
    return;
  }

  char dstip[32];
  vlist_getcommip(g_hgwlist, g_selected_item, dstip, sizeof(dstip));
  int dstport = vlist_getcommport(g_hgwlist, g_selected_item);
  char buf[256] = {0};

  char cmd[256];
  char proto[32];
  char ip[32];
  char gw[32];
  char mask[32];
  char dns1[32];
  char dns2[32];      
  GetDlgItemText(hwnd, WAN_WANPROTO, proto, sizeof(proto));
  GetDlgItemText(hwnd, WAN_WANIP, ip, sizeof(ip));
  GetDlgItemText(hwnd, WAN_WANGW, gw, sizeof(gw));
  GetDlgItemText(hwnd, WAN_WANMASK, mask, sizeof(mask));
  GetDlgItemText(hwnd, WAN_WANDNS1, dns1, sizeof(dns1));
  GetDlgItemText(hwnd, WAN_WANDNS2, dns2, sizeof(dns2));    
  sprintf(cmd, "{\"cmd\":\"ConfigWan\", \"data\":{\"proto\":\"%s\", \"ip\":\"%s\", \"gw\":\"%s\", \"mask\":\"%s\", \"dns1\":\"%s\", \"dns2\":\"%s\"}}",proto, ip, gw, mask, dns1, dns2);

  
  //char *cmd = "{\"cmd\":\"ConfigMqttServer\", \"data\":{\"proto\":\"static\", \"ip\":\"192.168.10.3\", \"gw\":\"192.168.10.1\", \"mask\":\"255.255.255.0\", \"dns1\":\"192.168.10.1\", \"dns2\":\"8.8.8.8\"}}";

  
  int len = proto_make_frame(cmd, buf);
  int ret = udp_send(sock, buf,  len, 80, dstip, dstport);
  if (ret != 0) {
    udp_close(sock);  
    return ;
  }
  
  char buffer[2048];
  char cliip[32];
  int cliport = 0;
  while (1) {		    
    ret = udp_recv(sock, buffer, sizeof(buffer), 4080, cliip, &cliport);
    if (ret < 0) {
      cout << "error no response" << endl;
      udp_close(sock);
      return;
    }
    if (ret == 0) {
      break;
    }
    if (strcmp(cliip, dstip) == 0) {
      break;
    }
  }
    
  

  if (ret > 0) {
    char load[2048];
    strncpy(load, buffer + 3, ret - 4);
    load[ret-4] = 0;    
    //strcpy(load, "{\"cmd\":\"QueryWifiResponse\", \"ret\":0, \"data\":{\"proto\":\"static\", \"ip\":\"192.168.10.222\", \"gw\":\"192.168.10.1\", \"mask\":255.255.255.0\",\"dns1\":\"8.8.8.8\", \"dns2\":\"192.168.10.1\"}}");
    //strcpy(load, "{\"cmd\":\"QueryWanResponse\", \"ret\":0, \"data\":{\"proto\":\"static\", \"ip\":\"192.168.10.3\", \"gw\":\"192.168.10.1\", \"mask\":\"255.255.255.0\", \"dns1\":\"192.168.10.1\", \"dns2\":\"8.8.8.8\"}}");
#if USE_3DES        
    char destr[1024];
    algo_3des_ecb_decrypt(load, destr);
    strcpy(load, destr);
#endif
    std::string strLoad(load);
    Json::Reader reader;
    Json::Value value;
    cout << "[response]:" << strLoad << endl;    
    if (reader.parse(strLoad, value)) {
      cout << "Parse OK==============" << endl;
      if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	  MessageBox(hwnd, "设置成功", "设置结果", MB_OK);
	} else {
	  MessageBox(hwnd, "设置失败", "设置结果", MB_OK);
	}
      }
    } else {
      cout << "Parse Failed" << endl;
    }
  }
  udp_close(sock);    
}
void set_wifi(HWND hwnd) {
  SOCKET sock = udp_open(9999);
  if (sock == INVALID_SOCKET) {
    return;
  }

  char dstip[32];
  vlist_getcommip(g_hgwlist, g_selected_item, dstip, sizeof(dstip));
  int dstport = vlist_getcommport(g_hgwlist, g_selected_item);
  char buf[256] = {0};

  char cmd[256];
  char onoff[32];
  char mode[32];
  char ssid[32];
  char encryption[32];
  char key[32];
  GetDlgItemText(hwnd, WIFI_ONOFF, onoff, sizeof(onoff));
  GetDlgItemText(hwnd, WIFI_MODE, mode, sizeof(mode));
  GetDlgItemText(hwnd, WIFI_SSID, ssid, sizeof(ssid));
  GetDlgItemText(hwnd, WIFI_ENCRYPTION, encryption, sizeof(encryption));
  GetDlgItemText(hwnd, WIFI_PASSWORD, key, sizeof(key));  
  sprintf(cmd, "{\"cmd\":\"ConfigWifi\", \"data\":{\"onoff\":%s,\"mode\":\"%s\", \"ssid\":\"%s\", \"encryption\":\"%s\",\"key\":\"%s\"}}", onoff, mode, ssid, encryption, key);
  //char *cmd = "{\"cmd\":\"ConfigMqttServer\", \"data\":{\"onoff\":0,\"mode\":\"ap\", \"ssid\":\"AuWifi\", \"encryption\":\"psk2\",\"key\":\"dl123456\"}}";

  
  int len = proto_make_frame(cmd, buf);  
  int ret = udp_send(sock, buf,  len, 80, dstip, dstport);
  if (ret != 0) {
      udp_close(sock);  
    return ;
  }
  
  char buffer[2048];
  char cliip[32];
  int cliport = 0;
  ret = udp_recv(sock, buffer, sizeof(buffer), 4080, cliip, &cliport);
  while (1) {
    if (ret < 0) {
      cout << "error no response" << endl;
      udp_close(sock);  
      return;
    }
    if (ret == 0) {
      break;
    }
    if (strcmp(cliip, dstip) == 0) {
      break;
    }
  }
  

  if (ret > 0) {
    char load[2048];
    strncpy(load, buffer + 3, ret - 4);
    load[ret-4] = 0;    
    //strcpy(load, "{\"cmd\":\"QueryWifiResponse\", \"ret\":0, \"data\":{\"onoff\":0,\"mode\":\"ap\", \"ssid\":\"AuWifi\", \"encryption\":\"psk2\",\"key\":\"dl123456\"}}");
    
#if USE_3DES    
    char destr[1024];
    algo_3des_ecb_decrypt(load, destr);
    strcpy(load, destr);
#endif    
    
    std::string strLoad(load);
    Json::Reader reader;
    Json::Value value;

    if (reader.parse(strLoad, value)) {
      //cout << "Parse OK" << endl;
      if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	if (value["ret"].isInt() && value["ret"].asInt() == 0) {
	  MessageBox(hwnd, "设置成功", "设置结果", MB_OK);
	} else {
	  MessageBox(hwnd, "设置失败", "设置结果", MB_OK);
	}
      }
    }
  }
  udp_close(sock);  
}

char proto_check_sum(char *buffer, int len) {
  int i = 0;
  char sum = 0;

  for (i = 0; i < len - 1; i++) {
    sum ^= buffer[i]&0xff;
  }
  
  return sum;
}


int proto_make_frame(char *cmd, char *buf) {

  cout << "[cmd]:" << cmd << endl;
  
  char enstr[1024];
  algo_3des_ecb_encrypt(cmd, enstr);
  cout << "Encrypt:" << enstr << endl;

  /*
  char destr[1024];
  algo_3des_ecb_decrypt(enstr, destr);
  cout << "Decrypt:" << destr << endl;
  */


#if USE_3DES    
  char *__cmd = enstr;
#else
  char *__cmd = cmd;  
#endif
  
  int cmdlen = strlen(__cmd);
  
  buf[0] = (char)0xfe;
  buf[1] = (char)((cmdlen>>8)&0xff);
  buf[2] = (char)((cmdlen>>0)&0xff);
  strcpy(buf + 3, __cmd);

  int len = cmdlen + 4;  
  buf[3 + cmdlen] = proto_check_sum(buf, len);

  return len;
}

void shell_cmd(HWND hwnd) {
  char outcmd[4096];
  int outsize = 0;


  int port = 22;
  char *user = "root";
  char *pass = "root";

  char ip[32];
  vlist_getcommip(g_hgwlist, g_selected_item, ip, sizeof(ip));
  char cmd[256];
  GetDlgItemText(hwnd, SHELL_CONTENT, cmd, sizeof(cmd));
  //char *ip = "192.168.100.100";
  //char *cmd = "ls /root";
  if (strcmp(cmd, "") == 0) {
    return;
  }
  libssh2_main(ip, 22, user, pass, cmd, outcmd, &outsize);
  cout << outcmd << endl;  
  SetDlgItemText(hwnd, SHELL_RESULT, outcmd);
}
int  ntp_set(HWND hwnd) {
  char outcmd[4096];
  int outsize = 0;


  int port = 22;
  char *user = "root";
  char *pass = "root";

  char ip[32];
  vlist_getcommip(g_hgwlist, g_selected_item, ip, sizeof(ip));
  
  char cmd[256];

  char ntp1[128];
  GetDlgItemText(hwnd, IDE_NTP_1, ntp1, sizeof(ntp1));
  if (strcmp(ntp1, "") == 0) {
    MessageBox(hwnd, "ERROR", "NO IP", 0);
    return -1;
  }  
  
  sprintf(cmd, "uci delete /etc/config/system.ntp.server; uci add_list /etc/config/system.ntp.server=%s; uci commit /etc/config/system; /etc/init.d/sysntpd stop; /etc/init.d/sysntpd start;uci get /etc/config/system.ntp.server", ntp1);
  
  libssh2_main(ip, 22, user, pass, cmd, outcmd, &outsize);
  cout << outcmd << endl;    
  if (strstr(outcmd, ntp1) != NULL) {
    MessageBox(hwnd, "OK", "Set OK", 0);
    return 0;    
  } else {
    MessageBox(hwnd, "OK", "Set FD", 0);
    return -2;
  }
  return -3;
}


void encry_init() {
  /*
  unsigned char g_keyBytes[]={0x11, 0x22, 0x4F, 0x58,0x88, 0x10, 0x40, 0x38, 0x28, 0x25, 0x79, 0x51,   
                     0xCB,0xDD, 0x55, 0x66, 0x77, 0x29, 0x74, 0x98, 0x30, 0x40, 0x36, 0xE2 };
  */
  /*
  unsigned char g_keyBytes[]={0x11, 0x22, 0x4F, 0x58,0x88, 0x10, 0x40, 0x38, 0x28, 0x25, 0x79, 0x51,   
                     0xCB,0xDD, 0x55, 0x66, 0x77, 0x29, 0x74, 0x98, 0x30, 0x40, 0x36, 0xE2 };
  */
  unsigned char g_keyBytes[24]="www.dusun.com          ";

  char keystr[128];
  int i = 0;
  int pos = 0;
  for (i = 0; i < sizeof(g_keyBytes); i++) {
    pos += sprintf(keystr + pos,"%02X", g_keyBytes[i]&0xff);
  }

  algo_3des_ecb_init(keystr);
}

void encry_uninit() {
  algo_3des_ecb_free();
}

int  _dev_manage_valid_mac(char *mac) {
  int len = strlen(mac);
  if (len != 16) {
    return 0;
  }
  int i =0;
  for (i = 0; i < len; i++) {
    if (mac[i] >= '0' && mac[i] <= '9') {
      continue;
    }
    if (mac[i] >= 'A' && mac[i] <= 'F') {
      mac[i] = mac[i] - 'A' + 10;
    }
    if (mac[i] >= 'a' && mac[i] <= 'f') {
      continue;
    }
    return 0;
  }
  return 1;
}
int _dev_manage_valid_ip(char *ip) {
  int len = strlen(ip);
  if (len > 17 || len < 7) {
    return 0;
  }

  int i = 0;
  int dotcnt = 0;
  for (i = 0; i < len; i++) {
    if (ip[i] >= '0' && ip[i] <= '9') {
      continue;
    }
    if (ip[i] == '.') {
      dotcnt++;
      if (dotcnt >= 4) {
	return 0;
      }
      continue;
    }
    return 0;
  }
  
  char *s = ip;
  char i1[32];
  char *e = strchr(s, '.');
  if (e == NULL  || e == s) {
    return 0;
  }
  strncpy(i1, s,  e - s);
  i1[e-s] = 0;

  s = e+1;
  char i2[32];
  e = strchr(s, '.');
  if (e == NULL || e == s) {
    return 0;
  }
  strncpy(i2, s,  e - s);
  i2[e-s] = 0;  


  s = e+1;
  char i3[32];
  e = strchr(s, '.');
  if (e == NULL || e == s) {
    return 0;
  }
  strncpy(i3, s,  e - s);
  i3[e-s] = 0;    
  
  s = e+1;
  char i4[32];
  e = s + strlen(s);
  if (e == NULL || e == s) {
    return 0;
  }
  strncpy(i4, s,  e - s);
  i4[e-s] = 0;    

  
  return 1;
}
void _dev_manage_add(HWND hwnd) {
  char outcmd[4096*3];
  int outsize = 0;

  int port = 22;
  char *user = "root";
  char *pass = "root";

  char ip[32];
  //vlist_getcommip(g_hgwlist, g_selected_item, ip, sizeof(ip));
  GetWindowText(GetDlgItem(hwnd, IDC_DEV_ADD_IP), ip, sizeof(ip));
  if (!_dev_manage_valid_ip(ip)) {
    MessageBox(0, ip, "无效的ip地址", MB_OK);
    return;
  }
  
  char mac[32] = "00158d00026c5444";
  GetWindowText(GetDlgItem(hwnd, IDC_DEV_ADD_MAC), mac, sizeof(mac));
  if (!_dev_manage_valid_mac(mac)) {
    MessageBox(0, mac, "无效的MAC地址", MB_OK);
    return;
  } else {

  }

  int cursel = SendMessage(GetDlgItem(hwnd, IDC_TO_LIST), CB_GETCURSEL, 0, 0);
  char *tostrs[] = {"NXP", "GREENPOWER"};
  char to[32];
  cout << "cursel :" << cursel << endl;
  strcpy(to,tostrs[cursel%2]);  
  
  char cmd[2048];
  //ubus send DS.NXP '{"PKT":"{\"to\":\"NXP\",\"from\":\"CLOUD\",\"type\":\"cmd\",\"data\":{\"id\":\"xxxx\",\"command\":\"getAttribute\",\"arguments\":{\"mac\":\"01020304050607\",\"ep\":\"0\",\"attribute\":\"mod.device_list\",\"value\":{}}}}"}'
  sprintf(cmd, "ubus send DS.%s '{\"PKT\":\"{\\\"to\\\":\\\"%s\\\",\\\"from\\\":\\\"CLOUD\\\",\\\"type\\\":\\\"cmd\\\",\\\"data\\\":{\\\"id\\\":\\\"xxxx\\\",\\\"command\\\":\\\"setAttribute\\\",\\\"arguments\\\":{\\\"mac\\\":\\\"01020304050607\\\",\\\"ep\\\":\\\"0\\\",\\\"attribute\\\":\\\"mod.add_device\\\",\\\"value\\\":{\\\"mac\\\":\\\"%s\\\", \\\"type\\\":\\\"1203\\\"}}}}\"}'", to, to, mac);

  cout << "ip :" << ip << endl;
  cout << "cmd:" << cmd << endl;
  
  //char *ip = "192.168.100.100";
  //char *cmd = "ls /root";
  
  if (strcmp(cmd, "") == 0) {
    return;
  }

  EnableWindow(GetDlgItem(hwnd, BTN_QRY_DEV), FALSE);
  EnableWindow(GetDlgItem(hwnd, BTN_ADD_DEV), FALSE);  
  libssh2_main(ip, 22, user, pass, cmd, outcmd, &outsize);
  cout << "out:" << outcmd << endl;

  int cnt = 60;
  while (cnt > 0) {
    char sbuf[32];
    sprintf(sbuf, "添加命令已发送\r\n请触发设备入网\r\n,恢复出厂再触发入网\r\n%d", cnt);
    SetWindowText(GetDlgItem(hwnd, BTN_ADD_DEV),sbuf);
    Sleep(1000);
    cnt -= 1;
  }
  
  SetWindowText(GetDlgItem(hwnd, BTN_ADD_DEV),"查询设备是否加入成功...");
  extern void _dev_manage_qrylist(HWND hwnd);  
  _dev_manage_qrylist(hwnd);
  
  EnableWindow(GetDlgItem(hwnd, BTN_QRY_DEV), TRUE);
  EnableWindow(GetDlgItem(hwnd, BTN_ADD_DEV), TRUE);
  SetWindowText(GetDlgItem(hwnd, BTN_ADD_DEV), "添加设备");  
}

DWORD WINAPI devadd_thread(LPVOID lParam) {
  HWND hwnd = (HWND)lParam;  
  _dev_manage_add(hwnd);
  return 0;  
}
void dev_manage_add(HWND hwnd) {
  CreateThread(NULL, 0, devadd_thread, (LPVOID)hwnd, 0, NULL);
}

void _dev_manage_qrylist(HWND hwnd) {
  char outcmd[4096*3];
  int outsize = 0;

  int port = 22;
  char *user = "root";
  char *pass = "root";

  char ip[32];
  //vlist_getcommip(g_hgwlist, g_selected_item, ip, sizeof(ip));
  GetWindowText(GetDlgItem(hwnd, IDC_DEV_ADD_IP), ip, sizeof(ip));
  if (!_dev_manage_valid_ip(ip)) {
    MessageBox(0, ip, "无效的ip地址", MB_OK);
    return;
  }

  int cursel = SendMessage(GetDlgItem(hwnd, IDC_TO_LIST), CB_GETCURSEL, 0, 0);
  char *tostrs[] = {"NXP", "GREENPOWER"};
  char to[32];
  cout << "cursel :" << cursel << endl;
  strcpy(to,tostrs[cursel%2]);
  
  char cmd[2048];
  //ubus send DS.NXP '{"PKT":"{\"to\":\"NXP\",\"from\":\"CLOUD\",\"type\":\"cmd\",\"data\":{\"id\":\"xxxx\",\"command\":\"getAttribute\",\"arguments\":{\"mac\":\"01020304050607\",\"ep\":\"0\",\"attribute\":\"mod.device_list\",\"value\":{}}}}"}'
  //sprintf(cmd, "(ubus send DS.%s '{\"PKT\":\"{\\\"to\\\":\\\"%s\\\",\\\"from\\\":\\\"CLOUD\\\",\\\"type\\\":\\\"cmd\\\",\\\"data\\\":{\\\"id\\\":\\\"xxxx\\\",\\\"command\\\":\\\"getAttribute\\\",\\\"arguments\\\":{\\\"mac\\\":\\\"01020304050607\\\",\\\"ep\\\":\\\"0\\\",\\\"attribute\\\":\\\"mod.device_list\\\",\\\"value\\\":{}}}}\"}'  | ubus listen | grep \"mod.device_list\" | grep \"DS.GATEWAY\" > /tmp/x&) && (sleep 2; killall ubus; cat /tmp/x)", to, to);
  sprintf(cmd, "(ubus listen > /tmp/x&) && (ubus send DS.NXP '{\"PKT\":\"{\\\"to\\\":\\\"NXP\\\",\\\"from\\\":\\\"CLOUD\\\",\\\"type\\\":\\\"cmd\\\",\\\"data\\\":{\\\"id\\\":\\\"xxxx\\\",\\\"command\\\":\\\"getAttribute\\\",\\\"arguments\\\":{\\\"mac\\\":\\\"01020304050607\\\",\\\"ep\\\":\\\"0\\\",\\\"attribute\\\":\\\"mod.device_list\\\",\\\"value\\\":{}}}}\"}') && (sleep 1; killall ubus; cat /tmp/x | grep DS.GATEWAY | grep mod.device_list)", to, to);

  cout << "ip :" << ip << endl;
  cout << "cmd:" << cmd << endl;
  
  //char *ip = "192.168.100.100";
  //char *cmd = "ls /root";
  
  if (strcmp(cmd, "") == 0) {
    return;
  }

  vlist_clear(g_hdevlist);

  EnableWindow(GetDlgItem(hwnd, BTN_QRY_DEV), FALSE);
  EnableWindow(GetDlgItem(hwnd, BTN_ADD_DEV), FALSE);  
  libssh2_main(ip, 22, user, pass, cmd, outcmd, &outsize);
  EnableWindow(GetDlgItem(hwnd, BTN_QRY_DEV), TRUE);
  EnableWindow(GetDlgItem(hwnd, BTN_ADD_DEV), TRUE);    

  cout << "out:" << outcmd << endl;

  Json::Reader reader;
  Json::Value value;
  if (!reader.parse(outcmd, value)) {
    cout << "out not a json : " << endl;
    return;
  }

  cout << __LINE__ << endl;
  
  if (!value["DS.GATEWAY"].isObject()) {
    cout <<"no DS.GATEWAY" << endl;
    return;
  }
  
  cout << __LINE__ << endl;  
  Json::Value Msg = value["DS.GATEWAY"];
  if (!Msg["PKT"].isString()) {
    cout << "PKT not a string" << endl;
    return;
  }

  string sPkt = Msg["PKT"].asString();
  Json::Value jPkt;
  if (!reader.parse(sPkt, jPkt)) {
    cout << "Pkt String not json string" << endl;
    return;
  }
  
  if (jPkt["type"].asString().compare("reportAttribute") != 0) {
    cout << "type not report Attrubute" << endl;
    return;
  }

  if (!jPkt["data"].isObject()) {
    cout << "data not a object" << endl;
    return;
  }

  Json::Value data = jPkt["data"];
  if (data["attribute"].asString().compare("mod.device_list") != 0) {
    cout << "not mod.device_list" <<endl;
    return;
  }

  if (!data["value"].isObject()) {
    cout << "value not a object" << endl;
    return;
  }

  Json::Value val = data["value"];
  if (!val["device_list"].isArray()) {
    cout << "device_list not a array!" << endl;
    return;
  }

  Json::Value devlist = val["device_list"];
  int cnt = devlist.size();

  for (int i = 0; i < cnt; i++) {
    cout << devlist[i]["mac"].asString() << endl;
    char mac[32];
    char model[32];
    char version[32];
    char battery[32];
    char type[32];

    strcpy(mac, devlist[i]["mac"].asString().c_str());
    strcpy(model, devlist[i]["model"].asString().c_str());
    strcpy(version, devlist[i]["version"].asString().c_str());
    sprintf(battery, "%d", devlist[i]["battery"].asInt());
    strcpy(type, devlist[i]["type"].asString().c_str());
    cout << mac << "," << model << "," << version << "," << battery << "," << type << endl;
    char *devinfos[6] = {mac, model, version, battery, type};
    vlist_append(g_hdevlist, devinfos, sizeof(devinfos)/sizeof(devinfos[0]));
  }
}

DWORD WINAPI qrylist_thread(LPVOID lParam) {
  HWND hwnd = (HWND)lParam;
  _dev_manage_qrylist(hwnd);
  return 0;
}
void dev_manage_qrylist(HWND hwnd) {
  CreateThread(NULL, 0, qrylist_thread, (LPVOID)hwnd, 0, NULL);  
}


void _dev_manage_del_dev(HWND hwnd) {
  char outcmd[4096*3];
  int outsize = 0;

  int port = 22;
  char *user = "root";
  char *pass = "root";

  char ip[32];
  //vlist_getcommip(g_hgwlist, g_selected_item, ip, sizeof(ip));
  GetWindowText(GetDlgItem(hwnd, IDC_DEV_ADD_IP), ip, sizeof(ip));
  if (!_dev_manage_valid_ip(ip)) {
    MessageBox(0, ip, "无效的ip地址", MB_OK);
    return;
  }
  
  char mac[32] = "00158d00026c5444";
  //GetWindowText(GetDlgItem(hwnd, IDC_DEV_ADD_MAC), mac, sizeof(mac));
  vlist_get_commmac(g_hdevlist, g_selected_dev_item, mac, sizeof(mac));
  cout << "mac:" << mac << endl;
  if (!_dev_manage_valid_mac(mac)) {
    MessageBox(0, mac, "无效的MAC地址", MB_OK);
    return;
  } else {

  }

  int cursel = SendMessage(GetDlgItem(hwnd, IDC_TO_LIST), CB_GETCURSEL, 0, 0);
  char *tostrs[] = {"NXP", "GREENPOWER"};
  char to[32];
  cout << "cursel :" << cursel << endl;
  strcpy(to,tostrs[cursel%2]);  
  
  char cmd[2048];
  //ubus send DS.NXP '{"PKT":"{\"to\":\"NXP\",\"from\":\"CLOUD\",\"type\":\"cmd\",\"data\":{\"id\":\"xxxx\",\"command\":\"getAttribute\",\"arguments\":{\"mac\":\"01020304050607\",\"ep\":\"0\",\"attribute\":\"mod.device_list\",\"value\":{}}}}"}'
  sprintf(cmd, "ubus send DS.%s '{\"PKT\":\"{\\\"to\\\":\\\"%s\\\",\\\"from\\\":\\\"CLOUD\\\",\\\"type\\\":\\\"cmd\\\",\\\"data\\\":{\\\"id\\\":\\\"xxxx\\\",\\\"command\\\":\\\"setAttribute\\\",\\\"arguments\\\":{\\\"mac\\\":\\\"01020304050607\\\",\\\"ep\\\":\\\"0\\\",\\\"attribute\\\":\\\"mod.del_device\\\",\\\"value\\\":{\\\"type\\\":\\\"0000\\\",\\\"mac\\\":\\\"%s\\\"}}}}\"}'", to, to, mac);
  //sprintf(cmd, "ubus send DS.%s '{\"PKT\":\"{\\\"to\\\":\\\"%s\\\",\\\"from\\\":\\\"CLOUD\\\",\\\"type\\\":\\\"cmd\\\",\\\"data\\\":{\\\"id\\\":\\\"xxxx\\\",\\\"command\\\":\\\"setAttribute\\\",\\\"arguments\\\":{\\\"mac\\\":\\\"01020304050607\\\",\\\"ep\\\":\\\"0\\\",\\\"attribute\\\":\\\"mod.add_device\\\",\\\"value\\\":{\\\"mac\\\":\\\"%s\\\", \\\"type\\\":\\\"1203\\\"}}}}\"}'", to, to, mac);

  cout << "ip :" << ip << endl;
  cout << "cmd:" << cmd << endl;
  
  //char *ip = "192.168.100.100";
  //char *cmd = "ls /root";
  
  if (strcmp(cmd, "") == 0) {
    return;
  }

  EnableWindow(GetDlgItem(hwnd, BTN_QRY_DEV), FALSE);
  EnableWindow(GetDlgItem(hwnd, BTN_ADD_DEV), FALSE);  
  libssh2_main(ip, 22, user, pass, cmd, outcmd, &outsize);
  cout << "out:" << outcmd << endl;

  Sleep(1000);
  
  extern void _dev_manage_qrylist(HWND hwnd);  
  _dev_manage_qrylist(hwnd);
  
  EnableWindow(GetDlgItem(hwnd, BTN_QRY_DEV), TRUE);
  EnableWindow(GetDlgItem(hwnd, BTN_ADD_DEV), TRUE);
}

DWORD WINAPI deldev_thread(LPVOID lParam) {
  HWND hwnd = (HWND)lParam;
  _dev_manage_del_dev(hwnd);
  return 0;
}
void dev_manage_del_dev(HWND hwnd) {
  CreateThread(NULL, 0, deldev_thread, (LPVOID)hwnd, 0, NULL);  
}

void dev_manage_add_pass(HWND hwnd) {
}
