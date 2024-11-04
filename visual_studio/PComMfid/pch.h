// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"

#include "afxwin.h"
#include "tchar.h"
#include "map"
#include <string>
#include <iostream>
#include <cstdio>

#include <sstream>
#include <iomanip>

// OS Specific sleep
#ifdef _WIN32
//#include <windows.h>
#else
#include <unistd.h>
#endif

#include "serial/serial.h"
#include "tinyxml.h"
#include <comdef.h>
using std::string;
using std::exception;
using std::vector;
using std::map;


class ILock
{
public:
    ILock() {}
    virtual ~ILock() {}
    virtual void Lock(void) = 0;
    virtual bool TryLock(void) = 0;
    virtual void UnLock(void) = 0;
};


class NLock : public ILock
{
public:
    NLock() {
        InitializeCriticalSection(&m_kSection);
    }
    virtual ~NLock() {
        DeleteCriticalSection(&m_kSection);
    }
    virtual void Lock(void) {
        EnterCriticalSection(&m_kSection);
    }
    virtual bool TryLock(void) {
        return (TryEnterCriticalSection(&m_kSection) > 0);
    }
    virtual void UnLock(void) {
        LeaveCriticalSection(&m_kSection);
    }
protected:
    CRITICAL_SECTION m_kSection;
};

void ShowTopMostMessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType) ;
BOOL run(CString szPosition, CString& cdCardNum, CString& csErrInfo);
BOOL CheckBCC(string src);
string GenerateBCC(string src);
string GetCardNum(string str);
#endif //PCH_H
