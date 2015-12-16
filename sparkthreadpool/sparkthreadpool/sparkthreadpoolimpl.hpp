#pragma once

#include <map>
#include <list>
#include <assert.h>
#include <iterator>
#include "sparkasyndef.hpp"
#include "sparkmsgwnd.hpp"
#include "sparkrunnable.hpp"
#include "sparkthread.hpp"
#include "sparkthreadpooldef.hpp"

#define FAILED_THEN_UNINIT_AND_RETURN(result)\
{\
if (!result){ UnInit(); return false; } \
}

namespace Spark
{
    namespace Thread
    {
        static const int MAX_TASK_COUNT = 100000;           // 最多并发任务数
        static const int RECREATE_MSGWND_COUNT = 3;         // 重试创建窗口次数
        static const int KEEP_ALIVE_TIME = 1000 * 60;       // 清理空闲线程间隔时间即1分钟清理一次空闲线程
        static const int CLEAN_POOL_INTERVAL = 1000 * 30;   // 间隔30s清理一次回收站线程

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

        class SparkThreadPoolImpl
        {
        private:
            SparkThreadPoolImpl(const SparkThreadPoolImpl&);
            void operator=(const SparkThreadPoolImpl&);
        private:
            struct RunObjRef
            {
                RunObjRef() : lRef(0l), isReleased(false) {}
                long lRef;
                bool isReleased;
                SparkLock lockRunObjOpt;
            };
            class SparkThreadWork : public SparkThread
            {
            public:
                SparkThreadWork(SparkThreadPoolImpl* pThreadPool) : m_pThreadPool(pThreadPool)
                                                                  , m_emWorkStatus(emSTWStatus_None)
                                                                  , m_lWorkRef(0)
                {
                    ::InterlockedIncrement(&m_lWorkRef);
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

                long AddWorkRef()
                {
                    return ::_InterlockedIncrement(&m_lWorkRef);
                }

                long ReleaseWorkRef()
                {
                    return ::_InterlockedDecrement(&m_lWorkRef);
                }

            private:
                SparkThreadPoolImpl* const m_pThreadPool;
                SparkThreadWorkStatus  m_emWorkStatus;
                long m_lWorkRef;

            };

        public:
            SparkThreadPoolImpl() : m_bIsInit(false)
                                  , m_nMinThreadNum(0)
                                  , m_nMaxThreadNum(0)
                                  , m_nMaxPendingTasks(0)
                                  , m_nKeepAliveTime(KEEP_ALIVE_TIME)
                                  , m_nMsgThreadId(0)
                                  , m_hExitEvt(NULL)
                                  , m_hNotifyEvt(NULL)
                                  , m_pCleanerThread(NULL)
            {

            }

            virtual ~SparkThreadPoolImpl()
            {
                UnInit();
            }

