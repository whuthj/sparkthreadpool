#pragma once

#include "sparkrunnable.hpp"

namespace Spark
{
    namespace Thread
    {
        static const int TASK_HANDLE_MSG_ID = WM_APP + 1001;
        static LPCWSTR SPARK_MSG_WND_CLASS_NAME = L"SparkMsgWnd";
        
        class SparkMsgWnd
        {
        public:
            SparkMsgWnd() : m_hWnd(NULL)
            {

            }

            ~SparkMsgWnd()
            {
            }

            HWND Create()
            {
                if (::IsWindow(m_hWnd)) return m_hWnd;

                HINSTANCE hInstance = ::GetModuleHandle(NULL);
                RegisterWndClass(hInstance);

                m_hWnd = ::CreateWindow(SPARK_MSG_WND_CLASS_NAME, L"", NULL, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, this);

                return m_hWnd;
            }

            LRESULT SendMessage( UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0 )
            {
                return ::SendMessage(m_hWnd, Msg, wParam, lParam);
            }

            LRESULT PostMessage( UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0 )
            {
                return ::PostMessage(m_hWnd, Msg, wParam, lParam);
            }

            BOOL IsWindow()
            {
                return ::IsWindow(m_hWnd);
            }

            BOOL DestroyWindow()
            {
                BOOL bRet = TRUE;

                if (IsWindow())
                {
                    bRet = ::DestroyWindow(m_hWnd);
                }

                if (bRet) m_hWnd = NULL;

                return bRet;
            }

        protected:
            BOOL RegisterWndClass(HINSTANCE hInstance)
            {
                BOOL bInit = FALSE;

                WNDCLASS wndClass;
                memset(&wndClass, 0, sizeof(wndClass));

                bInit = ::GetClassInfo(hInstance, SPARK_MSG_WND_CLASS_NAME, &wndClass);
                if (bInit) return bInit;

                wndClass.lpfnWndProc    = SparkMsgWnd::WndProc;
                wndClass.hInstance      = hInstance;
                wndClass.lpszClassName  = SPARK_MSG_WND_CLASS_NAME;

                bInit = ::RegisterClass(&wndClass) != 0 ? TRUE : FALSE;

                return bInit;
            }

            static LRESULT WINAPI WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
            {
                BOOL bHandled = FALSE;
                LRESULT lResult = S_OK;
                SparkMsgWnd* pKWnd = NULL;

                if (Msg == WM_CREATE)
                {
                    LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
                    if (lpcs)
                    {
                        pKWnd = static_cast<SparkMsgWnd*>(lpcs->lpCreateParams);
                        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pKWnd));
                        if (pKWnd)
                        {
                            pKWnd->m_hWnd = hWnd;
                        }
                    }
                }

                pKWnd = reinterpret_cast<SparkMsgWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
                if (pKWnd)
                {
                    lResult = pKWnd->ProcessMessage(Msg, wParam, lParam, bHandled);
                }

                if (Msg == WM_DESTROY)
                {
                    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
                }

                if (bHandled)
                {
                    return lResult;
                }
                else
                {
                    return ::DefWindowProc(hWnd, Msg, wParam, lParam);
                }
            }

            LRESULT ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
            {
                if (uMsg == TASK_HANDLE_MSG_ID)
                {
                    OnTaskHandleMsg(uMsg, wParam, lParam, bHandled);
                }

                return S_OK;
            }

            LRESULT OnTaskHandleMsg(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
            {
                Runnable* pRunnable = (Runnable *)wParam;

                if (pRunnable)
                {
                    pRunnable->Run();

                    if (pRunnable->IsBeHosted())
                    {
                        pRunnable->Release();
                    }
                }

                return 0;
            }

        private:
            HWND m_hWnd;

        };
    }
}