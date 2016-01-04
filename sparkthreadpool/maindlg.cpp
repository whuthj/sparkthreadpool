#include "stdafx.h"
#include "maindlg.h"

CTestTaskRelease::CTestTaskRelease()
{
    m_pTest = new int(123);
    m_pMainDlg = NULL;
}

CTestTaskRelease::CTestTaskRelease(CMainDlg* pDlg)
{
    m_pTest = new int(123);
    m_pMainDlg = pDlg;
}

CTestTaskRelease::~CTestTaskRelease()
{
    if (m_pMainDlg)
    {
        m_pMainDlg->PrintText(L"CTestTaskRelease::~CTestTaskRelease Start");
    }

    DWORD dwStart = ::GetTickCount();
    SPARK_INSTANCE_DESTROY_TASKS(this);
    if (m_pTest)
    {
        delete m_pTest;
        m_pTest = NULL;
    }
    DWORD dwCost = ::GetTickCount() - dwStart;

    if (m_pMainDlg)
    {
        m_pMainDlg->PrintText(L"CTestTaskRelease::~CTestTaskRelease End(%d)", dwCost);
    }
}

void CTestTaskRelease::TestDoAsync()
{
    SPARK_NOPARAM_INSTANCE_ASYN(CTestTaskRelease, DoAsync1);
    SPARK_NOPARAM_INSTANCE_ASYN(CTestTaskRelease, DoAsync2);
    SPARK_NOPARAM_INSTANCE_ASYN(CTestTaskRelease, DoAsync3);
    SPARK_NOPARAM_INSTANCE_ASYN(CTestTaskRelease, DoAsync4);
}

void CTestTaskRelease::DoAsync1()
{
    if (m_pMainDlg)
    {
        int a = 100;
        SPARK_MSG(CMainDlg, DoSendMsgToMainThread, m_pMainDlg, &SparkThreadPool::Instance(), true, &a);
        m_pMainDlg->PrintText(L"CTestTaskRelease::DoAsync1");
    }
    ::Sleep(12000);
    DoTest();
}

void CTestTaskRelease::DoAsync2()
{
    if (m_pMainDlg)
    {
        m_pMainDlg->PrintText(L"CTestTaskRelease::DoAsync2");
    }
    ::Sleep(12000);
    DoTest();
}

void CTestTaskRelease::DoAsync3()
{
    ::Sleep(12000);
    DoTest();
}

void CTestTaskRelease::DoAsync4()
{
    ::Sleep(12000);
    DoTest();
}

