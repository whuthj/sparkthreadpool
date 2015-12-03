#pragma once

#include "sparkrunnable.hpp"

namespace Spark
{
    namespace Thread
    {
        class SparkTimerTask : public Runnable
        {
        public:
            virtual ~SparkTimerTask() {};
            virtual int GetRunCount() const = 0;
            virtual void SetRunCount(int nValue) = 0;
            virtual int GetLimitRunCount() const = 0;
            virtual void SetLimitRunCount(int nValue) = 0;
            virtual void AddRunCount() = 0;
        };
        
        template<typename T>
        class MemberSparkTimerTask : public SparkTimerTask
        {
        public:
            typedef void (T::*RunFun)(void* pParam);

            MemberSparkTimerTask(T* pObj, RunFun pFun, void* lpParam = NULL)
            {
                m_pMemberFun = new MemberFunPtrRunnable<T>(pObj, pFun, lpParam);
                m_pMemberFun->AddRef();

                m_nRunCount = 0;
                m_nLimitRunCount = 0;
            }

            virtual ~MemberSparkTimerTask()
            {
                if (NULL != m_pMemberFun)
                {
                    m_pMemberFun->Release();
                }
            }

            virtual void Run()
            {
                if (NULL != m_pMemberFun)
                {
                    m_pMemberFun->Run();
                }
            }

            virtual int GetRunCount() const
            {
                return m_nRunCount;
            }

            virtual void SetRunCount(int nValue)
            {
                m_nRunCount = nValue;
            }

            virtual int GetLimitRunCount() const
            {
                return m_nLimitRunCount;
            }

            virtual void SetLimitRunCount(int nValue)
            {
                m_nLimitRunCount = nValue;
            }

            virtual void AddRunCount()
            {
                ++m_nRunCount;
            }

            virtual void* GetRunObj()
            {
                if (NULL == m_pMemberFun) { return NULL; }

                return m_pMemberFun->GetRunObj();
            }

        private:
            MemberFunPtrRunnable<T>* m_pMemberFun;
            int m_nRunCount;
            int m_nLimitRunCount;

        };

        template<typename T>
        inline SparkTimerTask* CreateTimerTask(T* pObj, void(T::*pFun)(void*), void* lpParam = NULL)
        {
            SparkTimerTask *pTask = new MemberSparkTimerTask<T>(pObj, pFun, lpParam);

            return pTask;
        };
    }
}
