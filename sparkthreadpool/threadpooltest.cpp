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

    SparkThreadPool::Instance().Init(4, 20, 5, 10000);

    int nRet = 0;
    {
        CMainDlg dlgMain;
        nRet = dlgMain.DoModal();
    }

    SparkThreadPool::Instance().UnInit();

    return nRet;
}