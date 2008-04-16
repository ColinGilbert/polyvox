#include <sstream>

#include "Constants.h"
#include "SurfaceVertex.h"

namespace PolyVox
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, float materialToSet, float alphaToSet)
		:material(materialToSet)
		,alpha(alphaToSet)
		,position(positionToSet)
	{
		
	}

	SurfaceVertex::SurfaceVertex(Vector3DFloat positionToSet, Vector3DFloat normalToSet)
		:position(positionToSet)
		,normal(normalToSet)
	{
	}

	float SurfaceVertex::getAlpha(void) const
	{
		return alpha;
	}	

	float SurfaceVertex::getMaterial(void) const
	{
		return material;
	}

	const Vector3DFloat& SurfaceVertex::getNormal(void) const
	{
		return normal;
	}

	const Vector3DFloat& SurfaceVertex::getPosition(void) const
	{
		return position;
	}

	void SurfaceVertex::setAlpha(float alphaToSet)
	{
		alpha = alphaToSet;
	}

	void SurfaceVertex::setMaterial(float materialToSet)
	{
		material = materialToSet;
	}

	void SurfaceVertex::setNormal(const Vector3DFloat& normalToSet)
	{
		normal = normalToSet;
		normal.normalise();
	}		

	std::string SurfaceVertex::tostring(void) const
	{
		std::stringstream ss;
		ss << "SurfaceVertex: Position = (" << position.x() << "," << position.y() << "," << position.z() << "), Normal = " << normal;
		return ss.str();
	}	
}
