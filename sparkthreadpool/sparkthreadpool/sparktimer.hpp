/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

#include "sparkmsgwnd.hpp"
#include "sparksynccontainer.hpp"
#include "sparktimertask.hpp"
#include "sparkthread.hpp"
#include <vector>

namespace Spark
{
    namespace Thread
    {
        static const LPCWSTR SPARK_TIMER_WND_CLASS_NAME = L"SparkWndTimer";

        class SparkWndTimer
        {
        private:
            class TimerWnd : public SparkMsgWnd
            {
            public:
                TimerWnd()
                {
                    m_lTimerId = -1;
                }

                virtual ~TimerWnd()
                {
                    DestroyWindow();
                    DestroyTimerTasks();
                }

            protected:
                virtual void OnTimer(UINT_PTR nTimerId)
                {
                    SparkTimerTask* pRunnable = m_mapTimerTask.Get(nTimerId);
                    if (NULL == pRunnable)
                    {
                        return;
                    }

                    pRunnable->AddRunCount();
                    pRunnable->Run();

                    if (pRunnable->GetLimitRunCount() <= 0)
                    {
                        return;
                    }

                    if (pRunnable->GetLimitRunCount() <= pRunnable->GetRunCount())
                    {
                        StopTimer(nTimerId);
                    }
                }

                void DestroyTimerTasks()
                {
                    m_mapTimerTask.Lock();
                    std::map<long, SparkTimerTask*>& map = m_mapTimerTask.GetMap();
                    std::map<long, SparkTimerTask*>::iterator itr = map.begin();
                    while (itr != map.end())
                    {
                        SparkTimerTask* pRunnable = itr->second;
                        SAFE_HOST_RELEASE(pRunnable);
                        itr++;
                    }
                    m_mapTimerTask.Clear();
                    m_mapTimerTask.Unlock();
                }

            public:
                long StartTimer(SparkTimerTask* pTask, UINT nElapse)
                {
                    ::InterlockedIncrement(&m_lTimerId);
                    SetTimer(m_lTimerId, nElapse);
                    m_mapTimerTask.Push(m_lTimerId, pTask);

                    return m_lTimerId;
                }

                void StopTimer(long lTimerId)
                {
                    SparkTimerTask* pRunnable = m_mapTimerTask.Get(lTimerId);
                    if (NULL == pRunnable) { return; }

                    if (pRunnable && pRunnable->IsBeHosted())
                    {
                        pRunnable->Release();
                    }
                    KillTimer(lTimerId);
                    m_mapTimerTask.Remove(lTimerId);
                }

                int GetTimerRunCount(long lTimerId)
                {
                    int nRunCount = 0;
                    SparkTimerTask* pRunnable = m_mapTimerTask.Get(lTimerId);
                    if (pRunnable)
                    {
                        nRunCount = pRunnable->GetRunCount();
                    }

                    return nRunCount;
                }

                int DestroyTimerTask(void* lpThis)
                {
                    int nDeleteCount = 0;

                    m_mapTimerTask.Lock();
                    std::map<long, SparkTimerTask*>& map = m_mapTimerTask.GetMap();
                    std::map<long, SparkTimerTask*>::iterator itr = map.begin();
                    while (itr != map.end())
                    {
                        SparkTimerTask* pRunnable = itr->second;
                        if (lpThis == pRunnable->GetRunObj())
                        {
                            SAFE_HOST_RELEASE(pRunnable);
                            itr = map.erase(itr);
                            nDeleteCount++;
                            continue;
                        }
                        itr++;
                    }
                    m_mapTimerTask.Unlock();

                    return nDeleteCount;
                }

            private:
                SparkSyncMap<long, SparkTimerTask*> m_mapTimerTask;
                volatile long m_lTimerId;

            };
        public:
            SparkWndTimer() 
            {
                m_lTimerId = -1;
            }

            template<typename T, typename ParamType>
            bool StartTimer(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam, UINT nElapse, int nRunCount = 0)
            {
                if (nRunCount < 0) { return false; }

                StopTimer();
                SparkTimerTask* pTask = CreateTimerTask<T, ParamType>(pObj, pFun, lpParam);

                RUNNABLE_PTR_HOST_ADDREF(pTask);
                pTask->SetLimitRunCount(nRunCount);

                s_wnd.Create(SPARK_TIMER_WND_CLASS_NAME);
                m_lTimerId = s_wnd.StartTimer(pTask, nElapse);

                return true;
            }

            template<typename T>
            bool StartTimer(T* pObj, void(T::*pFun)(), UINT nElapse, int nRunCount = 0)
            {
                if (nRunCount < 0) { return false; }

                StopTimer();
                SparkTimerTask* pTask = CreateTimerTask<T>(pObj, pFun);

                RUNNABLE_PTR_HOST_ADDREF(pTask);
                pTask->SetLimitRunCount(nRunCount);

                s_wnd.Create(SPARK_TIMER_WND_CLASS_NAME);
                m_lTimerId = s_wnd.StartTimer(pTask, nElapse);

                return true;
            }

