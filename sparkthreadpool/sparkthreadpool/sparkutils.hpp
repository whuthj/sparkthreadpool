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
        class SparkUtils
        {
        public:
            static void DebugString(LPCTSTR format, ...)
            {
                if (format)
                {
                    va_list args;
                    va_start(args, format);
                    CString strText = L"[SparkThreadPool]";
                    strText.AppendFormatV(format, args);
                    va_end(args);

                    ::OutputDebugString(strText);
                }
            }

        };
    }
}
