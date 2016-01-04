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
            }

            SparkSharedPtr(const SparkSharedPtr& t)
            {
                _AddRefForPtr(t);
            }

            SparkSharedPtr(const SparkWeakPtr<T>& t)
            {
                _AddRefForPtr(t);
            }

            SparkSharedPtr& operator = (const SparkSharedPtr& t)
            {
                if (t.m_ptr != m_ptr)
                {
                    _ReleaseRefPtr();
                    _AddRefForPtr(t);
                }
                return *this;
            }

            virtual ~SparkSharedPtr()
            {
                _ReleaseRefPtr();
            }

            long use_count()
            {
                return m_pRefCount == NULL ? 0 : m_pRefCount->use_count();
            }

            T& operator*() { return *m_ptr; }
            T* operator->() { return m_ptr; }

        private:
            void _ReleaseRefPtr()
            {
                if (NULL == m_pRefCount)
                {
                    return;
                }

               if (0 < m_pRefCount->DecRef())
               {
                    return;
               }

                if (m_ptr)
                {
                    delete m_ptr;
                    m_ptr = NULL;
                }

                if (m_pRefCount->TryRelease())
                {
                    m_pRefCount = NULL;
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

                return false;
            }

            void _InitAndAddWeakRef(SparkWeakPtr<T>& weak)
            {
                m_pRefCount->IncWeakRef();
                weak.m_pRefCount = m_pRefCount;
            }

         private:
           _SparkPtrRefCount* m_pRefCount;
            T* m_ptr;
        };
    }
}