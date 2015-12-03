#pragma once

//#include <WinBase.h>

#define RUNNABLE_PTR_HOST_ADDREF(pRunnable)\
{\
if (NULL == pRunnable){ return false; } pRunnable->SetBeHosted(true); pRunnable->AddRef(); \
}

#define SAFE_HOST_RELEASE(pObj) \
{\
if (pObj && pObj->IsBeHosted()) pObj->Release(); \
}

namespace Spark
{
    namespace Thread
    {
        class AddRefReleaseRunnable
        {
        public:
            AddRefReleaseRunnable() : m_bIsBeHosted(false), m_lRef(0){}
            virtual ~AddRefReleaseRunnable() {}

        public:
            virtual void AddRef(){ ::InterlockedIncrement(&m_lRef); }
            virtual void Release(){ ::InterlockedDecrement(&m_lRef); if (0 == m_lRef){ delete this; } }
            virtual void SetBeHosted(bool value){ m_bIsBeHosted = value; }
            virtual bool IsBeHosted(){ return m_bIsBeHosted; }

        private:
            bool m_bIsBeHosted;
            volatile long m_lRef;

        };

        class IRunnable
        {
        public:
            virtual ~IRunnable() { };
            virtual void Run() = 0;

        };

        class Runnable : public IRunnable
                       , public AddRefReleaseRunnable
        {
        public:
            virtual ~Runnable() {};
            virtual void* GetRunObj() { return NULL; };
            
        };

        class FunPtrRunnable : public Runnable
        {
        public:
            typedef void(*RunFun)(void* pParam);

            FunPtrRunnable(RunFun pFun, void* lpParam = NULL)
            {
                m_pFun = pFun;
                m_pParam = lpParam;
            }

            virtual void Run()
            {
                if (NULL != m_pFun)
                {
                    (*m_pFun)(m_pParam);
                }
            }

        private:
            RunFun        m_pFun;
            void*         m_pParam;

        };

        template<typename T>
        class MemberFunPtrRunnable : public Runnable
        {
        public:
            typedef void (T::*RunFun)(void* pParam);

            MemberFunPtrRunnable(T* pObj, RunFun pFun, void* lpParam = NULL)
            {
                m_pObj = pObj;
                m_pFun = pFun;
                m_pParam = lpParam;
            }

            virtual ~MemberFunPtrRunnable()
            {

            }

            virtual void Run()
            {
                if (NULL != m_pObj && NULL != m_pFun)
                {
                    (m_pObj->*m_pFun)(m_pParam);
                }
            }

            virtual void* GetRunObj()
            {
                return m_pObj;
            }

        private:
            T*            m_pObj;
            RunFun        m_pFun;
            void*         m_pParam;

        };

        template<typename T>
        inline Runnable* CreateRunnable(T* pObj, void(T::*pFun)(void*), void* lpParam = NULL)
        {
            Runnable *pTask = new MemberFunPtrRunnable<T>(pObj, pFun, lpParam);

            return pTask;
        };

        inline Runnable* CreateRunnable(void(*pFun)(void*), void* lpParam = NULL)
        {
            Runnable *pTask = new FunPtrRunnable(pFun, lpParam);

            return pTask;
        };
    }
}
