/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

#include <map>
#include <list>
#include "sparkasyndef.hpp"
#include "sparkmsgwnd.hpp"
#include "sparkrunnable.hpp"
#include "sparkthread.hpp"
#include "sparkthreadpoolimpl.hpp"

/*
使用示例

Spark::Thread::SparkThreadPool pool;
pool.Init(2, 10);

pool.Execute(&obj, &T::Fun, NULL)

// 界面线程调用
pool.InitMsgWnd();

// 切换到界面线程执行
pool.SwitchToWndThread(&obj, &T::Fun, NULL)

pool.UnInit();

异步宏使用示例

class Test
{
public:
    SPARK_ASYN_API(TestAsyn, int, a, int, b)
    void TestAsyn(int a, int b)
    {
        SPARK_SWTICH_TO_WORKTHREAD(Test1, TestAsyn, this, &g_threadPool, Spark::Thread::emSRType_Post, a, b);
        {
            DWORD dwThreadId = ::GetCurrentThreadId();
        }
    }
};

*/

#define DECLARE_SPARK_EXECUTE(ptype, classparam, args, var)\
        template<typename obj_type, typename ret_type, classparam>\
        bool Execute(obj_type* pObj, ret_type(obj_type::*pFun)(args), args,\
            SparkRunnableType emRunnableType = emSRType_Schedule_Post)\
        {\
            Runnable* pTask = Spark::Thread::CreateRunnableEx(pObj, pFun, var);\
            RUNNABLE_PTR_HOST_ADDREF(pTask);\
            return Execute(pTask, emRunnableType);\
        }\
        template<typename obj_type, typename ret_type, classparam>\
        bool Execute(SparkSharedPtr<obj_type> sharedObj, ret_type(obj_type::*pFun)(args), args,\
            SparkRunnableType emRunnableType = emSRType_Schedule_Post)\
        {\
            Runnable* pTask = Spark::Thread::CreateRunnableEx(sharedObj, pFun, var);\
            RUNNABLE_PTR_HOST_ADDREF(pTask);\
            return Execute(pTask, emRunnableType);\
        }\
        template<typename obj_type, typename ret_type, classparam>\
        bool SwitchToWndThread(obj_type* pObj, ret_type(obj_type::*pFun)(args), args, bool bIsSendMsg = false)\
        {\
            if (!bIsSendMsg)\
            {\
                Runnable* pTask = Spark::Thread::CreateRunnableEx(pObj, pFun, var);\
                RUNNABLE_PTR_HOST_ADDREF(pTask);\
                return SwitchToWndThread(pTask, bIsSendMsg);\
            }\
            ThreadFunction<obj_type, ret_type(ptype)> task(pObj, pFun);\
            task.SetValue(var);\
            return SwitchToWndThread(&task, bIsSendMsg);\
        }\

namespace Spark
{
    namespace Thread
    {
        class SparkThreadPool
        {
        private:
            SparkThreadPool(const SparkThreadPool&);
            void operator=(const SparkThreadPool&);

        public:
            static SparkThreadPool& Instance()
            {
                static SparkThreadPool* _pInst = NULL;
                static volatile LONG _lock = 0;
                while (!_pInst)
                {
                    if (::InterlockedIncrement(&_lock) == 1)
                    {
                        _pInst = CreateInstance();
                        ::InterlockedDecrement(&_lock);
                    }
                    else
                    {
                        ::InterlockedDecrement(&_lock);
                        ::Sleep(10);
                    }
                }
                return *_pInst;
            }

        private:
            static SparkThreadPool* CreateInstance()
            {
                static SparkThreadPool _inst;
                return &_inst;
            }

        public:
            SparkThreadPool()
            {

            }

            virtual ~SparkThreadPool()
            {
                UnInit();
            }

        public:
            /**
            * @brief     初始化线程池
            * @param[in] nMinThreadNum 初始化核心线程数目
            * @param[in] nMaxThreadNum 最大创建线程数目
            * @param[in] nMaxPendingTasks 每个线程执行任务数
            * @param[in] nKeepAliveTime 当线程数大于核心时，此为终止前多余的空闲线程等待新任务的最长时间
            * @return    返回初始化是否成功.
            * @remark    例如 nMinThreadNum(2), nMaxThreadNum(10), nMaxPendingTasks(100)
                         则初始化创建2个线程，每个线程可以执行100个任务，当外部调用同时
                         执行超过200个任务则会创建第3个线程来执行。
            */
            bool Init(int nMinThreadNum, int nMaxThreadNum, int nMaxPendingTasks = 1, int nKeepAliveTime = KEEP_ALIVE_TIME)
            {
                return m_threadPoolImpl.Init(nMinThreadNum, nMaxThreadNum, nMaxPendingTasks, nKeepAliveTime);
            }