            template<typename T, typename ParamType>
            static long Schedule(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam, UINT nElapse, int nRunCount = 0)
            {
                SparkTimerTask* pTask = CreateTimerTask<T, ParamType>(pObj, pFun, lpParam);

                RUNNABLE_PTR_HOST_ADDREF(pTask);
                pTask->SetLimitRunCount(nRunCount);

                s_wnd.Create(SPARK_TIMER_WND_CLASS_NAME);
                return s_wnd.StartTimer(pTask, nElapse);
            }

            template<typename T>
            static long Schedule(T* pObj, void(T::*pFun)(), UINT nElapse, int nRunCount = 0)
            {
                SparkTimerTask* pTask = CreateTimerTask<T>(pObj, pFun);

                RUNNABLE_PTR_HOST_ADDREF(pTask);
                pTask->SetLimitRunCount(nRunCount);

                s_wnd.Create(SPARK_TIMER_WND_CLASS_NAME);
                return s_wnd.StartTimer(pTask, nElapse);
            }

            static void StopTimer(long lTimerId)
            {
                s_wnd.StopTimer(lTimerId);
            }

            static int DestroyThisTimerTask(void* lpThis)
            {
                return s_wnd.DestroyTimerTask(lpThis);
            }

            void StopTimer()
            {
                s_wnd.StopTimer(m_lTimerId);
            }

            int GetRunCount()
            {
                return s_wnd.GetTimerRunCount(m_lTimerId);
            }

        private:
            static TimerWnd s_wnd;
            long m_lTimerId;
        };

        __declspec(selectany) SparkWndTimer::TimerWnd SparkWndTimer::s_wnd;

        class SparkTimerHeap
        {
        public:
            SparkTimerTask* Minimum() 
            {
                SparkLocker lock(m_taskLock);

                return m_vecTimerTask[0];
            }

            bool IsEmpty() 
            {
                SparkLocker lock(m_taskLock);

                return m_vecTimerTask.empty();
            }

            void Insert(SparkTimerTask* pTask) 
            {
                SparkLocker lock(m_taskLock);

                m_vecTimerTask.push_back(pTask);
                UpHeap();
            }

            void Delete(int pos) 
            {
                SparkLocker lock(m_taskLock);

                std::vector<SparkTimerTask*>::iterator _itr = m_vecTimerTask.begin() + pos;
                if (_itr != m_vecTimerTask.end())
                {
                    m_vecTimerTask.erase(_itr);
                }
            }

            void UpHeap() 
            {
                SparkLocker lock(m_taskLock);

                int size = m_vecTimerTask.size();
                int current = size - 1;
                int parent = (current - 1) / 2;

                while (m_vecTimerTask[current]->GetWhen() < m_vecTimerTask[parent]->GetWhen())
                {
                    SparkTimerTask* pTmp = m_vecTimerTask[current];
                    m_vecTimerTask[current] = m_vecTimerTask[parent];
                    m_vecTimerTask[parent] = pTmp;

                    current = parent;
                    parent = (current - 1) / 2;
                }
            }

            void DownHeap(int pos) 
            {
                SparkLocker lock(m_taskLock);

                int current = pos;
                int child = 2 * current + 1;
                int size = m_vecTimerTask.size();

                while (child < size && size > 0)
                {
                    if (child + 1 < size
                        && m_vecTimerTask[child + 1]->GetWhen() < m_vecTimerTask[child]->GetWhen()) {
                            child++;
                    }

                    if (m_vecTimerTask[current]->GetWhen() < m_vecTimerTask[child]->GetWhen()) {
                        break;
                    }

                    SparkTimerTask* pTmp = m_vecTimerTask[current];
                    m_vecTimerTask[current] = m_vecTimerTask[child];
                    m_vecTimerTask[child] = pTmp;

                    current = child;
                    child = 2 * current + 1;
                }
            }

            void Reset()
            {
                SparkLocker lock(m_taskLock);
                m_vecTimerTask.clear();
            }

            void AdjustMinimum() 
            {
                DownHeap(0);
            }

        private:
            std::vector<SparkTimerTask*> m_vecTimerTask;
            SparkLock m_taskLock;
            int m_nDeletedCancelledNumber;
        };

        class SparkTimerThread : public SparkThread
        {
        public:
            SparkTimerThread()
            {
                m_hExitEvt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
                m_hNotifyEvt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
            }

