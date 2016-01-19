/*
* Author: hujun
* Email: whuthj@163.com
* Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once;

#include "sparkasyndef.hpp"

#define TUPLE_ASSIGN_0(...)
#define TUPLE_ASSIGN_1(v, ...)         _##v = v
#ifdef _MSC_VER
#define TUPLE_ASSIGN_2(v, v1, ...)    _##v = v; CONCAT(TUPLE_ASSIGN_, 1) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_3(v, v1, ...)    _##v = v; CONCAT(TUPLE_ASSIGN_, 2) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_4(v, v1, ...)    _##v = v; CONCAT(TUPLE_ASSIGN_, 3) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_5(v, v1, ...)    _##v = v; CONCAT(TUPLE_ASSIGN_, 4) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_6(v, v1, ...)    _##v = v; CONCAT(TUPLE_ASSIGN_, 5) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_7(v, v1, ...)    _##v = v; CONCAT(TUPLE_ASSIGN_, 6) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_8(v, v1, ...)    _##v = v; CONCAT(TUPLE_ASSIGN_, 7) LEFTP v1, __VA_ARGS__ RIGHTP
#define TUPLE_ASSIGN_9(v, v1, ...)    _##v = v; CONCAT(TUPLE_ASSIGN_, 8) LEFTP v1, __VA_ARGS__ RIGHTP
#else
#define TUPLE_ASSIGN_2(v, v1, ...)    _##v = v; TUPLE_ASSIGN_1(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_3(v, v1, ...)    _##v = v; TUPLE_ASSIGN_2(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_4(v, v1, ...)    _##v = v; TUPLE_ASSIGN_3(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_5(v, v1, ...)    _##v = v; TUPLE_ASSIGN_4(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_6(v, v1, ...)    _##v = v; TUPLE_ASSIGN_5(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_7(v, v1, ...)    _##v = v; TUPLE_ASSIGN_6(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_8(v, v1, ...)    _##v = v; TUPLE_ASSIGN_7(v1, __VA_ARGS__)
#define TUPLE_ASSIGN_9(v, v1, ...)    _##v = v; TUPLE_ASSIGN_8(v1, __VA_ARGS__)
#endif
#define TUPLE_ASSIGNX_(N, args)     CONCAT(TUPLE_ASSIGN_, N) args
#define TUPLE_ASSIGN(...)                TUPLE_ASSIGNX_(__VA_ARGC__(__VA_ARGS__), (__VA_ARGS__))

namespace Spark
{
    namespace Thread
    {
        struct NullType {};
        template <class HT, class TT> struct Cons;
        template <
            class T0 = NullType, class T1 = NullType, class T2 = NullType,
            class T3 = NullType, class T4 = NullType, class T5 = NullType,
            class T6 = NullType, class T7 = NullType, class T8 = NullType,
            class T9 = NullType>
        class Tuple;

        template<int N, class T> // 这个int N会递减，以呈现递归的形式
        struct Element
        {
        private:
            typedef typename T::tail_type Next;
            // 在Cons<>内部tail_type被typedef为TT，请回顾上面Cons<>的代码
        public:             // Cons<>内部有两个关键的typedef：head_type、tail_type
            typedef typename Element<N-1, Next>::type type; //递归
        };

        template<class T>
        struct Element<0,T>  //递归至N=0时，山穷水尽
        {
            typedef typename T::head_type type; // 山穷水尽时直接将head_type定义为type
        };

        template <class HT, class TT>
        struct Cons {
            typedef HT head_type;
            typedef TT tail_type;
            head_type head; 
            tail_type tail;

            template <int N>
            typename Element<N, Cons<HT, TT> >::type Get()
            {
                return Spark::Thread::Get<N>(*this);  //转向全局的Get<>函数
            }

            Cons() {}

            template <class T1, class T2, class T3, class T4, class T5,
            class T6, class T7, class T8, class T9, class T10>
            Cons( T1& t1, T2& t2, T3& t3, T4& t4, T5& t5,
                T6& t6, T7& t7, T8& t8, T9& t9, T10& t10 )
                : head (t1), tail (t2, t3, t4, t5, t6, t7, t8, t9, t10, NullType())
            { }

            template <class T2, class T3, class T4, class T5,
            class T6, class T7, class T8, class T9, class T10>
                Cons( const NullType& t1, // 当接受的第一个参数为NullType时
                T2& t2, T3& t3, T4& t4, T5& t5,
                T6& t6, T7& t7, T8& t8, T9& t9, T10& t10 )
                : head (), tail (t2, t3, t4, t5, t6, t7, t8, t9, t10, NullType())
            {}
        };
        template <class HT>
        struct Cons<HT, NullType> {
            typedef HT head_type;
            typedef NullType tail_type;
            typedef Cons<HT, NullType> self_type;

            head_type head;

            Cons() {}

            template<class T1>
            Cons(T1& t1, const NullType&, const NullType&, const NullType&,
                const NullType&, const NullType&, const NullType&,
                const NullType&, const NullType&, const NullType&)
                : head (t1) {}

            Cons(const NullType&,
                const NullType&, const NullType&, const NullType&,
                const NullType&, const NullType&, const NullType&,
                const NullType&, const NullType&, const NullType&)
                : head () {}
        };

        template<int N>
        struct GetClass {
            template<class RET, class HT, class TT >
            inline static RET Get(Cons<HT, TT>& t)
            {
                return GetClass<N-1>::template Get<RET>(t.tail);
            }
        };
        template<>
        struct GetClass<0> {
            template<class RET, class HT, class TT>
            inline static RET Get(Cons<HT, TT>& t)
            {
                return t.head;
            }
        };
        template<int N, class HT, class TT>
        inline typename Element<N, Cons<HT, TT> >::type Get(Cons<HT, TT>& c)
        {
            return Spark::Thread::GetClass<N>::template Get<typename Element<N, Cons<HT, TT> >::type>(c); 
        }

        template<class T>
        struct Length {
            static const int value = 1 + Length<typename T::tail_type>::value; //递归
        };
        template<>
        struct Length<NullType> {
            static const int value = 0;
        };

        template <class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8, class T9>
        struct MapTupleToCons
        {
            typedef Cons<T0,
                typename MapTupleToCons<T1, T2, T3, T4, T5,
                T6, T7, T8, T9, NullType>::type
            > type;
        };
        template <>
        struct MapTupleToCons<NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType>
        {
            typedef NullType type;
        };

        //Tuple<int, long, double> => Cons<int,Cons<long,Cons<bool,Cons<double,NullType> > > >
        //Cons<T0,Cons<T1,Cons<T2,Cons<T3,... ... > > > >
        template <class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8, class T9>
        class Tuple : public MapTupleToCons<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type
        {
        public:
            typedef typename MapTupleToCons<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::type inherited; // 基类
            // 基类的head_type(通常即T0,见Cons<>的定义)
            typedef typename inherited::head_type head_type;
            // 基类的tail_type(一般仍为一个Cons<>)
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
        class Tuple<NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType, NullType> :
            public NullType
        {
        public:
            typedef NullType inherited;
        };

//////////////////////////////////////////////////////////////////////////

        template<typename Signature> struct SimpleTuple;
#define DECLARE_TUPLE(ptype, classparam, args, args_ex, var)\
        template<classparam>\
        struct SimpleTuple<void(ptype)>\
        {\
            SimpleTuple() {} \
            SimpleTuple(args)\
            {\
                TUPLE_ASSIGN(var);\
            }\
            FIELD(args_ex);\
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
