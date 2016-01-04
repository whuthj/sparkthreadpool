/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

namespace Spark
{
    namespace Memory
    {
        template<class T> class SparkSharedPtr;

        class _SparkPtrRefCount
        {
        public:
            _SparkPtrRefCount() : m_plRef(NULL), m_plWeakRef(NULL)
            {
                m_plRef = new long(1l);
                m_plWeakRef = new long(0l);
            }

            long use_count()
            {
                return m_plRef == NULL ? 0 : *m_plRef;
            }

            bool expired()
            {
                return use_count() == 0;
            }

            long IncRef()
            {
                if (NULL == m_plRef)
                {
                    return -1;
                }

                return ::InterlockedIncrement((long *)m_plRef);
            }

            long DecRef()
            {
                if (NULL == m_plRef)
                {
                    return -1;
                }

                 return ::InterlockedDecrement((long *)m_plRef);
            }

            long IncWeakRef()
            {
                if (NULL == m_plWeakRef)
                {
                    return -1;
                }

                ::InterlockedIncrement((long *)m_plWeakRef);
            }

            long DecWeakRef()
            {
                if (NULL == m_plWeakRef)
                {
                    return -1;
                }

                return ::InterlockedDecrement((long *)m_plWeakRef);
            }

            void ReleaseRef()
            {
                if (m_plRef)
                {
                    delete m_plRef;
                    m_plRef = NULL;
                }
            }

            bool TryRelease()
            {
                if (NULL == m_plWeakRef)
                {
                    return false;
                }

                if (0 != use_count())
                {
                    return false;
                }

                if (0 == DecWeakRef())
                {
                    if (m_plWeakRef)
                    {
                        delete m_plWeakRef;
                        m_plWeakRef = NULL;
                    }

                    delete this;
                    return true;
                }

                return false;
            }

        private:
            long* m_plRef;
            long* m_plWeakRef;
        };

        template<typename T>
        class SparkWeakPtr
        {
            friend class SparkSharedPtr<T>;
        public:
            SparkWeakPtr() : m_ptr(NULL), m_pRefCount(NULL)
            {
            }

            SparkWeakPtr(SparkSharedPtr<T>& t)
            {
                _InitRef(t);
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
                    return ;
                }

                if (m_pRefCount->TryRelease())
                {
                    m_pRefCount = NULL;
                }
            }

            void _InitRef(SparkSharedPtr<T>& t)
            {
                if (NULL == t.m_pRefCount)
                {
                    return;
                }

                t._InitAndAddWeakRef(*this);
            }

        private:
            typedef SparkWeakPtr<T> this_type;

            _SparkPtrRefCount* m_pRefCount;

            T* m_ptr;
        };
    }
}