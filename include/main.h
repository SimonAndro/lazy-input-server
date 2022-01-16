#ifndef _MAIN_HEADER_
#define _MAIN_HEADER_

#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif


#include "resource.h"

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <process.h>

#endif //_MAIN_HEADER_