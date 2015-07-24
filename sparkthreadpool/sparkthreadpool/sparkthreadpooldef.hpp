﻿#pragma once

#include "sparkasyndef.hpp"

//////////////////////////////////////////////////////////////////////////
// 数据和函数定义

#define SPARK_ASYN_API(F, ...)\
struct tag_dd_##F : public tag_dd_base{ FIELD(__VA_ARGS__); }; \
void asyn_##F(void* arg){ tag_dd_##F* __p__ = (tag_dd_##F*)arg; F(EXPAND(__VA_ARGS__)); delete __p__; }

struct tag_dd_base{ virtual ~tag_dd_base(){} };

//////////////////////////////////////////////////////////////////////////
// 切换到非主线程执行

#define SPARK_ASYN(T, F, LP_OBJ, LP_POOL, TYPE, PARAM)\
    {\
        Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
        if (pPool){pPool->Execute<T>(LP_OBJ, &T::F, (void*)PARAM, TYPE);}\
    }

#define SPARK_POST_ASYN(T, F, LP_POOL, PARAM)\
    {\
        Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
        if (pPool){pPool->Execute<T>(this, &T::F, (void*)PARAM, Spark::Thread::emSRType_Post);}\
    }

#define SPARK_INSTANCE_POST_ASYN(T, F, PARAM)\
    {\
        Spark::Thread::SparkThreadPool::Instance().Execute<T>(this, &T::F, (void*)PARAM, Spark::Thread::emSRType_Post);\
    }

#define SPARK_INSTANCE_ASYN(T, F, PARAM)\
    {\
        Spark::Thread::SparkThreadPool::Instance().Execute<T>(this, &T::F, (void*)PARAM, Spark::Thread::emSRType_Schedule_Post);\
    }

//////////////////////////////////////////////////////////////////////////
// 切换到主线程执行

#define SPARK_MSG(T, F, LP_OBJ, LP_POOL, IS_SEND_MSG, PARAM)\
    {\
        Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
        if (pPool){pPool->SwitchToWndThread<T>(LP_OBJ, &T::F, (void*)PARAM, IS_SEND_MSG);}\
    }

#define SPARK_POST_MSG(T, F, LP_POOL, PARAM)\
    {\
        Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
        if (pPool){pPool->SwitchToWndThread<T>(this, &T::F, (void*)PARAM, false);}\
    }

#define SPARK_SEND_MSG(T, F, LP_POOL, PARAM)\
    {\
        Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
        if (pPool){pPool->SwitchToWndThread<T>(this, &T::F, (void*)PARAM, true);}\
    }

#define SPARK_INSTANCE_POST_MSG(T, F, PARAM)\
    {\
        Spark::Thread::SparkThreadPool::Instance().SwitchToWndThread<T>(this, &T::F, (void*)PARAM, false);\
    }

#define SPARK_INSTANCE_SEND_MSG(T, F, PARAM)\
    {\
        Spark::Thread::SparkThreadPool::Instance().SwitchToWndThread<T>(this, &T::F, (void*)PARAM, true);\
    }


//////////////////////////////////////////////////////////////////////////
// 切换到非主线程执行支持变参，必须要使用SPARK_ASYN_API声明函数

#define SPARK_ASYN_EX(T, F, LP_OBJ, LP_POOL, TYPE, ...)\
    {\
        Spark::Thread::SparkThreadPool* pThreadPool = LP_POOL; \
        if (pThreadPool)\
        {\
            tag_dd_##F* __p__ = new (std::nothrow) tag_dd_##F; \
            ASSIGN(__VA_ARGS__); \
            bool br = pThreadPool->Execute<T>(LP_OBJ, &T::asyn_##F, (void*)__p__, TYPE); \
            if (!br){ delete __p__; }\
        }\
    }

#define SPARK_INSTANCE_ASYN_EX(T, F, ...)\
    {\
        SPARK_ASYN_EX(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), Spark::Thread::emSRType_Schedule_Post, __VA_ARGS__);\
    }

//////////////////////////////////////////////////////////////////////////
// 切换到主线程执行支持变参，必须要使用SPARK_ASYN_API声明函数

#define SPARK_POST_MSG_EX(T, F, LP_OBJ, LP_POOL, IS_SEND_MSG, ...)\
    {\
        Spark::Thread::SparkThreadPool* pThreadPool = LP_POOL; \
        if (pThreadPool)\
        {\
            tag_dd_##F* __p__ = new (std::nothrow) tag_dd_##F;ASSIGN(__VA_ARGS__);\
            bool br = pThreadPool->SwitchToWndThread<T>(LP_OBJ, &T::asyn_##F, (void*)__p__, IS_SEND_MSG);\
            if (!br){ delete __p__; }\
        }\
    }

#define SPARK_INSTANCE_POST_MSG_EX(T, F, ...)\
    {\
        SPARK_POST_MSG_EX(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), FALSE, __VA_ARGS__);\
    }
    
#define SPARK_INSTANCE_SEND_MSG_EX(T, F, ...)\
    {\
        SPARK_POST_MSG_EX(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), TRUE, __VA_ARGS__); \
    }

//////////////////////////////////////////////////////////////////////////
// 从主线程切换到非主线程执行支持变参，必须要使用SPARK_ASYN_API声明函数
// 注意：如果当前函数已经在非主线程函数运行则直接执行指定函数！！！

#define SPARK_SWTICH_TO_WORKTHREAD(T, F, LP_OBJ, LP_POOL, TYPE, ...)\
    {\
        Spark::Thread::SparkThreadPool* pThreadPool = LP_POOL; \
        if (NULL == pThreadPool) return; \
        int nMsgThreadId = pThreadPool->GetMsgThreadId();\
        if (nMsgThreadId == ::GetCurrentThreadId())\
        {\
            SPARK_ASYN_EX(T, F, LP_OBJ, LP_POOL, TYPE, __VA_ARGS__);\
            return;\
        }\
    }

#define SPARK_INSTANCE_SWTICH_TO_WORKTHREAD(T, F, ...)\
    {\
        SPARK_SWTICH_TO_WORKTHREAD(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), Spark::Thread::emSRType_Schedule_Post, __VA_ARGS__);\
    }

//////////////////////////////////////////////////////////////////////////
// 常用工具宏

#define SPARK_SAFE_CALL(obj, func, ...)\
    { if (obj){obj->func(__VA_ARGS__);} }