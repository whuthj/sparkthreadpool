#pragma once

#include <windows.h>
#include <process.h>
#include "sparklock.h"

/*
Ê¹ÓÃÊ¾Àý

SparkThread tWork;
tWork.SetRunnable(this, &Test::Fun);

tWork.Start();
tWork.Join();

*/

#pragma warning(disable:4800)

namespace Spark
{
    namespace Thread
    {
        class SparkThread : public IRunnable
        {
        private:
            DISALLOW_COPY_AND_ASSIGN(SparkThread);

        public:
            SparkThread() : m_pRunnable(NULL)
                          , m_handle(NULL)
                          , m_threadID(0)
            {

            }

            explicit SparkThread(Runnable* pRunnable) : m_pRunnable(pRunnable)
                                                      , m_handle(NULL)
                                                      , m_threadID(0)
            {

            }

            virtual ~SparkThread()
            {

            }

        public:
            template<typename T>
            bool SetRunnable(T* pObj, void(T::*pFun)(void*), void* lpParam = NULL)
            {
                m_pRunnable = Spark::Thread::CreateRunnable(pObj, pFun, lpParam);

                if (NULL == m_pRunnable)
                {
                    return false;
                }

                m_pRunnable->SetBeHosted(true);
                m_pRunnable->AddRef();

                return true;
            }

            bool SetRunnable(void(*pFun)(void*), void* lpParam = NULL)
            {
                m_pRunnable = Spark::Thread::CreateRunnable(pFun, lpParam);

                if (NULL == m_pRunnable)
                {
                    return false;
                }

                m_pRunnable->SetBeHosted(true);
                m_pRunnable->AddRef();

                return true;
            }

            template<typename T>
            bool SingletonStart(T* pObj, void(T::*pFun)(void*), void* lpParam = NULL)
            {
                SparkLocker locker(m_lockStart);

                if (!IsAlive())
                {
                    CloseHandle();
                    return Start(pObj, pFun, lpParam);
                }

                return false;
            }

            bool SingletonStart(void(*pFun)(void*), void* lpParam = NULL)
            {
                SparkLocker locker(m_lockStart);

                if (!IsAlive())
                {
                    CloseHandle();
                    return Start(pFun, lpParam);
                }

                return false;
            }

            bool SingletonStart()
            {
                SparkLocker locker(m_lockStart);

                if (!IsAlive())
                {
                    CloseHandle();
                    return Start();
                }

                return false;
            }

            template<typename T>
            bool Start(T* pObj, void(T::*pFun)(void*), void* lpParam = NULL)
            {
                if (SetRunnable(pObj, pFun, lpParam))
                {
                    return Start();
                }

                return false;
            }

            bool Start(void(*pFun)(void*), void* lpParam = NULL)
            {
                if (SetRunnable(pFun, lpParam))
                {
                    return Start();
                }

                return false;
            }

            bool Start()
            {
                if (NULL == m_handle)
                {
                    m_handle = (HANDLE)_beginthreadex(NULL, 0, StaticThreadFunc, this, 0, &m_threadID);
                    if (m_handle) return true;
                }

                ReleaseRunnable();

                return false;
            }

            void Join(DWORD dwMilliseconds = INFINITE)
            {
                if (NULL == m_handle)
                {
                    return;
                }

                if (dwMilliseconds <= 0)
                {
                    dwMilliseconds = INFINITE;
                }

                ::WaitForSingleObject(m_handle, dwMilliseconds);
            }

            bool Terminate(DWORD dwWaitMilliseconds = 100, DWORD dwExitCode = 0)
            {
                bool bIsTerminate = false;

                if (NULL == m_handle)
                {
                    return false;
                }

                if( ::WaitForSingleObject( m_handle, dwWaitMilliseconds ) != WAIT_OBJECT_0 )
                {
                    bIsTerminate = ::TerminateThread( m_handle, dwExitCode );
                }

                CloseHandle();

                return bIsTerminate;
            }

            void ReleaseRunnable()
            {
                if (NULL == m_pRunnable) 
                    return;

                if (m_pRunnable->IsBeHosted())
                {
                    m_pRunnable->Release();
                }
            }

            void CloseHandle()
            {
                if (m_handle)
                {
                    ::CloseHandle( m_handle );
                    m_handle = NULL;
                }
            }

            void Resume()
            {
                if (NULL == m_handle)
                {
                    return;
                }

                ::ResumeThread(m_handle);
            }

            void Suspend()
            {
                if (NULL == m_handle)
                {
                    return;
                }

                ::SuspendThread(m_handle);
            }

            DWORD GetThreadId()
            {
                return m_threadID;
            }

            bool IsAlive()
            {
                return ::WaitForSingleObject(m_handle, 0) == WAIT_TIMEOUT;
            }

        protected:
            virtual void Run()
            {
                if (NULL != m_pRunnable)
                {
                    m_pRunnable->Run();
                    ReleaseRunnable();
                }
            }

        protected:
            static unsigned int WINAPI StaticThreadFunc(void * lpParam)
            {
                SparkThread * pThread = (SparkThread *)lpParam;

                if (pThread)
                {
                    pThread->Run();
                }

                return 0;
            }

        protected:
            HANDLE          m_handle;
            Runnable*       m_pRunnable;
            SparkLock       m_lockStart;
            unsigned int    m_threadID;

        };
    }
}