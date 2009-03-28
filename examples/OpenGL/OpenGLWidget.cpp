#include "OpenGLWidget.h"

//Some namespaces we need
using namespace std;
using namespace PolyVox;
using namespace std;

OpenGLWidget::OpenGLWidget(QWidget *parent)
	:QGLWidget(parent)
{
	

	
	
}

void OpenGLWidget::setVolume(PolyVox::Volume<PolyVox::uint8>* volData)
{
	//First we free anything from the previous volume (if there was one).
	m_mapOpenGLSurfacePatches.clear();
	m_mapIndexedSurfacePatches.clear();
	m_volData = volData;

	//If we have any volume data then generate the new surface patches.
	if(m_volData != 0)
	{
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
					extractReferenceSurface(m_volData, Region(regLowerCorner, regUpperCorner), ispCurrent);


					Vector3DUint8 v3dRegPos(uRegionX,uRegionY,uRegionZ);
					if(m_bUseOpenGLVertexBufferObjects)
					{
						OpenGLSurfacePatch openGLSurfacePatch = BuildOpenGLSurfacePatch(*ispCurrent);					
						m_mapOpenGLSurfacePatches.insert(make_pair(v3dRegPos, openGLSurfacePatch));
					}
					else
					{
						m_mapIndexedSurfacePatches.insert(make_pair(v3dRegPos, ispCurrent));
					}
					//delete ispCurrent;
				}
			}
		}
	}
}

void OpenGLWidget::initializeGL()
{
	m_bUseOpenGLVertexBufferObjects = true;
	if(m_bUseOpenGLVertexBufferObjects)
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

void OpenGLWidget::resizeGL(int w, int h)
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

void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

	glMatrixMode   ( GL_MODELVIEW );  // Select The Model View Matrix
	glLoadIdentity();									// Reset The Current Modelview Matrix

	//Moves the camera back so we can see the volume
	glTranslatef(0.0f, 0.0f, -100.0f);	

	//Rotate the volume by the required amount
	//glRotatef(g_xRotation, 1.0f, 0.0f, 0.0f);
	//glRotatef(g_yRotation, 0.0f, 1.0f, 0.0f);

	//Centre the volume on the origin
	glTranslatef(-g_uVolumeSideLength/2,-g_uVolumeSideLength/2,-g_uVolumeSideLength/2);

	for(uint16 uRegionZ = 0; uRegionZ < g_uVolumeSideLengthInRegions; ++uRegionZ)
	{
		for(uint16 uRegionY = 0; uRegionY < g_uVolumeSideLengthInRegions; ++uRegionY)
		{
			for(uint16 uRegionX = 0; uRegionX < g_uVolumeSideLengthInRegions; ++uRegionX)
			{
				Vector3DUint8 v3dRegPos(uRegionX,uRegionY,uRegionZ);
				if(m_bUseOpenGLVertexBufferObjects)
				{
					renderRegionVertexBufferObject(m_mapOpenGLSurfacePatches[v3dRegPos]);
				}
				else
				{
					IndexedSurfacePatch* ispCurrent = m_mapIndexedSurfacePatches[v3dRegPos];
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
	}									// Reset The Current Modelview Matrix
}