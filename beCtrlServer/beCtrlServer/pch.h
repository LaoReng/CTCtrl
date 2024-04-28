// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头

// kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;

#include "framework.h"
#include <unknwn.h> // 需要通知
#include <windows.h>

#define  CLOSEAPP               (WM_USER + 1)       // 程序退出
#define  RESARTAPP              (WM_USER + 2)       // 程序重启
#define  WM_TRAYICON_NOTIFY		(WM_USER + 3)		// 系统托盘图标通知消息
#define  ONSETING               (WM_USER + 4)       // 打开设置面板
#define  ONCTRLCODE             (WM_USER + 5)       // 查看设备控制码

#endif //PCH_H
