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
                    long lTimerId = m_mapTimerTask.Count();
                    SetTimer(lTimerId, nElapse);
                    m_mapTimerTask.Push(lTimerId, pTask);

                    return lTimerId;
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

            private:
                SparkSyncMap<long, SparkTimerTask*> m_mapTimerTask;

            };
        public:
            SparkWndTimer() 
            {
                m_lTimerIndex = -1;
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
                m_lTimerIndex = s_wnd.StartTimer(pTask, nElapse);

                return true;
            }

            void StopTimer()
            {
                s_wnd.StopTimer(m_lTimerIndex);
            }

            int GetRunCount()
            {
                return s_wnd.GetTimerRunCount(m_lTimerIndex);
            }

        private:
            static TimerWnd s_wnd;
            long m_lTimerIndex;
        };

        __declspec(selectany) SparkWndTimer::TimerWnd SparkWndTimer::s_wnd;

        class SparkThreadTimer
        {

        };
    }
}