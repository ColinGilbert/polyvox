#include "PolyVoxCore/VoxelFilters.h"

#include "PolyVoxCore/VolumeIterator.h"

namespace PolyVox
{
	float computeSmoothedVoxel(VolumeIterator<uint8>& volIter)
	{
		assert(volIter.getPosX() >= 1);
		assert(volIter.getPosY() >= 1);
		assert(volIter.getPosZ() >= 1);
		assert(volIter.getPosX() <= volIter.getVolume().getSideLength() - 2);
		assert(volIter.getPosY() <= volIter.getVolume().getSideLength() - 2);
		assert(volIter.getPosZ() <= volIter.getVolume().getSideLength() - 2);

		float sum = 0.0;

		if(volIter.peekVoxel1nx1ny1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1ny0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1ny1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx0py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx0py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx0py1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1nx1py1pz() != 0) sum += 1.0f;

		if(volIter.peekVoxel0px1ny1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1ny0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1ny1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px0py1nz() != 0) sum += 1.0f;
		if(volIter.getVoxel() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px0py1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel0px1py1pz() != 0) sum += 1.0f;

		if(volIter.peekVoxel1px1ny1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1ny0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1ny1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px0py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px0py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px0py1pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1py1nz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1py0pz() != 0) sum += 1.0f;
		if(volIter.peekVoxel1px1py1pz() != 0) sum += 1.0f;

		sum /= 27.0f;
		return sum;
	}
}