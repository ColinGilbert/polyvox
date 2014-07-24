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

#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/Mesh.h"
#include "PolyVoxCore/SimpleVolume.h"

#include <QApplication>

//Use the PolyVox namespace
using namespace PolyVox;

void createSphereInVolume(SimpleVolume<uint8_t>& volData, float fRadius)
{
	//This vector hold the position of the center of the volume
	Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				// We actually want our volume to have high values in the center and low values as we move out, because our
				// eath should be a solid sphere surrounded by empty space. If we invert the distance then this is a step in
				// the right direction. We still have zero in the center, but lower (negative) values as we move out.
				float density = -fDistToCenter;

				// By adding the 'planetRadius' we now have a function which starts at 'planetRadius' and still decreases as it
				// moves out. The function passes through zero at a distance of 'planetRadius' and then continues do decrease
				// as it gets even further out.
				density += fRadius;

				// Ideally we would like our final density value to be '255' for voxels inside the planet and '0' for voxels
				// outside the planet. At the surface there should be a transition but this should occur not too quickly and
				// not too slowly, as both of these will result in a jagged appearance to the mesh.
				//
				// We probably want the transition to occur over a few voxels, whereas it currently occurs over 255 voxels
				// because it was derived from the distance. By scaling the density field we effectivly compress the rate
				// at which it changes at the surface. We also make the center much too high and the outside very low, but
				// we will clamp these to the corect range later.
				//
				// Note: You can try commenting out or changing the value on this line to see the effect it has.
				density *= 50;

				// Until now we've been defining our density field as if the threshold was at zero, with positive densities
				// being solid and negative densities being empty. But actually Cubiquity operates on the range 0 to 255, and
				// uses a threashold of 127 to decide where to place the generated surface.  Therefore we shift and clamp our
				// density value and store it in a byte.
				density += 127;
				uint8_t uVoxelValue = (uint8_t)(clamp(density, 0.0f, 255.0f));

				//Wrte the voxel value into the volume	
				volData.setVoxelAt(x, y, z, uVoxelValue);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	OpenGLWidget openGLWidget(0);
	openGLWidget.show();

	//Create an empty volume and then place a sphere in it
	SimpleVolume<uint8_t> volData(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(31, 31, 31)));
	createSphereInVolume(volData, 15);

	// Extract the surface for the specified region of the volume. Uncomment the line for the kind of surface extraction you want to see.
	//auto mesh = extractCubicMesh(&volData, volData.getEnclosingRegion());
	auto mesh = extractMarchingCubesMesh(&volData, volData.getEnclosingRegion());

	// The surface extractor outputs the mesh in an efficient compressed format which is not directly suitable for rendering. The easiest approach is to 
	// decode this on the CPU as shown below, though more advanced applications can upload the compressed mesh to the GPU and decompress in shader code.
	auto decodedMesh = decodeMesh(mesh);

	//Pass the surface to the OpenGL window
	openGLWidget.addMesh(decodedMesh);
	//openGLWidget.addMesh(mesh2);
	openGLWidget.setViewableRegion(volData.getEnclosingRegion());

	//Run the message pump.
	return app.exec();
}