void CTestTaskRelease::DoTest()
{
    int nTest = *m_pTest;
    if (m_pMainDlg)
    {
        m_pMainDlg->PrintText(L"CTestTaskRelease::DoTest %d", nTest);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CMainDlg::CMainDlg()
{
}

CMainDlg::~CMainDlg()
{
    SparkWndTimer::DestroyThisTimerTask(this);
    DWORD dwStart = ::GetTickCount();
    SPARK_INSTANCE_DESTROY_TASKS(this);
    DWORD dwCost = ::GetTickCount() - dwStart;
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

    DoTimerLog();
    m_timerLog.StartTimer(this, &CMainDlg::DoTimerLog, 1000);

    return TRUE;
}

LRESULT CMainDlg::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    EndDialog(0);

    return 0;
}

LRESULT CMainDlg::OnBnClickedButtonTest(BOOL& /*bHandled*/)
{
    int nNum1, nNum2;
    nNum1 = nNum2 = 0;

    nNum1 = GetDlgItemInt(IDC_EDIT_NUM1);
    nNum2 = GetDlgItemInt(IDC_EDIT_NUM2);

    DoInWorkThread(nNum1, nNum2);

    //CTestTaskRelease* pTest = new CTestTaskRelease(this);
    //pTest->TestDoAsync();
    SparkSharedPtr<CTestTaskRelease> testTaskRelease(new CTestTaskRelease(this));
    testTaskRelease->TestDoAsync();

    SparkWeakPtr<CTestTaskRelease> weakTaskRelease(testTaskRelease);

    SPARK_INSTANCE_ASYN_EX(CMainDlg, DoAsyncEx_2, testTaskRelease);

    //SparkThreadPool::Instance().Execute(this, &CMainDlg::DoAsyncEx, testTaskRelease);
    SPARK_PARAM_INSTANCE_ASYN(CMainDlg, DoAsyncEx, SparkWeakPtr<CTestTaskRelease>, weakTaskRelease);
    SPARK_INSTANCE_ASYN_EX(CMainDlg, DoAsyncEx_3);
    SPARK_PARAM_INSTANCE_ASYN(CMainDlg, DoAsyncEx_4, double, 1234.1234);

    //SPARK_POST_ASYN(CMainDlg, DoAsync, this, &Spark::Thread::SparkThreadPool::Instance(), NULL);
    SPARK_NOPARAM_INSTANCE_ASYN(CMainDlg, DoAsync);

    m_timer.StartTimer(this, &CMainDlg::DoTimer, 1000, 2);
    SparkWndTimer::Schedule(this, &CMainDlg::DoDelay, 123, 2000, 1);

    return 0;
}

void CMainDlg::DoTimerLog()
{
    CString strInfo;
    strInfo.Format(L"线程池运行线程数：%d，任务数：%d，回收站线程数：%d", SparkThreadPool::Instance().GetThreadCount(),
        SparkThreadPool::Instance().GetTaskCount(), SparkThreadPool::Instance().GetTrashThreadCount());
    SetDlgItemText(IDC_STATIC_INFO, strInfo);
}

void CMainDlg::DoTimer()
{
    PrintText(L"DoTimer2：%d", m_timer.GetRunCount());
    //if (mTimer2.GetRunCount() == 10)
    //{
    //    mTimer2.StopTimer();
    //}
}

void CMainDlg::DoDelay(int value)
{
    PrintText(L"DoDelay 延迟2s执行 value %d", value);
}

void CMainDlg::DoAsync()
{
    int a = 100;

    //SPARK_MSG(CMainDlg, DoSendMsgToMainThread, this, &Spark::Thread::SparkThreadPool::Instance(), true, &a);
    SPARK_INSTANCE_SEND_MSG(CMainDlg, DoSendMsgToMainThread, &a);
    //SPARK_MSG(CMainDlg, DoPostMsgToMainThread, this, &Spark::Thread::SparkThreadPool::Instance(), false, a);
    SPARK_INSTANCE_POST_MSG(CMainDlg, DoPostMsgToMainThread, a);

    SPARK_INSTANCE_ASYN(CMainDlg, DoPostMsgToMainThread, NULL);

    for (int i = 0; i < 100; i++)
    {
        SPARK_INSTANCE_SEND_MSG(CMainDlg, DoSendMsgToMainThread, &a);
        CTestTaskRelease test(this);
        test.TestDoAsync();
    }

    int b = 1000;
}

void CMainDlg::DoAsyncEx(SparkWeakPtr<CTestTaskRelease> param)
{
    ::Sleep(1000);

    if (!param.expired())
    {
        SparkSharedPtr<CTestTaskRelease> test(param);
        SparkThread tWork;
        tWork.Start(this, &CMainDlg::DoAsyncEx_1, test);
        tWork.Join();
    }
}

void CMainDlg::DoAsyncEx_1(SparkSharedPtr<CTestTaskRelease> param)
{
    param->TestDoAsync();
}

void CMainDlg::DoAsyncEx_2(SparkSharedPtr<CTestTaskRelease> testTaskRelease)
{
    testTaskRelease->TestDoAsync();
}

void CMainDlg::DoAsyncEx_3()
{

}

void CMainDlg::DoAsyncEx_4(double value)
{

}

void CMainDlg::DoSendMsgToMainThread(void* lpParam)
{
    int* lpTmp = (int*)lpParam;
    *lpTmp = 1234;
}

void CMainDlg::DoPostMsgToMainThread(void* lpParam)
{
    int a = 100;
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
    static int _s_nLogCount = 0;
    
    strText.AppendFormat(L"\r\n输出日志运行线程ID：%d\r\n\r\n", ::GetCurrentThreadId());
    CString strLog;
    GetDlgItemText(IDC_STATIC_TEXT, strLog);
    OutputDebugString(strLog);
    
    if (_s_nLogCount++ > 100)
    {
        _s_nLogCount = 0;
        strLog.Empty();
    }

    strLog.Insert(0, strText);
    SetDlgItemText(IDC_STATIC_TEXT, strLog);

    CString strInfo;
    strInfo.Format(L"线程池运行线程数：%d，任务数：%d，回收站线程数：%d", SparkThreadPool::Instance().GetThreadCount(),
        SparkThreadPool::Instance().GetTaskCount(), SparkThreadPool::Instance().GetTrashThreadCount());
    SetDlgItemText(IDC_STATIC_INFO, strInfo);
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