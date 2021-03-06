/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once;

#include "sparkasyndef.hpp"
#include "sparkrunnable.hpp"

#define F_ASSIGN_0(...)
#define F_ASSIGN_1(v, ...)        m_args._##v = v
#ifdef _MSC_VER
#define F_ASSIGN_2(v, v1, ...)    m_args._##v = v; CONCAT(F_ASSIGN_, 1) LEFTP v1, __VA_ARGS__ RIGHTP
#define F_ASSIGN_3(v, v1, ...)    m_args._##v = v; CONCAT(F_ASSIGN_, 2) LEFTP v1, __VA_ARGS__ RIGHTP
#define F_ASSIGN_4(v, v1, ...)    m_args._##v = v; CONCAT(F_ASSIGN_, 3) LEFTP v1, __VA_ARGS__ RIGHTP
#define F_ASSIGN_5(v, v1, ...)    m_args._##v = v; CONCAT(F_ASSIGN_, 4) LEFTP v1, __VA_ARGS__ RIGHTP
#define F_ASSIGN_6(v, v1, ...)    m_args._##v = v; CONCAT(F_ASSIGN_, 5) LEFTP v1, __VA_ARGS__ RIGHTP
#define F_ASSIGN_7(v, v1, ...)    m_args._##v = v; CONCAT(F_ASSIGN_, 6) LEFTP v1, __VA_ARGS__ RIGHTP
#define F_ASSIGN_8(v, v1, ...)    m_args._##v = v; CONCAT(F_ASSIGN_, 7) LEFTP v1, __VA_ARGS__ RIGHTP
#define F_ASSIGN_9(v, v1, ...)    m_args._##v = v; CONCAT(F_ASSIGN_, 8) LEFTP v1, __VA_ARGS__ RIGHTP
#else
#define F_ASSIGN_2(v, v1, ...)    m_args._##v = v; F_ASSIGN_1(v1, __VA_ARGS__)
#define F_ASSIGN_3(v, v1, ...)    m_args._##v = v; F_ASSIGN_2(v1, __VA_ARGS__)
#define F_ASSIGN_4(v, v1, ...)    m_args._##v = v; F_ASSIGN_3(v1, __VA_ARGS__)
#define F_ASSIGN_5(v, v1, ...)    m_args._##v = v; F_ASSIGN_4(v1, __VA_ARGS__)
#define F_ASSIGN_6(v, v1, ...)    m_args._##v = v; F_ASSIGN_5(v1, __VA_ARGS__)
#define F_ASSIGN_7(v, v1, ...)    m_args._##v = v; F_ASSIGN_6(v1, __VA_ARGS__)
#define F_ASSIGN_8(v, v1, ...)    m_args._##v = v; F_ASSIGN_7(v1, __VA_ARGS__)
#define F_ASSIGN_9(v, v1, ...)    m_args._##v = v; F_ASSIGN_8(v1, __VA_ARGS__)
#endif
#define F_ASSIGNX_(N, args)       CONCAT(F_ASSIGN_, N) args
#define F_ASSIGN(...)             F_ASSIGNX_(__VA_ARGC__(__VA_ARGS__), (__VA_ARGS__))

#define F_EXPAND_0(...)
#define F_EXPAND_1(t, n)          m_args._##n
#ifdef _MSC_VER
#define F_EXPAND_2(t, n, t1, ...) m_args._##n, CONCAT(F_EXPAND_, 1) LEFTP t1, __VA_ARGS__ RIGHTP
#define F_EXPAND_3(t, n, t1, ...) m_args._##n, CONCAT(F_EXPAND_, 2) LEFTP t1, __VA_ARGS__ RIGHTP
#define F_EXPAND_4(t, n, t1, ...) m_args._##n, CONCAT(F_EXPAND_, 3) LEFTP t1, __VA_ARGS__ RIGHTP
#define F_EXPAND_5(t, n, t1, ...) m_args._##n, CONCAT(F_EXPAND_, 4) LEFTP t1, __VA_ARGS__ RIGHTP
#define F_EXPAND_6(t, n, t1, ...) m_args._##n, CONCAT(F_EXPAND_, 5) LEFTP t1, __VA_ARGS__ RIGHTP
#define F_EXPAND_7(t, n, t1, ...) m_args._##n, CONCAT(F_EXPAND_, 6) LEFTP t1, __VA_ARGS__ RIGHTP
#define F_EXPAND_8(t, n, t1, ...) m_args._##n, CONCAT(F_EXPAND_, 7) LEFTP t1, __VA_ARGS__ RIGHTP
#define F_EXPAND_9(t, n, t1, ...) m_args._##n, CONCAT(F_EXPAND_, 8) LEFTP t1, __VA_ARGS__ RIGHTP
#else
#define F_EXPAND_2(t, n, t1, ...) m_args._##n, F_EXPAND_1(t1, __VA_ARGS__)
#define F_EXPAND_3(t, n, t1, ...) m_args._##n, F_EXPAND_2(t1, __VA_ARGS__)
#define F_EXPAND_4(t, n, t1, ...) m_args._##n, F_EXPAND_3(t1, __VA_ARGS__)
#define F_EXPAND_5(t, n, t1, ...) m_args._##n, F_EXPAND_4(t1, __VA_ARGS__)
#define F_EXPAND_6(t, n, t1, ...) m_args._##n, F_EXPAND_5(t1, __VA_ARGS__)
#define F_EXPAND_7(t, n, t1, ...) m_args._##n, F_EXPAND_6(t1, __VA_ARGS__)
#define F_EXPAND_8(t, n, t1, ...) m_args._##n, F_EXPAND_7(t1, __VA_ARGS__)
#define F_EXPAND_9(t, n, t1, ...) m_args._##n, F_EXPAND_8(t1, __VA_ARGS__)
#endif
#define F_EXPANDX_(N, args)       CONCAT(F_EXPAND_, N) args
#define F_EXPAND(...)             F_EXPANDX_(__VA_ARGC_HALF__(__VA_ARGS__), (__VA_ARGS__))