        public:
            bool Init(int nMinThreadNum, int nMaxThreadNum, int nMaxPendingTasks = 1, int nKeepAliveTime = KEEP_ALIVE_TIME)
            {
                if (m_bIsInit)
                {
                    return false;
                }
                if (!IsInitParamValid(nMinThreadNum, nMaxPendingTasks, nMaxThreadNum))
                {
                    return false;
                }

                InitParams(nMinThreadNum, nMaxThreadNum, nMaxPendingTasks, nKeepAliveTime);

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

            int DestroyThisTasks(void* lpThis)
            {
                return DestroyTasksByRunObj(lpThis);
            }

            void UnInit(DWORD dwPerWaitMilliseconds = 100)
            {
                SetExitEvent();

                DestroyCleanerThread();
                DestroyThreadPool(dwPerWaitMilliseconds);
                DestroyTasks();
                DestroyRunTasks();
                DestroyMsgWnd();

                CloseHandles();
            }

            template<typename T, typename ParamType>
            bool SwitchToWndThread(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam = NULL, bool bIsSendMsg = false)
            {
                Runnable* pTask = Spark::Thread::CreateRunnable(pObj, pFun, lpParam);

                RUNNABLE_PTR_HOST_ADDREF(pTask);

                return SwitchToWndThread(pTask, bIsSendMsg);
            }

            bool SwitchToWndThread(Runnable* pTask, bool bIsSendMsg = false)
            {
                if (!IsRunObjValid(pTask))
                {
                    SAFE_HOST_RELEASE(pTask);
                    return false;
                }

                if (m_msgWnd.IsWindow())
                {
                    if (bIsSendMsg) { m_msgWnd.SendMessage(TASK_HANDLE_MSG_ID, (WPARAM)pTask); }
                    else { m_msgWnd.PostMessage(TASK_HANDLE_MSG_ID, (WPARAM)pTask); }

                    return true;
                }

                SAFE_HOST_RELEASE(pTask);
                return false;
            }

            template<typename T, typename ParamType>
            bool Execute(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam = NULL,
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                Runnable* pTask = Spark::Thread::CreateRunnable(pObj, pFun, lpParam);

                RUNNABLE_PTR_HOST_ADDREF(pTask);
                
                return Execute(pTask, emRunnableType);
            }

            template<typename T>
            bool Execute(T* pObj, void(T::*pFun)(),
                SparkRunnableType emRunnableType = emSRType_Schedule_Post)
            {
                Runnable* pTask = Spark::Thread::CreateRunnable(pObj, pFun);

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
                if (NULL == pRunnable) return false;

                if (!m_bIsInit)
                {
                    SAFE_HOST_RELEASE(pRunnable);
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

                SAFE_HOST_RELEASE(pRunnable);

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

                    m_pCleanerThread->SetRunnable(this, &SparkThreadPoolImpl::CleanerRun);
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
                    SAFE_HOST_RELEASE(pRunnable);
                    itr++;
                }
                m_tasks.clear();
            }

            void DestroyRunTasks()
            {
                SparkLocker locker(m_lockRunTasks);
                m_runTasks.clear();
            }

            int DestroyTasksByRunObj(void* lpRunObj)
            {
                int nDeleteCount = 0;

                ReleseRunObj(lpRunObj);
                RunObjRef* pRunObjRef = FindRunObjRef(lpRunObj);
                if (NULL != pRunObjRef)
                {
                    SparkLocker locker(pRunObjRef->lockRunObjOpt);
                    nDeleteCount = DoDestroyTasksByRunObj(lpRunObj);
                }
                else
                {
                    nDeleteCount = DoDestroyTasksByRunObj(lpRunObj);
                }
                return nDeleteCount;
            }

            int DoDestroyTasksByRunObj(void* lpRunObj)
            {
                int nDeleteCount = 0;

                nDeleteCount = DestroyWaitTasksByRunObj(lpRunObj);
                nDeleteCount += DestroyRunTasksByRunObj(lpRunObj);

                // 再次检查有没有刚加入新的等待任务，
                // 避免正在跑的任务完成后又添加新的任务
                nDeleteCount += DestroyWaitTasksByRunObj(lpRunObj);

                return nDeleteCount;
            }

            int DestroyRunTasksByRunObj(void* lpRunObj)
            {
                int nDeleteCount = 0;

                Tasks tasks;
                {
                    SparkLocker locker(m_lockRunTasks);
                    TasksItr itr = m_runTasks.begin();
                    while (itr != m_runTasks.end())
                    {
                        Runnable* pRunnable = *itr;
                        if (NULL != pRunnable && lpRunObj == pRunnable->GetRunObj())
                        {
                            pRunnable->AddRef();
                            tasks.push_back(pRunnable);
                        }
                        itr++;
                    }
                }

                TasksItr itr = tasks.begin();
                while (itr != tasks.end())
                {
                    Runnable* pRunnable = *itr;
                    if (lpRunObj == pRunnable->GetRunObj())
                    {
                        SAFE_RELEASE_RUN_OBJ(pRunnable);
                        RemoveRunTask(pRunnable);
                        SAFE_HOST_RELEASE(pRunnable);
                        itr = tasks.erase(itr);
                        nDeleteCount++;
                        continue;
                    }
                    itr++;
                }
                tasks.clear();

                long lAddRefCount = -1 * nDeleteCount;
                UpdateRunObjRef(lpRunObj, lAddRefCount);

                return nDeleteCount;
            }

            int DestroyWaitTasksByRunObj(void* lpRunObj)
            {
                int nDeleteCount = 0;

                SparkLocker locker(m_lockTasks);

                TasksItr itr = m_tasks.begin();
                while (itr != m_tasks.end())
                {
                    Runnable* pRunnable = *itr;
                    if (lpRunObj == pRunnable->GetRunObj())
                    {
                        SAFE_RELEASE_RUN_OBJ(pRunnable);
                        itr = m_tasks.erase(itr);
                        nDeleteCount++;
                        continue;
                    }
                    itr++;
                }

                long lAddRefCount = -1 * nDeleteCount;
                UpdateRunObjRef(lpRunObj, lAddRefCount);

                return nDeleteCount;
            }

            void AddTaskAndNotify(Runnable* pRunnable)
            {
                AddTask(pRunnable); 
                NotifyAddTask();
            }

            void AddTask(Runnable* pRunnable)
            {
                SparkLocker locker(m_lockTasks);
                UpdateRunObjRef(pRunnable->GetRunObj(), 1);
                m_tasks.push_back(pRunnable);
            }

            Runnable* FindTask(void* lpRunObj)
            {
                SparkLocker locker(m_lockTasks);

                Runnable* pFindRunnable = NULL;
                TasksItr itr = m_tasks.begin();
                while (itr != m_tasks.end())
                {
                    pFindRunnable = *itr;
                    if (lpRunObj == pFindRunnable->GetRunObj())
                    {
                        itr++;
                        break;
                    }
                    itr++;
                }

                return pFindRunnable;
            }

            bool IsRunObjValid(Runnable* pRunnable)
            {
                if (NULL == pRunnable) { return false; }

                RunObjRef* pRunObjRef = FindRunObjRef(pRunnable->GetRunObj());
                if (NULL == pRunObjRef) { return false; }
                if (pRunObjRef->isReleased) { return false; }

                return true;
            }

            void UpdateRunObjRef(Runnable* pRunnable, long lRefCount)
            {
                if (NULL == pRunnable) { return; }

                UpdateRunObjRef(pRunnable->GetRunObj(), lRefCount);
            }

            void UpdateRunObjRef(void* lpRunObj, long lRefCount)
            {
                SparkLocker lockerObjRef(m_lockObjRefMap);

                RunObjRef* pObjRef = FindRunObjRef(lpRunObj);
                if (NULL == pObjRef)
                {
                    pObjRef = new RunObjRef;
                    m_objRefMap[lpRunObj] = pObjRef;
                }

                pObjRef->lRef += lRefCount;
                if (0 == pObjRef->lRef)
                {
                    delete pObjRef;
                    m_objRefMap.erase(lpRunObj);
                }
            }

            void ReleseRunObj(void* lpRunObj)
            {
                SparkLocker lockerObjRef(m_lockObjRefMap);

                RunObjRef* pObjRef = FindRunObjRef(lpRunObj);
                if (NULL != pObjRef)
                {
                    pObjRef->isReleased = true;
                }
            }

            RunObjRef* FindRunObjRef(void* lpRunObj)
            {
                SparkLocker locker(m_lockObjRefMap);

                RunObjRef* pRef = NULL;
                ObjRefMapItr itr = m_objRefMap.find(lpRunObj);
                if (itr != m_objRefMap.end())
                {
                    pRef = itr->second;
                }
                return pRef;
            }

            void AddRunTask(Runnable* pRunnable)
            {
                SparkLocker locker(m_lockRunTasks);
                m_runTasks.push_back(pRunnable);
            }

            void RemoveRunTask(Runnable* pRunnable)
            {
                SparkLocker locker(m_lockRunTasks);

                TasksItr itr = m_runTasks.begin();
                while (itr != m_runTasks.end())
                {
                    Runnable* pTmp = *itr;
                    if (pRunnable == pTmp)
                    {
                        itr = m_runTasks.erase(itr);
                        break;
                    }
                    itr++;
                }
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

                    DWORD dwRet = ::WaitForMultipleObjects(2, hWaitEvt, FALSE, m_nKeepAliveTime);
                    if (IsShouldExitRun(dwRet))
                    {
                        break;
                    }
                    if (IsShouldRecycleThread(dwRet))
                    {
                        RecycleThreadPool();
                        continue;
                    }

                    if (pWorkThread->AddWorkRef() <= 1)
                    {
                        pWorkThread->ReleaseWorkRef();
                        continue;
                    }

                    Runnable* pTask = GetTask();
                    if (pTask)
                    {
                        BeforeExecuteRun(pWorkThread, pTask);
                        ExecuteRun(pWorkThread, pTask);
                        AfterExecuteRun(pWorkThread, pTask);
                    }

                    pWorkThread->ReleaseWorkRef();
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
                AddRunTask(pTask);
            }

            void ExecuteRun( SparkThreadWork* pWorkThread, Runnable* pTask )
            {
                RunObjRef* pRunObjRef = FindRunObjRef(pTask->GetRunObj());
                if (NULL == pRunObjRef) { return; }

                SparkLocker locker(pRunObjRef->lockRunObjOpt);

                if (!IsRunObjValid(pTask)) { return; }

                if (pTask)
                {
                    pTask->Run();
                }
            }

            void AfterExecuteRun( SparkThreadWork* pWorkThread, Runnable* pTask )
            {
                UpdateRunObjRef(pTask, -1);
                RemoveRunTask(pTask);
                SAFE_HOST_RELEASE(pTask);
                ResetWorkThreadStatus(pWorkThread);
            }

            void CleanerRun(void* pParam)
            {
                for (;;)
                {
                    DWORD dwRet = ::WaitForSingleObject(m_hExitEvt, CLEAN_POOL_INTERVAL);
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

                    // 线程工作中
                    if (pThread->ReleaseWorkRef() >= 1)
                    {
                        pThread->AddWorkRef();
                        itr++;
                        continue;
                    }

                    pThread->Terminate(500, 0);
                    pThread->AddWorkRef();
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

            void InitParams(int nMinThreadNum, int nMaxThreadNum, int nMaxPendingTasks, int nKeepAliveTime)
            {
                m_nMinThreadNum = nMinThreadNum;
                m_nMaxThreadNum = nMaxThreadNum;
                m_nMaxPendingTasks = nMaxPendingTasks;
                m_nKeepAliveTime = nKeepAliveTime;
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
            typedef std::map<void*, RunObjRef*> ObjRefMap;
            typedef std::map<int, SparkThreadWork *> ThreadPool;
            typedef std::list<Runnable *> Tasks;
            typedef Tasks::iterator TasksItr;
            typedef ThreadPool::iterator ThreadPoolItr;
            typedef ObjRefMap::iterator ObjRefMapItr;

            bool m_bIsInit;
            int m_nMinThreadNum;
            int m_nMaxThreadNum;
            int m_nMaxPendingTasks;
            int m_nMsgThreadId;
            int m_nKeepAliveTime;

            HANDLE m_hExitEvt;
            HANDLE m_hNotifyEvt;

            ThreadPool m_threadPool;
            ThreadPool m_trashThread;
            ObjRefMap m_objRefMap;
            Tasks m_tasks;
            Tasks m_runTasks;

            SparkThread* m_pCleanerThread;
            SparkMsgWnd  m_msgWnd;

            SparkLock m_lockThreadPool;
            SparkLock m_lockTrashThreadPool;
            SparkLock m_lockTasks;
            SparkLock m_lockRunTasks;
            SparkLock m_lockObjRefMap;

        };

    }
}