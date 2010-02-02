#pragma region License
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
#pragma endregion

#include "MeshFace.h"

#include "MeshEdge.h"
#include "MeshVertex.h"

#include "Vector.h"

#include <iostream>

namespace PolyVox
{
	MeshFace::MeshFace()
	{
		m_pEdge = 0;
	}

	bool MeshFace::isSane(void)
	{
		if(m_pEdge == 0)
		{
			std::cout << "SANITY CHECK FAIL: Face has no edge attached" << std::endl;
			return false;
		}

		return true;
	}

	Vector3DFloat MeshFace::getNormal(void)
	{
		//Find the three vertices
		Vector3DFloat pV0 = m_pEdge->m_pSrc->m_vertexData.getPosition();
		Vector3DFloat pV1 = m_pEdge->m_pNextEdge->m_pSrc->m_vertexData.getPosition();
		Vector3DFloat pV2 = m_pEdge->m_pNextEdge->m_pNextEdge->m_pSrc->m_vertexData.getPosition();

		Vector3DFloat crossProduct = (pV1 - pV0).cross(pV2 - pV0);
		
		assert(crossProduct.length() > 0.001);

		crossProduct.normalise();

		return crossProduct;
	}

	bool MeshFace::collapseFlipsFace(MeshEdge* pEdgeToCollapse)
	{
		MeshVertex* pSrc = pEdgeToCollapse->m_pSrc;
		MeshVertex* pDest = pEdgeToCollapse->m_pDest;

		MeshVertex* pV0 = m_pEdge->m_pSrc;
		MeshVertex* pV1 = m_pEdge->m_pNextEdge->m_pSrc;
		MeshVertex* pV2 = m_pEdge->m_pNextEdge->m_pNextEdge->m_pSrc;

		if(pSrc == pV0)
		{
			return collapseFlipsFaceImpl(pV1->m_vertexData.getPosition(), pV2->m_vertexData.getPosition(), pSrc->m_vertexData.getPosition(), pDest->m_vertexData.getPosition());
		}
		if(pSrc == pV1)
		{
			return collapseFlipsFaceImpl(pV0->m_vertexData.getPosition(), pV2->m_vertexData.getPosition(), pSrc->m_vertexData.getPosition(), pDest->m_vertexData.getPosition());
		}
		if(pSrc == pV2)
		{
			return collapseFlipsFaceImpl(pV0->m_vertexData.getPosition(), pV1->m_vertexData.getPosition(), pSrc->m_vertexData.getPosition(), pDest->m_vertexData.getPosition());
		}

		return false;
	}

	bool MeshFace::collapseFlipsFaceImpl(Vector3DFloat fixed0, Vector3DFloat fixed1, Vector3DFloat oldPos, Vector3DFloat newPos)
	{
		Vector3DFloat oldCrossProduct = (fixed0 - oldPos).cross(fixed1 - oldPos);
		Vector3DFloat newCrossProduct = (fixed0 - newPos).cross(fixed1 - newPos);

		if(oldCrossProduct.length() < 0.001f)
		{
			return false;
		}

		if(newCrossProduct.length() < 0.001f)
		{
			return false;
		}

		oldCrossProduct.normalise();
		newCrossProduct.normalise();

		return oldCrossProduct.dot(newCrossProduct) < 0.999f;
	}
}