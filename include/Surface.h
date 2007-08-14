#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "Ogre.h"
#include <vector>

#include "Triangle.h"
#include "Vertex.h"

namespace Ogre
{
	class Surface : public SimpleRenderable
	{
	public:
	   Surface();
	   Surface(const String& material);
	   ~Surface(void);

	   void setGeometry(std::vector<Vertex> verticesToSet, std::vector<Triangle> indicesToSet);

	   Real getSquaredViewDepth(const Camera *cam) const;
	   Real getBoundingRadius(void) const;
	protected:
	   //void getWorldTransforms(Matrix4 *xform) const;
	   const Quaternion &getWorldOrientation(void) const;
	   const Vector3 &getWorldPosition(void) const;
	};	
}

#endif /* __SURFACE_H__ */