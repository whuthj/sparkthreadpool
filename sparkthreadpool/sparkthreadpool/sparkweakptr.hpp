#pragma once

namespace Spark
{
    namespace Memory
    {
        template<class T> class SparkSharedPtr;

        template<typename T>
        class SparkWeakPtr
        {
        public:
            SparkWeakPtr() : m_ptr(NULL), m_plRef(NULL)
            {
                m_plRef = new long(0l);
            }

            SparkWeakPtr(const SparkSharedPtr& t)
            {
                _InitRef(t);
            }

        public:
            long use_count()
            {
                return 0;
            }

            bool expired()
            {
                return false;
            }

        private:
            void _InitRef(const SparkSharedPtr& t)
            {
                if (NULL == t.m_plRef)
                {
                    return;
                }

                m_ptr = t.m_ptr;
                ::InterlockedExchange((long *)m_plRef, *t.m_plRef);
            }

        private:
            long* m_plRef;
            T* m_ptr;
        };
    }
}