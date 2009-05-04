#include "Log.h"
#include "Volume.h"
#include "IndexedSurfacePatch.h"
#include "SurfaceExtractors.h"
#include "PolyVoxImpl/Utility.h"

#include "OpenGLImmediateModeSupport.h"
#include "OpenGLVertexBufferObjectSupport.h"
#include "Shapes.h"

#include "OpenGLWidget.h"

#ifdef WIN32
#include <windows.h>   // Standard Header For Most Programs
#endif

#include <QApplication>

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
	Volume<PolyVox::uint8_t> volData(g_uVolumeSideLength, g_uVolumeSideLength, g_uVolumeSideLength);

	//Make our volume contain a sphere in the center.
	PolyVox::uint16_t minPos = 0;
	PolyVox::uint16_t midPos = g_uVolumeSideLength / 2;
	PolyVox::uint16_t maxPos = g_uVolumeSideLength - 1;
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

	cout << "Creating cubes" << std::endl;
	createCubeInVolume(volData, Vector3DUint16(minPos, minPos, minPos), Vector3DUint16(midPos-1, midPos-1, midPos-1), 0);
	createCubeInVolume(volData, Vector3DUint16(midPos+1, midPos+1, minPos), Vector3DUint16(maxPos, maxPos, midPos-1), 0);
	createCubeInVolume(volData, Vector3DUint16(midPos+1, minPos, midPos+1), Vector3DUint16(maxPos, midPos-1, maxPos), 0);
	createCubeInVolume(volData, Vector3DUint16(minPos, midPos+1, midPos+1), Vector3DUint16(midPos-1, maxPos, maxPos), 0);

	createCubeInVolume(volData, Vector3DUint16(1, midPos-10, midPos-10), Vector3DUint16(maxPos-1, midPos+10, midPos+10), 255);
	createCubeInVolume(volData, Vector3DUint16(midPos-10, 1, midPos-10), Vector3DUint16(midPos+10, maxPos-1, midPos+10), 255);
	createCubeInVolume(volData, Vector3DUint16(midPos-10, midPos-10 ,1), Vector3DUint16(midPos+10, midPos+10, maxPos-1), 255);

	cout << "Tidying memory...";
	volData.tidyUpMemory(0);
	cout << "done." << endl; 

	QApplication app(argc, argv);

	OpenGLWidget openGLWidget(0);


	openGLWidget.show();

	openGLWidget.setVolume(&volData);

	return app.exec();
} 
