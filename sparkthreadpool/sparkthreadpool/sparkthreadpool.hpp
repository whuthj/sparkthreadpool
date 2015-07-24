#pragma once

#include <map>
#include <list>
#include "sparkasyndef.hpp"
#include "sparkmsgwnd.hpp"
#include "sparkrunnable.hpp"
#include "sparkthread.hpp"
#include "sparkthreadpooldef.hpp"

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

#define RUNNABLE_PTR_HOST_ADDREF(pRunnable)\
{\
    if (NULL == pRunnable){ return false; } pRunnable->SetBeHosted(true); pRunnable->AddRef();\
}

#define FAILED_THEN_UNINIT_AND_RETURN(result)\
{\
if (!result){ UnInit(); return false; } \
}

namespace Spark
{
    namespace Thread
    {
        static const int MAX_TASK_COUNT = 100000; //最多并发任务数
        static const int RECREATE_MSGWND_COUNT = 3; //重试创建窗口次数
        static const int KEEP_ALIVE_TIME = 1000 * 60; //清理空闲线程间隔时间即1分钟清理一次空闲线程

        typedef enum __SparkThreadWorkStatus
        {
            emSTWStatus_None = 0,
            emSTWStatus_Idle = 1,
            emSTWStatus_Work = 2,
        }SparkThreadWorkStatus;

        typedef enum __SparkRunnableType
        {
            emSRType_None           = 0,
            emSRType_Schedule       = 1,  // 默认是添加任务需要扩展线程则扩展线程立即执行
            emSRType_Post           = 2,  // 只把任务加入任务队列，不开线程
            emSRType_Schedule_Post  = 3,  // 1的基础上如果扩展线程失败也继续把任务加入队列
        }SparkRunnableType;

        class SparkThreadPool
        {
        private:
            DISALLOW_COPY_AND_ASSIGN(SparkThreadPool);

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

        private:
            class SparkThreadWork : public SparkThread
            {
            public:
                SparkThreadWork(SparkThreadPool* pThreadPool) : m_pThreadPool(pThreadPool)
                                                              , m_emWorkStatus(emSTWStatus_None)
                {

                }

                virtual void Run()
                {
                    if (NULL != m_pThreadPool)
                    {
                        m_pThreadPool->Run(this);
                    }
                }

            public:
                void SetWorkStatus(SparkThreadWorkStatus emStatus)
                {
                    m_emWorkStatus = emStatus;
                }

                SparkThreadWorkStatus GetWorkStatus()
                {
                    return m_emWorkStatus;
                }

            private:
                SparkThreadPool* const m_pThreadPool;
                SparkThreadWorkStatus  m_emWorkStatus;

            };

        public:
            SparkThreadPool() : m_bIsInit(false)
                              , m_nMinThreadNum(0)
                              , m_nMaxThreadNum(0)
                              , m_nMaxPendingTasks(0)
                              , m_nMsgThreadId(0)
                              , m_hExitEvt(NULL)
                              , m_hNotifyEvt(NULL)
                              , m_pCleanerThread(NULL)
            {

            }

            virtual ~SparkThreadPool()
            {
                UnInit();
            }

        public:
            /**
            * @brief     初始化线程池
            * @param[in] nMinThreadNum 初始化则创建线程数目
            * @param[in] nMaxThreadNum 最大创建线程数目
            * @param[in] nMaxPendingTasks 每个线程执行任务数
            * @return    返回初始化是否成功.
            * @remark    例如 nMinThreadNum(2), nMaxThreadNum(10), nMaxPendingTasks(100)
                         则初始化创建2个线程，每个线程可以执行100个任务，当外部调用同时
                         执行超过200个任务则会创建第3个线程来执行。
            */
            bool Init(int nMinThreadNum, int nMaxThreadNum, int nMaxPendingTasks = 1)
            {
                if (m_bIsInit)
                {
                    return false;
                }
                if (!IsInitParamValid(nMinThreadNum, nMaxPendingTasks, nMaxThreadNum))
                {
                    return false;
                }

                InitParams(nMinThreadNum, nMaxThreadNum, nMaxPendingTasks);

                bool bCreate = false;
                
                if (nMinThreadNum != nMaxThreadNum)
                {
                    bCreate = CreateCleanerThread();
                    FAILED_THEN_UNINIT_AND_RETURN(bCreate);
                }

                bCreate = CreateInitThreads(nMinThreadNum);
                FAILED_THEN_UNINIT_AND_RETURN(bCreate);

                m_bIsInit = true;

                return true;
            }

            // 确保在界面线程调用
            bool InitMsgWnd()
            {
                CreateMsgWnd();
                SetMainThreadId();

                return m_msgWnd.IsWindow();
            }

