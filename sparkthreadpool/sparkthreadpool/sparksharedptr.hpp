/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

#include "sparkweakptr.hpp"

namespace Spark
{
    namespace Memory
    {
        template<typename T>
        class SparkSharedPtr
        {
            friend class SparkWeakPtr<T>;
        public:
            SparkSharedPtr() : m_ptr(NULL), m_pRefCount(NULL)
            {
            }

            SparkSharedPtr(T* ptr) : m_ptr(ptr), m_pRefCount(NULL)
            {
                m_pRefCount = new _SparkPtrRefCount();
                SparkUtils::DebugString(L"%d, new SparkSharedPtr \n", this);
            }

            SparkSharedPtr(const SparkSharedPtr& t)
            {
                _AddRefForPtr(t);
                SparkUtils::DebugString(L"%d, copy new SparkSharedPtr \n", this);
            }

            SparkSharedPtr(const SparkWeakPtr<T>& t)
            {
                _AddRefForPtr(t);
                SparkUtils::DebugString(L"%d, weak, new SparkSharedPtr \n", this);
            }

            SparkSharedPtr& operator = (const SparkSharedPtr& t)
            {
                if (t.m_ptr != m_ptr)
                {
                    _ReleaseRefPtr();
                    _AddRefForPtr(t);
                }
                SparkUtils::DebugString(L"%d, new SparkSharedPtr \n", this);
                return *this;
            }

            virtual ~SparkSharedPtr()
            {
                SparkUtils::DebugString(L"%d, delete SparkSharedPtr \n", this);
                _ReleaseRefPtr();
            }

            long use_count()
            {
                return m_pRefCount == NULL ? 0 : m_pRefCount->use_count();
            }

            bool expired()
            {
                return m_pRefCount == NULL ? true : m_pRefCount->expired();
            }

            T& operator*() { return *m_ptr; }
            T* operator->() { return m_ptr; }
            operator T* () { return m_ptr; }

        private:
            void _ReleaseRefPtr()
            {
                if (NULL == m_pRefCount)
                {
                    return;
                }

                if (0 == m_pRefCount->DecRef())
                {
                    if (m_ptr)
                    {
                        delete m_ptr;
                        m_ptr = NULL;
                    }

                    // 0 >= 是因为可能弱引用没被用过，可能减到-1 
                    if (0 >= m_pRefCount->DecWeakRef())
                    {
                        delete m_pRefCount;
                        m_pRefCount = NULL;
                    }
                }
            }

            bool _AddRefForPtr(const SparkSharedPtr& t)
            {
                if (NULL == t.m_pRefCount)
                {
                    return false;
                }

                m_ptr = t.m_ptr;
                m_pRefCount = t.m_pRefCount;
                m_pRefCount->IncRef();

                return true;
            }

            bool _AddRefForPtr(const SparkWeakPtr<T>& t)
            {
                if (NULL != t.m_pRefCount && !t.m_pRefCount->expired())
                {
                    m_ptr = t.m_ptr;
                    m_pRefCount = t.m_pRefCount;
                    m_pRefCount->IncRef();

                    return true;
                }

                m_ptr = NULL;
                m_pRefCount = NULL;

                return false;
            }

            void _InitAndAddWeakRef(SparkWeakPtr<T>& weak)
            {
                m_pRefCount->IncWeakRef();
                weak.m_pRefCount = m_pRefCount;
                weak.m_ptr = m_ptr;
            }

         private:
           _SparkPtrRefCount* m_pRefCount;
            T* m_ptr;
        };
    }
}