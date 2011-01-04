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

namespace PolyVox
{
	template <typename VertexType>
	MeshDecimator<VertexType>::MeshDecimator(SurfaceMesh<VertexType>* pInputMesh/*, SurfaceMesh<PositionMaterial>* pMeshOutput*/)
		:m_pInputMesh(pInputMesh)
		//,m_pOutputMesh(pOutputMesh)
	{
		fMinDotProductForCollapse = 0.999;
	}

	template <typename VertexType>
	void MeshDecimator<VertexType>::execute()
	{
		buildConnectivityData();
		fillInitialVertexMetadata(m_vecInitialVertexMetadata);

		uint32_t noOfEdgesCollapsed;
		do
		{
			noOfEdgesCollapsed = performDecimationPass(fMinDotProductForCollapse);
			m_pInputMesh->removeDegenerateTris();	
			if(noOfEdgesCollapsed > 0)
			{
				//Build the connectivity data for the next pass. If this is slow, then look
				//at adjusting it (based on vertex mapper?) rather than bulding from scratch.
				buildConnectivityData();
			}
			//m_pInputMesh->removeUnusedVertices();
		}while(noOfEdgesCollapsed > 0);

		//Decimation will have invalidated LOD levels.
		m_pInputMesh->m_vecLodRecords.clear();
		LodRecord lodRecord;
		lodRecord.beginIndex = 0;
		lodRecord.endIndex = m_pInputMesh->getNoOfIndices();
		m_pInputMesh->m_vecLodRecords.push_back(lodRecord);
	}

	template <typename VertexType>
	void MeshDecimator<VertexType>::buildConnectivityData(void)
	{
		m_vecTriangles.clear();
		m_vecTriangles.resize(m_pInputMesh->m_vecTriangleIndices.size() / 3);
		for(int triCt = 0; triCt < m_vecTriangles.size(); triCt++)
		{
			m_vecTriangles[triCt].v0 = m_pInputMesh->m_vecTriangleIndices[triCt * 3 + 0];
			m_vecTriangles[triCt].v1 = m_pInputMesh->m_vecTriangleIndices[triCt * 3 + 1];
			m_vecTriangles[triCt].v2 = m_pInputMesh->m_vecTriangleIndices[triCt * 3 + 2];

			Vector3DFloat v0Pos = m_pInputMesh->m_vecVertices[m_vecTriangles[triCt].v0].position;
			Vector3DFloat v1Pos = m_pInputMesh->m_vecVertices[m_vecTriangles[triCt].v1].position;
			Vector3DFloat v2Pos = m_pInputMesh->m_vecVertices[m_vecTriangles[triCt].v2].position;

			Vector3DFloat v0v1 = v1Pos - v0Pos;
			Vector3DFloat v0v2 = v2Pos - v0Pos;
			Vector3DFloat normal = v0v1.cross(v0v2);
			normal.normalise();

			m_vecTriangles[triCt].normal = normal;
		}

		trianglesUsingVertex.clear();
		trianglesUsingVertex.resize(m_pInputMesh->m_vecVertices.size());
		for(int ct = 0; ct < m_vecTriangles.size(); ct++)
		{
			trianglesUsingVertex[m_vecTriangles[ct].v0].push_back(ct);
			trianglesUsingVertex[m_vecTriangles[ct].v1].push_back(ct);
			trianglesUsingVertex[m_vecTriangles[ct].v2].push_back(ct);
		}
	}

