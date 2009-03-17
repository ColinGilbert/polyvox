#include "PolyVoxCore/BlockVolume.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/SurfaceExtractors.h"
#include "PolyVoxCore/Utility.h"

#include <windows.h>   // Standard Header For Most Programs

//#define USE_OPENGL_VERTEX_BUFFERS_OBJECTS

#ifdef USE_OPENGL_VERTEX_BUFFERS_OBJECTS
	#ifdef WIN32
		#include "glew/glew.h"
	#else
		#include <gl/gl.h>     // The GL Header File
	#endif
	#include <gl/glut.h>   // The GL Utility Toolkit (Glut) Header]
#else
	#include <gl/gl.h>
	#include <gl/glut.h>
#endif

#include <iostream>


//Some namespaces we need
using namespace std;
using namespace PolyVox;
using namespace std;

#ifdef USE_OPENGL_VERTEX_BUFFERS_OBJECTS
struct OpenGLSurfacePatch
{
	GLulong noOfIndices;
	GLuint indexBuffer;
	GLuint vertexBuffer;
};
#endif

//Global variables are easier for demonstration purposes, especially
//as I'm not sure how/if I can pass variables to the GLUT functions.
//Global variables are denoted by the 'g_' prefix
const uint16 g_uVolumeSideLength = 256;
const uint16 g_uRegionSideLength = 16;
const uint16 g_uVolumeSideLengthInRegions = g_uVolumeSideLength / g_uRegionSideLength;

int g_xRotation = 0.0f;
int g_yRotation = 0.0f;
int g_xOld = 0;
int g_yOld = 0;

int g_frameCounter = 0;

//Creates a volume 128x128x128
BlockVolume<uint8> g_volData(logBase2(g_uVolumeSideLength));

//Rather than storing one big mesh, the volume is broken into regions and a mesh is stored for each region
#ifdef USE_OPENGL_VERTEX_BUFFERS_OBJECTS
	OpenGLSurfacePatch g_openGLSurfacePatches[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];
#else
	IndexedSurfacePatch* g_indexedSurfacePatches[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];
#endif

#ifdef USE_OPENGL_VERTEX_BUFFERS_OBJECTS
OpenGLSurfacePatch BuildOpenGLSurfacePatch(IndexedSurfacePatch& isp)
{
	OpenGLSurfacePatch result;

	const vector<SurfaceVertex>& vecVertices = isp.getVertices();
	const vector<uint32>& vecIndices = isp.getIndices();

	glGenBuffers(1, &result.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.indexBuffer);
	int s = vecIndices.size() * sizeof(GLint);
	if(s != 0)
	{
		GLvoid* blah = (GLvoid*)(&(vecIndices[0]));				
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, s, blah, GL_STATIC_DRAW);
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
		const Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<Vector3DFloat>(isp.m_v3dRegionPosition);

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

		GLfloat red = 0.0f;
		GLfloat green = 0.0f;
		GLfloat blue = 0.0f;

		uint8 material = vertex.getMaterial() + 0.5;

		switch(material)
		{
		case 1:
			red = 1.0;
			green = 0.0;
			blue = 0.0;
			break;
		case 2:
			red = 0.0;
			green = 1.0;
			blue = 0.0;
			break;
		case 3:
			red = 0.0;
			green = 0.0;
			blue = 1.0;
			break;
		case 4:
			red = 1.0;
			green = 1.0;
			blue = 0.0;
			break;
		case 5:
			red = 1.0;
			green = 0.0;
			blue = 1.0;
			break;
		}

		*ptr = red;
		ptr++;
		*ptr = green;
		ptr++;
		*ptr = blue;
		ptr++;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	return result;
}
#endif

void createSphere(float fRadius, uint8 uValue)
{
	//This vector hold the position of the center of the volume
	Vector3DFloat v3dVolCenter(g_volData.getSideLength() / 2, g_volData.getSideLength() / 2, g_volData.getSideLength() / 2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < g_volData.getSideLength(); z++)
	{
		for (int y = 0; y < g_volData.getSideLength(); y++)
		{
			for (int x = 0; x < g_volData.getSideLength(); x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				//If the current voxel is less than 'radius' units from the center
				//then we make it solid, otherwise we make it empty space.
				if(fDistToCenter <= fRadius)
				{
					g_volData.setVoxelAt(x,y,z, uValue);
				}
			}
		}
	}
}

void createCube(Vector3DUint16 lowerCorner, Vector3DUint16 upperCorner, uint8 uValue)
{
	//This three-level for loop iterates over every voxel between the specified corners
	for (int z = lowerCorner.getZ(); z <= upperCorner.getZ(); z++)
	{
		for (int y = lowerCorner.getY(); y <= upperCorner.getY(); y++)
		{
			for (int x = lowerCorner.getX() ; x <= upperCorner.getX(); x++)
			{
				g_volData.setVoxelAt(x,y,z, uValue);
			}
		}
	}
}

void timerFunc(int value)
{
	cout << "FPS = " << g_frameCounter << endl;
	g_frameCounter = 0;
	glutTimerFunc(1000, timerFunc, 0);
}