            void SetMainThreadId()
            {
                m_nMsgThreadId = ::GetCurrentThreadId();
            }

            void UnInit(DWORD dwPerWaitMilliseconds = 100)
            {
                SetExitEvent();

                DestroyCleanerThread();
                DestroyThreadPool(dwPerWaitMilliseconds);
                DestroyTasks();
                DestroyMsgWnd();

                CloseHandles();
            }

            template<typename T>
            bool SwitchToWndThread(T* pObj, void(T::*pFun)(void*), void* lpParam = NULL, bool bIsSendMsg = false)
            {
                Runnable* pTask = Spark::Thread::CreateRunnable(pObj, pFun, lpParam);

                RUNNABLE_PTR_HOST_ADDREF(pTask);

                return SwitchToWndThread(pTask, bIsSendMsg);
            }

            bool SwitchToWndThread(Runnable* pTask, bool bIsSendMsg = false)
            {
                if (m_msgWnd.IsWindow())
                {
                    if (bIsSendMsg) m_msgWnd.SendMessage(TASK_HANDLE_MSG_ID, (WPARAM)pTask);
                    else m_msgWnd.PostMessage(TASK_HANDLE_MSG_ID, (WPARAM)pTask);

                    return true;
                }

                if (pTask->IsBeHosted())
                {
                    pTask->Release();
                }

                return false;
            }

