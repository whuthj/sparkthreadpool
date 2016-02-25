/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

#include "sparkutils.hpp"

using namespace Spark::Thread;

namespace Spark
{
    namespace Thread
    {
        template<class T> class SparkSharedPtr;

        class _SparkPtrRefCount
        {
        public:
            _SparkPtrRefCount() : m_lRef(0l), m_lWeakRef(0l)
            {
                IncRef();
                //SparkUtils::DebugString(L"new _SparkPtrRefCount \n");
            }

            virtual ~_SparkPtrRefCount()
            {
                //SparkUtils::DebugString(L"delete _SparkPtrRefCount \n");
            }

            long use_count()
            {
                return m_lRef;
            }

            long use_weak_count()
            {
                return m_lWeakRef;
            }

            bool expired()
            {
                return use_count() == 0;
            }

            long IncRef()
            {
                return ::InterlockedIncrement(&m_lRef);
            }

            long IncWeakRef()
            {
                return ::InterlockedIncrement(&m_lWeakRef);
            }

            long DecWeakRef()
            {
                return ::InterlockedDecrement(&m_lWeakRef);
            }

            long DecRef()
            {
                return ::InterlockedDecrement(&m_lRef);
            }

        private:
            volatile long m_lRef;
            volatile long m_lWeakRef;
        };

        template<typename T>
        class SparkWeakPtr
        {
            friend class SparkSharedPtr<T>;
        public:
            SparkWeakPtr() : m_ptr(NULL), m_pRefCount(NULL)
            {
            }

            SparkWeakPtr(SparkWeakPtr& t)
            {
                _AddRefForPtr(t);
            }

            SparkWeakPtr(SparkSharedPtr<T>& t)
            {
                _AddRefForPtr(t);
            }

            SparkWeakPtr& operator = (SparkWeakPtr& t)
            {
                if (t.m_ptr != m_ptr)
                {
                    _ReleaseRefPtr();
                    _AddRefForPtr(t);
                }
                return *this;
            }

            SparkWeakPtr& operator = (SparkSharedPtr<T>& t)
            {
                if (t.m_ptr != m_ptr)
                {
                    _ReleaseRefPtr();
                    _AddRefForPtr(t);
                }
                return *this;
            }

            virtual ~SparkWeakPtr()
            {
                _ReleaseRefPtr();
            }

        public:
            long use_count()
            {
                return m_pRefCount == NULL ? 0 : m_pRefCount->use_count();
            }

            bool expired()
            {
                return m_pRefCount == NULL ? true : m_pRefCount->expired();
            }

        private:
            void _ReleaseRefPtr()
            {
                if (NULL == m_pRefCount)
                {
                    return;
                }

                if (0 >= m_pRefCount->DecWeakRef())
                {
                    if (expired())
                    {
                        delete m_pRefCount;
                        m_pRefCount = NULL;
                    }
                }
            }

            void _AddRefForPtr(SparkSharedPtr<T>& t)
            {
                if (NULL == t.m_pRefCount)
                {
                    return;
                }

                t._InitAndAddWeakRef(*this);
                m_pRefCount->IncWeakRef();
            }

            void _AddRefForPtr(SparkWeakPtr& t)
            {
                if (NULL == t.m_pRefCount)
                {
                    return;
                }

                m_pRefCount = t.m_pRefCount;
                m_pRefCount->IncWeakRef();
                m_ptr = t.m_ptr;
            }

        private:
            typedef SparkWeakPtr<T> this_type;

            _SparkPtrRefCount* m_pRefCount;

            T* m_ptr;
        };
    }
}