	void MeshDecimator<PositionMaterial>::fillInitialVertexMetadata(std::vector<InitialVertexMetadata>& vecVertexMetadata)
	{
		vecVertexMetadata.clear();
		vecVertexMetadata.resize(m_pInputMesh->m_vecVertices.size());
		//Initialise the metadata
		for(int ct = 0; ct < vecVertexMetadata.size(); ct++)
		{
			vecVertexMetadata[ct].normal.setElements(0,0,0);
			vecVertexMetadata[ct].isOnMaterialEdge = false;
			vecVertexMetadata[ct].vertexFlags.reset();
		}

		for(int outerCt = 0; outerCt < m_pInputMesh->m_vecVertices.size()-1; outerCt++)
		{
			for(int innerCt = outerCt+1; innerCt < m_pInputMesh->m_vecVertices.size(); innerCt++)
			{
				if((m_pInputMesh->m_vecVertices[innerCt].position - m_pInputMesh->m_vecVertices[outerCt].position).lengthSquared() < 0.001f)
				{							
					vecVertexMetadata[innerCt].isOnMaterialEdge = true;
					vecVertexMetadata[outerCt].isOnMaterialEdge = true;
				}
			}
		}

		for(int ct = 0; ct < m_pInputMesh->m_vecVertices.size(); ct++)
		{
			Vector3DFloat sumOfNormals(0.0f,0.0f,0.0f);
			for(list<uint32_t>::const_iterator iter = trianglesUsingVertex[ct].cbegin(); iter != trianglesUsingVertex[ct].cend(); iter++)
			{
				sumOfNormals += m_vecTriangles[*iter].normal;
			}

			vecVertexMetadata[ct].normal = sumOfNormals;
			vecVertexMetadata[ct].normal.normalise();
		}

		for(int ct = 0; ct < vecVertexMetadata.size(); ct++)
		{
			Region regTransformed = m_pInputMesh->m_Region;
			regTransformed.shift(regTransformed.getLowerCorner() * static_cast<int16_t>(-1));

			//Plus and minus X
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_NEG_X, m_pInputMesh->m_vecVertices[ct].getPosition().getX() < regTransformed.getLowerCorner().getX() + 0.001f);
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_POS_X, m_pInputMesh->m_vecVertices[ct].getPosition().getX() > regTransformed.getUpperCorner().getX() - 0.001f);
			//Plus and minus Y
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_NEG_Y, m_pInputMesh->m_vecVertices[ct].getPosition().getY() < regTransformed.getLowerCorner().getY() + 0.001f);
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_POS_Y, m_pInputMesh->m_vecVertices[ct].getPosition().getY() > regTransformed.getUpperCorner().getY() - 0.001f);
			//Plus and minus Z
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_NEG_Z, m_pInputMesh->m_vecVertices[ct].getPosition().getZ() < regTransformed.getLowerCorner().getZ() + 0.001f);
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_POS_Z, m_pInputMesh->m_vecVertices[ct].getPosition().getZ() > regTransformed.getUpperCorner().getZ() - 0.001f);
		}
	}

	void MeshDecimator<PositionMaterialNormal>::fillInitialVertexMetadata(std::vector<InitialVertexMetadata>& vecVertexMetadata)
	{
		vecVertexMetadata.clear();
		vecVertexMetadata.resize(m_pInputMesh->m_vecVertices.size());

		//Initialise the metadata
		for(int ct = 0; ct < vecVertexMetadata.size(); ct++)
		{			
			vecVertexMetadata[ct].vertexFlags.reset();
			vecVertexMetadata[ct].isOnMaterialEdge = false;
			vecVertexMetadata[ct].normal = m_pInputMesh->m_vecVertices[ct].normal;
		}

		for(int ct = 0; ct < vecVertexMetadata.size(); ct++)
		{
			Region regTransformed = m_pInputMesh->m_Region;
			regTransformed.shift(regTransformed.getLowerCorner() * static_cast<int16_t>(-1));

			//Plus and minus X
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_NEG_X, m_pInputMesh->m_vecVertices[ct].getPosition().getX() < regTransformed.getLowerCorner().getX() + 0.001f);
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_POS_X, m_pInputMesh->m_vecVertices[ct].getPosition().getX() > regTransformed.getUpperCorner().getX() - 0.001f);
			//Plus and minus Y
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_NEG_Y, m_pInputMesh->m_vecVertices[ct].getPosition().getY() < regTransformed.getLowerCorner().getY() + 0.001f);
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_POS_Y, m_pInputMesh->m_vecVertices[ct].getPosition().getY() > regTransformed.getUpperCorner().getY() - 0.001f);
			//Plus and minus Z
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_NEG_Z, m_pInputMesh->m_vecVertices[ct].getPosition().getZ() < regTransformed.getLowerCorner().getZ() + 0.001f);
			vecVertexMetadata[ct].vertexFlags.set(VF_ON_GEOMETRY_EDGE_POS_Z, m_pInputMesh->m_vecVertices[ct].getPosition().getZ() > regTransformed.getUpperCorner().getZ() - 0.001f);
		}

		//If all three vertices have the same material then we are not on a material edge. If any vertex has a different
		//material then all three vertices are on a material edge. E.g. If one vertex has material 'a' and the other two 
		//have material 'b', then the two 'b's are still on an edge (with 'a') even though they are the same as eachother.
		for(int ct = 0; ct < m_vecTriangles.size(); ct++)
		{
			uint32_t v0 = m_vecTriangles[ct].v0;
			uint32_t v1 = m_vecTriangles[ct].v1;
			uint32_t v2 = m_vecTriangles[ct].v2;

			bool allMatch = 
				(m_pInputMesh->m_vecVertices[v0].material == m_pInputMesh->m_vecVertices[v1].material) && 
				(m_pInputMesh->m_vecVertices[v1].material == m_pInputMesh->m_vecVertices[v2].material);

			if(!allMatch)
			{
				vecVertexMetadata[v0].isOnMaterialEdge = true;
				vecVertexMetadata[v1].isOnMaterialEdge = true;
				vecVertexMetadata[v2].isOnMaterialEdge = true;
			}
		}
	}

	template <typename VertexType>
	uint32_t MeshDecimator<VertexType>::performDecimationPass(float fMinDotProductForCollapse)
	{
		// Count how many edges we have collapsed
		uint32_t noOfEdgesCollapsed = 0;

		// The vertex mapper track whick vertices collapse onto which.
		vertexMapper.clear();
		vertexMapper.resize(m_pInputMesh->m_vecVertices.size());

		// Once a vertex is involved in a collapse (either because it
		// moves onto a different vertex, or because a different vertex
		// moves onto it) it is forbidden to take part in another collapse
		// this pass. We enforce this by setting the vertex locked flag.
		vertexLocked.clear();
		vertexLocked.resize(m_pInputMesh->m_vecVertices.size());

		// Initialise the vectors
		for(uint32_t ct = 0; ct < m_pInputMesh->m_vecVertices.size(); ct++)
		{
			// Initiall all vertices points to themselves
			vertexMapper[ct] = ct;
			// All vertices are initially unlocked
			vertexLocked[ct] = false;
		}

		//For each triangle...
		for(int ctIter = 0; ctIter < m_vecTriangles.size(); ctIter++)
		{
			if(attemptEdgeCollapse(m_vecTriangles[ctIter].v0, m_vecTriangles[ctIter].v1))
			{
				++noOfEdgesCollapsed;
			}

			if(attemptEdgeCollapse(m_vecTriangles[ctIter].v1, m_vecTriangles[ctIter].v2))
			{
				++noOfEdgesCollapsed;
			}

			if(attemptEdgeCollapse(m_vecTriangles[ctIter].v2, m_vecTriangles[ctIter].v0))
			{
				++noOfEdgesCollapsed;
			}
		}

		if(noOfEdgesCollapsed > 0)
		{
			//Fix up the indices
			for(int triCt = 0; triCt < m_pInputMesh->m_vecTriangleIndices.size(); triCt++)
			{
				uint32_t before = m_pInputMesh->m_vecTriangleIndices[triCt];
				uint32_t after = vertexMapper[m_pInputMesh->m_vecTriangleIndices[triCt]];
				if(before != after)
				{
					m_pInputMesh->m_vecTriangleIndices[triCt] = vertexMapper[m_pInputMesh->m_vecTriangleIndices[triCt]];
				}
			}
		}

		return noOfEdgesCollapsed;
	}

	template <typename VertexType>
	bool MeshDecimator<VertexType>::attemptEdgeCollapse(uint32_t uSrc, uint32_t uDst)
	{
		//A vertex will be locked if it has already been involved in a collapse this pass.
		if(vertexLocked[uSrc] || vertexLocked[uDst])
		{
			return false;
		}

		if(canCollapseEdge(uSrc, uDst))
		{
			//Move v0 onto v1
			vertexMapper[uSrc] = uDst; //vertexMapper[v1];
			vertexLocked[uSrc] = true;
			vertexLocked[uDst] = true;

			//Increment the counter
			return true;
		}
		
		return false;
	}

	template <typename VertexType>
	bool MeshDecimator<VertexType>::canCollapseEdge(uint32_t uSrc, uint32_t uDst)
	{
		bool bCanCollapse = true;
		
		if(m_vecInitialVertexMetadata[uSrc].isOnMaterialEdge)
		{
			bCanCollapse &= canCollapseMaterialEdge(uSrc, uDst);
		}

		if(m_vecInitialVertexMetadata[uSrc].vertexFlags.any())
		{
			bCanCollapse &= canCollapseRegionEdge(uSrc, uDst);
		}

		if(bCanCollapse) //Only bother with this if the earlier tests passed.
		{
			bCanCollapse &= canCollapseNormalEdge(uSrc, uDst);
		}

		return bCanCollapse;
	}

	template<> 
	bool MeshDecimator<PositionMaterialNormal>::canCollapseNormalEdge(uint32_t uSrc, uint32_t uDst)
	{
		return !collapseChangesFaceNormals(uSrc, uDst, fMinDotProductForCollapse);
	}

	template<> 
	bool MeshDecimator<PositionMaterial>::canCollapseNormalEdge(uint32_t uSrc, uint32_t uDst)
	{
		return !collapseChangesFaceNormals(uSrc, uDst, 0.999f);
	}

	template <typename VertexType>
	bool MeshDecimator<VertexType>::canCollapseRegionEdge(uint32_t uSrc, uint32_t uDst)
	{		
		// We can collapse normal vertices onto edge vertices, and edge vertices
		// onto corner vertices, but not vice-versa. Hence we check whether all
		// the edge flags in the source vertex are also set in the destination vertex.
		if(isSubset(m_vecInitialVertexMetadata[uSrc].vertexFlags, m_vecInitialVertexMetadata[uDst].vertexFlags) == false)
		{
			return false;
		}

		// In general adjacent regions surface meshes may collapse differently
		// and this can cause cracks. We solve this by only allowing the collapse
		// is the normals are exactly the same. We do not use the user provided
		// tolerence here (but do allow for floating point error).
		if(m_vecInitialVertexMetadata[uSrc].normal.dot(m_vecInitialVertexMetadata[uDst].normal) < 0.999f)
		{
			return false;
		}

		return true;
	}

	template <typename VertexType>
	bool MeshDecimator<VertexType>::canCollapseMaterialEdge(uint32_t uSrc, uint32_t uDst)
	{
		return false;
	}

	template <typename VertexType>
	bool MeshDecimator<VertexType>::collapseChangesFaceNormals(uint32_t uSrc, uint32_t uDst, float fThreshold)
	{
		bool faceFlipped = false;
		list<uint32_t> triangles = trianglesUsingVertex[uSrc];

		for(list<uint32_t>::iterator triIter = triangles.begin(); triIter != triangles.end(); triIter++)
		{
			uint32_t tri = *triIter;
					
			uint32_t v0Old = m_pInputMesh->m_vecTriangleIndices[tri * 3];
			uint32_t v1Old = m_pInputMesh->m_vecTriangleIndices[tri * 3 + 1];
			uint32_t v2Old = m_pInputMesh->m_vecTriangleIndices[tri * 3 + 2];

			//Check if degenerate
			if((v0Old == v1Old) || (v1Old == v2Old) || (v2Old == v0Old))
			{
				continue;
			}

			uint32_t v0New = v0Old;
			uint32_t v1New = v1Old;
			uint32_t v2New = v2Old;

			if(v0New == uSrc)
				v0New = uDst;
			if(v1New == uSrc)
				v1New = uDst;
			if(v2New == uSrc)
				v2New = uDst;

			//Check if degenerate
			if((v0New == v1New) || (v1New == v2New) || (v2New == v0New))
			{
				continue;
			}

			Vector3DFloat v0OldPos = m_pInputMesh->m_vecVertices[vertexMapper[v0Old]].getPosition(); //Note: we need the vertex mapper here. These neighbouring vertices may have been moved.
			Vector3DFloat v1OldPos = m_pInputMesh->m_vecVertices[vertexMapper[v1Old]].getPosition();
			Vector3DFloat v2OldPos = m_pInputMesh->m_vecVertices[vertexMapper[v2Old]].getPosition();

			Vector3DFloat v0NewPos = m_pInputMesh->m_vecVertices[vertexMapper[v0New]].getPosition();
			Vector3DFloat v1NewPos = m_pInputMesh->m_vecVertices[vertexMapper[v1New]].getPosition();
			Vector3DFloat v2NewPos = m_pInputMesh->m_vecVertices[vertexMapper[v2New]].getPosition();

			/*Vector3DFloat v0OldPos = m_vecVertices[v0Old].getPosition();
			Vector3DFloat v1OldPos = m_vecVertices[v1Old].getPosition();
			Vector3DFloat v2OldPos = m_vecVertices[v2Old].getPosition();

			Vector3DFloat v0NewPos = m_vecVertices[v0New].getPosition();
			Vector3DFloat v1NewPos = m_vecVertices[v1New].getPosition();
			Vector3DFloat v2NewPos = m_vecVertices[v2New].getPosition();*/

			Vector3DFloat OldNormal = (v1OldPos - v0OldPos).cross(v2OldPos - v1OldPos);
			Vector3DFloat NewNormal = (v1NewPos - v0NewPos).cross(v2NewPos - v1NewPos);

			OldNormal.normalise();
			NewNormal.normalise();

			// Note for after holiday - We are still getting faces flipping despite the following test. I tried changing
			// the 0.0 to 0.9 (which should still let coplanar faces merge) but oddly nothing then merged. Investigate this.
			float dotProduct = OldNormal.dot(NewNormal);
			//cout << dotProduct << endl;
			if(dotProduct < fThreshold)
			{
				//cout << "   Face flipped!!" << endl;

				faceFlipped = true;

				/*vertexLocked[v0] = true;
				vertexLocked[v1] = true;*/

				break;
			}
		}

		return faceFlipped;
	}

	// Returns true if every bit which is set in 'a' is also set in 'b'. The reverse does not need to be true.
	template <typename VertexType>
	bool MeshDecimator<VertexType>::isSubset(std::bitset<VF_NO_OF_FLAGS> a, std::bitset<VF_NO_OF_FLAGS> b)
	{
		bool result = true;

		for(int ct = 0; ct < VF_NO_OF_FLAGS; ct++)
		{
			if(a.test(ct))
			{
				if(b.test(ct) == false)
				{
					result = false;
					break;
				}
			}
		}

		return result;
	}
}