            bool Init()
            {
                static int _s_cpu_num = _GetProcessNumber();
                return m_threadPoolImpl.Init(_s_cpu_num, _s_cpu_num * 2);
            }

            // 确保在界面线程调用
            bool InitMsgWnd()
            {
                return m_threadPoolImpl.InitMsgWnd();
            }

            void SetMainThreadId()
            {
                m_threadPoolImpl.SetMainThreadId();
            }

            void UnInit(DWORD dwPerWaitMilliseconds = 100)
            {
                m_threadPoolImpl.UnInit(dwPerWaitMilliseconds);
            }

            int DestroyThisTasks(void* lpThis)
            {
                return m_threadPoolImpl.DestroyThisTasks(lpThis);
            }

            bool SwitchToWndThread(Runnable* pTask, bool bIsSendMsg = false)
            {
                return m_threadPoolImpl.SwitchToWndThread(pTask, bIsSendMsg);
            }

            template<typename T>
            bool Execute(T* pObj, void(T::*pFun)(),
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                Runnable* pTask = Spark::Thread::CreateRunnableEx(pObj, pFun);
                RUNNABLE_PTR_HOST_ADDREF(pTask);

                return Execute(pTask, emRunnableType);
            }

            template<typename T>
            bool Execute(SparkSharedPtr<T> pObj, void(T::*pFun)(),
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                Runnable* pTask = Spark::Thread::CreateRunnableEx(pObj, pFun);
                RUNNABLE_PTR_HOST_ADDREF(pTask);

                return Execute(pTask, emRunnableType);
            }

            bool Execute(void(*pFun)(void*), void* lpParam = NULL,
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                Runnable* pTask = Spark::Thread::CreateRunnable(pFun, lpParam);
                RUNNABLE_PTR_HOST_ADDREF(pTask);

                return Execute(pTask, emRunnableType);
            }

            bool Execute(void(*pFun)(),
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                Runnable* pTask = Spark::Thread::CreateRunnable(pFun);
                RUNNABLE_PTR_HOST_ADDREF(pTask);

                return Execute(pTask, emRunnableType);
            }

            bool Execute(Runnable * pRunnable, 
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                return m_threadPoolImpl.Execute(pRunnable, emRunnableType);
            }

            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type), DECLARE_EXECUTE_TPYE(typename arg0_type), DECLARE_EXECUTE_ARGS(arg0_type a0), DECLARE_EXECUTE_VAR(a0));
            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1), DECLARE_EXECUTE_VAR(a0, a1));
            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2), DECLARE_EXECUTE_VAR(a0, a1, a2));
            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3), DECLARE_EXECUTE_VAR(a0, a1, a2, a3));
            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4));
            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4, a5));
            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4, a5, a6));
            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4, a5, a6, a7));
            DECLARE_SPARK_EXECUTE(DECLARE_EXECUTE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type), DECLARE_EXECUTE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type, typename arg8_type), DECLARE_EXECUTE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8), DECLARE_EXECUTE_VAR(a0, a1, a2, a3, a4, a5, a6, a7, a8));

        public:
            int GetMsgThreadId()
            {
                return m_threadPoolImpl.GetMsgThreadId();
            }

            int GetThreadCount()
            {
                return m_threadPoolImpl.GetThreadCount();
            }

            int GetTrashThreadCount()
            {
                return m_threadPoolImpl.GetTrashThreadCount();
            }

            int GetTaskCount()
            {
                return m_threadPoolImpl.GetTaskCount();
            }

        private:
            inline int _GetProcessNumber()
            {
                SYSTEM_INFO info;
                GetSystemInfo(&info);
                return (int)info.dwNumberOfProcessors;
            }

        private:
            SparkThreadPoolImpl m_threadPoolImpl;

        };

    }
}