#include "IndexedSurfacePatch.h"
#include "RegionGeometry.h"

namespace PolyVox
{

	RegionGeometry::RegionGeometry()
	{
	}

	long int RegionGeometry::getSizeInBytes(void)
		{
			long int size = sizeof(RegionGeometry);
			if(m_patchSingleMaterial)
			{
				size += m_patchSingleMaterial->getSizeInBytes();
			}
			if(m_patchMultiMaterial)
			{
				size += m_patchMultiMaterial->getSizeInBytes();
			}
			return size;
		}
}