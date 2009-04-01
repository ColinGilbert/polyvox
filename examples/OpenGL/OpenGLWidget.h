#ifndef __PolyVox_OpenGLWidget_H__
#define __PolyVox_OpenGLWidget_H__

#include "glew/glew.h"

#include <QGLWidget>

#include "PolyVoxCore/Volume.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/SurfaceExtractors.h"
#include "PolyVoxCore/PolyVoxImpl/Utility.h"

#include "OpenGLImmediateModeSupport.h"
#include "OpenGLVertexBufferObjectSupport.h"
#include "Shapes.h"

const PolyVox::uint16_t g_uVolumeSideLength = 128;
const PolyVox::uint16_t g_uRegionSideLength = 16;
const PolyVox::uint16_t g_uVolumeSideLengthInRegions = g_uVolumeSideLength / g_uRegionSideLength;

class OpenGLWidget : public QGLWidget
 {

 public:
     OpenGLWidget(QWidget *parent);

	 void setVolume(PolyVox::Volume<PolyVox::uint8_t>* volData);

 protected:
     void initializeGL();
     void resizeGL(int w, int h);
     void paintGL();

 private:

	 bool m_bUseOpenGLVertexBufferObjects;

	 //Creates a volume 128x128x128
	PolyVox::Volume<PolyVox::uint8_t>* m_volData;

	//Rather than storing one big mesh, the volume is broken into regions and a mesh is stored for each region
	std::map<PolyVox::Vector3DUint8, OpenGLSurfacePatch> m_mapOpenGLSurfacePatches;
	std::map<PolyVox::Vector3DUint8, PolyVox::IndexedSurfacePatch*> m_mapIndexedSurfacePatches;
 };

#endif //__PolyVox_OpenGLWidget_H__