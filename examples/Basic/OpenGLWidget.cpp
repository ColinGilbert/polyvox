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

void OpenGLWidget::setMeshToRender(const PolyVox::Mesh<CubicVertex<uint8_t> >& surfaceMesh)
{
	//Convienient access to the vertices and indices
	const auto& vecIndices = surfaceMesh.getIndices();
	const auto& vecVertices = surfaceMesh.getVertices();

	// This struct holds the OpenGL properties (buffer handles, etc) which will be used
	// to render our mesh. We copy the data from the PolyVox mesh into this structure.
	OpenGLMeshData meshData;
	
	//Create the VAO for the mesh
	glGenVertexArrays(1, &(meshData.vertexArrayObject));
	glBindVertexArray(meshData.vertexArrayObject);
	
	//The GL_ARRAY_BUFFER will contain the list of vertex positions
	glGenBuffers(1, &(meshData.vertexBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, meshData.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(CubicVertex<uint8_t>), vecVertices.data(), GL_STATIC_DRAW);
	
	//and GL_ELEMENT_ARRAY_BUFFER will contain the indices
	glGenBuffers(1, &(meshData.indexBuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(uint32_t), vecIndices.data(), GL_STATIC_DRAW);
	
	//We need to tell OpenGL how to understand the format of the vertex data
	glEnableVertexAttribArray(0); //We're talking about shader attribute '0' 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CubicVertex<uint8_t>), 0); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)
	
	glBindVertexArray(0);
	
	meshData.noOfIndices = vecIndices.size(); //Save this for the call to glDrawElements later

	mMeshData.push_back(meshData);
}

void OpenGLWidget::setViewableRegion(Region viewableRegion)
{
	m_viewableRegion = viewableRegion;
	setupWorldToCameraMatrix();
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
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0, 1.0);
	
	if (!shader.addShaderFromSourceCode(QGLShader::Vertex,
		"#version 140\n"
		
		"in vec4 position; //This will be the position of the vertex in model-space\n"
		
		"uniform mat4 cameraToClipMatrix;\n"
		"uniform mat4 worldToCameraMatrix;\n"
		"uniform mat4 modelToWorldMatrix;\n"
		
		"out vec4 worldPosition; //This is being passed to the fragment shader to calculate the normals\n"
		
		"void main()\n"
		"{\n"
		"	worldPosition = modelToWorldMatrix * position;\n"
		"	vec4 cameraPosition = worldToCameraMatrix * worldPosition;\n"
		"	gl_Position = cameraToClipMatrix * cameraPosition;\n"
		"}\n"
		))
	{
		std::cerr << shader.log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if (!shader.addShaderFromSourceCode(QGLShader::Fragment,
		"#version 130\n"
		
		"in vec4 worldPosition; //Passed in from the vertex shader\n"
		
		"out vec4 outputColor;\n"
		
		"void main()\n"
		"{\n"
		"	vec3 normal = normalize(cross(dFdy(worldPosition.xyz), dFdx(worldPosition.xyz)));\n"
			
		"	float color = clamp(abs(dot(normalize(normal.xyz), vec3(0.9,0.1,0.5))), 0, 1);\n"
		"	outputColor = vec4(1.0, 0.5, color, 1.0);\n"
		"}"
	))
	{
		std::cerr << shader.log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	shader.bindAttributeLocation("position", 0);
	
	if(!shader.link())
	{
		std::cerr << shader.log().toStdString() << std::endl;
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
	
	QMatrix4x4 cameraToClipMatrix;
	cameraToClipMatrix.frustum(-aspectRatio, aspectRatio, -1, 1, zNear, zFar);
	
	shader.bind();
	shader.setUniformValue("cameraToClipMatrix", cameraToClipMatrix);
	shader.release();
}

void OpenGLWidget::paintGL()
{
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QMatrix4x4 modelToWorldMatrix;
	//modelToWorldMatrix.rotate(m_xRotation, 0, 1, 0); //rotate around y-axis
	//modelToWorldMatrix.rotate(m_yRotation, 1, 0, 0); //rotate around x-axis
	//modelToWorldMatrix.translate(-32, -32, -32); //centre the model on the origin

	shader.bind();

	shader.setUniformValue("modelToWorldMatrix", modelToWorldMatrix); //Update to the latest camera matrix

	for (OpenGLMeshData meshData : mMeshData)
	{
		glBindVertexArray(meshData.vertexArrayObject);

		glDrawElements(GL_TRIANGLES, meshData.noOfIndices, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
	
	shader.release();
	
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
	shader.bind();

	QVector3D lowerCorner(m_viewableRegion.getLowerX(), m_viewableRegion.getLowerY(), m_viewableRegion.getLowerZ());
	QVector3D upperCorner(m_viewableRegion.getUpperX(), m_viewableRegion.getUpperY(), m_viewableRegion.getUpperZ());

	QVector3D centerPoint = (lowerCorner + upperCorner) * 0.5;
	float fDiagonalLength = (upperCorner - lowerCorner).length();

	QMatrix4x4 worldToCameraMatrix;
	worldToCameraMatrix.translate(0, 0, -fDiagonalLength / 2.0f); //Move the camera back by the required amount
	worldToCameraMatrix.rotate(m_xRotation, 0, 1, 0); //rotate around y-axis
	worldToCameraMatrix.rotate(m_yRotation, 1, 0, 0); //rotate around x-axis
	worldToCameraMatrix.translate(-centerPoint); //centre the model on the origin

	shader.setUniformValue("worldToCameraMatrix", worldToCameraMatrix);

	shader.release();
}