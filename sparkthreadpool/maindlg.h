#pragma once

#include "threadpooltest.h"

#define MY_COMMAND_HANDLER(id, code, func) \
if (uMsg == WM_COMMAND && id == LOWORD(wParam) && code == HIWORD(wParam)) \
{ \
    bHandled = TRUE; \
    lResult = func(bHandled); \
    if (bHandled) \
        return TRUE; \
}

#define MSG_WM_INITDIALOG(func) \
if (uMsg == WM_INITDIALOG) \
{ \
    SetMsgHandled(TRUE); \
    lResult = (LRESULT)func((HWND)wParam, lParam); \
    if (IsMsgHandled()) \
        return TRUE; \
}

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
    CMainDlg();
    ~CMainDlg();

    enum { IDD = IDD_DIALOG_MAIN };

    BEGIN_MSG_MAP(CMainDlg)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MSG_WM_INITDIALOG(OnInitDialog)
        MY_COMMAND_HANDLER(IDC_BUTTON_TEST, BN_CLICKED, OnBnClickedButtonTest)
    END_MSG_MAP()

protected:
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonTest(BOOL& /*bHandled*/);

    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

private:
    void PrintText(LPCTSTR format, ...);

private:
    SPARK_ASYN_API(DoInWorkThread, int, a, int, b)
    void DoInWorkThread(int a, int b);

    SPARK_ASYN_API(DoInMainThread, CString, strText)
    void DoInMainThread(CString strText);

};