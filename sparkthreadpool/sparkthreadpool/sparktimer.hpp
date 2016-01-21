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
                return m_vecTimerTask[0];
            }

            bool IsEmpty() 
            {
                return m_vecTimerTask.empty();
            }

            void Insert(SparkTimerTask* pTask) 
            {
                m_vecTimerTask.push_back(pTask);
                UpHeap();
            }

            void Delete(int pos) 
            {
                int size = m_vecTimerTask.size();
                if (pos >= 0 && pos < size) 
                {
                    m_vecTimerTask[pos] = m_vecTimerTask[--size];
                    SparkTimerTask* pTmp = m_vecTimerTask[size];
                    SAFE_HOST_RELEASE(pTmp);
                    m_vecTimerTask.erase(m_vecTimerTask.end());
                    DownHeap(pos);
                }
            }

            void UpHeap() 
            {
                int size = m_vecTimerTask.size();
                int current = size - 1;
                int parent = (current - 1) / 2;

                while (m_vecTimerTask[current]->GetElapse() < m_vecTimerTask[parent]->GetElapse())
                {
                    // swap the two
                    SparkTimerTask* pTmp = m_vecTimerTask[current];
                    m_vecTimerTask[current] = m_vecTimerTask[parent];
                    m_vecTimerTask[parent] = pTmp;

                    // update pos and current
                    current = parent;
                    parent = (current - 1) / 2;
                }
            }

            void DownHeap(int pos) 
            {
                int current = pos;
                int child = 2 * current + 1;
                int size = m_vecTimerTask.size();

                while (child < size && size > 0)
                {
                    // compare the children if they exist
                    if (child + 1 < size
                        && m_vecTimerTask[child + 1]->GetElapse() < m_vecTimerTask[child]->GetElapse()) {
                            child++;
                    }

                    // compare selected child with parent
                    if (m_vecTimerTask[current]->GetElapse() < m_vecTimerTask[child]->GetElapse()) {
                        break;
                    }

                    // swap the two
                    SparkTimerTask* pTmp = m_vecTimerTask[current];
                    m_vecTimerTask[current] = m_vecTimerTask[child];
                    m_vecTimerTask[child] = pTmp;

                    // update pos and current
                    current = child;
                    child = 2 * current + 1;
                }
            }

            void Reset()
            {
                m_vecTimerTask.clear();
            }

            void AdjustMinimum() 
            {
                DownHeap(0);
            }

            void DeleteIfCancelled() 
            {
                //for (int i = 0; i < size; i++) {
                //    if (timers[i].cancelled) {
                //        deletedCancelledNumber++;
                //        delete(i);
                //        // re-try this point
                //        i--;
                //    }
                //}
            }

            int GetTask(SparkTimerTask* pTask) 
            {
                /*for (int i = 0; i < timers.length; i++) {
                    if (timers[i] == task) {
                        return i;
                    }
                }
                return -1;*/
            }
        private:
            std::vector<SparkTimerTask*> m_vecTimerTask;
            int m_nDeletedCancelledNumber;

        };

        class SparkThreadTimer
        {
        public:
            SparkThreadTimer()
            {
                m_hExitEvt = ::CreateEvent(NULL, TRUE, FALSE, NULL);
                m_hNotifyEvt = ::CreateEvent(NULL, FALSE, FALSE, NULL);
                m_thread.Start(this, &SparkThreadTimer::Tick);
            }

            virtual ~SparkThreadTimer()
            {
                if (m_hExitEvt)
                {
                    ::SetEvent(m_hExitEvt);
                }
                m_thread.Join();

                if (m_hExitEvt)
                {
                    ::CloseHandle(m_hExitEvt);
                    m_hExitEvt = NULL;
                }
            }

        public:
            template<typename T, typename ParamType>
            bool StartTimer(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam, UINT nElapse, int nRunCount = 0)
            {
                if (nRunCount < 0) { return false; }

                StopTimer();
                SparkTimerTask* pTask = CreateTimerTask<T>(pObj, pFun, lpParam);

                RUNNABLE_PTR_HOST_ADDREF(pTask);
                pTask->SetLimitRunCount(nRunCount);
                pTask->SetElapse(nElapse);
                m_timerHeap.Insert(pTask);
                ::SetEvent(m_hNotifyEvt);

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
                pTask->SetElapse(nElapse);
                m_timerHeap.Insert(pTask);
                ::SetEvent(m_hNotifyEvt);

                return true;
            }

            void StopTimer()
            {
                
            }

        private:
            void Tick()
            {
                DWORD dwRet = 0;
                int nElapse = 1000;
                SparkTimerTask* pTask = NULL;
                HANDLE hWaitEvt[] = { m_hExitEvt, m_hNotifyEvt };

                for (;;)
                {
                    if (m_timerHeap.IsEmpty())
                    {
                        ::WaitForMultipleObjects(2, hWaitEvt, FALSE, INFINITE);
                    }

                    pTask = m_timerHeap.Minimum();
                    nElapse = pTask->GetElapse();

                    dwRet = ::WaitForMultipleObjects(2, hWaitEvt, FALSE, nElapse);
                    if (WAIT_TIMEOUT != dwRet)
                    {
                        break;
                    }

                    pTask->Run();
                }
            }

        private:
            SparkThread m_thread;
            SparkTimerHeap m_timerHeap;

            HANDLE   m_hExitEvt;
            HANDLE   m_hNotifyEvt;
        };
    }
}