#pragma once

#include <typeinfo>
#include <assert.h>

namespace Spark
{
    namespace Thread
    {
        class Any
        {
        protected:
            class placeholder
            {
            public:
                virtual ~placeholder() { }
            public:
                virtual const std::type_info & type() const = 0;
                virtual placeholder * clone() const = 0;
            };

            template<typename T>
            class holder : public placeholder
            {
            public:
                holder(const T& val) : held(val) { }

            public:
                virtual const std::type_info & type() const
                {
                    return typeid(T);
                }

                virtual placeholder * clone() const
                {
                    return new holder(held);
                }

            public: 
                T held;

              };

        public:
            Any() : m_pData(NULL) { } 

            template<typename T>  
            Any(const T & value) : m_pData(new holder<T>(value))
            {
            }

            Any(const Any & other)  
                : m_pData(other.m_pData ? other.m_pData->clone() : 0) 
            {
            }

            ~Any()
            {
                if(NULL != m_pData)
                {
                    delete m_pData;
                    m_pData = NULL;
                }
            }

            const std::type_info & type() const 
            {
                return m_pData ? m_pData->type() : typeid(void);
            }

        private:
            placeholder* m_pData;

            template<typename T>
            friend T AnyCast(const Any& operand); 
        };  

        template<typename T>
        T AnyCast(const Any& operand)
        {
            assert( operand.type() == typeid(T) );
            return static_cast<Any::holder<T> *>(operand.m_pData)->held;
        }

    }
}