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

#include "MaterialDensityPair.h"
#include "CubicSurfaceExtractor.h"
#include "CubicSurfaceExtractorWithNormals.h"
#include "SurfaceMesh.h"
#include "Volume.h"

#include "MeshDecimator.h"

#include <QApplication>

//Use the PolyVox namespace
using namespace PolyVox;

void createSphereInVolume(Volume<MaterialDensityPair44>& volData, float fRadius)
{
	//This vector hold the position of the center of the volume
	//Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	Vector3DFloat v3dVolCenter(16, 16, 16);

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

				//If the current voxel is less than 'radius' units from the center then we make it solid.
				if(fDistToCenter <= fRadius)
				{
					//Our new density value
					uint8_t uDensity = MaterialDensityPair44::getMaxDensity();
					uint8_t uMaterial = 3;

					//Get the old voxel
					MaterialDensityPair44 voxel = volData.getVoxelAt(x,y,z);

					//Modify the density
					voxel.setDensity(uDensity);
					voxel.setMaterial(uMaterial);

					//Write the voxel value into the volume	
					volData.setVoxelAt(x, y, z, voxel);
				}
			}
		}
	}
}

void addNormals(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& inputMesh,  PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& outputMesh)
{
	outputMesh.m_Region = inputMesh.m_Region;

	outputMesh.m_vecTriangleIndices.clear();
	for(int ct = 0; ct < inputMesh.m_vecTriangleIndices.size(); ++ct)
	{
		outputMesh.m_vecTriangleIndices.push_back(inputMesh.m_vecTriangleIndices[ct]);
	}

	outputMesh.m_vecVertices.clear();
	for(int ct = 0; ct < inputMesh.m_vecVertices.size(); ++ct)
	{
		PositionMaterialNormal vertex;
		vertex.position = inputMesh.m_vecVertices[ct].position;
		vertex.material = inputMesh.m_vecVertices[ct].material;
		outputMesh.m_vecVertices.push_back(vertex);
	}

	outputMesh.generateAveragedFaceNormals(true, true);
}

int main(int argc, char *argv[])
{
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	OpenGLWidget openGLWidget(0);
	openGLWidget.show();

	//Create an empty volume and then place a sphere in it
	Volume<MaterialDensityPair44> volData(32, 32, 32, 32);
	//createSphereInVolume(volData, 30);

	//This three-level for loop iterates over every voxel in the volume
	/*for (int z = 8; z < 24; z++)
	{
		for (int y = 8; y < 24; y++)
		{
			for (int x = 8; x < 16; x++)
			{				
				//Our new density value
				uint8_t uDensity = MaterialDensityPair44::getMaxDensity();

				//Get the old voxel
				MaterialDensityPair44 voxel = volData.getVoxelAt(x,y,z);

				//Modify the density
				voxel.setDensity(uDensity);
				voxel.setMaterial(3);

				//Write the voxel value into the volume	
				volData.setVoxelAt(x, y, z, voxel);
			}
		}
	}

	for (int z = 8; z < 24; z++)
	{
		for (int y = 8; y < 24; y++)
		{
			for (int x = 16; x < 24; x++)
			{				
				//Our new density value
				uint8_t uDensity = MaterialDensityPair44::getMaxDensity();

				//Get the old voxel
				MaterialDensityPair44 voxel = volData.getVoxelAt(x,y,z);

				//Modify the density
				voxel.setDensity(uDensity);
				voxel.setMaterial(5);

				//Write the voxel value into the volume	
				volData.setVoxelAt(x, y, z, voxel);
			}
		}
	}*/

	createSphereInVolume(volData, 10);

	//Extract the surface
	Region region(Vector3DInt16(0,0,0), Vector3DInt16(20,20,20));
	SurfaceMesh<PositionMaterial> mesh;
	//CubicSurfaceExtractor<MaterialDensityPair44> surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
	CubicSurfaceExtractor<MaterialDensityPair44> surfaceExtractor(&volData, region, &mesh);
	surfaceExtractor.execute();

	/*SurfaceMesh<PositionMaterialNormal> meshWithNormals;
	addNormals(mesh, meshWithNormals);

	meshWithNormals.decimate(0.99);*/

	MeshDecimator<PositionMaterial> decimator(&mesh);
	decimator.execute();

	SurfaceMesh<PositionMaterialNormal> meshWithNormals;
	addNormals(mesh, meshWithNormals);

	//Pass the surface to the OpenGL window
	openGLWidget.setSurfaceMeshToRender(meshWithNormals);
	//openGLWidget.setSurfaceMeshToRender(mesh);

	//Run the message pump.
	return app.exec();
} 
