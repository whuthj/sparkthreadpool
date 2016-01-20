/*
* Author: hujun
* Email: whuthj@163.com
* Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once;

#include "sparkasyndef.hpp"

#define LEFTP                    (
#define RIGHTP                    )

#define TUPLE_FIELD_0(...)    
#define TUPLE_FIELD_1(t, n)              t item##n
#ifdef _MSC_VER
#define TUPLE_FIELD_2(t, n, t1, ...)    t item##n; CONCAT(TUPLE_FIELD_, 1) LEFTP t1, __VA_ARGS__ RIGHTP
#define TUPLE_FIELD_3(t, n, t1, ...)    t item##n; CONCAT(TUPLE_FIELD_, 2) LEFTP t1, __VA_ARGS__ RIGHTP
#define TUPLE_FIELD_4(t, n, t1, ...)    t item##n; CONCAT(TUPLE_FIELD_, 3) LEFTP t1, __VA_ARGS__ RIGHTP
#define TUPLE_FIELD_5(t, n, t1, ...)    t item##n; CONCAT(TUPLE_FIELD_, 4) LEFTP t1, __VA_ARGS__ RIGHTP
#define TUPLE_FIELD_6(t, n, t1, ...)    t item##n; CONCAT(TUPLE_FIELD_, 5) LEFTP t1, __VA_ARGS__ RIGHTP
#define TUPLE_FIELD_7(t, n, t1, ...)    t item##n; CONCAT(TUPLE_FIELD_, 6) LEFTP t1, __VA_ARGS__ RIGHTP
#define TUPLE_FIELD_8(t, n, t1, ...)    t item##n; CONCAT(TUPLE_FIELD_, 7) LEFTP t1, __VA_ARGS__ RIGHTP
#define TUPLE_FIELD_9(t, n, t1, ...)    t item##n; CONCAT(TUPLE_FIELD_, 8) LEFTP t1, __VA_ARGS__ RIGHTP
#else
#define TUPLE_FIELD_2(t, n, t1, ...)    t item##n; TUPLE_FIELD_1(t1, __VA_ARGS__)
#define TUPLE_FIELD_3(t, n, t1, ...)    t item##n; TUPLE_FIELD_2(t1, __VA_ARGS__)
#define TUPLE_FIELD_4(t, n, t1, ...)    t item##n; TUPLE_FIELD_3(t1, __VA_ARGS__)
#define TUPLE_FIELD_5(t, n, t1, ...)    t item##n; TUPLE_FIELD_4(t1, __VA_ARGS__)
#define TUPLE_FIELD_6(t, n, t1, ...)    t item##n; TUPLE_FIELD_5(t1, __VA_ARGS__)
#define TUPLE_FIELD_7(t, n, t1, ...)    t item##n; TUPLE_FIELD_6(t1, __VA_ARGS__)
#define TUPLE_FIELD_8(t, n, t1, ...)    t item##n; TUPLE_FIELD_7(t1, __VA_ARGS__)
#define TUPLE_FIELD_9(t, n, t1, ...)    t item##n; TUPLE_FIELD_8(t1, __VA_ARGS__)
#endif
#define TUPLE_FIELDX_(N, args)        CONCAT(TUPLE_FIELD_, N) args
#define TUPLE_FIELD(...)                TUPLE_FIELDX_(__VA_ARGC_HALF__(__VA_ARGS__), (__VA_ARGS__))

#define TUPLE_ASSIGN_0(...)
#define TUPLE_ASSIGN_1(v, ...)         item##v = v
#ifdef _MSC_VER
#define TUPLE_ASSIGN_2(v, v1, ...)    item##v = v; CONCAT(TUPLE_ASSIGN_, 1) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_3(v, v1, ...)    item##v = v; CONCAT(TUPLE_ASSIGN_, 2) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_4(v, v1, ...)    item##v = v; CONCAT(TUPLE_ASSIGN_, 3) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_5(v, v1, ...)    item##v = v; CONCAT(TUPLE_ASSIGN_, 4) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_6(v, v1, ...)    item##v = v; CONCAT(TUPLE_ASSIGN_, 5) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_7(v, v1, ...)    item##v = v; CONCAT(TUPLE_ASSIGN_, 6) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_8(v, v1, ...)    item##v = v; CONCAT(TUPLE_ASSIGN_, 7) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_9(v, v1, ...)    item##v = v; CONCAT(TUPLE_ASSIGN_, 8) LEFTP v1, __VA_ARGS__ RIGHTP
#else
#define TUPLE_ASSIGN_2(v, v1, ...)    item##v = v; TUPLE_ASSIGN_1(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_3(v, v1, ...)    item##v = v; TUPLE_ASSIGN_2(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_4(v, v1, ...)    item##v = v; TUPLE_ASSIGN_3(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_5(v, v1, ...)    item##v = v; TUPLE_ASSIGN_4(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_6(v, v1, ...)    item##v = v; TUPLE_ASSIGN_5(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_7(v, v1, ...)    item##v = v; TUPLE_ASSIGN_6(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_8(v, v1, ...)    item##v = v; TUPLE_ASSIGN_7(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_9(v, v1, ...)    item##v = v; TUPLE_ASSIGN_8(v1, __VA_ARGS__)
#endif
#define TUPLE_ASSIGNX_(N, args)     CONCAT(TUPLE_ASSIGN_, N) args
#define TUPLE_ASSIGN(...)                TUPLE_ASSIGNX_(__VA_ARGC__(__VA_ARGS__), (__VA_ARGS__))

namespace Spark
{
    namespace Thread
    {
        struct NullType {};
        template <class HT, class TT> struct TupleBase;
        template <
            class T0 = NullType, class T1 = NullType, class T2 = NullType,
            class T3 = NullType, class T4 = NullType, class T5 = NullType,
            class T6 = NullType, class T7 = NullType, class T8 = NullType,
            class T9 = NullType>
        class Tuple;

        //Element<1, TupleBase<int,TupleBase<long,TupleBase<bool,TupleBase<double,NullType> > > > >
        //Element<1, Element<0, TupleBase<long,TupleBase<bool,TupleBase<double,NullType> > > > >
        //取TupleBase<long,TupleBase<bool,TupleBase<double,NullType> > > > head_type为long
        template<int N, class T> // 这个int N会递减，以呈现递归的形式
        struct Element
        {
        private:
            typedef typename T::tail_type Next;
        public:
            typedef typename Element<N-1, Next>::type type; //递归
        };
        template<class T>
        struct Element<0,T>
        {
            typedef typename T::head_type type;
        };

        //模板嵌套
        template <class HT, class TT>
        struct TupleBase {
            typedef HT head_type;
            typedef TT tail_type;
            head_type head; 
            tail_type tail;

            template <int N>
            typename Element<N, TupleBase<HT, TT> >::type Get()
            {
                return Spark::Thread::Get<N>(*this);
            }

            TupleBase() {}

            template <class T1, class T2, class T3, class T4, class T5,
            class T6, class T7, class T8, class T9, class T10>
            TupleBase( T1& t1, T2& t2, T3& t3, T4& t4, T5& t5,
                T6& t6, T7& t7, T8& t8, T9& t9, T10& t10 )
                : head (t1), tail (t2, t3, t4, t5, t6, t7, t8, t9, t10, NullType())
            { }

            template <class T2, class T3, class T4, class T5,
            class T6, class T7, class T8, class T9, class T10>
                TupleBase( const NullType& t1, // 当接受的第一个参数为NullType时
                T2& t2, T3& t3, T4& t4, T5& t5,
                T6& t6, T7& t7, T8& t8, T9& t9, T10& t10 )
                : head (), tail (t2, t3, t4, t5, t6, t7, t8, t9, t10, NullType())
            {}
        };
        template <class HT>
        struct TupleBase<HT, NullType> {
            typedef HT head_type;
            typedef NullType tail_type;
            typedef TupleBase<HT, NullType> self_type;

            head_type head;

            TupleBase() {}

            template<class T1>
            TupleBase(T1& t1, const NullType&, const NullType&, const NullType&,
                const NullType&, const NullType&, const NullType&,
                const NullType&, const NullType&, const NullType&)
                : head (t1) {}

            TupleBase(const NullType&,
                const NullType&, const NullType&, const NullType&,
                const NullType&, const NullType&, const NullType&,
                const NullType&, const NullType&, const NullType&)
                : head () {}
        };

        //获取元素
        //TupleBase<int,TupleBase<long,TupleBase<bool,TupleBase<double,NullType> > > >
        //GetClass<2>
        //return GetClass<1>::Get<long>(TupleBase<long,TupleBase<bool,TupleBase<double,NullType> > >);
        //return GetClass<0>::Get<long>(TupleBase<bool,TupleBase<double,NullType> >);
        //return bool值
        template<int N>
        struct GetClass {
            template<class RET, class HT, class TT >
            inline static RET Get(TupleBase<HT, TT>& t)
            {
                return GetClass<N-1>::Get<RET>(t.tail);
            }
        };
        template<>
        struct GetClass<0> {
            template<class RET, class HT, class TT>
            inline static RET Get(TupleBase<HT, TT>& t)
            {
                return t.head;
            }
        };
        template<int N, class HT, class TT>
        inline typename Element<N, TupleBase<HT, TT> >::type Get(TupleBase<HT, TT>& c)
        {
            return GetClass<N>::Get<typename Element<N, TupleBase<HT, TT> >::type>(c); 
        }

        //获取Tuple长度
        template<class T>
        struct TupleLength {
            static const int value = 1 + TupleLength<typename T::tail_type>::value; //递归
        };
        template<>
        struct TupleLength<NullType> {
            static const int value = 0;
        };

        //构建嵌套模板
        //MakeTupleBase<int, long, bool, double>
        //TupleBase<int, MakeTupleBase<long, bool, double>>
        //TupleBase<int, TupleBase<long, MakeTupleBase<bool, double>>>
        //TupleBase<int, TupleBase<long, TupleBase<bool, MakeTupleBase<double>>>>
        //TupleBase<int, TupleBase<long, TupleBase<bool, TupleBase<double, NullType>>>>
        template <class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8, class T9>
        struct MakeTupleBase
        {
            typedef TupleBase<T0,
                typename MakeTupleBase<T1, T2, T3, T4, T5,
                T6, T7, T8, T9, NullType>::type
            > type;
        };
        template <>
        struct MakeTupleBase<NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType>
        {
            typedef NullType type;
        };

        //Tuple<int, long, bool, double> => TupleBase<int,TupleBase<long,TupleBase<bool,TupleBase<double,NullType> > > >
        //TupleBase<T0,TupleBase<T1,TupleBase<T2,TupleBase<T3,... ... > > > >
        template <class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8, class T9>
        class Tuple : public MakeTupleBase<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type
        {
        public:
            typedef typename MakeTupleBase<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type inherited; // 基类
            // 基类的head_type(通常即T0,见TupleBase<>的定义)
            typedef typename inherited::head_type head_type;
            // 基类的tail_type(一般仍为一个TupleBase<>)
            typedef typename inherited::tail_type tail_type;
            
            Tuple() {}

            Tuple(T0 t0)
                : inherited(t0, NullType(),
                NullType(), NullType(),
                NullType(), NullType(),
                NullType(), NullType(),
                NullType(), NullType())
            { }

            Tuple(T0 t0, T1 t1)
                : inherited(t0, t1, NullType(), NullType(),
                NullType(), NullType(), NullType(),
                NullType(), NullType(), NullType())
            { }

            Tuple(T0 t0, T1 t1, T2 t2)
                : inherited(t0, t1, t2, NullType(),
                NullType(), NullType(), NullType(),
                NullType(), NullType(), NullType())
            { }

            Tuple(T0 t0, T1 t1, T2 t2, T3 t3)
                : inherited(t0, t1, t2, t3,
                NullType(), NullType(), NullType(),
                NullType(), NullType(), NullType())
            { }

            Tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4)
                : inherited(t0, t1, t2, t3, t4, 
                NullType(), NullType(),
                NullType(), NullType(), NullType())
            { }

            Tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
                : inherited(t0, t1, t2, t3, t4, t5,
                NullType(),
                NullType(), NullType(), NullType())
            { }

            Tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
                : inherited(t0, t1, t2, t3, t4, t5, t6,
                NullType(), NullType(), NullType())
            { }

            Tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
                : inherited(t0, t1, t2, t3, t4, t5, t6, t7,
                NullType(), NullType())
            { }

            Tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
                : inherited(t0, t1, t2, t3, t4, t5, t6, t7, t8 
                NullType())
            { }

            Tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9)
                 : inherited(t0, t1, t2, t3, t4, t5, t6, t7, t8, t9)
            { }
        };
        template <>
        class Tuple<NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType> : public NullType
        {
        public:
            typedef NullType inherited;
        };

//////////////////////////////////////////////////////////////////////////

        template <
            class T0 = NullType, class T1 = NullType, class T2 = NullType,
            class T3 = NullType, class T4 = NullType, class T5 = NullType,
            class T6 = NullType, class T7 = NullType, class T8 = NullType,
            class T9 = NullType>
        struct SimpleTuple;

#define DECLARE_TUPLE(ptype, classparam, args, args_ex, var)\
        template<classparam>\
        struct SimpleTuple<ptype>\
        {\
            SimpleTuple() {} \
            SimpleTuple(args)\
            {\
                TUPLE_ASSIGN(var);\
            }\
            TUPLE_FIELD(args_ex);\
        };\
        
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type), DECLARE_TPYE(typename arg0_type), DECLARE_ARGS(arg0_type _0), DECLARE_ARGS_EX(arg0_type, _0), DECLARE_VAR(_0));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type), DECLARE_TPYE(typename arg0_type, typename arg1_type), DECLARE_ARGS(arg0_type _0, arg1_type _1), DECLARE_ARGS_EX(arg0_type, _0, arg1_type, _1), DECLARE_VAR(_0, _1));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type), DECLARE_ARGS(arg0_type _0, arg1_type _1, arg2_type _2), DECLARE_ARGS_EX(arg0_type, _0, arg1_type, _1, arg2_type, _2), DECLARE_VAR(_0, _1, _2));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type), DECLARE_ARGS(arg0_type _0, arg1_type _1, arg2_type _2, arg3_type _3), DECLARE_ARGS_EX(arg0_type, _0, arg1_type, _1, arg2_type, _2, arg3_type, _3), DECLARE_VAR(_0, _1, _2, _3));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type), DECLARE_ARGS(arg0_type _0, arg1_type _1, arg2_type _2, arg3_type _3, arg4_type _4), DECLARE_ARGS_EX(arg0_type, _0, arg1_type, _1, arg2_type, _2, arg3_type, _3, arg4_type, _4), DECLARE_VAR(_0, _1, _2, _3, _4));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type), DECLARE_ARGS(arg0_type _0, arg1_type _1, arg2_type _2, arg3_type _3, arg4_type _4, arg5_type _5), DECLARE_ARGS_EX(arg0_type, _0, arg1_type, _1, arg2_type, _2, arg3_type, _3, arg4_type, _4, arg5_type, _5), DECLARE_VAR(_0, _1, _2, _3, _4, _5));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type), DECLARE_ARGS(arg0_type _0, arg1_type _1, arg2_type _2, arg3_type _3, arg4_type _4, arg5_type _5, arg6_type _6), DECLARE_ARGS_EX(arg0_type, _0, arg1_type, _1, arg2_type, _2, arg3_type, _3, arg4_type, _4, arg5_type, _5, arg6_type, _6), DECLARE_VAR(_0, _1, _2, _3, _4, _5, _6));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type), DECLARE_ARGS(arg0_type _0, arg1_type _1, arg2_type _2, arg3_type _3, arg4_type _4, arg5_type _5, arg6_type _6, arg7_type _7), DECLARE_ARGS_EX(arg0_type, _0, arg1_type, _1, arg2_type, _2, arg3_type, _3, arg4_type, _4, arg5_type, _5, arg6_type, _6, arg7_type, _7), DECLARE_VAR(_0, _1, _2, _3, _4, _5, _6, _7));
        DECLARE_TUPLE(DECLARE_PARAMS(arg0_type, arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type), DECLARE_TPYE(typename arg0_type, typename arg1_type, typename arg2_type, typename arg3_type, typename arg4_type, typename arg5_type, typename arg6_type, typename arg7_type, typename arg8_type), DECLARE_ARGS(arg0_type _0, arg1_type _1, arg2_type _2, arg3_type _3, arg4_type _4, arg5_type _5, arg6_type _6, arg7_type _7, arg8_type _8), DECLARE_ARGS_EX(arg0_type, _0, arg1_type, _1, arg2_type, _2, arg3_type, _3, arg4_type, _4, arg5_type, _5, arg6_type, _6, arg7_type, _7, arg8_type, _8), DECLARE_VAR(_0, _1, _2, _3, _4, _5, _6, _7, _8));
    }
};
