namespace PolyVox
{
	template <typename VoxelType>
	void smoothRegion(Volume<VoxelType>& volData, const Region& regionToSmooth)
	{
		Region croppedRegion = regionToSmooth;
		croppedRegion.cropTo(volData.getEnclosingRegion());

		Array<3, uint16_t> temp(ArraySizes(croppedRegion.width())(croppedRegion.height())(croppedRegion.depth()));

		for (int z = croppedRegion.getLowerCorner().getZ(); z < croppedRegion.getUpperCorner().getZ(); z++)
		{
			for (int y = croppedRegion.getLowerCorner().getY(); y < croppedRegion.getUpperCorner().getY(); y++)
			{
				for (int x = croppedRegion.getLowerCorner().getX(); x < croppedRegion.getUpperCorner().getX(); x++)
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

		for (int z = croppedRegion.getLowerCorner().getZ(); z < croppedRegion.getUpperCorner().getZ(); z++)
		{
			for (int y = croppedRegion.getLowerCorner().getY(); y < croppedRegion.getUpperCorner().getY(); y++)
			{
				for (int x = croppedRegion.getLowerCorner().getX(); x < croppedRegion.getUpperCorner().getX(); x++)
				{
					uint16_t& uDensity = temp[x-croppedRegion.getLowerCorner().getX()][y-croppedRegion.getLowerCorner().getY()][z-croppedRegion.getLowerCorner().getZ()];

					MaterialDensityPair44 val = volData.getVoxelAt(x,y,z);
					val.setDensity(uDensity);
					volData.setVoxelAt(x,y,z,val);
				}
			}
		}
	}
}