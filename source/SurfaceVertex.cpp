#include "Constants.h"
#include "SurfaceVertex.h"
#include "SurfaceTriangle.h"
#include "SurfaceEdge.h"

#include "OgreStringConverter.h"

namespace Ogre
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(UIntVector3 positionToSet)
		:position(positionToSet)
	{
		/*noOfUses = 0;
		flags = 0;
		if(position.x == 0)
			flags |= 1;
		if(position.x == 8)
			flags |= 2;
		if(position.y == 0)
			flags |= 4;
		if(position.y == 8)
			flags |= 8;
		if(position.z == 0)
			flags |= 16;
		if(position.z == 8)
			flags |= 32;*/
	}

	SurfaceVertex::SurfaceVertex(UIntVector3 positionToSet, Vector3 normalToSet)
		:position(positionToSet)
		,normal(normalToSet)
	{
		/*noOfUses = 0;
		flags = 0;
		if(position.x == 0)
			flags |= 1;
		if(position.x == 8)
			flags |= 2;
		if(position.y == 0)
			flags |= 4;
		if(position.y == 8)
			flags |= 8;
		if(position.z == 0)
			flags |= 16;
		if(position.z == 8)
			flags |= 32;*/
	}

	const UIntVector3& SurfaceVertex::getPosition(void) const
	{
		return position;
	}

	std::string SurfaceVertex::toString(void)
	{
		std::stringstream ss;
		ss << "SurfaceVertex: Position = (" << position.x << "," << position.y << "," << position.z << "), Normal = " << StringConverter::toString(normal);
		return ss.str();
	}

	bool operator==(const SurfaceVertex& lhs, const SurfaceVertex& rhs)
	{
		//We dont't check the normal here as it may not have been set. But if two vertices have the same position they should have the same normal too.
		/*return
		(
			(position.x == rhs.position.x) && 
			(position.y == rhs.position.y) && 
			(position.z == rhs.position.z) //&& 
			//(abs(alpha - rhs.alpha) <= 0.01)
		);*/

		/*ulong value = 0;
		value |= position.x;
		value << 10;
		value |= position.y;
		value << 10;
		value |= position.z;

		ulong rhsValue = 0;
		rhsValue |= rhs.position.x;
		rhsValue << 10;
		rhsValue |= rhs.position.y;
		rhsValue << 10;
		rhsValue |= rhs.position.z;

		return value == rhsValue;*/

		unsigned long lhsOffset = (lhs.getPosition().x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (lhs.getPosition().y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (lhs.getPosition().z);
		unsigned long rhsOffset = (rhs.getPosition().x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (rhs.getPosition().y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (rhs.getPosition().z);

		return (lhsOffset == rhsOffset) /*&& (abs(lhs.alpha - rhs.alpha) <= 0.01)*/;
	}

	bool operator < (const SurfaceVertex& lhs, const SurfaceVertex& rhs)
	{
		unsigned long lhsOffset = (lhs.getPosition().x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (lhs.getPosition().y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (lhs.getPosition().z);
		unsigned long rhsOffset = (rhs.getPosition().x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (rhs.getPosition().y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (rhs.getPosition().z);

		return lhsOffset < rhsOffset;		
	}	
}
