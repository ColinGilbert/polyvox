/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution. 	
*******************************************************************************/

#ifndef __BasicExample_OpenGLWidget_H__
#define __BasicExample_OpenGLWidget_H__

#include "PolyVoxCore/Mesh.h"

#include "glew/glew.h"

#include <QGLWidget>
#include <QGLShaderProgram>

struct OpenGLMeshData
{
	GLuint noOfIndices;
	GLuint indexBuffer;
	GLuint vertexBuffer;
	GLuint vertexArrayObject;
	PolyVox::Vector3DInt32 translation;
};

class OpenGLWidget : public QGLWidget
{
public:
	// Constructor
	OpenGLWidget(QWidget *parent);

	// Mouse handling
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);

	// The viewable region can be adjusted so that this example framework can be use for different volume sizes.
	void setViewableRegion(PolyVox::Region viewableRegion);

	void setShader(QGLShaderProgram* shader)
	{
		this->shader = shader;
	}

	// Convert a SurfaceMesh to OpenGL index/vertex buffers. Inlined because it's templatised.
	template <typename MeshType>
	void addMesh(const MeshType& surfaceMesh)
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

		meshData.translation = surfaceMesh.m_Region.getLowerCorner();

		mMeshData.push_back(meshData);
	}

protected:
	

	// Qt OpenGL functions
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

private:

	void setupWorldToCameraMatrix();

	// Index/vertex buffer data
	std::vector<OpenGLMeshData> mMeshData;
	
	QGLShaderProgram* shader;

	// Matrices
	QMatrix4x4 worldToCameraMatrix;
	QMatrix4x4 cameraToClipMatrix;

	// Mouse data
	QPoint m_LastFrameMousePos;
	QPoint m_CurrentMousePos;

	// Camera setup
	PolyVox::Region m_viewableRegion;
	int m_xRotation;
	int m_yRotation;
};

#endif //__BasicExample_OpenGLWidget_H__
