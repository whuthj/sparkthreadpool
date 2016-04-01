/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
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

        #if (_WIN32_WINNT >= 0x0600)
        class SparkRWLock
        {
        public:
            SparkRWLock(void)
            {
                ::InitializeSRWLock(&m_srwLock); 
            }

            void LockR(void)
            {
                ::AcquireSRWLockShared(&m_srwLock); 
            }

            void LockW(void)
            {
                ::AcquireSRWLockExclusive(&m_srwLock); 
            }

            void UnlockR(void)
            {
                ::ReleaseSRWLockShared(&m_srwLock); 
            }

            void UnlockW(void)
            {
                ::ReleaseSRWLockExclusive(&m_srwLock); 
            }

        private:
            SRWLOCK  m_srwLock; 
        };

        class SparkRWLocker
        {
        public:
            SparkRWLocker(SparkRWLock& locker, bool bRead) 
                : m_srwLocker(locker), m_bRead(bRead), m_bLocked(true)
            {
                if (m_bRead) { m_srwLocker.LockR(); }
                else { m_srwLocker.LockW(); } 
            }

            ~SparkRWLocker(void)
            {
                if (m_bRead) { m_srwLocker.UnlockR(); } 
                else { m_srwLocker.UnlockW(); }
            }

            operator bool (void) const
            {
                return m_bLocked; 
            }

            void Unlock(void)
            {
                m_bLocked = false; 
            }

        private:
            SparkRWLock& m_srwLocker; 
            bool      m_bLocked; 
            bool      m_bRead; 
        }; 
        #endif
    }
}
