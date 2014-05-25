#include "OpenGLWidget.h"

#include <QMouseEvent>
#include <QMatrix4x4>
//#include <QtMath>

using namespace PolyVox;
using namespace std;

OpenGLWidget::OpenGLWidget(QWidget *parent)
	:QGLWidget(parent)
	,m_viewableRegion(Region(0, 0, 0, 255, 255, 255))
	,m_xRotation(0)
	,m_yRotation(0)
{
}

void OpenGLWidget::setViewableRegion(Region viewableRegion)
{
	m_viewableRegion = viewableRegion;
	setupWorldToCameraMatrix();
}

void OpenGLWidget::setShader(QSharedPointer<QGLShaderProgram> shader)
{
	mShader = shader;
}

void OpenGLWidget::initializeGL()
{
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
	}
	
	//Print out some information about the OpenGL implementation.
	std::cout << "OpenGL Implementation Details:" << std::endl;
	if(glGetString(GL_VENDOR))
	  std::cout << "\tGL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
	if(glGetString(GL_RENDERER))
	  std::cout << "\tGL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
	if(glGetString(GL_VERSION))
	  std::cout << "\tGL_VERSION: " << glGetString(GL_VERSION) << std::endl;
	if(glGetString(GL_SHADING_LANGUAGE_VERSION))
	  std::cout << "\tGL_SHADING_LANGUAGE_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	//Set up the clear colour
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);

	mShader = QSharedPointer<QGLShaderProgram>(new QGLShaderProgram);
	
	if (!mShader->addShaderFromSourceCode(QGLShader::Vertex, R"(
		#version 140
		
		in vec4 position; // This will be the position of the vertex in model-space
		
		uniform mat4 cameraToClipMatrix;
		uniform mat4 worldToCameraMatrix;
		uniform mat4 modelToWorldMatrix;
		
		out vec4 worldPosition; //This is being passed to the fragment shader to calculate the normals
		
		void main()
		{
			worldPosition = modelToWorldMatrix * position;
			vec4 cameraPosition = worldToCameraMatrix * worldPosition;
			gl_Position = cameraToClipMatrix * cameraPosition;
		}
	)"))
	{
		std::cerr << mShader->log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if (!mShader->addShaderFromSourceCode(QGLShader::Fragment, R"(
		#version 130
		
		in vec4 worldPosition; //Passed in from the vertex shader
		in vec4 worldNormal;
		
		out vec4 outputColor;
		
		void main()
		{
			vec3 normal = normalize(cross(dFdy(worldPosition.xyz), dFdx(worldPosition.xyz)));
			
			float color = clamp(abs(dot(normalize(normal.xyz), vec3(0.9,0.1,0.5))), 0, 1);
			outputColor = vec4(abs(normal) * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);
		}
	)"))
	{
		std::cerr << mShader->log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	mShader->bindAttributeLocation("position", 0);
	mShader->bindAttributeLocation("normal", 1);
	mShader->bindAttributeLocation("material", 2);
	
	if (!mShader->link())
	{
		std::cerr << mShader->log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}

	setupWorldToCameraMatrix();
}

void OpenGLWidget::resizeGL(int w, int h)
{
	//Setup the viewport
	glViewport(0, 0, w, h);
	
	auto aspectRatio = w / (float)h;
	float zNear = 1.0;
	float zFar = 1000.0;
	
	cameraToClipMatrix.setToIdentity();
	cameraToClipMatrix.frustum(-aspectRatio, aspectRatio, -1, 1, zNear, zFar);
}

void OpenGLWidget::paintGL()
{
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mShader->bind();

	mShader->setUniformValue("worldToCameraMatrix", worldToCameraMatrix);
	mShader->setUniformValue("cameraToClipMatrix", cameraToClipMatrix);

	for (OpenGLMeshData meshData : mMeshData)
	{
		QMatrix4x4 modelToWorldMatrix{};
		modelToWorldMatrix.translate(meshData.translation); 
		modelToWorldMatrix.scale(meshData.scale);
		mShader->setUniformValue("modelToWorldMatrix", modelToWorldMatrix);

		glBindVertexArray(meshData.vertexArrayObject);

		glDrawElements(GL_TRIANGLES, meshData.noOfIndices, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
	
	mShader->release();
	
	GLenum errCode = glGetError();
	if(errCode != GL_NO_ERROR)
	{
	  std::cerr << "OpenGL Error: " << errCode << std::endl;
	}
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

	setupWorldToCameraMatrix();

	update();
}

void OpenGLWidget::setupWorldToCameraMatrix()
{
	QVector3D lowerCorner(m_viewableRegion.getLowerX(), m_viewableRegion.getLowerY(), m_viewableRegion.getLowerZ());
	QVector3D upperCorner(m_viewableRegion.getUpperX(), m_viewableRegion.getUpperY(), m_viewableRegion.getUpperZ());

	QVector3D centerPoint = (lowerCorner + upperCorner) * 0.5;
	float fDiagonalLength = (upperCorner - lowerCorner).length();

	worldToCameraMatrix.setToIdentity();
	worldToCameraMatrix.translate(0, 0, -fDiagonalLength / 2.0f); //Move the camera back by the required amount
	worldToCameraMatrix.rotate(m_xRotation, 0, 1, 0); //rotate around y-axis
	worldToCameraMatrix.rotate(m_yRotation, 1, 0, 0); //rotate around x-axis
	worldToCameraMatrix.translate(-centerPoint); //centre the model on the origin
}