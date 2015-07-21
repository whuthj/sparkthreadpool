// sparkthreadpool.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "threadpooltest.h"
#include "maindlg.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    SparkThreadPool::Instance().Init(2, 10, 1);

    int nRet = 0;
    {
        CMainDlg dlgMain;
        nRet = dlgMain.DoModal();
    }

    SparkThreadPool::Instance().UnInit();

    return nRet;
}