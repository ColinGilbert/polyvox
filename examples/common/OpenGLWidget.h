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

#include "PolyVox/Mesh.h"

#include <QOpenGLFunctions_3_1>

#include <QElapsedTimer>
#include <QGLWidget>
#include <QGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

// Our OpenGLWidget is used by all the examples to render the extracted meshes. It is
// fairly specific to our needs (you probably won't want to use it in your own project)
// but should provide a useful illustration of how PolyVox meshes can be rendered.
class OpenGLWidget : public QGLWidget, protected QOpenGLFunctions_3_1
{
public:
	// Constructor
	OpenGLWidget(QWidget *parent);

	void setCameraTransform(QVector3D position, float pitch, float yaw);

	// Mouse handling
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);

	// Keyboard handling
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

protected:

	// Qt OpenGL functions
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	virtual void initialize() {}
	virtual void renderOneFrame() {}

protected:

	// Matrices
	QMatrix4x4 worldToCameraMatrix;
	QMatrix4x4 cameraToClipMatrix;

	// Mouse data
	QPoint m_LastFrameMousePos;
	QPoint m_CurrentMousePos;

	// Keyboard data
	QList<int> mPressedKeys;

	// For input handling and movement
	float mCameraMoveSpeed = 50.0f;
	float mCameraRotateSpeed = 0.005f;

	// Camera properties
	QVector3D mCameraPosition = QVector3D(0, 0, -100);
	float mCameraYaw = 0.0f;
	float mCameraPitch = 0.0f;
	float mCameraFOV = 60.0f;

	QElapsedTimer mElapsedTimer;
};

#endif //__BasicExample_OpenGLWidget_H__
