#include "SurfaceExtractors.h"

#include "Volume.h"
#include "GradientEstimators.h"
#include "IndexedSurfacePatch.h"
#include "PolyVoxImpl/MarchingCubesTables.h"
#include "Region.h"
#include "SurfaceAdjusters.h"
#include "VolumeIterator.h"

#include "PolyVoxImpl/DecimatedSurfaceExtractor.h"
#include "PolyVoxImpl/FastSurfaceExtractor.h"
#include "PolyVoxImpl/ReferenceSurfaceExtractor.h"

#include <algorithm>

using namespace std;

namespace PolyVox
{
	void extractSurface(Volume<uint8_t>* volumeData, uint8_t uLevel, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{
		if(uLevel == 0)
		{
			extractFastSurfaceImpl(volumeData, region, singleMaterialPatch);
		}
		else
		{
			extractDecimatedSurfaceImpl(volumeData, uLevel, region, singleMaterialPatch);
		}

		singleMaterialPatch->m_v3dRegionPosition = region.getLowerCorner();
	}

	void extractReferenceSurface(Volume<uint8_t>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{
		extractReferenceSurfaceImpl(volumeData, region, singleMaterialPatch);

		singleMaterialPatch->m_v3dRegionPosition = region.getLowerCorner();
	}
}
