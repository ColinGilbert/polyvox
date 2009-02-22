#include "PolyVoxCore/SurfaceExtractors.h"

#include "PolyVoxCore/BlockVolume.h"
#include "PolyVoxCore/GradientEstimators.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/MarchingCubesTables.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceAdjusters.h"
#include "PolyVoxCore/BlockVolumeIterator.h"

#include "PolyVoxCore/PolyVoxImpl/DecimatedSurfaceExtractor.h"
#include "PolyVoxCore/PolyVoxImpl/FastSurfaceExtractor.h"
#include "PolyVoxCore/PolyVoxImpl/ReferenceSurfaceExtractor.h"

#include <algorithm>

using namespace std;

namespace PolyVox
{
	void extractSurface(BlockVolume<uint8>* volumeData, uint8 uLevel, Region region, IndexedSurfacePatch* singleMaterialPatch)
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

	void extractReferenceSurface(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{
		extractReferenceSurfaceImpl(volumeData, region, singleMaterialPatch);

		singleMaterialPatch->m_v3dRegionPosition = region.getLowerCorner();
	}
}
