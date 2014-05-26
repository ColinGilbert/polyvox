#include "OpenGLWidget.h"

#include <QMouseEvent>
#include <QMatrix4x4>
//#include <QtMath>

using namespace PolyVox;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////
OpenGLWidget::OpenGLWidget(QWidget *parent)
	:QGLWidget(parent)
	,m_viewableRegion(Region(0, 0, 0, 255, 255, 255))
	,m_xRotation(0)
	,m_yRotation(0)
{
}

void OpenGLWidget::setShader(QSharedPointer<QGLShaderProgram> shader)
{
	mShader = shader;
}

void OpenGLWidget::setViewableRegion(Region viewableRegion)
{
	m_viewableRegion = viewableRegion;

	// The user has specifed a new viewable region
	// so we need to regenerate our camera matrix.
	setupWorldToCameraMatrix();
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
	// Initialise these variables which will be used when the mouse actually moves.
	m_CurrentMousePos = event->pos();
	m_LastFrameMousePos = m_CurrentMousePos;
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
	// Update the x and y rotations based on the mouse movement.
	m_CurrentMousePos = event->pos();
	QPoint diff = m_CurrentMousePos - m_LastFrameMousePos;
	m_xRotation += diff.x();
	m_yRotation += diff.y();
	m_LastFrameMousePos = m_CurrentMousePos;

	// The camera rotation has changed so we need to regenerate the matrix.
	setupWorldToCameraMatrix();

	// Re-render.
	update();
}

////////////////////////////////////////////////////////////////////////////////
// Protected functions
////////////////////////////////////////////////////////////////////////////////
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
	
	// This is basically a simple fallback vertex shader which does the most basic rendering possible.  
	// PolyVox examples are able to provide their own shaders to demonstrate certain effects if desired. 
	if (!mShader->addShaderFromSourceCode(QGLShader::Vertex,
		"#version 140\n"
		
		"in vec4 position; // This will be the position of the vertex in model-space\n"
		
		"// The usual matrices are provided\n"
		"uniform mat4 cameraToClipMatrix;\n"
		"uniform mat4 worldToCameraMatrix;\n"
		"uniform mat4 modelToWorldMatrix;\n"
		
		"// This will be used by the fragment shader to calculate flat-shaded normals. This is an unconventional approach\n"
		"// but we use it in this example framework because not all surface extractor generate surface normals.\n"
		"out vec4 worldPosition;\n"
		
		"void main()\n"
		"{\n"
		"	// Standard sequence of OpenGL transformations.\n"
		"	worldPosition = modelToWorldMatrix * position;\n"
		"	vec4 cameraPosition = worldToCameraMatrix * worldPosition;\n"
		"	gl_Position = cameraToClipMatrix * cameraPosition;\n"
		"}\n"
	))
	{
		std::cerr << mShader->log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// This is basically a simple fallback fragment shader which does the most basic rendering possible.  
	// PolyVox examples are able to provide their own shaders to demonstrate certain effects if desired. 
	if (!mShader->addShaderFromSourceCode(QGLShader::Fragment,
		"#version 130\n"
		
		"// Passed in from the vertex shader\n"
		"in vec4 worldPosition;\n"
		"in vec4 worldNormal;\n"
		
		"// the color that gets written to the display\n"
		"out vec4 outputColor;\n"
		
		"void main()\n"
		"{\n"
		"	// Again, for the purposes of these examples we cannot be sure that per-vertex normals are provided. A sensible fallback \n"
		"	// is to use this little trick to compute per-fragment flat-shaded normals from the world positions using derivative operations.\n"
		"	vec3 normal = normalize(cross(dFdy(worldPosition.xyz), dFdx(worldPosition.xyz)));\n"
			
		"	// We are just using the normal as the output color, and making it lighter so it looks a bit nicer. \n"
		"	// Obviously a real shader would also do texuring, lighting, or whatever is required for the application.\n"
		"	outputColor = vec4(abs(normal) * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);\n"
		"}\n"
	))
	{
		std::cerr << mShader->log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// Bind the position semantic - this is defined in the vertex shader above.
	mShader->bindAttributeLocation("position", 0);

	// Bind the other semantics. Note that these don't actually exist in our example shader above! However, other
	// example shaders may choose to provide them and having the binding code here does not seem to cause any problems.
	mShader->bindAttributeLocation("normal", 1);
	mShader->bindAttributeLocation("material", 2);
	
	if (!mShader->link())
	{
		std::cerr << mShader->log().toStdString() << std::endl;
		exit(EXIT_FAILURE);
	}

	// Initial setup of camera.
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

	// Our example framework only uses a single shader for the scene (for all meshes).
	mShader->bind();

	// These two matrices are constant for all meshes.
	mShader->setUniformValue("worldToCameraMatrix", worldToCameraMatrix);
	mShader->setUniformValue("cameraToClipMatrix", cameraToClipMatrix);

	// Iterate over each mesh which the user added to our list, and render it.
	for (OpenGLMeshData meshData : mMeshData)
	{
		//Set up the model matrrix based on provided translation and scale.
		QMatrix4x4 modelToWorldMatrix;
		modelToWorldMatrix.translate(meshData.translation); 
		modelToWorldMatrix.scale(meshData.scale);
		mShader->setUniformValue("modelToWorldMatrix", modelToWorldMatrix);

		// Bind the vertex array for the current mesh
		glBindVertexArray(meshData.vertexArrayObject);
		// Draw the mesh
		glDrawElements(GL_TRIANGLES, meshData.noOfIndices, GL_UNSIGNED_INT, 0);
		// Unbind the vertex array.
		glBindVertexArray(0);
	}
	
	 // We're done with the shader for this frame.
	mShader->release();
	
	// Check for errors.
	GLenum errCode = glGetError();
	if(errCode != GL_NO_ERROR)
	{
	  std::cerr << "OpenGL Error: " << errCode << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////
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