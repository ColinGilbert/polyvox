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

#include "MeshEdge.h"

#include "Mesh.h"
#include "MeshFace.h"
#include "MeshVertex.h"
#include "SurfaceVertex.h"

#include <set>

namespace PolyVox
{

	bool operator==(const MeshEdge& lhs, const MeshEdge& rhs)
	{
		return lhs.m_pSrc == rhs.m_pSrc && lhs.m_pDest == rhs.m_pDest;
	}

	bool operator<(const MeshEdge& lhs, const MeshEdge& rhs)
	{
		if(lhs == rhs)
			return false;

		if (lhs.m_pSrc < rhs.m_pSrc)
			return true;
		if (rhs.m_pSrc < lhs.m_pSrc)
			return false;

		if (lhs.m_pDest < rhs.m_pDest)
			return true;
		if (rhs.m_pDest < lhs.m_pDest)
			return false;

		return false;
	}

	MeshEdge::MeshEdge()
	{
		m_pOtherEdge = 0;
		m_pNextEdge = 0;
		m_pPreviousEdge = 0;

		m_pSrc = 0;
		m_pDest = 0;

		m_pFace = 0;

		m_fCost = 1000000.0f;
	}

	void MeshEdge::computeEdgeCost(Mesh* pParentMesh)
	{
		PositionMaterialNormal v0Data = m_pSrc->m_vertexData;
		PositionMaterialNormal v1Data = m_pDest->m_vertexData;

		m_fCost = 1000000.0f;

		//For now, don't collapse vertices on material edges...
		if(v0Data.isOnMaterialEdge() || v1Data.isOnMaterialEdge())
		{
			return;
		}

		//...or those on geometry (region) edges.
		if(v0Data.isOnGeometryEdge() || v1Data.isOnGeometryEdge())
		{
			return;
		}

		//Check the normals are within the threshold.
		if(v0Data.getNormal().dot(v1Data.getNormal()) < 0.999)
		{
			return;
		}

		//Test for face flips
		for(std::set<MeshFace*>::iterator faceIter = pParentMesh->m_faces.begin(); faceIter != pParentMesh->m_faces.end(); faceIter++)
		{
			if((*faceIter)->collapseFlipsFace(this))
			{
				return;
			}
		}

		m_fCost = 0.0f;
		return;
	}

	bool MeshEdge::isSane(void)
	{
		if(m_pSrc == 0)
		{
			std::cout << "SANITY CHECK FAIL: Edge has no source vertex" << std::endl;
			return false;
		}

		if(m_pDest == 0)
		{
			std::cout << "SANITY CHECK FAIL: Edge has no destination vertex" << std::endl;
			return false;
		}

		if(m_pFace == 0)
		{
			std::cout << "SANITY CHECK FAIL: Edge has no face attached" << std::endl;
			return false;
		}

		//Check loops
		if(this !=  m_pNextEdge->m_pNextEdge->m_pNextEdge)
		{
			std::cout << "SANITY CHECK FAIL: Next edge loop is broken" << std::endl;
			return false;
		}

		if(this !=  m_pPreviousEdge->m_pPreviousEdge->m_pPreviousEdge)
		{
			std::cout << "SANITY CHECK FAIL: Previous edge loop is broken" << std::endl;
			return false;
		}

		//Make sure the other edge points back here
		if(m_pOtherEdge != 0)
		{
			if(this !=  m_pOtherEdge->m_pOtherEdge)
			{
				std::cout << "SANITY CHECK FAIL: Other edge exists but does not point back here." << std::endl;
				return false;
			}
		}

		return true;
	}
}