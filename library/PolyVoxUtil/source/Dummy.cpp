#include "PolyVoxImpl/TypeDef.h"

namespace PolyVox
{	
	class POLYVOXCORE_API DummyClass
	{
	public:
		int getx(void);
		int x;
	};
	
	int DummyClass::getx(void)
	{
		return x;
	}
}