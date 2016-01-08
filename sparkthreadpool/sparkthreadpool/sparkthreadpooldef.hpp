/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

#include "sparkasyndef.hpp"

//////////////////////////////////////////////////////////////////////////
// 宏定义修饰说明
// INSTANCE：代表使用线程池单例，并且函数执行主体为this！！！
// EX：代表支持变参函数

//////////////////////////////////////////////////////////////////////////
// 数据和函数定义

#define SPARK_ASYN_API(F, ...)\
struct tag_dd_##F : public tag_dd_base{ FIELD(__VA_ARGS__); }; \
void asyn_##F(tag_dd_##F arg){ tag_dd_##F* __p__ = (tag_dd_##F*)&arg; F(EXPAND(__VA_ARGS__)); }

struct tag_dd_base { virtual ~tag_dd_base() {} };

//////////////////////////////////////////////////////////////////////////
// 移除当前类任务
// 如果任务还没有在线程池运行则直接不运行了
// 如果任务正在运行中则等待任务完成

#define SPARK_INSTANCE_DESTROY_TASKS(LP_OBJ)\
{\
    Spark::Thread::SparkThreadPool::Instance().DestroyThisTasks(LP_OBJ);\
}

//////////////////////////////////////////////////////////////////////////
// 切换到非主线程执行

#define SPARK_ASYN(T, F, LP_OBJ, LP_POOL, TYPE, PARAM)\
{\
    Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
    if (pPool){pPool->Execute<T>(LP_OBJ, &T::F, (void*)PARAM, TYPE);}\
}

#define SPARK_POST_ASYN(T, F, LP_OBJ, LP_POOL, PARAM)\
{\
    SPARK_ASYN(T, F, LP_OBJ, LP_POOL, Spark::Thread::emSRType_Post, PARAM); \
}

#define SPARK_INSTANCE_POST_ASYN(T, F, PARAM)\
{\
    SPARK_POST_ASYN(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), PARAM);\
}

#define SPARK_INSTANCE_ASYN(T, F, PARAM)\
{\
    SPARK_ASYN(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), Spark::Thread::emSRType_Schedule_Post, PARAM); \
}

#define SPARK_NOPARAM_ASYN(T, F, LP_OBJ, LP_POOL, TYPE)\
{\
    Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
    if (pPool){pPool->Execute<T>(LP_OBJ, &T::F, TYPE);}\
}

#define SPARK_NOPARAM_POST_ASYN(T, F, LP_OBJ, LP_POOL)\
{\
    SPARK_NOPARAM_ASYN(T, F, LP_OBJ, LP_POOL, Spark::Thread::emSRType_Post); \
}

#define SPARK_NOPARAM_INSTANCE_POST_ASYN(T, F)\
{\
    SPARK_NOPARAM_POST_ASYN(T, F, this, &Spark::Thread::SparkThreadPool::Instance());\
}

#define SPARK_NOPARAM_INSTANCE_ASYN(T, F)\
{\
    SPARK_NOPARAM_ASYN(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), Spark::Thread::emSRType_Schedule_Post); \
}

#define SPARK_PARAM_ASYN(T, F, PARAM_TYPE, LP_OBJ, LP_POOL, TYPE, PARAM)\
{\
    Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
    if (pPool){pPool->Execute<T, PARAM_TYPE>(LP_OBJ, &T::F, PARAM, TYPE);}\
}

#define SPARK_PARAM_POST_ASYN(T, F, PARAM_TYPE, LP_OBJ, LP_POOL, PARAM)\
{\
    SPARK_PARAM_ASYN(T, F, PARAM_TYPE, LP_OBJ, LP_POOL, Spark::Thread::emSRType_Post, PARAM); \
}

#define SPARK_PARAM_INSTANCE_POST_ASYN(T, F, PARAM_TYPE, PARAM)\
{\
    SPARK_PARAM_POST_ASYN(T, F, PARAM_TYPE, this, &Spark::Thread::SparkThreadPool::Instance(), PARAM);\
}

#define SPARK_PARAM_INSTANCE_ASYN(T, F, PARAM_TYPE, PARAM)\
{\
    SPARK_PARAM_ASYN(T, F, PARAM_TYPE, this, &Spark::Thread::SparkThreadPool::Instance(), Spark::Thread::emSRType_Schedule_Post, PARAM); \
}

//////////////////////////////////////////////////////////////////////////
// 切换到主线程执行

#define SPARK_MSG(T, F, LP_OBJ, LP_POOL, IS_SEND_MSG, PARAM)\
{\
    Spark::Thread::SparkThreadPool* pPool = LP_POOL; \
    if (pPool){pPool->SwitchToWndThread<T>(LP_OBJ, &T::F, (void*)PARAM, IS_SEND_MSG);}\
}

#define SPARK_INSTANCE_POST_MSG(T, F, PARAM)\
{\
    SPARK_MSG(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), false, PARAM); \
}

#define SPARK_INSTANCE_SEND_MSG(T, F, PARAM)\
{\
    SPARK_MSG(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), true, PARAM); \
}

//////////////////////////////////////////////////////////////////////////
// 切换到非主线程执行支持变参，必须要使用SPARK_ASYN_API声明函数

#define SPARK_ASYN_EX(T, F, LP_OBJ, LP_POOL, TYPE, ...)\
{\
    Spark::Thread::SparkThreadPool* pThreadPool = LP_POOL; \
    if (pThreadPool)\
    {\
        tag_dd_##F  __p; \
        tag_dd_##F* __p__ = &__p; \
        ASSIGN(__VA_ARGS__); \
        pThreadPool->Execute<T, tag_dd_##F>(LP_OBJ, &T::asyn_##F, __p, TYPE); \
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
        tag_dd_##F __p; \
        tag_dd_##F* __p__ = &__p; ASSIGN(__VA_ARGS__);\
        pThreadPool->SwitchToWndThread<T, tag_dd_##F>(LP_OBJ, &T::asyn_##F, __p, IS_SEND_MSG);\
    }\
}

#define SPARK_INSTANCE_POST_MSG_EX(T, F, ...)\
{\
    SPARK_POST_MSG_EX(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), false, __VA_ARGS__);\
}
    
#define SPARK_INSTANCE_SEND_MSG_EX(T, F, ...)\
{\
    SPARK_POST_MSG_EX(T, F, this, &Spark::Thread::SparkThreadPool::Instance(), true, __VA_ARGS__); \
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