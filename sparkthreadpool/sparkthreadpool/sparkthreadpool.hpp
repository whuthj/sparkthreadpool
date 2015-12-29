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

            template<typename T>
            bool SwitchToWndThread(T* pObj, void(T::*pFun)(void*), void* lpParam = NULL, bool bIsSendMsg = false)
            {
                return m_threadPoolImpl.SwitchToWndThread(pObj, pFun, lpParam, bIsSendMsg);
            }

            bool SwitchToWndThread(Runnable* pTask, bool bIsSendMsg = false)
            {
                return m_threadPoolImpl.SwitchToWndThread(pTask, bIsSendMsg);
            }

            template<typename T, typename ParamType>
            bool Execute(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam = NULL,
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                return m_threadPoolImpl.Execute(pObj, pFun, lpParam, emRunnableType);
            }

            template<typename T>
            bool Execute(T* pObj, void(T::*pFun)(),
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                return m_threadPoolImpl.Execute(pObj, pFun, emRunnableType);
            }

            bool Execute(void(*pFun)(void*), void* lpParam = NULL, 
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                return m_threadPoolImpl.Execute(pFun, lpParam, emRunnableType);
            }

            bool Execute(Runnable * pRunnable, 
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                return m_threadPoolImpl.Execute(pRunnable, emRunnableType);
            }

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
            SparkThreadPoolImpl m_threadPoolImpl;

        };

    }
}