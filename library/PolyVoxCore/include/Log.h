#ifndef __PolyVox_Log_H__
#define __PolyVox_Log_H__

#include "PolyVoxImpl/TypeDef.h"

#include <string>

//Note: The functions in this file are not for the user to call - they are 
//intended for internal use only. The only exception is that you may set the
//logHandler pointer to point at your own handling funtion for printing, etc.

namespace PolyVox
{
	enum LogSeverity
	{
		LS_DEBUG,
		LS_INFO,
		LS_WARN,
		LS_ERROR
	};	

	POLYVOXCORE_API extern void (*logHandler)(std::string, int severity);
}

//Debug severity messages are only used if we are a debug build
#ifdef _DEBUG
	#define POLYVOX_LOG_DEBUG(message) if(logHandler){logHandler(message, LS_DEBUG);}
#else
	#define POLYVOX_LOG_DEBUG(message)
#endif

//Other severity levels work in both debug and release
#define POLYVOX_LOG_INFO(message) if(logHandler){logHandler(message, LS_INFO);}
#define POLYVOX_LOG_WARN(message) if(logHandler){logHandler(message, LS_WARN);}
#define POLYVOX_LOG_ERROR(message) if(logHandler){logHandler(message, LS_ERROR);}

#endif