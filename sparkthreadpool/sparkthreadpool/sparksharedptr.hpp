#pragma once

namespace Spark
{
    namespace Memory
    {
        template<typename T>
        class SparkSharedPtr
        {
        public:
            SparkSharedPtr() : m_ptr(NULL), m_plRef(NULL)
            {

            }

            SparkSharedPtr(T* ptr) : m_ptr(ptr), m_plRef(NULL)
            {
                m_plRef = new long(1l);
            }

            SparkSharedPtr(const SparkSharedPtr& t)
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

        public:
            long* m_plRef;
            T* m_ptr;
        };
    }
}