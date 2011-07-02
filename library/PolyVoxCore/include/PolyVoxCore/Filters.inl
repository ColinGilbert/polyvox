#include "PolyVoxCore/MaterialDensityPair.h"

namespace PolyVox
{
	template< template<typename> class VolumeType, typename VoxelType>
	void smoothRegion(VolumeType<VoxelType>& volData, const Region& regionToSmooth)
	{
		Region croppedRegion = regionToSmooth;

		uint32_t uArrayWidth = croppedRegion.getUpperCorner().getX() - croppedRegion.getLowerCorner().getX() + 1;
		uint32_t uArrayHeight = croppedRegion.getUpperCorner().getY() - croppedRegion.getLowerCorner().getY() + 1;
		uint32_t uArrayDepth = croppedRegion.getUpperCorner().getZ() - croppedRegion.getLowerCorner().getZ() + 1;
		Array<3, uint16_t> temp(ArraySizes(uArrayWidth)(uArrayHeight)(uArrayDepth));

		for (int32_t z = croppedRegion.getLowerCorner().getZ(); z <= croppedRegion.getUpperCorner().getZ(); z++)
		{
			for (int32_t y = croppedRegion.getLowerCorner().getY(); y <= croppedRegion.getUpperCorner().getY(); y++)
			{
				for (int32_t x = croppedRegion.getLowerCorner().getX(); x <= croppedRegion.getUpperCorner().getX(); x++)
				{
					uint16_t& uDensity = temp[x-croppedRegion.getLowerCorner().getX()][y-croppedRegion.getLowerCorner().getY()][z-croppedRegion.getLowerCorner().getZ()];

					uDensity=0;
					uDensity += volData.getVoxelAt(x-1,y-1,z-1).getDensity();
					uDensity += volData.getVoxelAt(x-1,y-1,z-0).getDensity();
					uDensity += volData.getVoxelAt(x-1,y-1,z+1).getDensity();
					uDensity += volData.getVoxelAt(x-1,y-0,z-1).getDensity();
					uDensity += volData.getVoxelAt(x-1,y-0,z-0).getDensity();
					uDensity += volData.getVoxelAt(x-1,y-0,z+1).getDensity();
					uDensity += volData.getVoxelAt(x-1,y+1,z-1).getDensity();
					uDensity += volData.getVoxelAt(x-1,y+1,z-0).getDensity();
					uDensity += volData.getVoxelAt(x-1,y+1,z+1).getDensity();

					uDensity += volData.getVoxelAt(x-0,y-1,z-1).getDensity();
					uDensity += volData.getVoxelAt(x-0,y-1,z-0).getDensity();
					uDensity += volData.getVoxelAt(x-0,y-1,z+1).getDensity();
					uDensity += volData.getVoxelAt(x-0,y-0,z-1).getDensity();
					uDensity += volData.getVoxelAt(x-0,y-0,z-0).getDensity();
					uDensity += volData.getVoxelAt(x-0,y-0,z+1).getDensity();
					uDensity += volData.getVoxelAt(x-0,y+1,z-1).getDensity();
					uDensity += volData.getVoxelAt(x-0,y+1,z-0).getDensity();
					uDensity += volData.getVoxelAt(x-0,y+1,z+1).getDensity();

					uDensity += volData.getVoxelAt(x+1,y-1,z-1).getDensity();
					uDensity += volData.getVoxelAt(x+1,y-1,z-0).getDensity();
					uDensity += volData.getVoxelAt(x+1,y-1,z+1).getDensity();
					uDensity += volData.getVoxelAt(x+1,y-0,z-1).getDensity();
					uDensity += volData.getVoxelAt(x+1,y-0,z-0).getDensity();
					uDensity += volData.getVoxelAt(x+1,y-0,z+1).getDensity();
					uDensity += volData.getVoxelAt(x+1,y+1,z-1).getDensity();
					uDensity += volData.getVoxelAt(x+1,y+1,z-0).getDensity();
					uDensity += volData.getVoxelAt(x+1,y+1,z+1).getDensity();
					uDensity /= 27;
				}
			}
		}

		for (int32_t z = croppedRegion.getLowerCorner().getZ(); z < croppedRegion.getUpperCorner().getZ(); z++)
		{
			for (int32_t y = croppedRegion.getLowerCorner().getY(); y < croppedRegion.getUpperCorner().getY(); y++)
			{
				for (int32_t x = croppedRegion.getLowerCorner().getX(); x < croppedRegion.getUpperCorner().getX(); x++)
				{
					uint16_t& uDensity = temp[x-croppedRegion.getLowerCorner().getX()][y-croppedRegion.getLowerCorner().getY()][z-croppedRegion.getLowerCorner().getZ()];

					VoxelType val = volData.getVoxelAt(x,y,z);
					val.setDensity(uDensity);
					volData.setVoxelAt(x,y,z,val);
				}
			}
		}
	}
}