#include "PolyVoxCore/BlockVolume.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/SurfaceExtractors.h"
#include "PolyVoxCore/Utility.h"

#include "OpenGLImmediateModeSupport.h"
#include "OpenGLVertexBufferObjectSupport.h"
#include "Shapes.h"

#include <windows.h>   // Standard Header For Most Programs

#define USE_OPENGL_VERTEX_BUFFERS_OBJECTS

#ifdef WIN32
#include "glew/glew.h"
#else
#include <gl/gl.h>     // The GL Header File
#endif
#include <gl/glut.h>   // The GL Utility Toolkit (Glut) Header]

#include <iostream>

//Some namespaces we need
using namespace std;
using namespace PolyVox;
using namespace std;

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

bool g_bUseOpenGLVertexBufferObjects;

//Creates a volume 128x128x128
BlockVolume<uint8> g_volData(logBase2(g_uVolumeSideLength));

//Rather than storing one big mesh, the volume is broken into regions and a mesh is stored for each region
OpenGLSurfacePatch g_openGLSurfacePatches[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];
IndexedSurfacePatch* g_indexedSurfacePatches[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];

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
				if(g_bUseOpenGLVertexBufferObjects)
				{
					renderRegionVertexBufferObject(g_openGLSurfacePatches[uRegionX][uRegionY][uRegionZ]);
				}
				else
				{
					IndexedSurfacePatch* ispCurrent = g_indexedSurfacePatches[uRegionX][uRegionY][uRegionZ];
					renderRegionImmediateMode(*ispCurrent);

				}
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
	g_bUseOpenGLVertexBufferObjects = true;
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

	if(g_bUseOpenGLVertexBufferObjects)
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
	}

	//Make our volume contain a sphere in the center.
	uint16 minPos = 0;
	uint16 midPos = g_volData.getSideLength() / 2;
	uint16 maxPos = g_volData.getSideLength() - 1;
	createCubeInVolume(g_volData, Vector3DUint16(minPos, minPos, minPos), Vector3DUint16(maxPos, maxPos, maxPos), 0);

	createSphereInVolume(g_volData, 50.0f, 5);
	createSphereInVolume(g_volData, 40.0f, 4);
	createSphereInVolume(g_volData, 30.0f, 3);
	createSphereInVolume(g_volData, 20.0f, 2);
	createSphereInVolume(g_volData, 10.0f, 1);	

	createCubeInVolume(g_volData, Vector3DUint16(minPos, minPos, minPos), Vector3DUint16(midPos-1, midPos-1, midPos-1), 0);
	createCubeInVolume(g_volData, Vector3DUint16(midPos+1, midPos+1, minPos), Vector3DUint16(maxPos, maxPos, midPos-1), 0);
	createCubeInVolume(g_volData, Vector3DUint16(midPos+1, minPos, midPos+1), Vector3DUint16(maxPos, midPos-1, maxPos), 0);
	createCubeInVolume(g_volData, Vector3DUint16(minPos, midPos+1, midPos+1), Vector3DUint16(midPos-1, maxPos, maxPos), 0);

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


				if(g_bUseOpenGLVertexBufferObjects)
				{
					g_openGLSurfacePatches[uRegionX][uRegionY][uRegionZ] = BuildOpenGLSurfacePatch(*ispCurrent);
				}
				else
				{
					g_indexedSurfacePatches[uRegionX][uRegionY][uRegionZ] = ispCurrent;
				}
				//delete ispCurrent;
			}
		}
	}

	init ();

	glutMainLoop        ( );          // Initialize The Main Loop
}