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

#include "PolyVoxExample.h"
#include "Perlin.h"

#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/PagedVolume.h"

#include <QApplication>

// Use the PolyVox namespace
using namespace PolyVox;

/**
 * Generates data using Perlin noise.
 */
class PerlinNoisePager : public PolyVox::PagedVolume<MaterialDensityPair44>::Pager
{
public:
	/// Constructor
	PerlinNoisePager()
		:PagedVolume<MaterialDensityPair44>::Pager()
	{
	}

	/// Destructor
	virtual ~PerlinNoisePager() {};

	virtual void pageIn(const PolyVox::Region& region, PagedVolume<MaterialDensityPair44>::Chunk* pChunk)
	{
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
						if((x-xpos)*(x-xpos) + (z-zpos)*(z-zpos) < 200)
						{
							// tunnel
							voxel.setMaterial(0);
							voxel.setDensity(MaterialDensityPair44::getMinDensity());
						} else
						{
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

					// Voxel position within a chunk always start from zero. So if a chunk represents region (4, 8, 12) to (11, 19, 15)
					// then the valid chunk voxels are from (0, 0, 0) to (7, 11, 3). Hence we subtract the lower corner position of the
					// region from the volume space position in order to get the chunk space position.
					pChunk->setVoxel(x - region.getLowerX(), y - region.getLowerY(), z - region.getLowerZ(), voxel);
				}
			}
		}
	}

	virtual void pageOut(const PolyVox::Region& region, PagedVolume<MaterialDensityPair44>::Chunk* /*pChunk*/)
	{
		std::cout << "warning unloading region: " << region.getLowerCorner() << " -> " << region.getUpperCorner() << std::endl;
	}
};

class PagingExample : public PolyVoxExample
{
public:
	PagingExample(QWidget *parent)
		:PolyVoxExample(parent)
	{
	}

protected:
	void initializeExample() override
	{
		PerlinNoisePager* pager = new PerlinNoisePager();
		PagedVolume<MaterialDensityPair44> volData(pager, 8 * 1024 * 1024, 64);

		// Just some tests of memory usage, etc. 
		std::cout << "Memory usage: " << (volData.calculateSizeInBytes() / 1024.0 / 1024.0) << "MB" << std::endl;
		PolyVox::Region reg(Vector3DInt32(-255, 0, 0), Vector3DInt32(255, 255, 255));
		std::cout << "Prefetching region: " << reg.getLowerCorner() << " -> " << reg.getUpperCorner() << std::endl;
		volData.prefetch(reg);
		std::cout << "Memory usage: " << (volData.calculateSizeInBytes() / 1024.0 / 1024.0) << "MB" << std::endl;
		std::cout << "Flushing entire volume" << std::endl;
		volData.flushAll();
		std::cout << "Memory usage: " << (volData.calculateSizeInBytes() / 1024.0 / 1024.0) << "MB" << std::endl;

		// Extract the surface
		PolyVox::Region reg2(Vector3DInt32(0, 0, 0), Vector3DInt32(254, 254, 254));
		auto mesh = extractCubicMesh(&volData, reg2);
		std::cout << "#vertices: " << mesh.getNoOfVertices() << std::endl;

		auto decodedMesh = decodeMesh(mesh);

		// Pass the surface to the OpenGL window
		addMesh(decodedMesh);

		setCameraTransform(QVector3D(300.0f, 300.0f, 300.0f), -(PI / 4.0f), PI + (PI / 4.0f));
	}
};

int main(int argc, char *argv[])
{
	// Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	PagingExample openGLWidget(0);
	openGLWidget.show();

	// Run the message pump.
	return app.exec();
} 
