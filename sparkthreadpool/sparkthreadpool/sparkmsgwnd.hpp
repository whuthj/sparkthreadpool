/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

#include "sparkrunnable.hpp"
#include "sparklock.hpp"

namespace Spark
{
    namespace Thread
    {
        static const int TASK_HANDLE_SEND_MSG_ID = WM_APP + 1001;
        static const int TASK_HANDLE_POST_MSG_ID = WM_APP + 1002;
        static const LPCWSTR SPARK_MSG_WND_CLASS_NAME = L"SparkMsgWnd";
        
        class SparkMsgWnd
        {
        public:
            SparkMsgWnd() : m_hWnd(NULL)
            {

            }

            virtual ~SparkMsgWnd()
            {
            }

            HWND Create(LPCWSTR lpszClassName = SPARK_MSG_WND_CLASS_NAME)
            {
                if (::IsWindow(m_hWnd)) { return m_hWnd; }
                if (NULL == lpszClassName) { return NULL; }

                HINSTANCE hInstance = ::GetModuleHandle(NULL);
                RegisterWndClass(hInstance, lpszClassName);

                m_hWnd = ::CreateWindow(lpszClassName, L"", NULL, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, this);

                return m_hWnd;
            }

            LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0, UINT uTimeout = 2000) throw()
            {
                ATLASSERT(::IsWindow(m_hWnd));
                //LRESULT hr = ::SendMessage(m_hWnd, message, wParam, lParam);
                
                DWORD dwResult;
                LRESULT hr = ::SendMessageTimeout(m_hWnd, message, wParam, lParam, SMTO_NORMAL, uTimeout, &dwResult);

                return hr;
            }

            BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) throw()
            {
                ATLASSERT(::IsWindow(m_hWnd));
                return ::PostMessage(m_hWnd, message, wParam, lParam);
            }

            UINT_PTR SetTimer(UINT_PTR nIDEvent, UINT nElapse, void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) = NULL) throw()
            {
                ATLASSERT(::IsWindow(m_hWnd));
                return ::SetTimer(m_hWnd, nIDEvent, nElapse, (TIMERPROC)lpfnTimer);
            }

            BOOL KillTimer(UINT_PTR nIDEvent) throw()
            {
                ATLASSERT(::IsWindow(m_hWnd));
                return ::KillTimer(m_hWnd, nIDEvent);
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
            BOOL RegisterWndClass(HINSTANCE hInstance, LPCWSTR lpszClassName)
            {
                BOOL bInit = FALSE;
                if (NULL == lpszClassName) { return bInit; }

                WNDCLASS wndClass;
                memset(&wndClass, 0, sizeof(wndClass));

                bInit = ::GetClassInfo(hInstance, lpszClassName, &wndClass);
                if (bInit) { return bInit; }

                wndClass.lpfnWndProc    = SparkMsgWnd::WndProc;
                wndClass.hInstance      = hInstance;
                wndClass.lpszClassName = lpszClassName;

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
                bHandled = TRUE;

                if (uMsg == TASK_HANDLE_SEND_MSG_ID)
                {
                    OnTaskHandleSendMsg(uMsg, wParam, lParam, bHandled);
                }
                else if (uMsg == TASK_HANDLE_POST_MSG_ID)
                {
                    OnTaskHandlePostMsg(uMsg, wParam, lParam, bHandled);
                }
                else if (uMsg == WM_TIMER)
                {
                    OnTimer(uMsg, wParam, lParam, bHandled);
                }

                return S_OK;
            }

            LRESULT OnTaskHandlePostMsg(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
            {
                Runnable* pRunnable = (Runnable *)wParam;

                if (pRunnable)
                {
                    pRunnable->Run();
                    SAFE_HOST_RELEASE(pRunnable);
                }

                return 0;
            }

            LRESULT OnTaskHandleSendMsg(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
            {
                Runnable* pRunnable = (Runnable *)wParam;
                bool* bIsHandled = (bool*)lParam;

                if (pRunnable) { pRunnable->Run(); }
                if (bIsHandled) { *bIsHandled = true; }

                return 0;
            }

            LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
            {
                UINT_PTR nTimerId = (UINT_PTR)wParam;

                OnTimer(nTimerId);

                return 0;
            }

            virtual void OnTimer(UINT_PTR nTimerId) {}

        private:
            HWND m_hWnd;
            SparkLock m_lockSendMsg;

        };
    }
}