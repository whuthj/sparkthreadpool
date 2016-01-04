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

        template<typename T>
        class SparkWeakPtr
        {
            friend class SparkSharedPtr<T>;
        public:
            SparkWeakPtr() : m_ptr(NULL), m_plRef(NULL), m_plWeakRef(NULL)
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
                return m_plRef == NULL ? 0 : *m_plRef;
            }

            bool expired()
            {
                return m_plRef == NULL;
            }

        private:
            void _ReleaseRefPtr()
            {
                if (NULL == m_plWeakRef)
                {
                    return;
                }

                if (0 == ::InterlockedDecrement((long *)m_plWeakRef) && 0 == use_count())
                {
                    if (m_plWeakRef)
                    {
                        delete m_plWeakRef;
                        m_plWeakRef = NULL;
                    }
                }
                else
                {
                    ::InterlockedIncrement((long *)m_plWeakRef);
                }
            }

            void _InitRef(SparkSharedPtr<T>& t)
            {
                if (NULL == t.m_plRef)
                {
                    return;
                }

                t._InitAndAddWeakRef(*this);
            }

        private:
            typedef SparkWeakPtr<T> this_type;

            long* m_plRef;
            long* m_plWeakRef;

            T* m_ptr;
        };
    }
}