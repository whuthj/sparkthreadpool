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

    PrintText(L"���߳�ID��%d", ::GetCurrentThreadId());

    SetDlgItemInt(IDC_EDIT_NUM1, 100);
    SetDlgItemInt(IDC_EDIT_NUM2, 200);

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

        // POST�������߳�ִ��
        SPARK_INSTANCE_POST_MSG_EX(CMainDlg, DoInMainThread, strText);
    }
}

void CMainDlg::DoInMainThread(CString strText)
{
    strText.AppendFormat(L"\r\n�����־�����߳�ID��%d\r\n\r\n", ::GetCurrentThreadId());
    CString strLog;
    GetDlgItemText(IDC_STATIC_TEXT, strLog);

    strLog.Append(strText);
    SetDlgItemText(IDC_STATIC_TEXT, strLog);

    HWND hEditWnd = GetDlgItem(IDC_STATIC_TEXT);
    ::SendMessage(hEditWnd, EM_SETSEL, -1, -1);
}

void CMainDlg::DoInWorkThread(int a, int b)
{
    // һ�д����ô˺����������߳���
    SPARK_INSTANCE_SWTICH_TO_WORKTHREAD(CMainDlg, DoInWorkThread, a, b);

    PrintText(L"�����߳�ID��%d,��ʼ����,��Ϣ2��", ::GetCurrentThreadId());

    ::Sleep(2000);
    int n = a + b;

    PrintText(L"�����߳�ID��%d,��ɹ���,�����%d", ::GetCurrentThreadId(), n);
}