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

#ifndef __PolyVox_MeshDecimator_H__
#define __PolyVox_MeshDecimator_H__

#include <bitset>

namespace PolyVox
{
	template <typename VertexType>
	class MeshDecimator
	{
		//Used to keep track of when a vertex is
		//on one or more faces of  the region
		enum POLYVOXCORE_API RegionFaceFlags
		{
			RFF_ON_REGION_FACE_NEG_X,
			RFF_ON_REGION_FACE_POS_X ,
			RFF_ON_REGION_FACE_NEG_Y ,
			RFF_ON_REGION_FACE_POS_Y ,
			RFF_ON_REGION_FACE_NEG_Z ,
			RFF_ON_REGION_FACE_POS_Z,
			RFF_NO_OF_REGION_FACE_FLAGS
		};

		//Data about the initial mesh - this
		//will be fill in once at the start
		struct InitialVertexMetadata
		{
			Vector3DFloat normal;
			bool isOnMaterialEdge;
			std::bitset<RFF_NO_OF_REGION_FACE_FLAGS> isOnRegionFace;
		};

		//Representing a triangle for decimation purposes.
		struct Triangle
		{
			uint32_t v0;
			uint32_t v1;
			uint32_t v2;
			Vector3DFloat normal;
		};
	public:
		MeshDecimator(const SurfaceMesh<VertexType>* pInputMesh, SurfaceMesh<VertexType>* pOutputMesh, float fEdgeCollapseThreshold = 0.95f);

		void execute();

	private:

		void fillInitialVertexMetadata(std::vector<InitialVertexMetadata>& vecInitialVertexMetadata);

		void buildConnectivityData(void);

		bool attemptEdgeCollapse(uint32_t uSrc, uint32_t uDest);

		const SurfaceMesh<VertexType>* m_pInputMesh;
		SurfaceMesh<VertexType>* m_pOutputMesh;

		uint32_t performDecimationPass(float m_fMinDotProductForCollapse);
		bool isSubset(std::bitset<RFF_NO_OF_REGION_FACE_FLAGS> a, std::bitset<RFF_NO_OF_REGION_FACE_FLAGS> b);

		bool canCollapseEdge(uint32_t uSrc, uint32_t uDest);
		bool canCollapseNormalEdge(uint32_t uSrc, uint32_t uDst);
		bool canCollapseRegionEdge(uint32_t uSrc, uint32_t uDst);
		bool canCollapseMaterialEdge(uint32_t uSrc, uint32_t uDst);
		bool collapseChangesFaceNormals(uint32_t uSrc, uint32_t uDst, float fThreshold);

		//Data structures used during decimation

		std::vector<bool> vertexLocked;
		std::vector<uint32_t> vertexMapper;

		std::vector<Triangle> m_vecTriangles;
		std::vector< list<uint32_t> > trianglesUsingVertex; //Should probably use vector of vectors, and resise in advance.

		std::vector<InitialVertexMetadata> m_vecInitialVertexMetadata;

		float m_fMinDotProductForCollapse;
	};
}

#include "MeshDecimator.inl"

#endif //__PolyVox_MeshDecimator_H__
