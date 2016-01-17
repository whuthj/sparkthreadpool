#pragma once

#include "threadpooltest.h"
#include "sparkthreadpool\sparktimer.hpp"
#include "sparkthreadpool\sparksharedptr.hpp"

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

// void OnTimer(UINT_PTR nIDEvent)
#define MSG_WM_TIMER(func) \
if (uMsg == WM_TIMER) \
{ \
    SetMsgHandled(TRUE); \
    func((UINT_PTR)wParam); \
    lResult = 0; \
    if(IsMsgHandled()) \
        return TRUE; \
}

class CMainDlg;
class CTestTaskRelease
{
public:
    CTestTaskRelease();
    CTestTaskRelease(CMainDlg* pDlg);
    virtual ~CTestTaskRelease();
    void TestDoAsync();
    void DoAsync1();
    void DoAsync2();
    void DoAsync3();
    void DoAsync4();
    void DoTest();
private:
    int* m_pTest;
    CMainDlg* m_pMainDlg;
};

class CTest : public SparkEnableSharedFromThis<CTest>
{
public:
    virtual ~CTest();
    void TestDoAsync();
    void DoAsync1();

private:
    SparkSharedPtr<CTest> GetSelfSharedPtr();
};

class Woman;

class Man{  
private:  
    SparkWeakPtr<Woman> _wife;  
public:  
    void setWife(SparkSharedPtr<Woman> woman){  
        _wife = woman;  
    }  

    void doSomthing();  

    ~Man(){  
        
    }  
};  

class Woman{  
private:  
    SparkWeakPtr<Man> _husband;  
public:  
    void setHusband(SparkSharedPtr<Man> man){  
        _husband = man;
    }  

    void doSomthing(){  
        
    }  

    ~Woman(){  
        
    }  
};  

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

public:
    void PrintText(LPCTSTR format, ...);

public:
    SPARK_ASYN_API(DoInWorkThread, int, a, int, b)
    void DoInWorkThread(int a, int b);

    SPARK_ASYN_API(DoInMainThread, CString, strText)
    void DoInMainThread(CString strText);

    void DoAsync();
    void DoAsyncEx(SparkWeakPtr<CTestTaskRelease> param);
    void DoAsyncEx_1(SparkSharedPtr<CTestTaskRelease> param);

    SPARK_ASYN_API(DoAsyncEx_2, SparkSharedPtr<CTestTaskRelease>, testTaskRelease)
    void DoAsyncEx_2(SparkSharedPtr<CTestTaskRelease> testTaskRelease);

    SPARK_ASYN_API(DoAsyncEx_3)
    void DoAsyncEx_3();

    void DoAsyncEx_4(double value);
    void DoAsyncEx_5(SparkSharedPtr<CTest> param);

    void DoSendMsgToMainThread(void* lpParam);
    void DoPostMsgToMainThread(void* lpParam);

    void DoTimerLog();
    void DoTimer();
    void DoDelay(int value);
    void DoFunction(int a, float b, double c);

private:
    SparkWndTimer m_timerLog;
    SparkWndTimer m_timer;

};