#ifndef __PolyVox_Log_H__
#define __PolyVox_Log_H__

#include "PolyVoxImpl/TypeDef.h"

#include <string>

POLYVOXCORE_API extern void (*logImpl)(std::string);

#endif