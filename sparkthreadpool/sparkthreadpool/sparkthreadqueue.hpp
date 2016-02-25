/*
 * Author: hujun
 * Email: whuthj@163.com
 * Website: https://github.com/whuthj/sparkthreadpool
*/
#pragma once

#include "sparklock.hpp"

namespace Spark
{
    namespace Thread
    {
        template<typename T>
        class SparkThreadQueue
        {
        public:
            SparkThreadQueue()
            {
                m_uMaxThread = 0;
                m_uWaitThreadCount = 0;
                m_uQueueLength = 0;
                m_hCompletionPort = NULL;
            }

            int Open(unsigned int uMaxThread)
            {
                SparkLocker guard(m_mutex);
                if (m_hCompletionPort != NULL)
                {
                    return -1;
                }

                m_uMaxThread = uMaxThread;
                m_hCompletionPort = ::CreateIoCompletionPort(
                    INVALID_HANDLE_VALUE,
                    NULL,
                    0,
                    uMaxThread
                    );
                if (m_hCompletionPort == NULL)
                {
                    return GetLastError();
                }

                return 0;
            }

            int Close()
            {
                SparkLocker guard(m_mutex);
                if (m_hCompletionPort == NULL)
                {
                    return -1;
                }

                // 发给所有
                for (unsigned int i = 0; i < m_uWaitThreadCount; ++i)
                {
                    ::PostQueuedCompletionStatus(
                        m_hCompletionPort,
                        0,
                        0xffffffff,
                        NULL
                        );
                }

                CloseHandle(m_hCompletionPort);
                m_hCompletionPort = NULL;

                return 0;
            }

            int Enqueue(T* pValue)
            {
                SparkLocker guard(m_mutex);
                if (m_hCompletionPort == NULL)
                {
                    return -1;
                }

                BOOL bRet = ::PostQueuedCompletionStatus(
                    m_hCompletionPort,
                    0,
                    0,
                    reinterpret_cast<LPOVERLAPPED>(pValue)
                    );
                if (!bRet)
                {
                    return GetLastError();
                }

                return 0;
            }

            int Dequeue(
                T** ppValue,
                unsigned int uMillisecond
                )
            {
                {
                    SparkLocker guard(m_mutex);
                    if (m_hCompletionPort == NULL)
                    {
                        return -1;
                    }

                    ++m_uWaitThreadCount;
                }

                DWORD dwSize = 0;
                DWORD dwKey = 0;
                BOOL bRet = ::GetQueuedCompletionStatus(
                    m_hCompletionPort,
                    &dwSize,
                    &dwKey,
                    reinterpret_cast<LPOVERLAPPED*>(ppValue),
                    uMillisecond
                    );

                {
                    SparkLocker guard(m_mutex);
                    if (m_hCompletionPort == NULL)
                    {
                        return -1;
                    }

                    --m_uWaitThreadCount;
                }

                if (!bRet)
                {
                    return ::GetLastError();
                }

                if (dwKey == 0xFFFFFFFF) // 标志结束
                {
                    return -1;
                }
                else  // 取出了正确数据
                {
                    return 0;
                }
            }

        private:
            SparkLock m_mutex;
            unsigned int m_uWaitThreadCount;
            unsigned int m_uMaxThread;
            unsigned int m_uQueueLength;
            HANDLE m_hCompletionPort;
        };
    }
}