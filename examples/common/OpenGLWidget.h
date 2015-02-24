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

#ifndef __OpenGLWidget_H_A72D4D2F8E__ // Random junk as 'OpenGLWidget seems like it could be a common name.
#define __OpenGLWidget_H_A72D4D2F8E__

#include <QElapsedTimer>
#include <QGLWidget>
#include <QMatrix4x4>

// This is a very basic class for getting an OpenGL example up and running with Qt5. It simply displays
// an OpenGL widget and implements an FPS-style camera as well as other very basic functionality. User
// code can derive from this and override the provided virtual functions to implement functionality.
// The class is templatized so users can specify the OpenGL version via the appropriate QOpenGLFunctions.
template <typename QOpenGLFunctionsType>
class OpenGLWidget : public QGLWidget, protected QOpenGLFunctionsType
{
protected:
	// Protected constructor because this widget should not be created directly - it should only be subclassed.
	OpenGLWidget(QWidget *parent);

	// Derived classes should override these to provide functionality.
	virtual void initialize() {}
	virtual void renderOneFrame() {}

	// Getters for properties defined by this widget.
	const QMatrix4x4& viewMatrix();
	const QMatrix4x4& projectionMatrix();

	// Setters for properties defined by this widget.
	void setCameraTransform(QVector3D position, float pitch, float yaw);

private:

	// Qt OpenGL functions
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	// Mouse handling
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);

	// Keyboard handling
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

	// Matrices
	QMatrix4x4 mViewMatrix;
	QMatrix4x4 mProjectionMatrix;

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

#include "OpenGLWidget.inl"

#endif //__OpenGLWidget_H_A72D4D2F8E__
