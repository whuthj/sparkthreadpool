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
                    if (pRunnable)
                    {
                        pRunnable->AddRunCount();
                        pRunnable->Run();
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
                    long lTimerIndex = m_mapTimerTask.Count();
                    SetTimer(lTimerIndex, nElapse);
                    m_mapTimerTask.Push(lTimerIndex, pTask);

                    return lTimerIndex;
                }

                void StopTimer(long lTimerIndex)
                {
                    SparkTimerTask* pRunnable = m_mapTimerTask.Get(lTimerIndex);
                    if (NULL == pRunnable) { return; }

                    if (pRunnable && pRunnable->IsBeHosted())
                    {
                        pRunnable->Release();
                    }
                    KillTimer(lTimerIndex);
                    m_mapTimerTask.Remove(lTimerIndex);
                }

                int GetTimerRunCount(long lTimerIndex)
                {
                    int nRunCount = 0;
                    SparkTimerTask* pRunnable = m_mapTimerTask.Get(lTimerIndex);
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
            void StartTimer(T* pObj, void(T::*pFun)(void*), void* lpParam, UINT nElapse)
            {
                StopTimer();
                SparkTimerTask* pTask = CreateTimerTask(pObj, pFun, lpParam);

                pTask->SetBeHosted(true);
                pTask->AddRef();

                s_wnd.Create(SPARK_TIMER_WND_CLASS_NAME);
                m_lTimerIndex = s_wnd.StartTimer(pTask, nElapse);
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
