/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
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
            virtual int GetLimitRunCount() const = 0;
            virtual int GetElapse() const = 0;
            virtual int GetWhen() const = 0;

            virtual void SetRunCount(int nValue) = 0;
            virtual void SetLimitRunCount(int nValue) = 0;
            virtual void AddRunCount() = 0;
            virtual void SetElapse(int nValue) = 0;
            virtual void SetWhen(int nValue) = 0;
            virtual void AddWhen(int nValue) = 0;

            virtual bool IsStop() = 0;
            virtual void Stop() = 0;
        };
        
        template<typename T, typename ParamType>
        class MemberSparkTimerTask : public SparkTimerTask
        {
        public:
            typedef void (T::*RunFun)(ParamType);
            typedef void (T::*NoParamRunFun)();

            MemberSparkTimerTask(T* pObj, RunFun pFun, ParamType lpParam = NULL)
            {
                m_pMemberFun = new MemberFunPtrRunnable<T, ParamType>(pObj, pFun, lpParam);
                m_pMemberFun->AddRef();

                _InitParam();
            }

            MemberSparkTimerTask(T* pObj, NoParamRunFun pFun)
            {
                m_pMemberFun = new MemberFunPtrRunnable<T, void*>(pObj, pFun);
                m_pMemberFun->AddRef();

                _InitParam();
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

            virtual void* GetRunObj()
            {
                if (NULL == m_pMemberFun) { return NULL; }

                return m_pMemberFun->GetRunObj();
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

            virtual int GetElapse() const
            {
                return m_nElapse;
            }

            virtual void SetElapse(int nValue)
            {
                m_nElapse = nValue;
            }

            virtual int GetWhen() const
            {
                return m_nWhen;
            }

            virtual void SetWhen(int nValue)
            {
                m_nWhen = nValue;
            }

            virtual void AddWhen(int nValue)
            {
                m_nWhen += nValue;
            }

            virtual bool IsStop()
            {
                return m_bIsStop;
            }

            virtual void Stop()
            {
                m_bIsStop = true;
            }

        private:
            void _InitParam()
            {
                m_nRunCount = 0;
                m_nLimitRunCount = 0;
                m_nElapse = 0;
                m_nWhen = 0;
                m_bIsStop = false;
            }

        private:
            MemberFunPtrRunnable<T, ParamType>* m_pMemberFun;
            int m_nRunCount;
            int m_nLimitRunCount;
            int m_nElapse;
            int m_nDelay;
            int m_nPeriod;
            int m_nWhen;
            bool m_bIsStop; 
        };

        template<typename T, typename ParamType>
        inline SparkTimerTask* CreateTimerTask(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam = NULL)
        {
            SparkTimerTask *pTask = new MemberSparkTimerTask<T, ParamType>(pObj, pFun, lpParam);

            return pTask;
        };

        template<typename T>
        inline SparkTimerTask* CreateTimerTask(T* pObj, void(T::*pFun)())
        {
            SparkTimerTask *pTask = new MemberSparkTimerTask<T, void*>(pObj, pFun);

            return pTask;
        };
    }
}
