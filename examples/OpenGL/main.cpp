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

#include "Log.h"
#include "MaterialDensityPair.h"
#include "Volume.h"
#include "SurfaceMesh.h"
#include "PolyVoxImpl/Utility.h"

#include "OpenGLImmediateModeSupport.h"
#include "OpenGLVertexBufferObjectSupport.h"
#include "Shapes.h"

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

void exampleLog(string message, int severity)
{
	//Identify how severe the mesage is
	switch(severity)
	{
	case LS_DEBUG:
		cout << "DEBUG: ";
		break;
	case LS_INFO:
		cout << "INFO: ";
		break;
	case LS_WARN:
		cout << "WARN: ";
		break;
	case LS_ERROR:
		cout << "ERROR: ";
		break;
	}

	//Print the message
	cout << message << endl;
}

int main(int argc, char *argv[])
{
	logHandler = &exampleLog;
	Volume<MaterialDensityPair44> volData(g_uVolumeSideLength, g_uVolumeSideLength, g_uVolumeSideLength);
	Volume<MaterialDensityPair44> volDataAveraged(g_uVolumeSideLength, g_uVolumeSideLength, g_uVolumeSideLength);

	//Make our volume contain a sphere in the center.
	uint16_t minPos = 0;
	uint16_t midPos = g_uVolumeSideLength / 2;
	uint16_t maxPos = g_uVolumeSideLength - 1;
	//createCubeInVolume(volData, Vector3DUint16(minPos, minPos, minPos), Vector3DUint16(maxPos, maxPos, maxPos), 0);

	cout << "Creating sphere 1" << std::endl;
	createSphereInVolume(volData, 60.0f, 5);
	cout << "Creating sphere 2" << std::endl;
	createSphereInVolume(volData, 50.0f, 4);
	cout << "Creating sphere 3" << std::endl;
	createSphereInVolume(volData, 40.0f, 3);
	cout << "Creating sphere 4" << std::endl;
	createSphereInVolume(volData, 30.0f, 2);
	cout << "Creating sphere 5" << std::endl;
	createSphereInVolume(volData, 20.0f, 1);

	/*cout << "Creating sphere 1" << std::endl;
	createSphereInVolume(volData, 120.0f, 5);
	cout << "Creating sphere 2" << std::endl;
	createSphereInVolume(volData, 100.0f, 4);
	cout << "Creating sphere 3" << std::endl;
	createSphereInVolume(volData, 80.0f, 3);
	cout << "Creating sphere 4" << std::endl;
	createSphereInVolume(volData, 60.0f, 2);
	cout << "Creating sphere 5" << std::endl;
	createSphereInVolume(volData, 40.0f, 1);*/

	cout << "Creating cubes" << std::endl;
	createCubeInVolume(volData, Vector3DUint16(minPos, minPos, minPos), Vector3DUint16(midPos-1, midPos-1, midPos-1), 0);
	createCubeInVolume(volData, Vector3DUint16(midPos+1, midPos+1, minPos), Vector3DUint16(maxPos, maxPos, midPos-1), 0);
	createCubeInVolume(volData, Vector3DUint16(midPos+1, minPos, midPos+1), Vector3DUint16(maxPos, midPos-1, maxPos), 0);
	createCubeInVolume(volData, Vector3DUint16(minPos, midPos+1, midPos+1), Vector3DUint16(midPos-1, maxPos, maxPos), 0);

	createCubeInVolume(volData, Vector3DUint16(1, midPos-10, midPos-10), Vector3DUint16(maxPos-1, midPos+10, midPos+10), MaterialDensityPair44::getMaxDensity());
	createCubeInVolume(volData, Vector3DUint16(midPos-10, 1, midPos-10), Vector3DUint16(midPos+10, maxPos-1, midPos+10), MaterialDensityPair44::getMaxDensity());
	createCubeInVolume(volData, Vector3DUint16(midPos-10, midPos-10 ,1), Vector3DUint16(midPos+10, midPos+10, maxPos-1), MaterialDensityPair44::getMaxDensity());

	//createCubeInVolume(volData, Vector3DUint16(1, 1, 1), Vector3DUint16(maxPos-1, maxPos-1, midPos/4), 255);

	for (int z = 1; z < volData.getWidth()-1; z++)
	{
		for (int y = 1; y < volData.getHeight()-1; y++)
		{
			for (int x = 1; x < volData.getDepth()-1; x++)
			{
				int uDensity = 0;
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

				MaterialDensityPair44 val = volData.getVoxelAt(x,y,z);
				val.setDensity(uDensity);

				volDataAveraged.setVoxelAt(x,y,z,val);
			}
		}
	}

	/*std::vector<long int> counts(256);
	fill(counts.begin(), counts.end(), 0);

	for (int z = 0; z < volData.getWidth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getDepth(); x++)
			{
				counts[volData.getVoxelAt(x,y,z).getMaterial()]++;
			}
		}
	}*/

	cout << "Tidying memory...";
	volData.tidyUpMemory(0);
	volDataAveraged.tidyUpMemory(0);
	cout << "done." << endl; 

	QApplication app(argc, argv);

	OpenGLWidget openGLWidget(0);


	openGLWidget.show();

	QTime time;
	time.start();
	openGLWidget.setVolume(&volDataAveraged);
	cout << endl << "Time taken = " << time.elapsed() / 1000.0f << "s" << endl << endl;

	//return 0;

	return app.exec();
} 
