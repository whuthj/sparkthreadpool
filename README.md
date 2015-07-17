# sparkthreadpool
c++98/03 windows threadpool

一行代码让函数运行在工作线程中/主线程中

SPARK_INSTANCE_SWTICH_TO_WORKTHREAD

SPARK_INSTANCE_POST_MSG_EX

还有更多简易的异步任务宏


void CMainDlg::PrintText(LPCTSTR format, ...)

{

    if (format)
    
    {
        va_list args;
        va_start(args, format);

        CString strText;
        strText.FormatV(format, args);

        va_end(args);

        SPARK_INSTANCE_POST_MSG_EX(CMainDlg, DoInMainThread, strText);
    }
}

void CMainDlg::DoInMainThread(CString strText)

{

    strText.AppendFormat(L"\r\n输出日志运行线程ID：%d\r\n\r\n", ::GetCurrentThreadId());
    CString strLog;
    GetDlgItemText(IDC_STATIC_TEXT, strLog);

    strLog.Append(strText);
    SetDlgItemText(IDC_STATIC_TEXT, strLog);

    HWND hEditWnd = GetDlgItem(IDC_STATIC_TEXT);
    ::SendMessage(hEditWnd, EM_SETSEL, -1, -1);
}

void CMainDlg::DoInWorkThread(int a, int b)

{

    SPARK_INSTANCE_SWTICH_TO_WORKTHREAD(CMainDlg, DoInWorkThread, a, b);

    PrintText(L"工作线程ID：%d,开始工作,休息2秒", ::GetCurrentThreadId());

    ::Sleep(2000);
    int n = a + b;

    PrintText(L"工作线程ID：%d,完成工作,结果：%d", ::GetCurrentThreadId(), n);
}
