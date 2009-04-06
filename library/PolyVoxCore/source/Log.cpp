#include "Log.h"

namespace PolyVox
{
	void (*logHandler)(std::string, int severity) = 0;
}