            template<typename T>
            bool Execute(T* pObj, void(T::*pFun)(void*), void* lpParam = NULL, 
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                Runnable* pTask = Spark::Thread::CreateRunnable(pObj, pFun, lpParam);

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

            bool Execute(Runnable * pRunnable, 
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                if (NULL == pRunnable) return false;

                if (!m_bIsInit)
                {
                    if (pRunnable->IsBeHosted())
                    {
                        pRunnable->Release();
                    }

                    return false;
                }

                if (!IsShouldResizePool() || emSRType_Post == emRunnableType)
                {
                    AddTaskAndNotify(pRunnable);

                    return true;
                }

                // 需要扩展线程
                // 当前线程池数目<最大线程数则扩展线程
                if (CompareThreadCountWithMax() < 0)
                {
                    AddWorkThread();
                    AddTaskAndNotify(pRunnable);

                    return true;
                }

                // 当前线程池数目>=最大线程数，但Default_Post还是添加任务到队列
                if (emSRType_Schedule_Post == emRunnableType)
                {
                    AddTaskAndNotify(pRunnable);

                    return true;
                }

                if (pRunnable->IsBeHosted())
                {
                    pRunnable->Release();
                }

                return false;
            }

        public:
            int GetMsgThreadId()
            {
                return m_nMsgThreadId;
            }

            int GetThreadCount()
            {
                int nThreadPoolCount = 0;

                SparkLocker locker(m_lockThreadPool);
                nThreadPoolCount = m_threadPool.size();

                return nThreadPoolCount;
            }

            int GetTrashThreadCount()
            {
                int nThreadCount = 0;

                SparkLocker locker(m_lockTrashThreadPool);
                nThreadCount = m_trashThread.size();

                return nThreadCount;
            }

            int GetTaskCount()
            {
                int nTasksCount = 0;

                SparkLocker locker(m_lockTasks);
                nTasksCount = m_tasks.size();

                return nTasksCount;
            }

        protected:
            bool CreateCleanerThread()
            {
                if (NULL == m_pCleanerThread)
                {
                    m_pCleanerThread = new SparkThread();
                    if (NULL == m_pCleanerThread)
                    {
                        return false;
                    }

                    m_pCleanerThread->SetRunnable(this, &SparkThreadPool::CleanerRun);
                    m_pCleanerThread->Start();
                }

                return true;
            }

            bool CreateInitThreads(int nThreadNum)
            {
                bool bResult = true;

                for (int i = 0; i < nThreadNum; i++)
                {
                    bResult = AddWorkThread();
                    if (!bResult)
                    {
                        break;
                    }
                }

                return bResult;
            }

            bool AddWorkThread()
            {
                SparkThreadWork* pWorker = new SparkThreadWork(this);
                if (NULL == pWorker)
                {
                    return false;
                }

                pWorker->Start();
                int nThreadId = pWorker->GetThreadId();

                {
                    SparkLocker locker(m_lockThreadPool);
                    m_threadPool.insert(std::make_pair(nThreadId, pWorker));
                }
                
                return true;
            }

            void DestroyCleanerThread()
            {
                if (m_pCleanerThread)
                {
                    m_pCleanerThread->Terminate(1000, 0);
                    delete m_pCleanerThread;
                    m_pCleanerThread = NULL;
                }
            }

            void DestroyThreadPool(DWORD dwPerWaitMilliseconds)
            {
                {
                    SparkLocker locker(m_lockThreadPool);

                    ThreadPoolItr itr = m_threadPool.begin();
                    while (itr != m_threadPool.end())
                    {
                        SparkThreadWork* pWorker = itr->second;
                        if (NULL != pWorker)
                        {
                            pWorker->Terminate(dwPerWaitMilliseconds, 0);
                            delete pWorker;
                            pWorker = NULL;
                        }
                        itr++;
                    }
                    m_threadPool.clear();
                }

                {
                    SparkLocker locker(m_lockTrashThreadPool);

                    ThreadPoolItr itr = m_trashThread.begin();
                    while (itr != m_trashThread.end())
                    {
                        SparkThreadWork* pWorker = itr->second;
                        if (NULL != pWorker)
                        {
                            pWorker->Terminate(dwPerWaitMilliseconds, 0);
                            delete pWorker;
                            pWorker = NULL;
                        }
                        itr++;
                    }
                    m_trashThread.clear();
                }
            }

            void DestroyTasks()
            {
                SparkLocker locker(m_lockTasks);

                TasksItr itr = m_tasks.begin();
                while (itr != m_tasks.end())
                {
                    Runnable* pRunnable = *itr;
                    if (NULL != pRunnable)
                    {
                        if (pRunnable->IsBeHosted())
                        {
                            pRunnable->Release();
                        }
                        pRunnable = NULL;
                    }
                    itr++;
                }
                m_tasks.clear();
            }

            void AddTaskAndNotify(Runnable* pRunnable)
            {
                AddTask(pRunnable); 
                NotifyAddTask();
            }

            void AddTask(Runnable* pRunnable)
            {
                SparkLocker locker(m_lockTasks);

                m_tasks.push_back(pRunnable);
            }

            void NotifyAddTask()
            {
                if (m_hNotifyEvt)
                {
                    ::ReleaseSemaphore(m_hNotifyEvt, 1, NULL);
                }
            }

            Runnable * GetTask()
            {
                Runnable* pTask = NULL;

                {
                    SparkLocker locker(m_lockTasks);

                    if (!m_tasks.empty())
                    {
                        pTask = m_tasks.front();
                        m_tasks.pop_front();
                    }
                }

                return pTask;
            }

            bool IsShouldResizePool()
            {
                int nTasksCount = 0;

                {
                    SparkLocker locker(m_lockTasks);
                    nTasksCount = m_tasks.size();
                }

                if (nTasksCount > m_nMinThreadNum * m_nMaxPendingTasks)
                {
                    return true;
                }

                if (0 == m_nMinThreadNum)
                {
                    return true;
                }
                
                return false;
            }

            int CompareThreadCountWithMax()
            {
                int nThreadPoolCount = 0;

                {
                    SparkLocker locker(m_lockThreadPool);
                    nThreadPoolCount = m_threadPool.size();
                }

                if (nThreadPoolCount == m_nMaxThreadNum)
                {
                    return 0;
                }

                return nThreadPoolCount > m_nMaxThreadNum ? 1 : -1;
            }

            void Run(void* lpParam)
            {
                HANDLE hWaitEvt[] = { m_hExitEvt, m_hNotifyEvt };
                SparkThreadWork* pWorkThread = static_cast<SparkThreadWork*>(lpParam);

                for (;;)
                {
                    ResetWorkThreadStatus(pWorkThread);

                    DWORD dwRet = ::WaitForMultipleObjects(2, hWaitEvt, FALSE, KEEP_ALIVE_TIME);
                    if (IsShouldExitRun(dwRet))
                    {
                        break;
                    }
                    if (IsShouldRecycleThread(dwRet))
                    {
                        RecycleThreadPool();
                        continue;
                    }

                    Runnable* pTask = GetTask();
                    if (pTask)
                    {
                        BeforeExecuteRun(pWorkThread, pTask);
                        ExecuteRun(pWorkThread, pTask);
                        AfterExecuteRun(pWorkThread, pTask);
                    }
                }
            }

            void ResetWorkThreadStatus( SparkThreadWork* pWorkThread )
            {
                if (pWorkThread)
                {
                    pWorkThread->SetWorkStatus(emSTWStatus_Idle);
                }
            }

            bool IsShouldRecycleThread( DWORD dwRet )
            {
                return WAIT_TIMEOUT == dwRet;
            }

            bool IsShouldExitRun( DWORD dwRet )
            {
                return WAIT_OBJECT_0 + 1 != dwRet && WAIT_TIMEOUT != dwRet;
            }

            void BeforeExecuteRun( SparkThreadWork* pWorkThread, Runnable* pTask )
            {
                if (pWorkThread)
                {
                    pWorkThread->SetWorkStatus(emSTWStatus_Work);
                }
            }

            void ExecuteRun( SparkThreadWork* pWorkThread, Runnable* pTask )
            {
                if (pTask)
                {
                    pTask->Run();
                }
            }

            void AfterExecuteRun( SparkThreadWork* pWorkThread, Runnable* pTask )
            {
                ResetWorkThreadStatus(pWorkThread);

                if (pTask && pTask->IsBeHosted())
                {
                    pTask->Release();
                }
            }

            void CleanerRun(void* pParam)
            {
                for (;;)
                {
                    DWORD dwRet = ::WaitForSingleObject(m_hExitEvt, 1000 * 10);
                    if (WAIT_TIMEOUT != dwRet)
                    {
                        break;
                    }

                    CleanThreadPool();
                }
            }

            void RecycleThreadPool()
            {
                // 检测大于最小线程数则回收当前线程
                SparkLocker locker(m_lockThreadPool);

                int nThreadId = ::GetCurrentThreadId();
                int nThreadPoolCount = m_threadPool.size();

                if (nThreadPoolCount > m_nMinThreadNum)
                {
                    ThreadPoolItr itr = m_threadPool.find(nThreadId);
                    if (itr != m_threadPool.end())
                    {
                        SparkThreadWork* pWoker = itr->second;

                        m_threadPool.erase(itr);

                        {
                            SparkLocker locker(m_lockTrashThreadPool);
                            m_trashThread.insert(std::make_pair(nThreadId, pWoker));
                        }
                    }
                }
            }

            void CleanThreadPool()
            {
                ThreadPool trashThread;

                // 清理回收站线程
                {
                    SparkLocker locker(m_lockTrashThreadPool);
                    trashThread.swap(m_trashThread);
                }

                ThreadPoolItr itr = trashThread.begin();
                while (itr != trashThread.end())
                {
                    SparkThreadWork* pThread = itr->second;

                    if (emSTWStatus_Work == pThread->GetWorkStatus())
                    {
                        itr++;
                        continue;
                    }

                    pThread->Terminate(500, 0);
                    delete pThread;

                    itr = trashThread.erase(itr);
                }

                {
                    SparkLocker locker(m_lockTrashThreadPool);
                    m_trashThread.insert(trashThread.begin(), trashThread.end());
                }
            }

        private:
            bool IsInitParamValid(int nMinThreadNum, int nMaxPendingTasks, int nMaxThreadNum)
            {
                if (nMinThreadNum < 0) return false;
                if (nMaxPendingTasks <= 0) return false;
                if (nMinThreadNum > nMaxThreadNum) return false;

                return true;
            }

            void InitParams(int nMinThreadNum, int nMaxThreadNum, int nMaxPendingTasks)
            {
                m_nMinThreadNum = nMinThreadNum;
                m_nMaxThreadNum = nMaxThreadNum;
                m_nMaxPendingTasks = nMaxPendingTasks;
                m_hExitEvt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
                m_hNotifyEvt = ::CreateSemaphore(NULL, 0, MAX_TASK_COUNT, NULL);
            }

            void CreateMsgWnd()
            {
                int nTryTime = 0;
                int nMaxTryTime = RECREATE_MSGWND_COUNT;

                while (nTryTime < nMaxTryTime)
                {
                    if (!m_msgWnd.IsWindow())
                    {
                        m_msgWnd.Create();
                        nTryTime++;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            void SetExitEvent()
            {
                if (m_hExitEvt)
                {
                    ::SetEvent(m_hExitEvt);
                }
            }

            void DestroyMsgWnd()
            {
                if (m_msgWnd.IsWindow())
                {
                    m_msgWnd.DestroyWindow();
                }
            }

            void CloseHandles()
            {
                if (m_hExitEvt)
                {
                    ::CloseHandle(m_hExitEvt);
                    m_hExitEvt = NULL;
                }
                if (m_hNotifyEvt)
                {
                    ::CloseHandle(m_hNotifyEvt);
                    m_hNotifyEvt = NULL;
                }
            }

        private:
            typedef std::map<int, SparkThreadWork *> ThreadPool;
            typedef std::list<Runnable *> Tasks;
            typedef Tasks::iterator TasksItr;
            typedef ThreadPool::iterator ThreadPoolItr;

            bool m_bIsInit;
            int m_nMinThreadNum;
            int m_nMaxThreadNum;
            int m_nMaxPendingTasks;
            int m_nMsgThreadId;

            HANDLE m_hExitEvt;
            HANDLE m_hNotifyEvt;

            ThreadPool m_threadPool;
            ThreadPool m_trashThread;
            Tasks m_tasks;

            SparkThread* m_pCleanerThread;
            SparkMsgWnd  m_msgWnd;

            SparkLock m_lockThreadPool;
            SparkLock m_lockTrashThreadPool;
            SparkLock m_lockTasks;

        };

    }
}