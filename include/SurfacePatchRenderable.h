#ifndef __SurfacePatchRenderable_H__
#define __SurfacePatchRenderable_H__

#include "Ogre.h"
#include <vector>

#include "SurfacePatch.h"
#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"

namespace Ogre
{
	//IDEA - If profiling identifies this class as a bottleneck, we could implement a memory pooling system.
	//All buffers could be powers of two, and we get the smallest one which is big enough for our needs.
	//See http://www.ogre3d.org/wiki/index.php/DynamicGrowingBuffers
	class SurfacePatchRenderable : public SimpleRenderable
	{
	public:
	   SurfacePatchRenderable(SurfacePatch& patchToRender, const String& material = "BaseWhiteNoLighting");
	   ~SurfacePatchRenderable(void);

	   void updateWithNewSurfacePatch(SurfacePatch& patchToRender);
	   void setGeometry(SurfacePatch& patchToRender);

	   Real getSquaredViewDepth(const Camera *cam) const;
	   Real getBoundingRadius(void) const;
	protected:
	   //void getWorldTransforms(Matrix4 *xform) const;
	   const Quaternion &getWorldOrientation(void) const;
	   const Vector3 &getWorldPosition(void) const;
	};	
}

#endif /* __SurfacePatchRenderable_H__ */
