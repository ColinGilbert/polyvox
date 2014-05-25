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

#include "PolyVoxCore/FilePager.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/RLEBlockCompressor.h"
#include "PolyVoxCore/Mesh.h"
#include "PolyVoxCore/Impl/Utility.h"

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

const int32_t g_uVolumeSideLength = 128;

int main(int argc, char *argv[])
{
	RLEBlockCompressor<MaterialDensityPair88>* compressor = new RLEBlockCompressor<MaterialDensityPair88>();
	FilePager<MaterialDensityPair88>* pager = new FilePager<MaterialDensityPair88>("./");
	LargeVolume<MaterialDensityPair88> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(g_uVolumeSideLength - 1, g_uVolumeSideLength - 1, g_uVolumeSideLength - 1)), compressor, pager);

	//Make our volume contain a sphere in the center.
	int32_t minPos = 0;
	int32_t midPos = g_uVolumeSideLength / 2;
	int32_t maxPos = g_uVolumeSideLength - 1;

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
	createCubeInVolume(volData, Vector3DInt32(minPos, minPos, minPos), Vector3DInt32(midPos-1, midPos-1, midPos-1), 0);
	createCubeInVolume(volData, Vector3DInt32(midPos+1, midPos+1, minPos), Vector3DInt32(maxPos, maxPos, midPos-1), 0);
	createCubeInVolume(volData, Vector3DInt32(midPos+1, minPos, midPos+1), Vector3DInt32(maxPos, midPos-1, maxPos), 0);
	createCubeInVolume(volData, Vector3DInt32(minPos, midPos+1, midPos+1), Vector3DInt32(midPos-1, maxPos, maxPos), 0);

	createCubeInVolume(volData, Vector3DInt32(1, midPos-10, midPos-10), Vector3DInt32(maxPos-1, midPos+10, midPos+10), MaterialDensityPair44::getMaxDensity());
	createCubeInVolume(volData, Vector3DInt32(midPos-10, 1, midPos-10), Vector3DInt32(midPos+10, maxPos-1, midPos+10), MaterialDensityPair44::getMaxDensity());
	createCubeInVolume(volData, Vector3DInt32(midPos-10, midPos-10 ,1), Vector3DInt32(midPos+10, midPos+10, maxPos-1), MaterialDensityPair44::getMaxDensity());

	QApplication app(argc, argv);

	OpenGLWidget openGLWidget(0);


	openGLWidget.show();

	QGLShaderProgram* shader = new QGLShaderProgram;

	if (!shader->addShaderFromSourceCode(QGLShader::Vertex, R"(
		#version 140
		
		in vec4 position; // This will be the position of the vertex in model-space
		in vec4 normal; // The normal data may not have been set
		in ivec2 material;
		
		uniform mat4 cameraToClipMatrix;
		uniform mat4 worldToCameraMatrix;
		uniform mat4 modelToWorldMatrix;
		
		out vec4 worldPosition; //This is being passed to the fragment shader to calculate the normals
		out vec4 worldNormal;
		flat out ivec2 outMaterial;
		
		void main()
		{
			worldPosition = modelToWorldMatrix * position;
			worldNormal = normal;
			outMaterial = ivec2(material.x, material.y);
			vec4 cameraPosition = worldToCameraMatrix * worldPosition;
			gl_Position = cameraToClipMatrix * cameraPosition;
		}
	)"))
	{
		std::cerr << shader->log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!shader->addShaderFromSourceCode(QGLShader::Fragment, R"(
		#version 130
		
		in vec4 worldPosition; //Passed in from the vertex shader
		in vec4 worldNormal;
		flat in ivec2 outMaterial;
		
		out vec4 outputColor;
		
		void main()
		{
			//vec3 normal = normalize(cross(dFdy(worldPosition.xyz), dFdx(worldPosition.xyz)));

			vec3 normal = worldNormal.xyz;

			switch(outMaterial.x)
			{
			case 1:
				outputColor = vec4(1.0, 0.0, 0.0, 1.0);
				break;
			case 2:
				outputColor = vec4(0.0, 1.0, 0.0, 1.0);
				break;
			case 3:
				outputColor = vec4(0.0, 0.0, 1.0, 1.0);
				break;
			case 4:
				outputColor = vec4(1.0, 1.0, 0.0, 1.0);
				break;
			case 5:
				outputColor = vec4(1.0, 0.0, 1.0, 1.0);
				break;
			default:
				outputColor = vec4(1.0, 1.0, 1.0, 1.0);
				break;
			}
			
			//float color = clamp(abs(dot(normalize(normal.xyz), vec3(0.9,0.1,0.5))), 0, 1);
			//outputColor = vec4(1.0, 0.5, color, 1.0);
		}
	)"))
	{
		std::cerr << shader->log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}

	openGLWidget.setShader(shader);

	QTime time;
	time.start();
	//openGLWidget.setVolume(&volData);
	cout << endl << "Time taken = " << time.elapsed() / 1000.0f << "s" << endl << endl;

	const int32_t extractedRegionSize = 32;
	int meshCounter = 0;

	for (int32_t z = 0; z < volData.getDepth(); z += extractedRegionSize)
	{
		for (int32_t y = 0; y < volData.getHeight(); y += extractedRegionSize)
		{
			for (int32_t x = 0; x < volData.getWidth(); x += extractedRegionSize)
			{
				// Specify the region to extract based on a starting position and the desired region sze.
				Region regToExtract(x, y, z, x + extractedRegionSize, y + extractedRegionSize, z + extractedRegionSize);

				// If you uncomment this line you will be able to see that the volume is rendered as multiple seperate meshes.
				//regToExtract.shrink(1);

				// Perform the extraction for this region of the volume
				auto mesh = extractMarchingCubesMesh(&volData, regToExtract);

				//Pass the surface to the OpenGL window
				openGLWidget.addMesh(mesh, Vector3DInt32(x, y, z));

				meshCounter++;
			}
		}
	}

	cout << "Rendering volume as " << meshCounter << " seperate meshes" << endl;


	openGLWidget.setViewableRegion(volData.getEnclosingRegion());


	return app.exec();
} 
