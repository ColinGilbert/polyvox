#include "PolyVoxCore/SurfaceExtractors.h"

#include "PolyVoxCore/BlockVolume.h"
#include "PolyVoxCore/GradientEstimators.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/MarchingCubesTables.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/RegionGeometry.h"
#include "PolyVoxCore/SurfaceAdjusters.h"
#include "PolyVoxCore/BlockVolumeIterator.h"

#include "PolyVoxCore/PolyVoxImpl/DecimatedSurfaceExtractor.h"
#include "PolyVoxCore/PolyVoxImpl/FastSurfaceExtractor.h"
#include "PolyVoxCore/PolyVoxImpl/ReferenceSurfaceExtractor.h"

#include <algorithm>

using namespace std;

namespace PolyVox
{
	void generateReferenceMeshDataForRegion(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{
		generateReferenceMeshDataForRegion(volumeData, region, singleMaterialPatch);
	}

	void generateRoughMeshDataForRegion(BlockVolume<uint8>* volumeData, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{
		generateRoughMeshDataForRegionImpl(volumeData, region, singleMaterialPatch);
	}

	void generateDecimatedMeshDataForRegion(BlockVolume<uint8>* volumeData, uint8 uLevel, Region region, IndexedSurfacePatch* singleMaterialPatch)
	{
		generateDecimatedMeshDataForRegionImpl(volumeData, uLevel, region, singleMaterialPatch);
	}
}