namespace Spark
{
    namespace Thread
    {
        template<typename Signature> class Function;
        template<typename obj_type, typename Signature> class ThreadFunction;
        template<typename obj_type, typename Signature> class SharedThreadFunction;

        template<typename ret_type>
        class Function<ret_type()>
        {
        private:
            typedef Function selftype;
            typedef ret_type(*RunFun)();
        public:
            Function() : m_pFun(NULL) {} 
            Function(RunFun pFun)
            {
                operator = (pFun);
            }
            Function& operator=(RunFun pFun)
            {
                m_pFun = pFun;
                return *this;
            }
            ret_type operator()()
            {
                return _CallFun();
            }
        private:
            ret_type _CallFun()
            {
                return m_pFun();
            }
        private:
            RunFun m_pFun;
        };
        template<typename obj_type, typename ret_type>
        class ThreadFunction<obj_type, ret_type()> : public Runnable
        {
        private:
            typedef ThreadFunction selftype;
            typedef ret_type(obj_type::*RunFun)();
        public:
            ThreadFunction() : m_pObj(NULL), m_pFun(NULL) {} 
            ThreadFunction(obj_type* pObj, RunFun pFun)
            {
                m_pObj = pObj;
                m_pFun = pFun;
            }
            ret_type operator()()
            {
                return _CallFun();
            }
            virtual void* GetRunObj()
            {
                return m_pObj;
            }
            virtual void SafeRun()
            {
                if (NULL == m_pObj) { return; }
                if (NULL == m_pFun) { return; }

                if (m_pFun) { (m_pObj->*m_pFun)(); }
            }
        private:
            ret_type _CallFun()
            {
                return (m_pObj->*m_pFun)();
            }
        private:
            RunFun m_pFun;
            obj_type* m_pObj;
        };
        template<typename obj_type, typename ret_type>
        class SharedThreadFunction<obj_type, ret_type()> : public Runnable
        {
        private:
            typedef SharedThreadFunction selftype;
            typedef ret_type(obj_type::*RunFun)();
        public:
            SharedThreadFunction() : m_pFun(NULL) {}
            SharedThreadFunction(SparkSharedPtr<obj_type> sharedObj, RunFun pFun)
            {
                m_pObj = sharedObj;
                m_pFun = pFun;
            }
            ret_type operator()()
            {
                return _CallFun();
            }
            virtual void* GetRunObj()
            {
                return m_pObj;
            }
            virtual void SafeRun()
            {
                if (NULL == m_pObj) { return; }
                if (NULL == m_pFun) { return; }

                if (m_pFun) { (m_pObj->*m_pFun)(); }
            }
        private:
            ret_type _CallFun()
            {
                return (m_pObj->*m_pFun)();
            }
        private:
            RunFun m_pFun;
            SparkSharedPtr<obj_type> m_pObj;
        };
        template<typename obj_type, typename ret_type>
        inline Runnable* CreateRunnableEx(obj_type* pObj, ret_type(obj_type::*pFun)())
        {
            ThreadFunction<obj_type, ret_type()>* pTask = new ThreadFunction<obj_type, ret_type()>(pObj, pFun);
            return pTask;
        }
        template<typename obj_type, typename ret_type>
        inline Runnable* CreateRunnableEx(SparkSharedPtr<obj_type> shareObj, ret_type(obj_type::*pFun)())
        {
            SharedThreadFunction<obj_type, ret_type()>* pTask = new SharedThreadFunction<obj_type, ret_type()>(shareObj, pFun);
            return pTask;
        }

#define DECLARE_FUNCTION(ptype, classparam, args, args_ex, var)\
        template<typename ret_type, classparam>\
        class Function<ret_type (ptype)>\
        {\
        private:\
            typedef Function selftype;\
            typedef ret_type(*RunFun)(args);\
        public:\
            Function() : m_pFun(NULL) {} \
            Function(RunFun pFun)\
            {\
                operator = (pFun);\
            }\
            Function& operator=(RunFun pFun)\
            {\
                m_pFun = pFun;\
                return *this;\
            }\
            ret_type operator()(args)\
            {\
                return _CallFun(var);\
            }\
        private:\
            ret_type _CallFun(args)\
            {\
                return m_pFun(var);\
            }\
        private:\
            RunFun m_pFun;\
        };\
        template<typename obj_type, typename ret_type, classparam>\
        class ThreadFunction<obj_type, ret_type (ptype)> : public Runnable\
        {\
        private:\
            struct _args_data { FIELD(args_ex); };\
            typedef ThreadFunction selftype;\
            typedef ret_type(obj_type::*RunFun)(args);\
        public:\
            ThreadFunction() : m_pObj(NULL), m_pFun(NULL) {} \
            ThreadFunction(obj_type* pObj, RunFun pFun)\
            {\
                m_pObj = pObj;\
                m_pFun = pFun;\
            }\
            ret_type operator()(args)\
            {\
                return _CallFun(var);\
            }\
            void SetValue(args)\
            {\
                F_ASSIGN(var);\
            }\
            virtual void* GetRunObj()\
            {\
                return m_pObj;\
            }\
            virtual void SafeRun()\
            {\
                if (NULL == m_pObj) { return; }\
                if (NULL == m_pFun) { return; }\
                (m_pObj->*m_pFun)(F_EXPAND(args_ex));\
            }\
        private:\
            ret_type _CallFun(args)\
            {\
                return (m_pObj->*m_pFun)(var);\
            }\
        private:\
            RunFun m_pFun;\
            obj_type* m_pObj;\
            _args_data m_args;\
        };\
        template<typename obj_type, typename ret_type, classparam>\
        class SharedThreadFunction<obj_type, ret_type (ptype)> : public Runnable\
        {\
        private:\
            struct _args_data { FIELD(args_ex); };\
            typedef SharedThreadFunction selftype;\
            typedef ret_type(obj_type::*RunFun)(args);\
        public:\
            SharedThreadFunction() : m_pFun(NULL) {} \
            SharedThreadFunction(SparkSharedPtr<obj_type> pObj, RunFun pFun)\
            {\
                m_pObj = pObj;\
                m_pFun = pFun;\
            }\
            ret_type operator()(args)\
            {\
                return _CallFun(var);\
            }\
            void SetValue(args)\
            {\
                F_ASSIGN(var);\
            }\
            virtual void* GetRunObj()\
            {\
                return m_pObj;\
            }\
            virtual void SafeRun()\
            {\
                if (NULL == m_pObj) { return; }\
                if (NULL == m_pFun) { return; }\
                (m_pObj->*m_pFun)(F_EXPAND(args_ex));\
            }\
        private:\
            ret_type _CallFun(args)\
            {\
                return (m_pObj->*m_pFun)(var);\
            }\
        private:\
            RunFun m_pFun;\
            SparkSharedPtr<obj_type> m_pObj;\
            _args_data m_args;\
        };\
        template<typename obj_type, typename ret_type, classparam>\
        inline Runnable* CreateRunnableEx(obj_type* pObj, ret_type(obj_type::*pFun)(ptype), args)\
        {\
            ThreadFunction<obj_type, ret_type(ptype)>* pTask = new ThreadFunction<obj_type, ret_type(ptype)>(pObj, pFun);\
            pTask->SetValue(var);\
            return pTask;\
        }\
        template<typename obj_type, typename ret_type, classparam>\
        inline Runnable* CreateRunnableEx(SparkSharedPtr<obj_type> shareObj, ret_type(obj_type::*pFun)(ptype), args)\
        {\
            SharedThreadFunction<obj_type, ret_type(ptype)>* pTask = new SharedThreadFunction<obj_type, ret_type(ptype)>(shareObj, pFun);\
            pTask->SetValue(var);\
            return pTask;\
        }\

        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type), DECLARE_TPYE(typename arg0_type), DECLARE_ARGS(arg0_type a0), DECLARE_ARGS_EX(arg0_type, a0), DECLARE_VAR(a0));
        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type, arg1_type), DECLARE_TPYE(typename arg0_type, typename arg1_type), DECLARE_ARGS(arg0_type a0, arg1_type a1), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1), DECLARE_VAR(a0, a1));
        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2), DECLARE_VAR(a0, a1, a2));
        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3), DECLARE_VAR(a0, a1, a2, a3));
        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4), DECLARE_VAR(a0, a1, a2, a3, a4));
        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4, arg5_type, a5), DECLARE_VAR(a0, a1, a2, a3, a4, a5));
        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4, arg5_type, a5, arg6_type, a6), DECLARE_VAR(a0, a1, a2, a3, a4, a5, a6));
        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4, arg5_type, a5, arg6_type, a6, arg7_type, a7), DECLARE_VAR(a0, a1, a2, a3, a4, a5, a6, a7));
        DECLARE_FUNCTION(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type, typename arg8_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4, arg5_type, a5, arg6_type, a6, arg7_type, a7, arg8_type, a8), DECLARE_VAR(a0, a1, a2, a3, a4, a5, a6, a7, a8));
    }
};