/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*******************************************************************************/

#ifndef __PolyVox_Raycast_H__
#define __PolyVox_Raycast_H__

#include "PolyVoxCore/Vector.h"

namespace PolyVox
{
	/// Stores the result of a raycast operation.
	////////////////////////////////////////////////////////////////////////////////
	/// A instance of this structure is passed to a Raycast object, and is filled in
	/// as the ray traverses the volume. The 'foundIntersection' field indicates whether
	/// the ray hit any solid voxels, and if so the 'intersectionVoxel' field indicates
	///the voxel's position
	////////////////////////////////////////////////////////////////////////////////
	struct RaycastResult
	{
		///Indicates whether an intersection was found
		bool foundIntersection;
		///If an intersection was found then this field holds the intersecting voxel, otherwise it is undefined.
		Vector3DInt32 intersectionVoxel;
		Vector3DInt32 previousVoxel;
	};

	/// The Raycast class can be used to find the fist filled voxel along a given path.
	////////////////////////////////////////////////////////////////////////////////
	/// The principle behind raycasting is to fire a 'ray' through the volume and determine
	/// what (if anything) that ray hits. This simple test can be used for the purpose of
	/// picking, visibility checks, lighting calculations, or numerous other applications.
	///
	/// A ray is a stright line in space define by a start point and a direction vector.
	/// The length of the direction vector represents the length of the ray. When you call a
	/// Raycast object's execute() method it will iterate over each voxel which lies on the ray,
	/// starting from the defined start point. It will examine each voxel and terminate
	/// either when it encounters a solid voxel or when it reaches the end of the ray. If a
	/// solid voxel is encountered then its position is stored in the intersectionVoxel field
	/// of the RaycastResult structure and the intersectionFound flag is set to true, otherwise
	/// the intersectionFound flag is set to false.
	///
	/// **Important Note:** These has been confusion in the past with people not realising
	/// that the length of the direction vector is important. Most graphics API can provide
	/// a camera position and view direction for picking purposes, but the view direction is
	/// usually normalised (i.e. of length one). If you use this view direction directly you
	/// will only iterate over a single voxel and won't find what you are looking for. Instead
	/// you must scale the direction vector so that it's length represents the maximum distance
	/// over which you want the ray to be cast.
	///
	/// The following code snippet shows how the class is used:
	/// \code
	/// Vector3DFloat start(rayOrigin.x(), rayOrigin.y(), rayOrigin.z());
	/// Vector3DFloat direction(rayDir.x(), rayDir.y(), rayDir.z());
	/// direction.normalise();
	/// direction *= 1000.0f; //Casts ray of length 1000
	/// 
	/// RaycastResult raycastResult;
	/// Raycast<Material8> raycast(m_pPolyVoxVolume, start, direction, raycastResult);
	/// raycast.execute();
	/// 
	/// if(raycastResult.foundIntersection)
	/// {
	/// 	//...
	/// }
	/// \endcode
	///
	/// Some further notes, the Raycast uses full 26-connectivity, which basically means it 
	/// will examine every voxel the ray touches, even if it just passes through the corner.
	/// Also, it peforms a simple binary test against a voxel's threshold, rather than making
	/// use of it's density. Therefore it will work best in conjunction with one of the 'cubic'
	/// surace extractors. It's behaviour with the Marching Cubes surface extractor has not
	/// been tested yet.
	////////////////////////////////////////////////////////////////////////////////
	template<typename VolumeType>
	class Raycast
	{
	public:
		///Constructor
		Raycast(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirectionAndLength, RaycastResult& result, polyvox_function<bool(const typename VolumeType::Sampler& sampler)> funcIsPassable);

		///Sets the start position for the ray.
		void setStart(const Vector3DFloat& v3dStart);
		///Set the direction for the ray.
		void setDirection(const Vector3DFloat& v3dDirectionAndLength);

		///Performs the raycast.
		void execute();

	private:
		RaycastResult& m_result;

		polyvox_function<bool(const typename VolumeType::Sampler& position)> m_funcIsPassable;

		void doRaycast(float x1, float y1, float z1, float x2, float y2, float z2);

		VolumeType* m_volData;
		typename VolumeType::Sampler m_sampVolume;

		Vector3DFloat m_v3dStart;
		Vector3DFloat m_v3dDirectionAndLength;
		float m_fMaxDistance;
	};

	template<typename VolumeType, typename Callback>
	void raycast(VolumeType* volData, /*const*/ Vector3DFloat/*&*/ v3dStart, const Vector3DFloat& v3dDirectionAndLength, Callback& callback)
	{
		VolumeType::Sampler sampler(volData);

		//The doRaycast function is assuming that it is iterating over the areas defined between
		//voxels. We actually want to define the areas as being centered on voxels (as this is
		//what the CubicSurfaceExtractor generates). We add (0.5,0.5,0.5) here to adjust for this.
		v3dStart = v3dStart + Vector3DFloat(0.5f, 0.5f, 0.5f);

		//Compute the end point
		Vector3DFloat v3dEnd = v3dStart + v3dDirectionAndLength;

		float x1 = v3dStart.getX();
		float y1 = v3dStart.getY();
		float z1 = v3dStart.getZ();
		float x2 = v3dEnd.getX();
		float y2 = v3dEnd.getY();
		float z2 = v3dEnd.getZ();
		
		int i = (int)floorf(x1);
		int j = (int)floorf(y1);
		int k = (int)floorf(z1);

		int iend = (int)floorf(x2);
		int jend = (int)floorf(y2);
		int kend = (int)floorf(z2);

		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));
		int dk = ((z1 < z2) ? 1 : ((z1 > z2) ? -1 : 0));

		float deltatx = 1.0f / std::abs(x2 - x1);
		float deltaty = 1.0f / std::abs(y2 - y1);
		float deltatz = 1.0f / std::abs(z2 - z1);

		float minx = floorf(x1), maxx = minx + 1.0f;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1)) * deltatx;
		float miny = floorf(y1), maxy = miny + 1.0f;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1)) * deltaty;
		float minz = floorf(z1), maxz = minz + 1.0f;
		float tz = ((z1 > z2) ? (z1 - minz) : (maxz - z1)) * deltatz;

		sampler.setPosition(i,j,k);
		//m_result.previousVoxel = Vector3DInt32(i,j,k);

		for(;;)
		{
			if(!callback(sampler))
			{
				//m_result.foundIntersection = true;
				//m_result.intersectionVoxel = Vector3DInt32(i,j,k);
				return;
			}
			//m_result.previousVoxel = Vector3DInt32(i,j,k);

			if(tx <= ty && tx <= tz)
			{
				if(i == iend) break;
				tx += deltatx;
				i += di;

				if(di == 1) sampler.movePositiveX();
				if(di == -1) sampler.moveNegativeX();
			} else if (ty <= tz)
			{
				if(j == jend) break;
				ty += deltaty;
				j += dj;

				if(dj == 1) sampler.movePositiveY();
				if(dj == -1) sampler.moveNegativeY();
			} else 
			{
				if(k == kend) break;
				tz += deltatz;
				k += dk;

				if(dk == 1) sampler.movePositiveZ();
				if(dk == -1) sampler.moveNegativeZ();
			}
		}
	}
}

#include "PolyVoxCore/Raycast.inl"

#endif //__PolyVox_Raycast_H__
