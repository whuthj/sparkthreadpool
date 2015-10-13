#pragma once

#include "sparkrunnable.hpp"

#define DECLEAR_WND_CLASS_NAME(Name)\
public:\
    static LPCWSTR GetWndClassName()\
    {\
    return Name;\
    }

namespace Spark
{
    namespace Thread
    {
        static const int TASK_HANDLE_MSG_ID = WM_APP + 1001;
        
        class SparkMsgWnd
        {
            DECLEAR_WND_CLASS_NAME(L"SparkMsgWnd")
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

                m_hWnd = ::CreateWindow(GetWndClassName(), L"", NULL, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, this);

                return m_hWnd;
            }

            LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) throw()
            {
                ATLASSERT(::IsWindow(m_hWnd));
                return ::SendMessage(m_hWnd, message, wParam, lParam);
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
            BOOL RegisterWndClass(HINSTANCE hInstance)
            {
                BOOL bInit = FALSE;

                WNDCLASS wndClass;
                memset(&wndClass, 0, sizeof(wndClass));

                bInit = ::GetClassInfo(hInstance, GetWndClassName(), &wndClass);
                if (bInit) return bInit;

                wndClass.lpfnWndProc    = SparkMsgWnd::WndProc;
                wndClass.hInstance      = hInstance;
                wndClass.lpszClassName  = GetWndClassName();

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
                else if (uMsg == WM_TIMER)
                {
                    OnTimer(uMsg, wParam, lParam, bHandled);
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

            LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
            {
                UINT_PTR nEventId = (UINT_PTR)wParam;

                return 0;
            }

        private:
            HWND m_hWnd;

        };
    }
}