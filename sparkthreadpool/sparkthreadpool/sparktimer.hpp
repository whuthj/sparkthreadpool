#pragma once

#include "sparkmsgwnd.hpp"
#include "sparksynccontainer.hpp"
#include "sparktimertask.hpp"

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

            template<typename T>
            bool StartTimer(T* pObj, void(T::*pFun)(void*), void* lpParam, UINT nElapse, int nRunCount = 0)
            {
                if (nRunCount < 0) { return false; }

                StopTimer();
                SparkTimerTask* pTask = CreateTimerTask(pObj, pFun, lpParam);

                RUNNABLE_PTR_HOST_ADDREF(pTask);
                pTask->SetLimitRunCount(nRunCount);

                s_wnd.Create(SPARK_TIMER_WND_CLASS_NAME);
                m_lTimerId = s_wnd.StartTimer(pTask, nElapse);

                return true;
            }

            template<typename T, typename ParamType = void*>
            static long Schedule(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam, UINT nElapse, int nRunCount = 0)
            {
                SparkTimerTask* pTask = CreateTimerTask<T, ParamType>(pObj, pFun, lpParam);

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

        class SparkThreadTimer
        {

        };
    }
}