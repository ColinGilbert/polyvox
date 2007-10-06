#ifndef __SurfacePatchRenderable_H__
#define __SurfacePatchRenderable_H__

#include "Ogre.h"
#include <vector>

#include "AbstractSurfacePatch.h"
//#include "SurfaceTriangle.h"
//#include "SurfaceVertex.h"

namespace Ogre
{
	//IDEA - If profiling identifies this class as a bottleneck, we could implement a memory pooling system.
	//All buffers could be powers of two, and we get the smallest one which is big enough for our needs.
	//See http://www.ogre3d.org/wiki/index.php/DynamicGrowingBuffers
	class SurfacePatchRenderable : public SimpleRenderable
	{
	public:
	   SurfacePatchRenderable(AbstractSurfacePatch* patchToRender, const String& material = "BaseWhiteNoLighting");
	   ~SurfacePatchRenderable(void);

	   void updateWithNewSurfacePatch(AbstractSurfacePatch* patchToRender);
	   void setGeometry(AbstractSurfacePatch* patchToRender);

	   Real getSquaredViewDepth(const Camera *cam) const;
	   Real getBoundingRadius(void) const;
	protected:
	   //void getWorldTransforms(Matrix4 *xform) const;
	   const Quaternion &getWorldOrientation(void) const;
	   const Vector3 &getWorldPosition(void) const;
	};	
}

#endif /* __SurfacePatchRenderable_H__ */
