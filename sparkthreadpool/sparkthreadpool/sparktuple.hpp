/*
* Author: hujun
* Email: whuthj@163.com
* Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once;

#include "sparkasyndef.hpp"

#define TUPLE_ASSIGN_0(...)
#define TUPLE_ASSIGN_1(v, ...)        m_data._##v = v
#ifdef _MSC_VER
#define TUPLE_ASSIGN_2(v, v1, ...)    m_data._##v = v; CONCAT(TUPLE_ASSIGN_, 1) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_3(v, v1, ...)    m_data._##v = v; CONCAT(TUPLE_ASSIGN_, 2) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_4(v, v1, ...)    m_data._##v = v; CONCAT(TUPLE_ASSIGN_, 3) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_5(v, v1, ...)    m_data._##v = v; CONCAT(TUPLE_ASSIGN_, 4) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_6(v, v1, ...)    m_data._##v = v; CONCAT(TUPLE_ASSIGN_, 5) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_7(v, v1, ...)    m_data._##v = v; CONCAT(TUPLE_ASSIGN_, 6) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_8(v, v1, ...)    m_data._##v = v; CONCAT(TUPLE_ASSIGN_, 7) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_9(v, v1, ...)    m_data._##v = v; CONCAT(TUPLE_ASSIGN_, 8) LEFTP v1, __VA_ARGS__ RIGHTP
#else
#define TUPLE_ASSIGN_2(v, v1, ...)    m_data._##v = v; TUPLE_ASSIGN_1(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_3(v, v1, ...)    m_data._##v = v; TUPLE_ASSIGN_2(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_4(v, v1, ...)    m_data._##v = v; TUPLE_ASSIGN_3(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_5(v, v1, ...)    m_data._##v = v; TUPLE_ASSIGN_4(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_6(v, v1, ...)    m_data._##v = v; TUPLE_ASSIGN_5(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_7(v, v1, ...)    m_data._##v = v; TUPLE_ASSIGN_6(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_8(v, v1, ...)    m_data._##v = v; TUPLE_ASSIGN_7(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_9(v, v1, ...)    m_data._##v = v; TUPLE_ASSIGN_8(v1, __VA_ARGS__)
#endif
#define TUPLE_ASSIGNX_(N, args)       CONCAT(TUPLE_ASSIGN_, N) args
#define TUPLE_ASSIGN(...)             TUPLE_ASSIGNX_(__VA_ARGC__(__VA_ARGS__), (__VA_ARGS__))

namespace Spark
{
    namespace Thread
    {
        struct null_type {};

        template <class HT, class TT> struct cons;

        template <
            class T0 = null_type, class T1 = null_type, class T2 = null_type,
            class T3 = null_type, class T4 = null_type, class T5 = null_type,
            class T6 = null_type, class T7 = null_type, class T8 = null_type,
            class T9 = null_type>
        class tuple;

        template <class HT, class TT>
        struct cons {
            typedef HT head_type;
            typedef TT tail_type;
        };

        template <class HT>
        struct cons<HT, null_type> {
            typedef HT head_type;
            typedef null_type tail_type;
            typedef cons<HT, null_type> self_type;
        };

        template <class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8, class T9>
        struct map_tuple_to_cons
        {
            typedef cons<T0,
                typename map_tuple_to_cons<T1, T2, T3, T4, T5,
                T6, T7, T8, T9, null_type>::type
            > type;
        };

        template <>
        struct map_tuple_to_cons<null_type, null_type, null_type, null_type, null_type, null_type, null_type, null_type, null_type, null_type>
        {
            typedef null_type type;
        };

        template <class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8, class T9>
        class tuple : public map_tuple_to_cons<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type
        {
        public:
            tuple() {}
        };

        template <>
        class tuple<null_type, null_type, null_type, null_type, null_type, null_type, null_type, null_type, null_type, null_type> :
            public null_type
        {
        public:
            typedef null_type inherited;
        };

        template<typename Signature> class Tuple;

#define DECLARE_TUPLE(ptype, classparam, args, args_ex, var)\
        template<classparam>\
        class Tuple<void(ptype)>\
        {\
        private:\
            struct _data { FIELD(args_ex); };\
        public:\
            Tuple() {} \
            Tuple(args)\
            {\
                TUPLE_ASSIGN(var);\
            }\
        private:\
            _data m_data;\
        };\
        
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type), DECLARE_TPYE(typename arg0_type), DECLARE_ARGS(arg0_type a0), DECLARE_ARGS_EX(arg0_type, a0), DECLARE_VAR(a0));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type), DECLARE_TPYE(typename arg0_type, typename arg1_type), DECLARE_ARGS(arg0_type a0, arg1_type a1), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1), DECLARE_VAR(a0, a1));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2), DECLARE_VAR(a0, a1, a2));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3), DECLARE_VAR(a0, a1, a2, a3));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4), DECLARE_VAR(a0, a1, a2, a3, a4));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4, arg5_type, a5), DECLARE_VAR(a0, a1, a2, a3, a4, a5));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4, arg5_type, a5, arg6_type, a6), DECLARE_VAR(a0, a1, a2, a3, a4, a5, a6));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4, arg5_type, a5, arg6_type, a6, arg7_type, a7), DECLARE_VAR(a0, a1, a2, a3, a4, a5, a6, a7));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type, typename arg8_type), DECLARE_ARGS(arg0_type a0, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8), DECLARE_ARGS_EX(arg0_type, a0, arg1_type, a1, arg2_type, a2, arg3_type, a3, arg4_type, a4, arg5_type, a5, arg6_type, a6, arg7_type, a7, arg8_type, a8), DECLARE_VAR(a0, a1, a2, a3, a4, a5, a6, a7, a8));
    }
};
