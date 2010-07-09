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

#include "Filters.h"
#include "Log.h"
#include "MaterialDensityPair.h"
#include "Volume.h"
#include "SurfaceExtractor.h"
#include "SurfaceMesh.h"
#include "PolyVoxImpl/Utility.h"



#include "OpenGLWidget.h"

#ifdef WIN32
#include <windows.h>   // Standard Header For Most Programs
#endif

#include <QApplication>

#include <QTime>

//Some namespaces we need
using namespace std;
using namespace PolyVox;
using namespace std;





void createSphereInVolume(Volume<MaterialDensityPair44>& volData, float fRadius, uint8_t uValue)
{
	//This vector hold the position of the center of the volume
	Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getWidth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getDepth(); x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				//If the current voxel is less than 'radius' units from the center
				//then we make it solid, otherwise we make it empty space.
				if(fDistToCenter <= fRadius)
				{
					volData.setVoxelAt(x,y,z, MaterialDensityPair44(uValue, uValue > 0 ? MaterialDensityPair44::getMaxDensity() : MaterialDensityPair44::getMinDensity()));
				}
			}
		}
	}
}

void createCubeInVolume(Volume<MaterialDensityPair44>& volData, Vector3DUint16 lowerCorner, Vector3DUint16 upperCorner, uint8_t uValue)
{
	//This three-level for loop iterates over every voxel between the specified corners
	for (int z = lowerCorner.getZ(); z <= upperCorner.getZ(); z++)
	{
		for (int y = lowerCorner.getY(); y <= upperCorner.getY(); y++)
		{
			for (int x = lowerCorner.getX() ; x <= upperCorner.getX(); x++)
			{
				volData.setVoxelAt(x,y,z, MaterialDensityPair44(uValue, uValue > 0 ? MaterialDensityPair44::getMaxDensity() : MaterialDensityPair44::getMinDensity()));
			}
		}
	}
}

const uint16_t g_uVolumeSideLength = 64;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	OpenGLWidget openGLWidget(0);
	openGLWidget.show();

	Volume<MaterialDensityPair44> volData(g_uVolumeSideLength, g_uVolumeSideLength, g_uVolumeSideLength);

	createSphereInVolume(volData, 30, 1);

	SurfaceExtractor<MaterialDensityPair44> surfaceExtractor(volData);

	shared_ptr<SurfaceMesh> surface = surfaceExtractor.extractSurfaceForRegion(volData.getEnclosingRegion());

	//OpenGLSurfaceMesh mesh = BuildOpenGLSurfaceMesh(*surface);

	
	//openGLWidget.mesh = BuildOpenGLSurfaceMesh(*(surface.get()));
	//openGLWidget.surfaceMesh = *surface;
	openGLWidget.setSurfaceMeshToRender(*surface);
	return app.exec();
} 
