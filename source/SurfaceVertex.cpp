#include "Constants.h"
#include "SurfaceVertex.h"
#include "SurfaceTriangle.h"
#include "SurfaceEdge.h"

namespace Ogre
{
	SurfaceVertex::SurfaceVertex()
	{
	}

	SurfaceVertex::SurfaceVertex(UIntVector3 positionToSet)
		:position(positionToSet)
	{
		flags = 0;
		if(position.x == 0)
			flags |= 0x01;
		if(position.x == 32)
			flags |= 0x02;
		if(position.y == 0)
			flags |= 0x04;
		if(position.y == 32)
			flags |= 0x08;
	}

	SurfaceVertex::SurfaceVertex(UIntVector3 positionToSet, Vector3 normalToSet)
		:position(positionToSet)
		,normal(normalToSet)
	{
		flags = 0;
		if(position.x == 0)
			flags |= 0x01;
		if(position.x == 32)
			flags |= 0x02;
		if(position.y == 0)
			flags |= 0x04;
		if(position.y == 32)
			flags |= 0x08;
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

		unsigned long lhsOffset = (lhs.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (lhs.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (lhs.position.z);
		unsigned long rhsOffset = (rhs.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (rhs.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (rhs.position.z);

		return (lhsOffset == rhsOffset) /*&& (abs(lhs.alpha - rhs.alpha) <= 0.01)*/;
	}

	bool operator < (const SurfaceVertex& lhs, const SurfaceVertex& rhs)
	{
		/*if((*this) == rhs)
		{
			return false;
		}
		if(alpha < rhs.alpha)
		{
			return true;
		}*/
		/*if(position.z < rhs.position.z)
		{
			return true;
		}
		if(position.y < rhs.position.y)
		{
			return true;
		}
		if(position.x < rhs.position.x)
		{
			return true;
		}
		
		return false;*/

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

		return value < rhsValue;*/

		unsigned long lhsOffset = (lhs.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (lhs.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (lhs.position.z);
		unsigned long rhsOffset = (rhs.position.x*(OGRE_REGION_SIDE_LENGTH*2+1)*(OGRE_REGION_SIDE_LENGTH*2+1)) + (rhs.position.y*(OGRE_REGION_SIDE_LENGTH*2+1)) + (rhs.position.z);

		return lhsOffset < rhsOffset;

		/*if(lhsOffset == rhsOffset)
		{
			return (lhs.alpha < rhs.alpha);
		}
		return (lhsOffset < rhsOffset)*/;
	}
}
