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
            SparkSharedPtr() : m_ptr(NULL), m_plRef(NULL), m_plWeakRef(NULL)
            {

            }

            SparkSharedPtr(T* ptr) : m_ptr(ptr), m_plRef(NULL), m_plWeakRef(NULL)
            {
                m_plRef = new long(1l);
                m_plWeakRef = new long(0l);
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
                return m_plRef == NULL ? 0 : *m_plRef;
            }

            T& operator*() { return *m_ptr; }
            T* operator->() { return m_ptr; }

        private:
            void _ReleaseRefPtr()
            {
                if (NULL == m_plRef)
                {
                    return;
                }

                if (0 == ::InterlockedDecrement((long *)m_plRef))
                {
                    if (m_ptr)
                    {
                        delete m_ptr;
                        m_ptr = NULL;
                    }

                    if (m_plRef)
                    {
                        delete m_plRef;
                        m_plRef = NULL;
                    }
                }
            }

            void _AddRefForPtr(const SparkSharedPtr& t)
            {
                if (NULL == t.m_plRef)
                {
                    return;
                }

                m_ptr = t.m_ptr;
                m_plRef = t.m_plRef;
                ::InterlockedIncrement((long *)m_plRef);
            }

            void _AddRefForPtr(const SparkWeakPtr<T>& t)
            {
                if (NULL == t.m_plRef)
                {
                    return;
                }

                m_ptr = t.m_ptr;
                m_plRef = t.m_plRef;
                ::InterlockedIncrement((long *)m_plRef);
            }

            void _InitAndAddWeakRef(SparkWeakPtr<T>& weak)
            {
                ::InterlockedIncrement((long *)m_plWeakRef);

                weak.m_plWeakRef = m_plWeakRef;
                weak.m_plRef = m_plRef;
                weak.m_ptr = m_ptr;
            }

         private:
            long* m_plRef;
            long* m_plWeakRef;

            T* m_ptr;
        };
    }
}