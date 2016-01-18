// sparkthreadpool.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "threadpooltest.h"
#include "maindlg.h"

int APIENTRY _tWinMain( HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR    lpCmdLine,
                      int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    SparkThreadPool::Instance().Init();

    int nRet = 0;
    {
        SparkSharedPtr<CMainDlg> dlgMain(new CMainDlg);
        nRet = dlgMain->DoModal();
    }

    SparkThreadPool::Instance().UnInit(INFINITE);

    return nRet;
}