#pragma once

#define WINVER			0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE		0x0501

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>

#include "sparkthreadpool\sparkthreadpool.hpp"
#include "sparkthreadpool\sparksharedptr.hpp"

using namespace Spark::Thread;
using namespace Spark::Memory;