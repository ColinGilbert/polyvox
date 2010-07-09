#include "OpenGLWidget.h"

#include <QMouseEvent>

using namespace PolyVox;
using namespace std;

OpenGLWidget::OpenGLWidget(QWidget *parent)
	:QGLWidget(parent)
	,m_xRotation(0)
	,m_yRotation(0)
{
}

void OpenGLWidget::setSurfaceMeshToRender(const PolyVox::SurfaceMesh& surfaceMesh)
{
	//Sanity check that we have something to render.
	if(surfaceMesh.getVertices().empty() || surfaceMesh.getIndices().empty())
	{
		return;
	}

	//Convienient access to the vertices and indices
	vector<uint32_t>& vecIndices = const_cast<vector<uint32_t>&>(surfaceMesh.getIndices());
	vector<SurfaceVertex>& vecVertices = const_cast<vector<SurfaceVertex>&>(surfaceMesh.getVertices());

	//Build an OpenGL index buffer
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	GLvoid* pIndices = static_cast<GLvoid*>(&(vecIndices[0]));		
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(uint32_t), pIndices, GL_STATIC_DRAW);

	//Build an OpenGL vertex buffer
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	GLvoid* pVertices = static_cast<GLvoid*>(&(vecVertices[0]));	
	glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(SurfaceVertex), pVertices, GL_STATIC_DRAW);

	m_uBeginIndex = surfaceMesh.m_vecLodRecords[0].beginIndex;
	m_uEndIndex = surfaceMesh.m_vecLodRecords[0].endIndex;
}

void OpenGLWidget::initializeGL()
{
#ifdef WIN32
	//If we are on Windows we will need GLEW to access recent OpenGL functionality
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
	}
#endif

	//Set up the clear colour
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		
	glClearDepth(1.0f);		

	//Enable the depth buffer
	glEnable(GL_DEPTH_TEST);					
	glDepthFunc(GL_LEQUAL);						

	//Anable smooth lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	//We'll be rendering with index/vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}

void OpenGLWidget::resizeGL(int w, int h)
{
	//Setup the viewport
	glViewport(0, 0, w, h);

	//Set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float frustumSize = 32.0f; //Half the volume size
	float aspect = static_cast<float>(width()) / static_cast<float>(height());
	glOrtho(frustumSize*aspect, -frustumSize*aspect, frustumSize, -frustumSize, 1.0, 1000);
}

void OpenGLWidget::paintGL()
{
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set up the viewing transformation
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-100.0f); //Centre volume and move back
	glRotatef(m_xRotation, 1.0f, 0.0f, 0.0f);
	glRotatef(m_yRotation, 0.0f, 1.0f, 0.0f);
	glTranslatef(-32.0f,-32.0f,-32.0f); //Centre volume and move back

	//Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	//Bind the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexPointer(3, GL_FLOAT, sizeof(SurfaceVertex), 0);
	glNormalPointer(GL_FLOAT, sizeof(SurfaceVertex), (GLvoid*)12);

	glDrawRangeElements(GL_TRIANGLES, m_uBeginIndex, m_uEndIndex-1, m_uEndIndex - m_uBeginIndex, GL_UNSIGNED_INT, 0);
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	m_CurrentMousePos = event->pos();
	m_LastFrameMousePos = m_CurrentMousePos;

	update();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	m_CurrentMousePos = event->pos();
	QPoint diff = m_CurrentMousePos - m_LastFrameMousePos;
	m_xRotation += diff.x();
	m_yRotation += diff.y();
	m_LastFrameMousePos = m_CurrentMousePos;

	update();
}