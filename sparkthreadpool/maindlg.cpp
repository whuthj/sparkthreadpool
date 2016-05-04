#include "stdafx.h"
#include "maindlg.h"
#include "sparkthreadpool/sparkfunction.hpp"
#include "sparkthreadpool/sparkany.hpp"
#include "TestRunnable.h"

//////////////////////////////////////////////////////////////////////////

void Man::doSomthing()
{
    if (!_wife.expired())
    {
        SparkSharedPtr<Woman> woman(_wife);
        woman->doSomthing();
    }
}

//////////////////////////////////////////////////////////////////////////

CTest::~CTest()
{
    SparkUtils::DebugString(L"~CTest \n");
}

void CTest::TestDoAsync()
{
    SparkThreadPool::Instance().Execute(GetSelfSharedPtr(), &CTest::DoAsync1, 1, 2.0f);
}

void CTest::DoAsync1(int a, float b)
{
    ::Sleep(12000);
}

SparkSharedPtr<CTest> CTest::GetSelfSharedPtr()
{
    return SharedFromThis();
}

//////////////////////////////////////////////////////////////////////////

CTestTaskRelease::CTestTaskRelease()
{
    m_pTest = new int(123);
    m_pMainDlg = NULL;

    SparkUtils::DebugString(L"CTestTaskRelease \n");
}

CTestTaskRelease::CTestTaskRelease(CMainDlg* pDlg)
{
    m_pTest = new int(123);
    m_pMainDlg = pDlg;

    SparkUtils::DebugString(L"CTestTaskRelease \n");
}

