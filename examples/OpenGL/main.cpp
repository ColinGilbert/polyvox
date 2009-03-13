#include "PolyVoxCore/BlockVolume.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/SurfaceExtractors.h"
#include "PolyVoxCore/Utility.h"

#include <windows.h>   // Standard Header For Most Programs

#ifdef WIN32
	#include "glew/glew.h"
#else
	#include <gl/gl.h>     // The GL Header File
#endif
#include <gl/glut.h>   // The GL Utility Toolkit (Glut) Header

#include <iostream>


//Some namespaces we need
using namespace std;
using namespace PolyVox;
using namespace std;

//Global variables are easier for demonstration purposes, especially
//as I'm not sure how/if I can pass variables to the GLUT functions.
//Global variables are denoted by the 'g_' prefix
const uint16 g_uVolumeSideLength = 128;
const uint16 g_uRegionSideLength = 16;
const uint16 g_uVolumeSideLengthInRegions = g_uVolumeSideLength / g_uRegionSideLength;

//Creates a volume 128x128x128
BlockVolume<uint8> g_volData(logBase2(g_uVolumeSideLength));

//Rather than storing one big mesh, the volume is broken into regions and a mesh is stored for each region
IndexedSurfacePatch* g_ispRegionSurfaces[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];
GLuint indexBuffers[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];
GLuint vertexBuffers[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];

void createSphereInVolume(float fRadius, uint8 uValue)
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
				else
				{
					g_volData.setVoxelAt(x,y,z, 0);
				}
			}
		}
	}
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
}

void display ( void )   // Create The Display Function
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(-g_uVolumeSideLength/2,-g_uVolumeSideLength/2,-200.0f);

	for(uint16 uRegionZ = 0; uRegionZ < g_uVolumeSideLengthInRegions; ++uRegionZ)
	{
		for(uint16 uRegionY = 0; uRegionY < g_uVolumeSideLengthInRegions; ++uRegionY)
		{
			for(uint16 uRegionX = 0; uRegionX < g_uVolumeSideLengthInRegions; ++uRegionX)
			{

				const vector<uint32>& vecIndices = g_ispRegionSurfaces[uRegionX][uRegionY][uRegionZ]->getIndices();

				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[uRegionX][uRegionY][uRegionZ]);
				glVertexPointer(3, GL_FLOAT, 0, 0);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[uRegionX][uRegionY][uRegionZ]);

				glEnableClientState(GL_VERTEX_ARRAY);

				int s = vecIndices.size();
				glDrawElements(GL_TRIANGLE_STRIP, s, GL_UNSIGNED_INT, 0);

				glDisableClientState(GL_VERTEX_ARRAY); 
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



	glutSwapBuffers ( );
	// Swap The Buffers To Not Be Left With A Clear Screen
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
	init ();
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
	glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
	glutCreateWindow    ( "PolyVox OpenGL Example" ); // Window Title (argv[0] for current directory as title)
	//glutFullScreen      ( );          // Put Into Full Screen
	glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
	glutReshapeFunc     ( reshape );
	glutKeyboardFunc    ( keyboard );
	glutSpecialFunc     ( arrow_keys );

#ifdef WIN32
	//If we are on Windows we will need GLEW to access recent OpenGL functionality
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  cout << "Error: " << glewGetErrorString(err) << endl;
	}
#endif

	//Make our volume contain a sphere in the center.
	createSphereInVolume(50.0f, 1);

	//Our volume is broken down into cuboid regions, and we create one mesh for each region.
	//This three-level for loop iterates over each region.
	for(uint16 uRegionZ = 0; uRegionZ < g_uVolumeSideLengthInRegions; ++uRegionZ)
	{
		for(uint16 uRegionY = 0; uRegionY < g_uVolumeSideLengthInRegions; ++uRegionY)
		{
			for(uint16 uRegionX = 0; uRegionX < g_uVolumeSideLengthInRegions; ++uRegionX)
			{
				//Create a new surface patch (which is basiaclly the PolyVox term for a mesh).
				g_ispRegionSurfaces[uRegionX][uRegionY][uRegionZ] = new IndexedSurfacePatch();
				IndexedSurfacePatch* ispCurrent = g_ispRegionSurfaces[uRegionX][uRegionY][uRegionZ];

				//Compute the extents of the current region
				//FIXME - This is a little complex? PolyVox could
				//provide more functions for dealing with regions?
				uint16 regionStartX = uRegionX * g_uRegionSideLength;
				uint16 regionStartY = uRegionY * g_uRegionSideLength;
				uint16 regionStartZ = uRegionZ * g_uRegionSideLength;

				uint16 regionEndX = regionStartX + g_uRegionSideLength;
				uint16 regionEndY = regionStartY + g_uRegionSideLength;
				uint16 regionEndZ = regionStartZ + g_uRegionSideLength;

				Vector3DInt32 regLowerCorner(regionStartX, regionStartY, regionStartZ);
				Vector3DInt32 regUpperCorner(regionEndX, regionEndY, regionEndZ);

				//Extract the surface for this region
				extractReferenceSurface(&g_volData, Region(regLowerCorner, regUpperCorner), ispCurrent);

				const vector<SurfaceVertex>& vecVertices = g_ispRegionSurfaces[uRegionX][uRegionY][uRegionZ]->getVertices();
				const vector<uint32>& vecIndices = g_ispRegionSurfaces[uRegionX][uRegionY][uRegionZ]->getIndices();

				glGenBuffers(1, &(indexBuffers[uRegionX][uRegionY][uRegionZ]));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[uRegionX][uRegionY][uRegionZ]);
				int s = vecIndices.size() * sizeof(GLint);
				if(s != 0)
				{
					GLvoid* blah = (GLvoid*)(&(vecIndices[0]));				
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, s, blah, GL_STATIC_DRAW);
				}

				glGenBuffers(1, &(vertexBuffers[uRegionX][uRegionY][uRegionZ]));
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[uRegionX][uRegionY][uRegionZ]);
				glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(GLfloat) * 3, 0, GL_STATIC_DRAW);
				GLfloat* ptr = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

				for(vector<SurfaceVertex>::const_iterator iterVertex = vecVertices.begin(); iterVertex != vecVertices.end(); ++iterVertex)
				{
					const SurfaceVertex& vertex = *iterVertex;
					const Vector3DFloat& v3dVertexPos = vertex.getPosition();
					const Vector3DFloat v3dRegionOffset(uRegionX * g_uRegionSideLength, uRegionY * g_uRegionSideLength, uRegionZ * g_uRegionSideLength);
					const Vector3DFloat v3dFinalVertexPos = v3dVertexPos + v3dRegionOffset;

					*ptr = v3dFinalVertexPos.getX();
					ptr++;
					*ptr = v3dFinalVertexPos.getY();
					ptr++;
					*ptr = v3dFinalVertexPos.getZ();
					ptr++;
				}

				glUnmapBuffer(GL_ARRAY_BUFFER);

			}
		}
	}

	
	glutMainLoop        ( );          // Initialize The Main Loop

	//Delete all the surface patches we created.
	for(uint16 uRegionZ = 0; uRegionZ < g_uVolumeSideLengthInRegions; ++uRegionZ)
	{
		for(uint16 uRegionY = 0; uRegionY < g_uVolumeSideLengthInRegions; ++uRegionY)
		{
			for(uint16 uRegionX = 0; uRegionX < g_uVolumeSideLengthInRegions; ++uRegionX)
			{
				delete g_ispRegionSurfaces[uRegionX][uRegionY][uRegionZ];
			}
		}
	}
}

