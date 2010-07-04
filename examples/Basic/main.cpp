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

#include "glew/glew.h"

#ifdef WIN32
#include <windows.h>   // Standard Header For Most Programs
#endif

#include <QApplication>
#include <QGLWidget>
#include <QTime>

//Some namespaces we need
using namespace std;
using namespace PolyVox;
using namespace std;

struct OpenGLColour
{
	GLfloat red;
	GLfloat green;
	GLfloat blue;
};

struct OpenGLSurfaceMesh
{
	GLulong noOfIndices;
	GLuint indexBuffer;
	GLuint vertexBuffer;
	const PolyVox::SurfaceMesh* sourceMesh;
};

OpenGLColour convertMaterialIDToColour(uint8_t materialID)
{
	OpenGLColour colour;

	switch(materialID)
	{
	case 1:
		colour.red = 1.0f;
		colour.green = 0.0f;
		colour.blue = 0.0f;
		break;
	case 2:
		colour.red = 0.0f;
		colour.green = 1.0f;
		colour.blue = 0.0f;
		break;
	case 3:
		colour.red = 0.0f;
		colour.green = 0.0f;
		colour.blue = 1.0f;
		break;
	case 4:
		colour.red = 1.0f;
		colour.green = 1.0f;
		colour.blue = 0.0f;
		break;
	case 5:
		colour.red = 1.0f;
		colour.green = 0.0f;
		colour.blue = 1.0f;
		break;
	default:
		colour.red = 1.0f;
		colour.green = 1.0f;
		colour.blue = 1.0f;
	}

	return colour;
}

OpenGLSurfaceMesh BuildOpenGLSurfaceMesh(const PolyVox::SurfaceMesh& mesh);
void renderRegionVertexBufferObject(const OpenGLSurfaceMesh& openGLSurfaceMesh, unsigned int uLodLevel);

