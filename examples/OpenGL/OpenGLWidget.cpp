#include "OpenGLWidget.h"

#include <QMouseEvent>

#include "GradientEstimators.h"
#include "SurfaceAdjusters.h"

//Some namespaces we need
using namespace std;
using namespace PolyVox;
using namespace std;

OpenGLWidget::OpenGLWidget(QWidget *parent)
	:QGLWidget(parent)
	,m_volData(0)
{	
	m_xRotation = 0;
	m_yRotation = 0;
	m_uRegionSideLength = 32.0f;

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(0);
}

void OpenGLWidget::setVolume(PolyVox::Volume<PolyVox::uint8_t>* volData)
{
	//First we free anything from the previous volume (if there was one).
	m_mapOpenGLSurfacePatches.clear();
	m_mapIndexedSurfacePatches.clear();
	m_volData = volData;

	//If we have any volume data then generate the new surface patches.
	if(m_volData != 0)
	{
		m_uVolumeWidthInRegions = volData->getWidth() / m_uRegionSideLength;
		m_uVolumeHeightInRegions = volData->getHeight() / m_uRegionSideLength;
		m_uVolumeDepthInRegions = volData->getDepth() / m_uRegionSideLength;

		//Our volume is broken down into cuboid regions, and we create one mesh for each region.
		//This three-level for loop iterates over each region.
		for(PolyVox::uint16_t uRegionZ = 0; uRegionZ < m_uVolumeDepthInRegions; ++uRegionZ)
		{
			std::cout << "uRegionZ = " << uRegionZ << " of " << m_uVolumeDepthInRegions << std::endl;
			for(PolyVox::uint16_t uRegionY = 0; uRegionY < m_uVolumeHeightInRegions; ++uRegionY)
			{
				for(PolyVox::uint16_t uRegionX = 0; uRegionX < m_uVolumeWidthInRegions; ++uRegionX)
				{
					//Create a new surface patch (which is basiaclly the PolyVox term for a mesh).
					IndexedSurfacePatch* ispCurrent = new IndexedSurfacePatch();

					//Compute the extents of the current region
					//FIXME - This is a little complex? PolyVox could
					//provide more functions for dealing with regions?
					PolyVox::uint16_t regionStartX = uRegionX * m_uRegionSideLength;
					PolyVox::uint16_t regionStartY = uRegionY * m_uRegionSideLength;
					PolyVox::uint16_t regionStartZ = uRegionZ * m_uRegionSideLength;

					PolyVox::uint16_t regionEndX = regionStartX + m_uRegionSideLength; //Why do we need the '+1' here?
					PolyVox::uint16_t regionEndY = regionStartY + m_uRegionSideLength; //Why do we need the '+1' here?
					PolyVox::uint16_t regionEndZ = regionStartZ + m_uRegionSideLength; //Why do we need the '+1' here?

					Vector3DInt32 regLowerCorner(regionStartX, regionStartY, regionStartZ);
					Vector3DInt32 regUpperCorner(regionEndX, regionEndY, regionEndZ);

					//Extract the surface for this region
					extractSurface(m_volData, 2, PolyVox::Region(regLowerCorner, regUpperCorner), ispCurrent);

					//computeNormalsForVertices(m_volData, *ispCurrent, SOBEL_SMOOTHED);
					//*ispCurrent = getSmoothedSurface(*ispCurrent);


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

		//Projection matrix is dependant on volume size, so we need to set it up again.
		setupProjectionMatrix();
	}
}

void OpenGLWidget::initializeGL()
{
	m_bUseOpenGLVertexBufferObjects = false;
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
	//Setup the viewport based on the window size
	glViewport(0, 0, w, h);

	//Projection matrix is also dependant on the size of the current volume.
	if(m_volData)
	{
		setupProjectionMatrix();
	}
}

void OpenGLWidget::paintGL()
{
	if(m_volData)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

		glMatrixMode(GL_MODELVIEW);  // Select The Model View Matrix
		glLoadIdentity();									// Reset The Current Modelview Matrix

		//Moves the camera back so we can see the volume
		glTranslatef(0.0f, 0.0f, -m_volData->getDiagonalLength());	

		glRotatef(m_xRotation, 1.0f, 0.0f, 0.0f);
		glRotatef(m_yRotation, 0.0f, 1.0f, 0.0f);

		//Centre the volume on the origin
		glTranslatef(-g_uVolumeSideLength/2,-g_uVolumeSideLength/2,-g_uVolumeSideLength/2);

		for(PolyVox::uint16_t uRegionZ = 0; uRegionZ < m_uVolumeDepthInRegions; ++uRegionZ)
		{
			for(PolyVox::uint16_t uRegionY = 0; uRegionY < m_uVolumeHeightInRegions; ++uRegionY)
			{
				for(PolyVox::uint16_t uRegionX = 0; uRegionX < m_uVolumeWidthInRegions; ++uRegionX)
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
		}
	}
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	m_CurrentMousePos = event->pos();
	m_LastFrameMousePos = m_CurrentMousePos;
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	m_CurrentMousePos = event->pos();
	QPoint diff = m_CurrentMousePos - m_LastFrameMousePos;
	m_xRotation += diff.x();
	m_yRotation += diff.y();
	m_LastFrameMousePos = m_CurrentMousePos;;
}

void OpenGLWidget::wheelEvent(QWheelEvent* event)
{
}

void OpenGLWidget::setupProjectionMatrix(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float frustumSize = m_volData->getDiagonalLength() / 2.0f;
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	glOrtho(frustumSize*aspect, -frustumSize*aspect, frustumSize, -frustumSize, 1.0, 5000);
}