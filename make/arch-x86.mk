# compiler env
# ##################################################################
VSInstallDir		:= D:\Program Files\Microsoft Visual Studio 9.0
VCInstallDir		:= $(VSInstallDir)\VC
WindowsSdkDir	:= C:\Program Files\\Microsoft SDKs\Windows\v6.0A

#FrameworkDir		:= 
#FrameworkSdkDir	:=

PATH			:=$(VSInstallDir)\Common7\IDE;
PATH			+=$(VSInstallDir)\Common7\Tools;
PATH			+=$(VCInstallDir)\BIN;
PATH			+=$(VCInstallDir)\VCPackages;
PATH			+=$(WindowsSdkDir)\bin;
#PATH			+=$(FrameworkDir)\$(Framework35Version)
#PATH			+=$(FrameworkDir)\$(FrameworkVersion)
PATH			+=C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;
PATH			+=C:\Program Files\ATI Technologies\ATI.ACE\Core-Static;
PATH			+=C:\Program Files\Microsoft SQL Server\90\Tools\binn\
#PATH			:=PATH=D:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;D:\Program Files\Microsoft Visual Studio 9.0\VC\BIN;D:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools;C:\WINDOWS\Microsoft.NET\Framework\v3.5;C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;D:\Program Files\Microsoft Visual Studio 9.0\VC\VCPackages;C:\Program Files\\Microsoft SDKs\Windows\v6.0A\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Program Files\ATI Technologies\ATI.ACE\Core-Static;C:\Program Files\Microsoft SQL Server\90\Tools\binn\;F:\Program Files\emacs\bin
PATH			:=PATH=d:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;d:\Program Files\Microsoft Visual Studio 9.0\VC\BIN;d:\Program Files\Microsoft Visual Studio 9.0\Common7\Tools;C:\WINDOWS\Microsoft.NET\Framework\v3.5;C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;d:\Program Files\Microsoft Visual Studio 9.0\VC\VCPackages;C:\Program Files\\Microsoft SDKs\Windows\v6.0A\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\system32\WindowsPowerShell\v1.0;c:\Program Files\Microsoft SQL Server\90\Tools\binn\
# tools
# ##############################################################################
CC			:= cl
#CFLAGS		:= /Wall /c /nologo /O2
CFLAGS		:= /W3 /nologo /O2 /EHsc /D_CRT_SECURE_NO_WARNINGS /DJSON_IS_AMALGAMATION
CFLAGS		+= /I"$(VCInstallDir)\ATLMFC\INCLUDE"
CFLAGS		+= /I"$(VCInstallDir)\INCLUDE"
CFLAGS		+= /I"$(WindowsSdkDir)\include"
CFLAGS		+= /I"./src" /I"." /I"./libs/libxl" /I"./libs/json" /I"./libs/3des-ecb" /I"./libs/libssh2"

CPP			:= cl
CPPFLAGS		:= $(CFLAGS)

LD			:= link
#LDFLAGS		:= /nologo /SUBSYSTEM:CONSOLE /MACHINE:X86 /ENTRY:mainCRTStartup
#LDFLAGS		:= /nologo  /MACHINE:X86
LDFLAGS		:= /nologo  /MACHINE:X86  /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
LDFLAGS		+= /libpath:"$(VSInstallDir)"
LDFLAGS		+= /libpath:"$(VSInstallDir)\lib"
LDFLAGS		+= /libpath:"$(VCInstallDir)\LIB"
LDFLAGS		+= /libpath:"$(VCInstallDir)\atlmfc\LIB"
LDFLAGS		+= /libpath:"$(VCInstallDir)\atlmfc\LIB\i386"
LDFLAGS		+= /libpath:"$(WindowsSdkDir)\lib"
#LDFLAGS		+= /libpath:"$(FrameworkSdkDir)\lib"
#LDFLAGS		+= /libpath:"$(FrameworkSdKDir)\lib"
LDFLAGS		+= /libpath:"$(ROOTDIR)/libs/PCOMM"
LDFLAGS		+= /libpath:"$(ROOTDIR)/libs/libxl"
LDFLAGS		+= /libpath:"$(ROOTDIR)/libs/libssh2"
LDFLAGS		+= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib 
LDFLAGS		+= ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
#LDFLAGS    		+= PCOMM.lib
#LDFLAGS    		+= libxl.lib
LDFLAGS		+= libssh2.lib

CP			:= cp
CPFLAGS		:= -rf
RM			:= rm
RMFLAGS		:= -rf
RCC			:= rc
RCCFLAGS		:= /n /i"$(WindowsSdkDir)\Include"
RCCFLAGS		+= /i"$(VCInstallDir)\include"

CPPEXT		:= .cpp
CEXT			:= .c
HEXT			:= .h 
OEXT			:= .obj
RESEXT		:= .res
RCEXT			:= .rc
COMPILE		:= /c
CCOUT		:= /Fo
LDOUT		:= /OUT:
RCOUT		:= /fo

MKDIR		:= mmdir
MKDIRFLAGS		:= -p

