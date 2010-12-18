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

namespace PolyVox
{
	template <typename VertexType>
	class MeshDecimator
	{
	public:
		MeshDecimator(SurfaceMesh<VertexType>* pInputMesh/*, SurfaceMesh<PositionMaterial>* pMeshOutput*/);

		void execute();

	private:
		SurfaceMesh<VertexType>* m_pInputMesh;
		//SurfaceMesh<PositionMaterial>* pMeshOutput;

		void countNoOfNeighboursUsingMaterial(void);
		uint32_t performDecimationPass(float fMinDotProductForCollapse);
		bool isSubset(std::bitset<VF_NO_OF_FLAGS> a, std::bitset<VF_NO_OF_FLAGS> b);

		bool canCollapseEdge(uint32_t uSrc, uint32_t uDest);

		//Data structures used during decimation
		std::vector<uint8_t> m_vecNoOfNeighboursUsingMaterial;

		vector<bool> vertexLocked;
		vector<uint32_t> vertexMapper;

		vector< list<uint32_t> > trianglesUsingVertex;
		vector<int> noOfDifferentNormals;

		float fMinDotProductForCollapse;
	};
}

#include "MeshDecimator.inl"

#endif //__PolyVox_MeshDecimator_H__
