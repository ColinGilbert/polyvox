#ifndef __SurfacePatchRenderable_H__
#define __SurfacePatchRenderable_H__

#include "Ogre.h"
#include <vector>

#include "SurfaceTriangle.h"
#include "SurfaceVertex.h"

namespace Ogre
{
	class SurfacePatchRenderable : public SimpleRenderable
	{
	public:
	   SurfacePatchRenderable(const String& material = "BaseWhiteNoLighting");
	   ~SurfacePatchRenderable(void);

	   void setGeometry(std::vector<SurfaceVertex> verticesToSet, std::vector<uint> indicesToSet);

	   Real getSquaredViewDepth(const Camera *cam) const;
	   Real getBoundingRadius(void) const;
	protected:
	   //void getWorldTransforms(Matrix4 *xform) const;
	   const Quaternion &getWorldOrientation(void) const;
	   const Vector3 &getWorldPosition(void) const;
	};	
}

#endif /* __SurfacePatchRenderable_H__ */
