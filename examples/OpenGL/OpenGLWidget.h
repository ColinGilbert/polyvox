#ifndef __PolyVox_OpenGLWidget_H__
#define __PolyVox_OpenGLWidget_H__

#ifdef WIN32
#include "glew/glew.h"
#else
#include <gl/gl.h>     // The GL Header File
#endif

#include <QGLWidget>

#include "PolyVoxCore/Volume.h"
#include "PolyVoxCore/IndexedSurfacePatch.h"
#include "PolyVoxCore/SurfaceExtractors.h"
#include "PolyVoxCore/Utility.h"

#include "OpenGLImmediateModeSupport.h"
#include "OpenGLVertexBufferObjectSupport.h"
#include "Shapes.h"

const PolyVox::uint16 g_uVolumeSideLength = 256;
const PolyVox::uint16 g_uRegionSideLength = 16;
const PolyVox::uint16 g_uVolumeSideLengthInRegions = g_uVolumeSideLength / g_uRegionSideLength;

class OpenGLWidget : public QGLWidget
 {

 public:
     OpenGLWidget(QWidget *parent);

 protected:
     void initializeGL();
     void resizeGL(int w, int h);
     void paintGL();

 private:

	 bool g_bUseOpenGLVertexBufferObjects;

	 //Creates a volume 128x128x128
	PolyVox::Volume<PolyVox::uint8>* g_volData;

	//Rather than storing one big mesh, the volume is broken into regions and a mesh is stored for each region
	OpenGLSurfacePatch g_openGLSurfacePatches[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];
	PolyVox::IndexedSurfacePatch* g_indexedSurfacePatches[g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions][g_uVolumeSideLengthInRegions];
 };

#endif //__PolyVox_OpenGLWidget_H__