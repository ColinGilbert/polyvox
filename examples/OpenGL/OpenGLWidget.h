#ifndef __PolyVox_OpenGLWidget_H__
#define __PolyVox_OpenGLWidget_H__

#include <QGLWidget>

class OpenGLWidget : public QGLWidget
 {

 public:
     OpenGLWidget(QWidget *parent);

 protected:
     void initializeGL();
     void resizeGL(int w, int h);
     void paintGL();

 };

#endif //__PolyVox_OpenGLWidget_H__