CTestTaskRelease::~CTestTaskRelease()
{
    SparkUtils::DebugString(L"~Start CTestTaskRelease \n");
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

    SparkUtils::DebugString(L"~CTestTaskRelease \n");
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
    SparkThreadTimer::DestroyThisTimerTask(this);
    DWORD dwStart = ::GetTickCount();
    m_tWork.Join();
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

    SparkSharedPtr<Man> m(new Man());  
    SparkSharedPtr<Woman> w(new Woman());  
    m->setWife(w);
    w->setHusband(m);
    m->doSomthing();

    SparkSharedPtr<CTest> test(new CTest);
    test->TestDoAsync();

    SPARK_PARAM_INSTANCE_ASYN(CMainDlg, DoAsyncEx_5, SparkSharedPtr<CTest>, test);
    //SparkThreadPool::Instance().Execute(this, &CMainDlg::DoAsyncEx_5, test);

    SparkThreadPool::Instance().Execute(SharedFromThis(), &CMainDlg::DoInWorkThread, 123, 123);
    SparkThreadPool::Instance().Execute(SharedFromThis(), &CMainDlg::DoAsyncEx_4, 123.45);
    SparkThreadPool::Instance().Execute(SharedFromThis(), &CMainDlg::DoFunction, 1, 2.1f, 3.12);
    SparkThreadPool::Instance().Execute(SharedFromThis(), &CMainDlg::DoAsyncEx_3);

    TestRunnable* pTestRun = new TestRunnable;
    pTestRun->HostAndAddRef();
    SparkThreadPool::Instance().Execute(pTestRun);

    Tuple<int, float, double, char> t(123, 2.0f, 3.0, 'a');
    SimpleTuple<int, float, double, CString> t1(1, 2.0f, 3.0, L"Hello");
    int t1_0 = t1.item_0;
    float t1_1 = t1.item_1;
    CString t1_3 = t1.item_3;

    m_tWork.Start(SharedFromThis(), &CMainDlg::DoFunction_1, 1, 2.0f, 3.00, t, t1);

    ThreadFunction<CMainDlg, void (int, float, double)> fun(this, &CMainDlg::DoFunction);
    fun(1, 2.0f, 3.0);

    Tuple<int, float, double, CString> t2 = DoTuple1();
    int a = t2.head;
    CString str = t2.Get<3>();

    Any any_1(10);
    Any any_2(CString(L"Any"));
    Any any_3(10.1f);
    Any any_4(10.123);

    int n_1 = AnyCast<int>(any_1);
    if (any_2.type() == typeid(CString))
    {
        CString str_2 = AnyCast<CString>(any_2);
    }
    float f_3 = AnyCast<float>(any_3);
    double dd_4 = AnyCast<double>(any_4);

    std::vector<Any> vec;
    vec.push_back(any_1);
    vec.push_back(any_2);
    vec.push_back(any_3);
    vec.push_back(any_4);

    m_timer1_1.StartTimer(this, &CMainDlg::DoTimer_1, 100, 2);
    m_timer1_1.StartTimer(this, &CMainDlg::DoTimer_1, 1000, 2);
    m_timer1_2.StartTimer(this, &CMainDlg::DoTimer_2, 200, 2);
    m_timer1_2.StartTimer(this, &CMainDlg::DoTimer_2, 42000, 2);
    m_timer1_3.StartTimer(this, &CMainDlg::DoTimer_3, 3000, 2);

    SparkThreadTimer timer1_4;
    timer1_4.StartTimer(this, &CMainDlg::DoTimer_3, 3000, 2);

    SparkThreadTimer timer1_5;
    timer1_5.StartTimer(this, &CMainDlg::DoTimer_3, 3000, 2);

    m_timer1_4.SetTimerTask(SparkThreadTimer::Schedule(this, &CMainDlg::DoTimer_2, 10000, 10));

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

    //SPARK_INSTANCE_ASYN_EX(CMainDlg, DoAsyncEx_2, testTaskRelease);
    SparkThreadPool::Instance().Execute(this, &CMainDlg::DoAsyncEx_2, testTaskRelease);

    //SparkThreadPool::Instance().Execute(this, &CMainDlg::DoAsyncEx, testTaskRelease);
    SPARK_PARAM_INSTANCE_ASYN(CMainDlg, DoAsyncEx, SparkWeakPtr<CTestTaskRelease>, weakTaskRelease);
    SPARK_INSTANCE_ASYN_EX(CMainDlg, DoAsyncEx_3);
    SPARK_PARAM_INSTANCE_ASYN(CMainDlg, DoAsyncEx_4, double, 1234.1234);

    //SPARK_POST_ASYN(CMainDlg, DoAsync, this, &Spark::Thread::SparkThreadPool::Instance(), NULL);
    //SPARK_NOPARAM_INSTANCE_ASYN(CMainDlg, DoAsync);
    SparkThreadPool::Instance().Execute(this, &CMainDlg::DoAsync);

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

void CMainDlg::DoTimer_1()
{
    SparkUtils::DebugString(L"DoTimer_1:%d\n", ::GetTickCount());
    PrintText(L"DoTimer_1");
    m_timer1_3.StopTimer();
}

void CMainDlg::DoTimer_2()
{
    SparkUtils::DebugString(L"DoTimer_2:%d\n", ::GetTickCount());
    PrintText(L"DoTimer_2");
    m_timer1_3.StartTimer(this, &CMainDlg::DoTimer_3, 3000, 2);
}

void CMainDlg::DoTimer_3()
{
    SparkUtils::DebugString(L"DoTimer_3:%d\n", ::GetTickCount());
    PrintText(L"DoTimer_3");
}

void CMainDlg::DoDelay(int value)
{
    PrintText(L"DoDelay 延迟2s执行 value %d", value);
}

void CMainDlg::DoFunction(int a, float b, double c)
{
    int _a = a;
}

void CMainDlg::DoFunction_1( int a, float b, double c, Tuple<int, float, double, char> d, SimpleTuple<int, float, double, CString> e )
{
    int d_0 = d.Get<0>();
    float d_1 = d.Get<1>();
    double d_2 = d.Get<2>();
    char d_3 = d.Get<3>();

    d_0 = e.item_0;
    CString e_3 = e.item_3;

    int len = TupleLength<Tuple<int, float, double>>::value;
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

    SparkSharedPtr<CTestTaskRelease> test(param);
    if (test)
    {
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
    ::Sleep(100);
}

void CMainDlg::DoAsyncEx_5(SparkSharedPtr<CTest> param)
{
    ::Sleep(15000);
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
    //OutputDebugString(strLog);
    
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

Tuple<int, float, double, CString> CMainDlg::DoTuple1()
{
    int a = 123;
    float b = 234.0f;
    double c = 345.0;
    CString str = L"Tuple多返回值爽";

    Tuple<int&, float&, double&, CString&> t(a, b, c, str);
    a = 1234;
    str = L"Tuple多返回值爽!";
    float& _b = t.Get<1>();
    _b = 345.0f;

    Tuple<int, float, double, CString> t1(a, b, c, str);
    SimpleTuple<int, float, double, CString> t2(a, b, c, str);
    SimpleTuple<int, float, double> t3(a, b, c);
    SimpleTuple<> t4;

    return t1;
}