OpenGLSurfaceMesh BuildOpenGLSurfaceMesh(const SurfaceMesh& mesh)
{
	//Represents our filled in OpenGL vertex and index buffer objects.
	OpenGLSurfaceMesh result;

	//The source
	result.sourceMesh = &mesh;

	//Convienient access to the vertices and indices
	const vector<SurfaceVertex>& vecVertices = mesh.getVertices();
	const vector<uint32_t>& vecIndices = mesh.getIndices();

	//If we have any indices...
	if(!vecIndices.empty())
	{
		//Create an OpenGL index buffer
		glGenBuffers(1, &result.indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.indexBuffer);

		//Get a pointer to the first index
		GLvoid* pIndices = (GLvoid*)(&(vecIndices[0]));		

		//Fill the OpenGL index buffer with our data. 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(uint32_t), pIndices, GL_STATIC_DRAW);
	}

	result.noOfIndices = vecIndices.size();

	glGenBuffers(1, &result.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, result.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(GLfloat) * 9, 0, GL_STATIC_DRAW);
	GLfloat* ptr = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	for(vector<SurfaceVertex>::const_iterator iterVertex = vecVertices.begin(); iterVertex != vecVertices.end(); ++iterVertex)
	{
		const SurfaceVertex& vertex = *iterVertex;
		const Vector3DFloat& v3dVertexPos = vertex.getPosition();
		//const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
		const Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<Vector3DFloat>(mesh.m_Region.getLowerCorner());

		*ptr = v3dFinalVertexPos.getX();
		ptr++;
		*ptr = v3dFinalVertexPos.getY();
		ptr++;
		*ptr = v3dFinalVertexPos.getZ();
		ptr++;

		*ptr = vertex.getNormal().getX();
		ptr++;
		*ptr = vertex.getNormal().getY();
		ptr++;
		*ptr = vertex.getNormal().getZ();
		ptr++;

		uint8_t material = vertex.getMaterial() + 0.5;

		OpenGLColour colour = convertMaterialIDToColour(material);

		*ptr = colour.red;
		ptr++;
		*ptr = colour.green;
		ptr++;
		*ptr = colour.blue;
		ptr++;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	return result;
}

void renderRegionVertexBufferObject(const OpenGLSurfaceMesh& openGLSurfaceMesh, unsigned int uLodLevel)
{
	int beginIndex = openGLSurfaceMesh.sourceMesh->m_vecLodRecords[uLodLevel].beginIndex;
	int endIndex = openGLSurfaceMesh.sourceMesh->m_vecLodRecords[uLodLevel].endIndex;
	glBindBuffer(GL_ARRAY_BUFFER, openGLSurfaceMesh.vertexBuffer);
	glVertexPointer(3, GL_FLOAT, 36, 0);
	glNormalPointer(GL_FLOAT, 36, (GLvoid*)12);
	glColorPointer(3, GL_FLOAT, 36, (GLvoid*)24);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLSurfaceMesh.indexBuffer);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//glDrawElements(GL_TRIANGLES, openGLSurfaceMesh.noOfIndices, GL_UNSIGNED_INT, 0);
	glDrawRangeElements(GL_TRIANGLES, beginIndex, endIndex-1, endIndex - beginIndex,/* openGLSurfaceMesh.noOfIndices,*/ GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void renderRegionImmediateMode(PolyVox::SurfaceMesh& mesh, unsigned int uLodLevel)
{
	const vector<SurfaceVertex>& vecVertices = mesh.getVertices();
	const vector<uint32_t>& vecIndices = mesh.getIndices();

	int beginIndex = mesh.m_vecLodRecords[uLodLevel].beginIndex;
	int endIndex = mesh.m_vecLodRecords[uLodLevel].endIndex;

	glBegin(GL_TRIANGLES);
	//for(vector<PolyVox::uint32_t>::const_iterator iterIndex = vecIndices.begin(); iterIndex != vecIndices.end(); ++iterIndex)
	for(int index = beginIndex; index < endIndex; ++index)
	{
		const SurfaceVertex& vertex = vecVertices[vecIndices[index]];
		const Vector3DFloat& v3dVertexPos = vertex.getPosition();
		//const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
		const Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<Vector3DFloat>(mesh.m_Region.getLowerCorner());




		uint8_t material = vertex.getMaterial() + 0.5;

		OpenGLColour colour = convertMaterialIDToColour(material);

		glColor3f(colour.red, colour.green, colour.blue);
		glNormal3f(vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ());
		glVertex3f(v3dFinalVertexPos.getX(), v3dFinalVertexPos.getY(), v3dFinalVertexPos.getZ());


	}
	glEnd();
}

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

class OpenGLWidget : public QGLWidget
 {
 public:
     OpenGLWidget(QWidget *parent)
		 :QGLWidget(parent)
	 {
	 }

 protected:
     void initializeGL()
	 {
		#ifdef WIN32
		//If we are on Windows we will need GLEW to access recent OpenGL functionality
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			cout << "Error: " << glewGetErrorString(err) << endl;
		}
		#endif

		glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
		glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glEnable ( GL_COLOR_MATERIAL );
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_LIGHTING);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_LIGHT0);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glShadeModel(GL_SMOOTH);
	 }

     void resizeGL(int w, int h)
	 {
		 //Setup the viewport based on the window size
		glViewport(0, 0, w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//float frustumSize = m_volData->getDiagonalLength() / 2.0f;
		float frustumSize = 100.0f;
		float aspect = static_cast<float>(width()) / static_cast<float>(height());

		glOrtho(frustumSize*aspect, -frustumSize*aspect, frustumSize, -frustumSize, 1.0, 5000);
	 }

     void paintGL()
	 {
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
		 glMatrixMode(GL_MODELVIEW);  // Select The Model View Matrix
		glLoadIdentity();									// Reset The Current Modelview Matrix

		glTranslatef(0.0f,0.0f,-100.0f);					// Move Left 1.5 Units And Into The Screen 6.0


		//renderRegionVertexBufferObject(mesh, 0);
		renderRegionImmediateMode(surfaceMesh, 0);
	 }

 public:
	 OpenGLSurfaceMesh mesh;
	 SurfaceMesh surfaceMesh;
 };

const uint16_t g_uVolumeSideLength = 32;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	OpenGLWidget openGLWidget(0);
	openGLWidget.show();

	Volume<MaterialDensityPair44> volData(g_uVolumeSideLength, g_uVolumeSideLength, g_uVolumeSideLength);

	createSphereInVolume(volData, 10, 1);

	SurfaceExtractor<MaterialDensityPair44> surfaceExtractor(volData);

	shared_ptr<SurfaceMesh> surface = surfaceExtractor.extractSurfaceForRegion(volData.getEnclosingRegion());

	//OpenGLSurfaceMesh mesh = BuildOpenGLSurfaceMesh(*surface);

	
	openGLWidget.mesh = BuildOpenGLSurfaceMesh(*(surface.get()));
	openGLWidget.surfaceMesh = *surface;
	return app.exec();
} 
