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

#ifndef __PolyVox_AStarPathfinder_H__
#define __PolyVox_AStarPathfinder_H__

#include "Array.h"
#include "PolyVoxImpl/AStarPathfinderImpl.h"
#include "PolyVoxForwardDeclarations.h"
#include "Volume.h"
#include "VolumeSampler.h"

#include "PolyVoxImpl/TypeDef.h"

namespace PolyVox
{
	const float sqrt_1 = 1.0f;
	const float sqrt_2 = 1.4143f;
	const float sqrt_3 = 1.7321f;

	extern const POLYVOXCORE_API Vector3DInt16 arrayPathfinderFaces[6];
	extern const POLYVOXCORE_API Vector3DInt16 arrayPathfinderEdges[12];
	extern const POLYVOXCORE_API Vector3DInt16 arrayPathfinderCorners[8];

	template <typename VoxelType>
	bool aStarDefaultVoxelValidator(const Volume<VoxelType>* volData, const Vector3DInt16& v3dPos);

	template <typename VoxelType>
	struct AStarPathfinderParams
	{
	public:
		AStarPathfinderParams
		(
			Volume<VoxelType>* volData,
			const Vector3DInt16& v3dStart,
			const Vector3DInt16& v3dEnd,
			std::list<Vector3DInt16>* listResult,
			float fHBias = 1.0,
			uint32_t uMaxNoOfNodes = 10000,
			Connectivity connectivity = TwentySixConnected,
			polyvox_function<bool (const Volume<VoxelType>*, const Vector3DInt16&)> funcIsVoxelValidForPath = &aStarDefaultVoxelValidator<VoxelType>,
			polyvox_function<void (float)> funcProgressCallback = 0
		)
			:volume(volData)
			,start(v3dStart)
			,end(v3dEnd)
			,result(listResult)
			,hBias(fHBias)
			,connectivity(connectivity)
			,isVoxelValidForPath(funcIsVoxelValidForPath)
			,maxNumberOfNodes(uMaxNoOfNodes)
			,progressCallback(funcProgressCallback)
		{
		}

		//The volume data.
		Volume<VoxelType>* volume;

		Vector3DInt16 start;
		Vector3DInt16 end;

		//The resulting path
		std::list<Vector3DInt16>* result;

		//The requied connectivity
		Connectivity connectivity;

		//Bias applied to h()
		float hBias;

		//Max number of nodes to examine
		uint32_t maxNumberOfNodes;

		//Used to determine whether a given voxel is valid.
		polyvox_function<bool (const Volume<VoxelType>*, const Vector3DInt16&)> isVoxelValidForPath;

		//Progress callback
		polyvox_function<void (float)> progressCallback;
	};

	template <typename VoxelType>
	class AStarPathfinder
	{
	public:
		AStarPathfinder(const AStarPathfinderParams<VoxelType>& params);

		void execute();

	private:
		void processNeighbour(const Vector3DInt16& neighbourPos, float neighbourGVal);

		float SixConnectedCost(const Vector3DInt16& a, const Vector3DInt16& b);
		float EighteenConnectedCost(const Vector3DInt16& a, const Vector3DInt16& b);
		float TwentySixConnectedCost(const Vector3DInt16& a, const Vector3DInt16& b);
		float computeH(const Vector3DInt16& a, const Vector3DInt16& b);		

		//Node containers
		AllNodesContainer allNodes;
		OpenNodesContainer openNodes;
		ClosedNodesContainer closedNodes;

		//The current node
		AllNodesContainer::iterator current;
		
		float m_fProgress;

		AStarPathfinderParams<VoxelType> m_params;
	};
}

#include "AStarPathfinder.inl"

#endif //__PolyVox_AStarPathfinder_H__
