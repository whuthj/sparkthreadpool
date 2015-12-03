#pragma once

#include <list>
#include "sparklock.hpp"

namespace Spark
{
    namespace Thread
    {
        template<typename T>
        class SparkSyncQueue
        {
        public:
            void Push(T x)
            {
                SparkLocker locker(m_lockQueue);
                m_Queue.push_back(x);
            }

            T PopFront()
            {
                T pObj = NULL;
                {
                    SparkLocker locker(m_lockQueue);

                    if (!m_Queue.empty())
                    {
                        pObj = m_Queue.front();
                        m_Queue.pop_front();
                    }
                }
                return pObj;
            }

            bool Empty()
            {
                SparkLocker locker(m_lockQueue);
                return m_Queue.empty();
            }

            int Count()
            {
                int nCount = 0;

                SparkLocker locker(m_lockQueue);
                nCount = m_Queue.size();

                return nCount;
            }
        private:
            std::list<T> m_Queue;
            SparkLock m_lockQueue;
        };

        template<typename K, typename V>
        class SparkSyncMap
        {
        public:
            void Push(K k, V v)
            {
                SparkLocker locker(m_lockMap);
                m_Map.insert(std::make_pair(k, v));
            }

            void Remove(K k)
            {
                SparkLocker locker(m_lockMap);
                std::map<K, V>::iterator itr = m_Map.find(k);
                if (itr != m_Map.end())
                {
                    m_Map.erase(itr);
                }
            }

            V Get(const K& k)
            {
                V pObj = NULL;
                {
                    SparkLocker locker(m_lockMap);
                    std::map<K, V>::iterator itr = m_Map.find(k);

                    if (itr != m_Map.end())
                    {
                        pObj = itr->second;
                    }
                }
                return pObj;
            }

            int Count()
            {
                int nCount = 0;

                SparkLocker locker(m_lockMap);
                nCount = m_Map.size();

                return nCount;
            }

        private:
            std::map<K, V> m_Map;
            SparkLock m_lockMap;
        };
    }
}