#pragma once

#include <map>
#include "sparksharedptr.hpp"

namespace Spark
{
    namespace Thread
    {
        template<typename T>
        class SparkMultiton
        {
        private:
            typedef std::map<int, SparkSharedPtr<T>> MAP_INSTANCE;
            typedef typename MAP_INSTANCE::iterator MAP_INSTANCE_ITR;

        public:
            static SparkSharedPtr<T> Instance(int key = 0)
            {
                static MAP_INSTANCE _mapInstance;
                static volatile LONG _lock = 0;

                SparkSharedPtr<T> retIns;
                MAP_INSTANCE_ITR itr;
                bool bFind = _SafeFindInstance(key, _lock, _mapInstance, itr);

                while (!bFind)
                {
                    if (::InterlockedIncrement(&_lock) == 1)
                    {
                        bFind = _FindInstance(key, _lock, _mapInstance, itr);
                        if (!bFind)
                        {
                            retIns = SparkSharedPtr<T>(new T);
                            _mapInstance.insert(std::make_pair(key, retIns));
                        }
                        ::InterlockedDecrement(&_lock);
                    }
                    else
                    {
                        ::InterlockedDecrement(&_lock);
                        ::Sleep(10);
                    }
                    bFind = _SafeFindInstance(key, _lock, _mapInstance, itr);
                }

                if (bFind)
                {
                    retIns = itr->second;
                }

                return retIns;
            }

        private:
            static bool _SafeFindInstance(
                int key,
                volatile LONG& lock, 
                MAP_INSTANCE& mapInstance,
                MAP_INSTANCE_ITR& itr)
            {
                bool bFind = false;

                if (::InterlockedIncrement(&lock) == 1)
                {
                    itr = mapInstance.find(key);
                    bFind = mapInstance.end() != itr;
                }
                ::InterlockedDecrement(&lock);

                return bFind;
            }

            static bool _FindInstance(
                int key,
                volatile LONG& lock,
                MAP_INSTANCE& mapInstance,
                MAP_INSTANCE_ITR& itr)
            {
                bool bFind = false;

                itr = mapInstance.find(key);
                bFind = mapInstance.end() != itr;

                return bFind;
            }

        };
    }
}