            virtual ~SparkTimerThread()
            {
                if (m_hExitEvt)
                {
                    ::SetEvent(m_hExitEvt);
                }
                Join();

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

        public:
            bool AddTaskAndNotify(SparkTimerTask* pTask, UINT nElapse, int nRunCount)
            {
                if (NULL == pTask)
                {
                    return false;
                }

                RUNNABLE_PTR_HOST_ADDREF(pTask);
                pTask->SetLimitRunCount(nRunCount);
                pTask->SetElapse(nElapse);
                int nTimeWhen = ::GetTickCount() + pTask->GetElapse();
                pTask->SetWhen(nTimeWhen);

                m_timerHeap.Insert(pTask);
                _NotifyAddTask();

                return true;
            }

        protected:
            virtual void Run()
            {
                int nTimeToSleep = 0;
                bool bIsStop = false;
                DWORD dwRet = 0;
                SparkTimerTask* pTask = NULL;
                HANDLE hWaitEvt[] = { m_hExitEvt, m_hNotifyEvt };

                for (;;)
                {
                    if (m_timerHeap.IsEmpty())
                    {
                        ::WaitForMultipleObjects(2, hWaitEvt, FALSE, INFINITE);
                    }

                    pTask = m_timerHeap.Minimum();
                    nTimeToSleep = _CalcTimeToSleep(pTask);
                    m_timerHeap.Delete(0);
                    m_timerHeap.AdjustMinimum();

                    bIsStop = _IsTimeTaskStop(pTask);
                    if (bIsStop)
                    {
                        SAFE_HOST_RELEASE(pTask);
                        _NotifyRemoveTask();
                        continue;
                    }

                    dwRet = ::WaitForSingleObject(m_hExitEvt, nTimeToSleep);
                    if (WAIT_TIMEOUT != dwRet)
                    {
                        break;
                    }

                    pTask->AddRunCount();
                    pTask->Run();
                    pTask->AddWhen(pTask->GetElapse());

                    bIsStop = _IsTimeTaskStop(pTask);
                    if (bIsStop)
                    {
                        SAFE_HOST_RELEASE(pTask);
                        _NotifyRemoveTask();
                        continue;
                    }

                    m_timerHeap.Insert(pTask);
                }
            }

        private:
            void _NotifyAddTask()
            {
                if (m_hNotifyEvt)
                {
                    ::SetEvent(m_hNotifyEvt);
                }
            }

            void _NotifyRemoveTask()
            {
                if (m_hNotifyEvt)
                {
                    ::ResetEvent(m_hNotifyEvt);
                }
            }

            int _CalcTimeToSleep( SparkTimerTask* pTask )
            {
                int nTimeWhen = pTask->GetWhen();
                int nTimeToSleep = nTimeWhen - ::GetTickCount();
                nTimeToSleep = nTimeToSleep > 0 ? nTimeToSleep : 0;

                return nTimeToSleep;
            }

            bool _IsTimeTaskStop(SparkTimerTask* pTask)
            {
                bool bIsStop = pTask->IsStop();

                if (pTask->GetLimitRunCount() > 0 && pTask->GetLimitRunCount() <= pTask->GetRunCount())
                {
                    bIsStop = true;
                }

                return bIsStop;
            }

        private:
            SparkThread m_thread;
            SparkTimerHeap m_timerHeap;
            HANDLE m_hExitEvt;
            HANDLE m_hNotifyEvt;
        };

        class SparkThreadTimer
        {
        public:
            SparkThreadTimer()
            {
                m_pTimeTask = NULL;
                s_timerThread.SingletonStart();
            }

            virtual ~SparkThreadTimer()
            {
                _ReleaseTimer();
            }

        public:
            template<typename T, typename ParamType>
            bool StartTimer(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam, UINT nElapse, int nRunCount = 0)
            {
                if (nRunCount < 0) { return false; }

                _ReleaseTimer();
                m_pTimeTask = CreateTimerTask<T>(pObj, pFun, lpParam);
                RUNNABLE_PTR_HOST_ADDREF(m_pTimeTask);

                s_timerThread.AddTaskAndNotify(m_pTimeTask, nElapse, nRunCount);

                return true;
            }

            template<typename T>
            bool StartTimer(T* pObj, void(T::*pFun)(), UINT nElapse, int nRunCount = 0)
            {
                if (nRunCount < 0) { return false; }

                _ReleaseTimer();
                m_pTimeTask = CreateTimerTask<T>(pObj, pFun);
                RUNNABLE_PTR_HOST_ADDREF(m_pTimeTask);

                s_timerThread.AddTaskAndNotify(m_pTimeTask, nElapse, nRunCount);

                return true;
            }

            void StopTimer()
            {
                if (m_pTimeTask)
                {
                    m_pTimeTask->Stop();
                }
            }

        private:
            void _ReleaseTimer()
            {
                StopTimer();
                SAFE_HOST_RELEASE(m_pTimeTask);
            }

        private:
            static SparkTimerThread s_timerThread;
            SparkTimerTask* m_pTimeTask;
        };

        __declspec(selectany) Spark::Thread::SparkTimerThread SparkThreadTimer::s_timerThread;

    }
}