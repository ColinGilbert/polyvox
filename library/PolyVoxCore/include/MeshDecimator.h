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
	enum POLYVOXCORE_API NormalFlags
	{
		NF_NORMAL_NEG_X,
		NF_NORMAL_POS_X,
		NF_NORMAL_NEG_Y,
		NF_NORMAL_POS_Y,
		NF_NORMAL_NEG_Z,
		NF_NORMAL_POS_Z,
		NF_NO_OF_FLAGS
	};

	struct VertexMetadata
	{
		bool hasDuplicate;
		uint64_t materialKey;
		list<uint32_t> trianglesUsingVertex;
		int noOfDifferentNormals;
		Vector3DFloat normal;
		std::bitset<NF_NO_OF_FLAGS> m_bNormalFlags;
		bool isOnRegionEdge;
		bool isOnMaterialEdge;
	};

	template <typename VertexType>
	class MeshDecimator
	{
	public:
		MeshDecimator(SurfaceMesh<VertexType>* pInputMesh/*, SurfaceMesh<PositionMaterial>* pMeshOutput*/);

		void execute();

	private:

		void fillVertexMetadata(std::vector<VertexMetadata>& vecVertexMetadata);

		SurfaceMesh<VertexType>* m_pInputMesh;
		//SurfaceMesh<PositionMaterial>* pMeshOutput;

		void countNoOfNeighboursUsingMaterial(void);
		uint32_t performDecimationPass(float fMinDotProductForCollapse);
		bool isSubset(std::bitset<VF_NO_OF_FLAGS> a, std::bitset<VF_NO_OF_FLAGS> b);
		bool isSubsetCubic(std::bitset<NF_NO_OF_FLAGS> a, std::bitset<NF_NO_OF_FLAGS> b);

		bool canCollapseEdge(uint32_t uSrc, uint32_t uDest);
		bool canCollapseNormalEdge(uint32_t uSrc, uint32_t uDest);
		bool canCollapseRegionEdge(uint32_t uSrc, uint32_t uDest);
		bool canCollapseMaterialEdge(uint32_t uSrc, uint32_t uDest);
		bool collapseCausesFaceFlip(uint32_t uSrc, uint32_t uDest);

		//Data structures used during decimation
		std::vector<uint8_t> m_vecNoOfNeighboursUsingMaterial;

		vector<bool> vertexLocked;
		vector<uint32_t> vertexMapper;

		//vector< list<uint32_t> > trianglesUsingVertexCurrently;

		vector<int> vecOfTriCts;
		vector<Vector3DFloat> vecOfTriNormals;

		//vector<int> noOfDifferentNormals;

		vector<VertexMetadata> m_vecInitialVertexMetadata;
		vector<VertexMetadata> m_vecCurrentVertexMetadata;

		float fMinDotProductForCollapse;
	};
}

#include "MeshDecimator.inl"

#endif //__PolyVox_MeshDecimator_H__