void init ( GLvoid )     // Create Some Everyday Functions
{
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

	glShadeModel(GL_SMOOTH);
}

void display ( void )   // Create The Display Function
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glMatrixMode   ( GL_MODELVIEW );  // Select The Model View Matrix
	glLoadIdentity();									// Reset The Current Modelview Matrix
	
	//Moves the camera back so we can see the volume
	glTranslatef(0.0f, 0.0f, -100.0f);	

	//Rotate the volume by the required amount
	glRotatef(g_xRotation, 1.0f, 0.0f, 0.0f);
	glRotatef(g_yRotation, 0.0f, 1.0f, 0.0f);

	//Centre the volume on the origin
	glTranslatef(-g_uVolumeSideLength/2,-g_uVolumeSideLength/2,-g_uVolumeSideLength/2);

	for(uint16 uRegionZ = 0; uRegionZ < g_uVolumeSideLengthInRegions; ++uRegionZ)
	{
		for(uint16 uRegionY = 0; uRegionY < g_uVolumeSideLengthInRegions; ++uRegionY)
		{
			for(uint16 uRegionX = 0; uRegionX < g_uVolumeSideLengthInRegions; ++uRegionX)
			{
#ifdef USE_OPENGL_VERTEX_BUFFERS_OBJECTS
				glBindBuffer(GL_ARRAY_BUFFER, g_openGLSurfacePatches[uRegionX][uRegionY][uRegionZ].vertexBuffer);
				glVertexPointer(3, GL_FLOAT, 36, 0);
				glNormalPointer(GL_FLOAT, 36, (GLvoid*)12);
				glColorPointer(3, GL_FLOAT, 36, (GLvoid*)24);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_openGLSurfacePatches[uRegionX][uRegionY][uRegionZ].indexBuffer);

				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_NORMAL_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);

				glDrawElements(GL_TRIANGLES, g_openGLSurfacePatches[uRegionX][uRegionY][uRegionZ].noOfIndices, GL_UNSIGNED_INT, 0);

				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
				
#else		
				IndexedSurfacePatch* ispCurrent = g_indexedSurfacePatches[uRegionX][uRegionY][uRegionZ];

				const vector<SurfaceVertex>& vecVertices = ispCurrent->getVertices();
				const vector<uint32>& vecIndices = ispCurrent->getIndices();

				glBegin(GL_TRIANGLES);
				for(vector<uint32>::const_iterator iterIndex = vecIndices.begin(); iterIndex != vecIndices.end(); ++iterIndex)
				{
					const SurfaceVertex& vertex = vecVertices[*iterIndex];
					const Vector3DFloat& v3dVertexPos = vertex.getPosition();
					//const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
					const Vector3DFloat v3dFinalVertexPos = v3dVertexPos + static_cast<Vector3DFloat>(ispCurrent->m_v3dRegionPosition);

					
					

					GLfloat red = 0.0f;
					GLfloat green = 0.0f;
					GLfloat blue = 0.0f;

					uint8 material = vertex.getMaterial() + 0.5;

					switch(material)
					{
					case 1:
						red = 1.0;
						green = 0.0;
						blue = 0.0;
						break;
					case 2:
						red = 0.0;
						green = 1.0;
						blue = 0.0;
						break;
					case 3:
						red = 0.0;
						green = 0.0;
						blue = 1.0;
						break;
					case 4:
						red = 1.0;
						green = 1.0;
						blue = 0.0;
						break;
					case 5:
						red = 1.0;
						green = 0.0;
						blue = 1.0;
						break;
					}

					glColor3f(red, green, blue);
					glNormal3f(vertex.getNormal().getX(), vertex.getNormal().getY(), vertex.getNormal().getZ());
					glVertex3f(v3dFinalVertexPos.getX(), v3dFinalVertexPos.getY(), v3dFinalVertexPos.getZ());
					
					
				}
				glEnd();
#endif
			}
		}
	}

	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR)
	{
		errString = gluErrorString(errCode);
		cout << "OpenGL Error: " << errString << endl;
	}



	g_frameCounter++;
	glutSwapBuffers ( );
	// Swap The Buffers To Not Be Left With A Clear Screen
}

void idle()
{
	glutPostRedisplay();
}


void reshape ( int w, int h )   // Create The Reshape Function (the viewport)
{
	glViewport     ( 0, 0, w, h );
	glMatrixMode   ( GL_PROJECTION );  // Select The Projection Matrix
	glLoadIdentity ( );                // Reset The Projection Matrix
	if ( h==0 )  // Calculate The Aspect Ratio Of The Window
		gluPerspective ( 80, ( float ) w, 1.0, 5000.0 );
	else
		gluPerspective ( 80, ( float ) w / ( float ) h, 1.0, 5000.0 );
	glMatrixMode   ( GL_MODELVIEW );  // Select The Model View Matrix
	glLoadIdentity ( );    // Reset The Model View Matrix
}

void keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{
	switch ( key ) {
case 27:        // When Escape Is Pressed...
	exit ( 0 );   // Exit The Program
	break;        // Ready For Next Case
default:        // Now Wrap It Up
	break;
	}
}

void motion(int x, int y)
{
	glMatrixMode(GL_MODELVIEW);
	int xDiff = x - g_xOld;
	int yDiff = y - g_yOld;
	g_xOld = x;
	g_yOld = y;
	g_xRotation += xDiff;
	g_yRotation += yDiff;
}


void arrow_keys ( int a_keys, int x, int y )  // Create Special Function (required for arrow keys)
{
	switch ( a_keys ) {
case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
	glutFullScreen ( ); // Go Into Full Screen Mode
	break;
case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
	glutReshapeWindow ( 500, 500 ); // Go Into A 500 By 500 Window
	break;
default:
	break;
	}
}

void main ( int argc, char** argv )   // Create Main Function For Bringing It All Together
{
	glutInit ( &argc, argv ); // Erm Just Write It =)
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
	glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
	glutCreateWindow    ( "PolyVox OpenGL Example" ); // Window Title (argv[0] for current directory as title)
	glutMotionFunc(motion);
	glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
	glutReshapeFunc     ( reshape );
	glutKeyboardFunc    ( keyboard );
	glutSpecialFunc     ( arrow_keys );
	glutTimerFunc(1000, timerFunc, 0);
	glutIdleFunc(idle);

#ifdef USE_OPENGL_VERTEX_BUFFERS_OBJECTS
#ifdef WIN32
	//If we are on Windows we will need GLEW to access recent OpenGL functionality
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		cout << "Error: " << glewGetErrorString(err) << endl;
	}
#endif
#endif

	//Make our volume contain a sphere in the center.
	uint16 minPos = 0;
	uint16 midPos = g_volData.getSideLength() / 2;
	uint16 maxPos = g_volData.getSideLength() - 1;
	createCube(Vector3DUint16(minPos, minPos, minPos), Vector3DUint16(maxPos, maxPos, maxPos), 0);

	createSphere(50.0f, 5);
	createSphere(40.0f, 4);
	createSphere(30.0f, 3);
	createSphere(20.0f, 2);
	createSphere(10.0f, 1);	

	createCube(Vector3DUint16(minPos, minPos, minPos), Vector3DUint16(midPos-1, midPos-1, midPos-1), 0);
	createCube(Vector3DUint16(midPos+1, midPos+1, minPos), Vector3DUint16(maxPos, maxPos, midPos-1), 0);
	createCube(Vector3DUint16(midPos+1, minPos, midPos+1), Vector3DUint16(maxPos, midPos-1, maxPos), 0);
	createCube(Vector3DUint16(minPos, midPos+1, midPos+1), Vector3DUint16(midPos-1, maxPos, maxPos), 0);

	//Our volume is broken down into cuboid regions, and we create one mesh for each region.
	//This three-level for loop iterates over each region.
	for(uint16 uRegionZ = 0; uRegionZ < g_uVolumeSideLengthInRegions; ++uRegionZ)
	{
		for(uint16 uRegionY = 0; uRegionY < g_uVolumeSideLengthInRegions; ++uRegionY)
		{
			for(uint16 uRegionX = 0; uRegionX < g_uVolumeSideLengthInRegions; ++uRegionX)
			{
				//Create a new surface patch (which is basiaclly the PolyVox term for a mesh).
				IndexedSurfacePatch* ispCurrent = new IndexedSurfacePatch();

				//Compute the extents of the current region
				//FIXME - This is a little complex? PolyVox could
				//provide more functions for dealing with regions?
				uint16 regionStartX = uRegionX * g_uRegionSideLength;
				uint16 regionStartY = uRegionY * g_uRegionSideLength;
				uint16 regionStartZ = uRegionZ * g_uRegionSideLength;

				uint16 regionEndX = regionStartX + g_uRegionSideLength + 1; //Why do we need the '+1' here?
				uint16 regionEndY = regionStartY + g_uRegionSideLength + 1; //Why do we need the '+1' here?
				uint16 regionEndZ = regionStartZ + g_uRegionSideLength + 1; //Why do we need the '+1' here?

				Vector3DInt32 regLowerCorner(regionStartX, regionStartY, regionStartZ);
				Vector3DInt32 regUpperCorner(regionEndX, regionEndY, regionEndZ);

				//Extract the surface for this region
				extractReferenceSurface(&g_volData, Region(regLowerCorner, regUpperCorner), ispCurrent);

#ifdef USE_OPENGL_VERTEX_BUFFERS_OBJECTS
				g_openGLSurfacePatches[uRegionX][uRegionY][uRegionZ] = BuildOpenGLSurfacePatch(*ispCurrent);
#else
				g_indexedSurfacePatches[uRegionX][uRegionY][uRegionZ] = ispCurrent;
#endif

				//delete ispCurrent;
			}
		}
	}

	init ();

	glutMainLoop        ( );          // Initialize The Main Loop
}

