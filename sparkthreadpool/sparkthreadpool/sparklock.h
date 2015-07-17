#pragma once 

namespace Spark
{
    namespace Thread
    {
        class SparkLock
        {
        public:
            SparkLock()
            {
                memset(&sec, 0, sizeof(CRITICAL_SECTION));
                ::InitializeCriticalSection(&sec);
            }

            ~SparkLock()
            {
                ::DeleteCriticalSection(&sec);
            }
    
            void Lock()
            {
                EnterCriticalSection(&sec);
            }

            void Unlock()
            {
                LeaveCriticalSection(&sec);
            }

            BOOL TryLock()
            {
                return TryEnterCriticalSection(&sec);
            }

        private:
            CRITICAL_SECTION  sec;

        };

        class SparkLocker
        {
        private:
            SparkLock& m_lock;

        public:
            SparkLocker(SparkLock& lock) : m_lock(lock)
            {
                m_lock.Lock();
            }

            ~SparkLocker()
            {
                m_lock.Unlock();
            }
        };
    }
}
