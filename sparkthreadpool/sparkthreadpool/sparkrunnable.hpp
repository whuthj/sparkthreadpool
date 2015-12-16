#pragma once

#include "sparksharedptr.hpp"

#define RUNNABLE_PTR_HOST_ADDREF(pRunnable)\
{\
if (NULL == pRunnable){ return false; } pRunnable->SetBeHosted(true); pRunnable->AddRef(); \
}

#define SAFE_HOST_RELEASE(pObj) \
{\
if (pObj && pObj->IsBeHosted()) pObj->Release(); \
}

#define SAFE_RELEASE_RUN_OBJ(pObj) \
{\
if (pObj) pObj->ReleaseRunObj(); \
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
            virtual void Release(){ if (0 == ::InterlockedDecrement(&m_lRef)){ delete this; } }
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
            virtual void ReleaseRunObj() {};
        };

        class FunPtrRunnable : public Runnable
        {
        public:
            typedef void(*RunFun)(void* pParam);
            typedef void (*NoParamRunFun)();

            FunPtrRunnable(RunFun pFun, void* lpParam = NULL)
            {
                m_pFun = pFun;
                m_pNoParamFun = NULL;
                m_pParam = lpParam;
            }

            FunPtrRunnable(NoParamRunFun pFun)
            {
                m_pNoParamFun = pFun;
                m_pFun = NULL;
                m_pParam = NULL;
            }

            virtual void Run()
            {
                if (NULL != m_pFun)
                {
                    (*m_pFun)(m_pParam);
                    return;
                }

                if (NULL != m_pNoParamFun)
                {
                    (*m_pNoParamFun)();
                    return;
                }
            }

        private:
            RunFun        m_pFun;
            NoParamRunFun m_pNoParamFun;
            void*         m_pParam;

        };

        template<typename T, typename ParamType>
        class MemberFunPtrRunnable : public Runnable
        {
        public:
            typedef void (T::*RunFun)(ParamType);
            typedef void (T::*NoParamRunFun)();

            MemberFunPtrRunnable(T* pObj, RunFun pFun, ParamType lpParam = NULL)
            {
                m_pObj = pObj;
                m_pFun = pFun;
                m_pNoParamFun = NULL;
                m_pParam = lpParam;
                m_lWorkRef = 0;

                ::InterlockedIncrement(&m_lWorkRef);
            }

            MemberFunPtrRunnable(T* pObj, NoParamRunFun pFun)
            {
                m_pObj = pObj;
                m_pNoParamFun = pFun;
                m_pFun = NULL;
                m_pParam = NULL;
                m_lWorkRef = 0;

                ::InterlockedIncrement(&m_lWorkRef);
            }

            virtual ~MemberFunPtrRunnable()
            {

            }

            virtual void Run()
            {
                if (NULL == m_pObj) { return; }
                if (NULL == m_pFun && NULL == m_pNoParamFun) { return; }

                if (::InterlockedIncrement(&m_lWorkRef) <= 1)
                {
                    ::InterlockedDecrement(&m_lWorkRef);
                    return;
                }

                Execute();

                ::InterlockedDecrement(&m_lWorkRef);
            }

            virtual void Execute()
            {
                if (NULL == m_pObj) { return; }
                if (NULL == m_pFun && NULL == m_pNoParamFun) { return; }

                if (NULL != m_pFun)
                {
                    (m_pObj->*m_pFun)(m_pParam);
                    return;
                }

                if (NULL != m_pNoParamFun)
                {
                    (m_pObj->*m_pNoParamFun)();
                    return;
                }
            }

            virtual void* GetRunObj()
            {
                return m_pObj;
            }

            virtual void ReleaseRunObj()
            {
                for (;;)
                {
                    if (::InterlockedDecrement(&m_lWorkRef) <= 0)
                    {
                        break;
                    }

                    ::InterlockedIncrement(&m_lWorkRef);
                    ::Sleep(10);
                }
            }

        private:
            T*                      m_pObj;
            RunFun                  m_pFun;
            NoParamRunFun           m_pNoParamFun;
            ParamType               m_pParam;
            volatile long           m_lWorkRef;

        };

        template<typename T, typename ParamType>
        inline Runnable* CreateRunnable(T* pObj, void(T::*pFun)(ParamType), ParamType lpParam = NULL)
        {
            Runnable *pTask = new MemberFunPtrRunnable<T, ParamType>(pObj, pFun, lpParam);

            return pTask;
        };

        template<typename T>
        inline Runnable* CreateRunnable(T* pObj, void(T::*pFun)())
        {
            Runnable *pTask = new MemberFunPtrRunnable<T, void*>(pObj, pFun);

            return pTask;
        };

        inline Runnable* CreateRunnable(void(*pFun)(void*), void* lpParam = NULL)
        {
            Runnable *pTask = new FunPtrRunnable(pFun, lpParam);

            return pTask;
        };

        inline Runnable* CreateRunnable(void(*pFun)())
        {
            Runnable *pTask = new FunPtrRunnable(pFun);

            return pTask;
        };
    }
}
