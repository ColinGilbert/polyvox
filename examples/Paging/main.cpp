/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#include "OpenGLWidget.h"
#include "Perlin.h"

#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/Pager.h"
#include "PolyVoxCore/Mesh.h"
#include "PolyVoxCore/LargeVolume.h"

#include <QApplication>

//Use the PolyVox namespace
using namespace PolyVox;

void createSphereInVolume(LargeVolume<MaterialDensityPair44>& volData, Vector3DFloat v3dVolCenter, float fRadius)
{
	//This vector hold the position of the center of the volume
	//Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	int iRadius = fRadius;

	//This three-level for loop iterates over every voxel in the volume
	for (int z = v3dVolCenter.getZ() - iRadius; z <= v3dVolCenter.getZ() + iRadius; z++)
	{
		for (int y = v3dVolCenter.getY() - iRadius; y <= v3dVolCenter.getY() + iRadius; y++)
		{
			for (int x = v3dVolCenter.getX() - iRadius; x <= v3dVolCenter.getX() + iRadius; x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				//If the current voxel is less than 'radius' units from the center then we make it solid.
				if(fDistToCenter <= fRadius)
				{
					//Our new density value
					uint8_t uDensity = MaterialDensityPair44::getMaxDensity();

					//Get the old voxel
					MaterialDensityPair44 voxel = volData.getVoxel(x,y,z);

					//Modify the density
					voxel.setDensity(uDensity);

					//Wrte the voxel value into the volume	
					volData.setVoxelAt(x, y, z, voxel);
				}
			}
		}
	}
}

/**
 * Generates data using Perlin noise.
 */
class PerlinNoisePager : public PolyVox::Pager<MaterialDensityPair44>
{
public:
	/// Constructor
	PerlinNoisePager()
		:Pager<MaterialDensityPair44>()
	{
	}

	/// Destructor
	virtual ~PerlinNoisePager() {};

	virtual void pageIn(const PolyVox::Region& region, std::shared_ptr< UncompressedBlock<MaterialDensityPair44> > pBlockData)
	{
		// FIXME - this isn't a great example... it's a shame we have to hard clode the block size and also create/destroy
		// a compressor each time. These could at least be moved outside somewhere if we can't fix it in a better way...
		//UncompressedBlock<MaterialDensityPair44> block(256);

		Perlin perlin(2,2,1,234);

		for(int x = region.getLowerX(); x <= region.getUpperX(); x++)
		{
			for(int y = region.getLowerY(); y <= region.getUpperY(); y++)
			{
				float perlinVal = perlin.Get(x / static_cast<float>(255-1), y / static_cast<float>(255-1));
				perlinVal += 1.0f;
				perlinVal *= 0.5f;
				perlinVal *= 255;
				for(int z = region.getLowerZ(); z <= region.getUpperZ(); z++)
				{
					MaterialDensityPair44 voxel;
					if(z < perlinVal)
					{
						const int xpos = 50;
						const int zpos = 100;
						if((x-xpos)*(x-xpos) + (z-zpos)*(z-zpos) < 200) {
							// tunnel
							voxel.setMaterial(0);
							voxel.setDensity(MaterialDensityPair44::getMinDensity());
						} else {
							// solid
							voxel.setMaterial(245);
							voxel.setDensity(MaterialDensityPair44::getMaxDensity());
						}
					}
					else
					{
						voxel.setMaterial(0);
						voxel.setDensity(MaterialDensityPair44::getMinDensity());
					}

					// Voxel position within a block always start from zero. So if a block represents region (4, 8, 12) to (11, 19, 15)
					// then the valid block voxels are from (0, 0, 0) to (7, 11, 3). Hence we subtract the lower corner position of the
					// region from the volume space position in order to get the block space position.
					pBlockData->setVoxelAt(x - region.getLowerX(), y - region.getLowerY(), z - region.getLowerZ(), voxel);
				}
			}
		}

		// Now compress the computed data into the provided block.
		//RLEBlockCompressor<MaterialDensityPair44>* compressor = new RLEBlockCompressor<MaterialDensityPair44>();
		//compressor->compress(&block, pBlockData);
		//delete compressor;
	}

	virtual void pageOut(const PolyVox::Region& region, std::shared_ptr< UncompressedBlock<MaterialDensityPair44> > /*pBlockData*/)
	{
		std::cout << "warning unloading region: " << region.getLowerCorner() << " -> " << region.getUpperCorner() << std::endl;
	}
};

int main(int argc, char *argv[])
{
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	OpenGLWidget openGLWidget(0);
	openGLWidget.show();

	PerlinNoisePager* pager = new PerlinNoisePager();
	LargeVolume<MaterialDensityPair44> volData(PolyVox::Region::MaxRegion, pager, 256);
	//volData.setMaxNumberOfBlocksInMemory(4096);
	volData.setMaxNumberOfUncompressedBlocks(64);

	//volData.setMaxNumberOfUncompressedBlocks(4096);
	//createSphereInVolume(volData, 30);
	//createPerlinTerrain(volData);
	//createPerlinVolumeSlow(volData);
	std::cout << "Memory usage: " << (volData.calculateSizeInBytes()/1024.0/1024.0) << "MB" << std::endl;
	//std::cout << "Compression ratio: 1 to " << (1.0/(volData.calculateCompressionRatio())) << std::endl;
	//volData.setBlockCacheSize(64);
	PolyVox::Region reg(Vector3DInt32(-255,0,0), Vector3DInt32(255,255,255));
	std::cout << "Prefetching region: " << reg.getLowerCorner() << " -> " << reg.getUpperCorner() << std::endl;
	volData.prefetch(reg);
	std::cout << "Memory usage: " << (volData.calculateSizeInBytes()/1024.0/1024.0) << "MB" << std::endl;
	//std::cout << "Compression ratio: 1 to " << (1.0/(volData.calculateCompressionRatio())) << std::endl;
	PolyVox::Region reg2(Vector3DInt32(0,0,0), Vector3DInt32(255,255,255));
	std::cout << "Flushing region: " << reg2.getLowerCorner() << " -> " << reg2.getUpperCorner() << std::endl;
	volData.flush(reg2);
	std::cout << "Memory usage: " << (volData.calculateSizeInBytes()/1024.0/1024.0) << "MB" << std::endl;
	//std::cout << "Compression ratio: 1 to " << (1.0/(volData.calculateCompressionRatio())) << std::endl;
	std::cout << "Flushing entire volume" << std::endl;
	volData.flushAll();
	std::cout << "Memory usage: " << (volData.calculateSizeInBytes()/1024.0/1024.0) << "MB" << std::endl;
	//std::cout << "Compression ratio: 1 to " << (1.0/(volData.calculateCompressionRatio())) << std::endl;

	//Extract the surface
	auto mesh = extractCubicMesh(&volData, reg2);
	std::cout << "#vertices: " << mesh.getNoOfVertices() << std::endl;

	auto decodedMesh = decodeMesh(mesh);

	//Pass the surface to the OpenGL window
	openGLWidget.addMesh(decodedMesh);

	openGLWidget.setViewableRegion(reg2);

	//Run the message pump.
	return app.exec();
} 
