#include "OpenGLWidget.h"

#include <QMouseEvent>
#include <QMatrix4x4>
#include <QtMath>

using namespace PolyVox;
using namespace std;

OpenGLWidget::OpenGLWidget(QWidget *parent)
	:QGLWidget(parent)
	,m_xRotation(0)
	,m_yRotation(0)
	,gl(nullptr)
{
}

void OpenGLWidget::setSurfaceMeshToRender(const PolyVox::SurfaceMesh<PositionMaterial>& surfaceMesh)
{
	//Convienient access to the vertices and indices
	const auto& vecIndices = surfaceMesh.getIndices();
	const auto& vecVertices = surfaceMesh.getVertices();
	
	//Create the VAO for the mesh
	gl->glGenVertexArrays(1, &vertexArrayObject);
	gl->glBindVertexArray(vertexArrayObject);
	
	//The GL_ARRAY_BUFFER will contain the list of vertex positions
	gl->glGenBuffers(1, &vertexBuffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	gl->glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(PositionMaterial), vecVertices.data(), GL_STATIC_DRAW);
	
	//and GL_ELEMENT_ARRAY_BUFFER will contain the indices
	gl->glGenBuffers(1, &indexBuffer);
	gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(uint32_t), vecIndices.data(), GL_STATIC_DRAW);
	
	//We need to tell OpenGL how to understand the format of the vertex data
	gl->glEnableVertexAttribArray(0); //We're talking about shader attribute '0' 
	gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PositionMaterial), 0); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)
	
	gl->glBindVertexArray(0);
	
	noOfIndices = vecIndices.size(); //Save this for the call to glDrawElements later
}

void OpenGLWidget::initializeGL()
{
	//'gl' will give us access to all the OpenGL functions
	gl = context()->contextHandle()->versionFunctions<QOpenGLFunctions_3_1>();
	if(!gl)
	{
		std::cerr << "Could not obtain required OpenGL context version" << std::endl;
		exit(EXIT_FAILURE);
	}
	if(!gl->initializeOpenGLFunctions())
	{
		std::cerr << "Could not initialise OpenGL functions" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	//Print out some information about the OpenGL implementation.
	std::cout << "OpenGL Implementation Details:" << std::endl;
	if(gl->glGetString(GL_VENDOR))
	  std::cout << "\tGL_VENDOR: " << gl->glGetString(GL_VENDOR) << std::endl;
	if(gl->glGetString(GL_RENDERER))
	  std::cout << "\tGL_RENDERER: " << gl->glGetString(GL_RENDERER) << std::endl;
	if(gl->glGetString(GL_VERSION))
	  std::cout << "\tGL_VERSION: " << gl->glGetString(GL_VERSION) << std::endl;
	if(gl->glGetString(GL_SHADING_LANGUAGE_VERSION))
	  std::cout << "\tGL_SHADING_LANGUAGE_VERSION: " << gl->glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	//Set up the clear colour
	gl->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	gl->glClearDepth(1.0f);
	
	gl->glEnable(GL_CULL_FACE);
	gl->glEnable(GL_DEPTH_TEST);
	gl->glDepthMask(GL_TRUE);
	gl->glDepthFunc(GL_LEQUAL);
	gl->glDepthRange(0.0, 1.0);
	
	if(!shader.addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
		#version 140
		
		in vec4 position; //This will be the position of the vertex in model-space
		
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
		std::cerr << shader.log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	if(!shader.addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
		#version 130
		
		in vec4 worldPosition; //Passed in from the vertex shader
		
		out vec4 outputColor;
		
		void main()
		{
			vec3 normal = normalize(cross(dFdy(worldPosition.xyz), dFdx(worldPosition.xyz)));
			
			float color = clamp(abs(dot(normalize(normal.xyz), vec3(0.9,0.1,0.5))), 0, 1);
			outputColor = vec4(1.0, 0.5, color, 1.0);
		}
	)"))
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
	
	shader.bind();
	
	QMatrix4x4 worldToCameraMatrix{};
	worldToCameraMatrix.translate(0, 0, -50); //Move the camera back by 50 units
	
	shader.setUniformValue("worldToCameraMatrix", worldToCameraMatrix);
	
	shader.release();
}

void OpenGLWidget::resizeGL(int w, int h)
{
	//Setup the viewport
	gl->glViewport(0, 0, w, h);
	
	auto aspectRatio = w / (float)h;
	float zNear = 1.0;
	float zFar = 1000.0;
	
	QMatrix4x4 cameraToClipMatrix{};
	cameraToClipMatrix.frustum(-aspectRatio, aspectRatio, -1, 1, zNear, zFar);
	
	shader.bind();
	shader.setUniformValue("cameraToClipMatrix", cameraToClipMatrix);
	shader.release();
}

void OpenGLWidget::paintGL()
{
	//Clear the screen
	gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	QMatrix4x4 modelToWorldMatrix{};
	modelToWorldMatrix.rotate(m_xRotation, 0, 1, 0); //rotate around y-axis
	modelToWorldMatrix.rotate(m_yRotation, 1, 0, 0); //rotate around x-axis
	modelToWorldMatrix.translate(-32, -32, -32); //centre the model on the origin
	
	shader.bind();
	
	shader.setUniformValue("modelToWorldMatrix", modelToWorldMatrix); //Update to the latest camera matrix
	
	gl->glBindVertexArray(vertexArrayObject);
	
	gl->glDrawElements(GL_TRIANGLES, noOfIndices, GL_UNSIGNED_INT, 0);
	
	gl->glBindVertexArray(0);
	
	shader.release();
	
	GLenum errCode = gl->glGetError();
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

	update();
}
