#include "Shapes.h"

using namespace PolyVox;

void createSphereInVolume(Volume<uint8_t>& volData, float fRadius, uint8_t uValue)
{
	//This vector hold the position of the center of the volume
	Vector3DFloat v3dVolCenter(volData.getSideLength() / 2, volData.getSideLength() / 2, volData.getSideLength() / 2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getSideLength(); z++)
	{
		for (int y = 0; y < volData.getSideLength(); y++)
		{
			for (int x = 0; x < volData.getSideLength(); x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				//If the current voxel is less than 'radius' units from the center
				//then we make it solid, otherwise we make it empty space.
				if(fDistToCenter <= fRadius)
				{
					volData.setVoxelAt(x,y,z, uValue);
				}
			}
		}
	}
}

void createCubeInVolume(Volume<uint8_t>& volData, Vector3DUint16 lowerCorner, Vector3DUint16 upperCorner, uint8_t uValue)
{
	//This three-level for loop iterates over every voxel between the specified corners
	for (int z = lowerCorner.getZ(); z <= upperCorner.getZ(); z++)
	{
		for (int y = lowerCorner.getY(); y <= upperCorner.getY(); y++)
		{
			for (int x = lowerCorner.getX() ; x <= upperCorner.getX(); x++)
			{
				volData.setVoxelAt(x,y,z, uValue);
			}
		}
	}
}