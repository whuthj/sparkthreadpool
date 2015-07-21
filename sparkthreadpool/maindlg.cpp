#include "stdafx.h"
#include "maindlg.h"

CMainDlg::CMainDlg()
{
}

CMainDlg::~CMainDlg()
{
}

LRESULT CMainDlg::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 0;
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    SparkThreadPool::Instance().InitMsgWnd();

    PrintText(L"主线程ID：%d", ::GetCurrentThreadId());

    SetDlgItemInt(IDC_EDIT_NUM1, 100);
    SetDlgItemInt(IDC_EDIT_NUM2, 200);

    SetWindowText(L"SparkThreadPool测试");
    SetDlgItemText(IDC_BUTTON_TEST, L"异步执行");

    SetTimer(1, 1000);

    return TRUE;
}

LRESULT CMainDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    EndDialog(0);

    return 0;
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (1 == nIDEvent)
    {
        CString strInfo;
        strInfo.Format(L"线程池运行线程数：%d，任务数：%d，回收站线程数：%d", SparkThreadPool::Instance().GetThreadCount(),
            SparkThreadPool::Instance().GetTaskCount(), SparkThreadPool::Instance().GetTrashThreadCount());
        SetDlgItemText(IDC_STATIC_INFO, strInfo);
    }
}

LRESULT CMainDlg::OnBnClickedButtonTest(BOOL& /*bHandled*/)
{
    int nNum1, nNum2;
    nNum1 = nNum2 = 0;

    nNum1 = GetDlgItemInt(IDC_EDIT_NUM1);
    nNum2 = GetDlgItemInt(IDC_EDIT_NUM2);

    DoInWorkThread(nNum1, nNum2);

    return 0;
}

void CMainDlg::PrintText(LPCTSTR format, ...)
{
    if (format)
    {
        va_list args;
        va_start(args, format);

        CString strText;
        strText.FormatV(format, args);

        va_end(args);

        // POST到界面线程执行
        SPARK_INSTANCE_POST_MSG_EX(CMainDlg, DoInMainThread, strText);
    }
}

void CMainDlg::DoInMainThread(CString strText)
{
    strText.AppendFormat(L"\r\n输出日志运行线程ID：%d\r\n\r\n", ::GetCurrentThreadId());
    CString strLog;
    GetDlgItemText(IDC_STATIC_TEXT, strLog);

    strLog.Insert(0, strText);
    SetDlgItemText(IDC_STATIC_TEXT, strLog);

    CString strInfo;
    strInfo.Format(L"线程池运行线程数：%d，任务数：%d，回收站线程数：%d", SparkThreadPool::Instance().GetThreadCount(),
        SparkThreadPool::Instance().GetTaskCount(), SparkThreadPool::Instance().GetTrashThreadCount());
    SetDlgItemText(IDC_STATIC_INFO, strInfo);

    //HWND hEditWnd = GetDlgItem(IDC_STATIC_TEXT);
    //::SendMessage(hEditWnd, EM_SETSEL, -1, -1);
}

void CMainDlg::DoInWorkThread(int a, int b)
{
    // 一行代码让此函数运行在线程中
    SPARK_INSTANCE_SWTICH_TO_WORKTHREAD(CMainDlg, DoInWorkThread, a, b);

    PrintText(L"工作线程ID：%d,开始工作,休息2秒", ::GetCurrentThreadId());

    ::Sleep(2000);
    int n = a + b;

    PrintText(L"工作线程ID：%d,完成工作,结果：%d", ::GetCurrentThreadId(), n);
}