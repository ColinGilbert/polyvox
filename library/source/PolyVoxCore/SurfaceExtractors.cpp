#include "PolyVoxCore/SurfaceExtractors.h"

#include "PolyVoxCore/Volume.h"
#include "PolyVoxCore/GradientEstimators.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/MarchingCubesTables.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceAdjusters.h"
#include "PolyVoxCore/VolumeIterator.h"

#include "PolyVoxCore/PolyVoxImpl/DecimatedSurfaceExtractor.h"
#include "PolyVoxCore/PolyVoxImpl/FastSurfaceExtractor.h"
#include "PolyVoxCore/PolyVoxImpl/ReferenceSurfaceExtractor.h"

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
