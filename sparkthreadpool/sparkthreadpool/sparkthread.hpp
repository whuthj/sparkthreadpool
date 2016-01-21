/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

#include <windows.h>
#include <process.h>
#include "sparklock.hpp"
#include "sparkfunction.hpp"

/*
使用示例

SparkThread tWork;
tWork.SetRunnable(this, &Test::Fun);

tWork.Start();
tWork.Join();

*/

#pragma warning(disable:4800)

#define DECLARE_EXECUTE_PARAMS(...) __VA_ARGS__
#define DECLARE_EXECUTE_TPYE(...) __VA_ARGS__
#define DECLARE_EXECUTE_ARGS(...) __VA_ARGS__
#define DECLARE_EXECUTE_VAR(...) __VA_ARGS__

#define DECLARE_SPARK_THREAD_FUNCTION(ptype, classparam, args, var)\
        template<typename obj_type, typename ret_type, classparam>\
        bool SetRunnable(obj_type* pObj, ret_type(obj_type::*pFun)(args), args)\
        {\
            m_pRunnable = Spark::Thread::CreateRunnableEx(pObj, pFun, var);\
            RUNNABLE_PTR_HOST_ADDREF(m_pRunnable);\
            return true;\
        }\
        template<typename obj_type, typename ret_type, classparam>\
        bool SetRunnable(SparkSharedPtr<obj_type> sharedObj, ret_type(obj_type::*pFun)(args), args)\
        {\
            m_pRunnable = Spark::Thread::CreateRunnableEx(sharedObj, pFun, var);\
            RUNNABLE_PTR_HOST_ADDREF(m_pRunnable);\
            return true;\
        }\
        template<typename obj_type, typename ret_type, classparam>\
        bool SingletonStart(obj_type* pObj, ret_type(obj_type::*pFun)(args), args)\
        {\
            SparkLocker locker(m_lockStart);\
            if (!IsAlive())\
            {\
                CloseHandle();\
                return Start(pObj, pFun, var);\
            }\
            return false;\
        }\
        template<typename obj_type, typename ret_type, classparam>\
        bool SingletonStart(SparkSharedPtr<obj_type> sharedObj, ret_type(obj_type::*pFun)(args), args)\
        {\
            SparkLocker locker(m_lockStart);\
            if (!IsAlive())\
            {\
                CloseHandle();\
                return Start(sharedObj, pFun, var);\
            }\
            return false;\
        }\
        template<typename obj_type, typename ret_type, classparam>\
        bool Start(obj_type* pObj, ret_type(obj_type::*pFun)(args), args)\
        {\
            if (SetRunnable(pObj, pFun, var))\
            {\
                return Start();\
            }\
            return false;\
        }\
        template<typename obj_type, typename ret_type, classparam>\
        bool Start(SparkSharedPtr<obj_type> sharedObj, ret_type(obj_type::*pFun)(args), args)\
        {\
            if (SetRunnable(sharedObj, pFun, var))\
            {\
                return Start();\
            }\
            return false;\
        }\

namespace Spark
{
    namespace Thread
    {
        class SparkThread : public IRunnable
        {
        private:
            SparkThread(const SparkThread&);
            void operator=(const SparkThread&);

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
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type), DECLARE_EXECUTE_TPYE(typename arg0_type), DECLARE_EXECUTE_ARGS(arg0_type a0), DECLARE_EXECUTE_VAR(a0));
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1), DECLARE_EXECUTE_VAR(a0, a1));
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2), DECLARE_EXECUTE_VAR(a0, a1, a2));
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3), DECLARE_EXECUTE_VAR(a0, a1, a2, a3));
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4));
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4, a5));
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4, a5, a6));
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4, a5, a6, a7));
            DECLARE_SPARK_THREAD_FUNCTION(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type, typename arg8_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4, a5, a6, a7, a8));

            template<typename T>
            bool SetRunnable(T* pObj, void(T::*pFun)())
            {
                m_pRunnable = Spark::Thread::CreateRunnableEx(pObj, pFun);
                RUNNABLE_PTR_HOST_ADDREF(m_pRunnable);
                return true;
            }

            template<typename T>
            bool SetRunnable(SparkSharedPtr<T> pObj, void(T::*pFun)())
            {
                m_pRunnable = Spark::Thread::CreateRunnableEx(pObj, pFun);
                RUNNABLE_PTR_HOST_ADDREF(m_pRunnable);
                return true;
            }

            bool SetRunnable(void(*pFun)(void*), void* lpParam = NULL)
            {
                m_pRunnable = Spark::Thread::CreateRunnable(pFun, lpParam);
                RUNNABLE_PTR_HOST_ADDREF(m_pRunnable);
                return true;
            }

            template<typename T>
            bool SingletonStart(T* pObj, void(T::*pFun)())
            {
                SparkLocker locker(m_lockStart);

                if (!IsAlive())
                {
                    CloseHandle();
                    return Start(pObj, pFun);
                }

                return false;
            }

            template<typename T>
            bool SingletonStart(SparkSharedPtr<T> pObj, void(T::*pFun)())
            {
                SparkLocker locker(m_lockStart);

                if (!IsAlive())
                {
                    CloseHandle();
                    return Start(pObj, pFun);
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
            bool Start(T* pObj, void(T::*pFun)())
            {
                if (SetRunnable(pObj, pFun))
                {
                    return Start();
                }

                return false;
            }

            template<typename T>
            bool Start(SparkSharedPtr<T> pObj, void(T::*pFun)())
            {
                if (SetRunnable(pObj, pFun))
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

            DWORD Join(DWORD dwMilliseconds = INFINITE)
            {
                if (NULL == m_handle)
                {
                    return WAIT_FAILED;
                }

                if (dwMilliseconds <= 0)
                {
                    dwMilliseconds = INFINITE;
                }

                return ::WaitForSingleObject(m_handle, dwMilliseconds);
            }

            bool Terminate(DWORD dwWaitMilliseconds = 100, DWORD dwExitCode = 1000)
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
                SAFE_HOST_RELEASE(m_pRunnable);
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
                SparkThread * pThread = static_cast<SparkThread*>(lpParam);

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