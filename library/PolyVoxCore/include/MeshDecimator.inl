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
		// We will need the information from this function to
		// determine when material boundary edges can collapse.
		countNoOfNeighboursUsingMaterial();

		buildTriangles();
		fillInitialVertexMetadata(m_vecInitialVertexMetadata);

		uint32_t noOfEdgesCollapsed;
		do
		{
			noOfEdgesCollapsed = performDecimationPass(fMinDotProductForCollapse);
			m_pInputMesh->removeDegenerateTris();	
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
	void MeshDecimator<VertexType>::buildTriangles(void)
	{
		// Each triangle exists in this vector once.
		vecOfTriCts.clear();
		vecOfTriCts.resize(m_pInputMesh->m_vecTriangleIndices.size() / 3);
		for(int triCt = 0; triCt < vecOfTriCts.size(); triCt++)
		{
			vecOfTriCts[triCt] = triCt;
		}

		vecOfTriNormals.clear();
		vecOfTriNormals.resize(vecOfTriCts.size());
		for(int ct = 0; ct < vecOfTriCts.size(); ct++)
		{
			int triCt = vecOfTriCts[ct];
			int v0 = m_pInputMesh->m_vecTriangleIndices[triCt * 3 + 0];
			int v1 = m_pInputMesh->m_vecTriangleIndices[triCt * 3 + 1];
			int v2 = m_pInputMesh->m_vecTriangleIndices[triCt * 3 + 2];

			//Handle degenerates?

			Vector3DFloat v0v1 = m_pInputMesh->m_vecVertices[v1].position - m_pInputMesh->m_vecVertices[v0].position;
			Vector3DFloat v0v2 = m_pInputMesh->m_vecVertices[v2].position - m_pInputMesh->m_vecVertices[v0].position;
			Vector3DFloat normal = v0v1.cross(v0v2);

			normal.normalise();

			vecOfTriNormals[ct] = normal;
		}

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
	}

	template <typename VertexType>
	void MeshDecimator<VertexType>::fillInitialVertexMetadata(std::vector<InitialVertexMetadata>& vecVertexMetadata)
	{
		vecVertexMetadata.clear();
		vecVertexMetadata.resize(m_pInputMesh->m_vecVertices.size());
		//Initialise the metadata
		for(int ct = 0; ct < vecVertexMetadata.size(); ct++)
		{
			vecVertexMetadata[ct].trianglesUsingVertex.clear();
			vecVertexMetadata[ct].normal.setElements(0,0,0);
			vecVertexMetadata[ct].isOnRegionEdge = false;
			vecVertexMetadata[ct].isOnMaterialEdge = false;
		}
		
		for(int ct = 0; ct < m_vecTriangles.size(); ct++)
		{
			vecVertexMetadata[m_vecTriangles[ct].v0].trianglesUsingVertex.push_back(ct);
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

		//noOfDifferentNormals.clear();
		//noOfDifferentNormals.resize(m_pInputMesh->m_vecVertices.size());
		//std::fill(vecVertexMetadata.noOfDifferentNormals.begin(), vecVertexMetadata.noOfDifferentNormals.end(), 0);
		for(int ct = 0; ct < m_pInputMesh->m_vecVertices.size(); ct++)
		{
			Vector3DFloat sumOfNormals(0.0f,0.0f,0.0f);
			for(list<uint32_t>::const_iterator iter = vecVertexMetadata[ct].trianglesUsingVertex.cbegin(); iter != vecVertexMetadata[ct].trianglesUsingVertex.cend(); iter++)
			{
				sumOfNormals += m_vecTriangles[*iter].normal;
			}

			vecVertexMetadata[ct].normal = sumOfNormals;
			vecVertexMetadata[ct].normal.normalise();
		}

		Vector3DFloat offset = static_cast<Vector3DFloat>(m_pInputMesh->m_Region.getLowerCorner());
		for(int ct = 0; ct < m_pInputMesh->m_vecVertices.size(); ct++)
		{
			bool bInside = m_pInputMesh->m_Region.containsPoint(m_pInputMesh->m_vecVertices[ct].getPosition() + offset);
			vecVertexMetadata[ct].isOnRegionEdge = !bInside;
		}
	}

	template <typename VertexType>
	void MeshDecimator<VertexType>::fillCurrentVertexMetadata(std::vector<CurrentVertexMetadata>& vecVertexMetadata)
	{
		vecVertexMetadata.clear();
		vecVertexMetadata.resize(m_pInputMesh->m_vecVertices.size());
		//Initialise the metadata
		for(int ct = 0; ct < vecVertexMetadata.size(); ct++)
		{
			vecVertexMetadata[ct].trianglesUsingVertex.clear();
		}

		//Determine triangles using each vertex
		/*trianglesUsingVertex.clear();
		trianglesUsingVertex.resize(m_pInputMesh->m_vecVertices.size());*/
		for(int ct = 0; ct < m_pInputMesh->m_vecTriangleIndices.size(); ct++)
		{
			int triangle = ct / 3;

			vecVertexMetadata[m_pInputMesh->m_vecTriangleIndices[ct]].trianglesUsingVertex.push_back(triangle);
		}
	}

	template <typename VertexType>
	uint32_t MeshDecimator<VertexType>::performDecimationPass(float fMinDotProductForCollapse)
	{
		// I'm using a vector of lists here, rather than a vector of sets,
		// because I don't believe that duplicates should occur. But this
		// might be worth checking if we have problems in the future.
		/*trianglesUsingVertexCurrently.clear();
		trianglesUsingVertexCurrently.resize(m_pInputMesh->m_vecVertices.size());
		for(int ct = 0; ct < m_pInputMesh->m_vecTriangleIndices.size(); ct++)
		{
			int triangle = ct / 3;

			trianglesUsingVertexCurrently[m_pInputMesh->m_vecTriangleIndices[ct]].push_back(triangle);
		}*/

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

		buildTriangles();
		fillCurrentVertexMetadata(m_vecCurrentVertexMetadata);

		//For each triange...
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

	//This function looks at every vertex in the mesh and determines
	//how many of it's neighbours have the same material.
	template <typename VertexType>
	void MeshDecimator<VertexType>::countNoOfNeighboursUsingMaterial(void)
	{
		//Find all the neighbouring vertices for each vertex
		std::vector< std::set<int> > neighbouringVertices(m_pInputMesh->m_vecVertices.size());
		for(int triCt = 0; triCt < m_pInputMesh->m_vecTriangleIndices.size() / 3; triCt++)
		{
			int v0 = m_pInputMesh->m_vecTriangleIndices[(triCt * 3 + 0)];
			int v1 = m_pInputMesh->m_vecTriangleIndices[(triCt * 3 + 1)];
			int v2 = m_pInputMesh->m_vecTriangleIndices[(triCt * 3 + 2)];

			neighbouringVertices[v0].insert(v1);
			neighbouringVertices[v0].insert(v2);

			neighbouringVertices[v1].insert(v0);
			neighbouringVertices[v1].insert(v2);

			neighbouringVertices[v2].insert(v0);
			neighbouringVertices[v2].insert(v1);
		}

		//For each vertex, check how many neighbours have the same material
		m_vecNoOfNeighboursUsingMaterial.resize(m_pInputMesh->m_vecVertices.size());
		for(int vertCt = 0; vertCt < m_pInputMesh->m_vecVertices.size(); vertCt++)
		{
			m_vecNoOfNeighboursUsingMaterial[vertCt] = 0;
			for(std::set<int>::iterator iter = neighbouringVertices[vertCt].begin(); iter != neighbouringVertices[vertCt].end(); iter++)
			{
				if(m_pInputMesh->m_vecVertices[vertCt].getMaterial() == m_pInputMesh->m_vecVertices[*iter].getMaterial())
				{
					m_vecNoOfNeighboursUsingMaterial[vertCt]++;
				}
			}
		}
	}

	// Returns true if every bit which is set in 'a' is also set in 'b'. The reverse does not need to be true.
	template <typename VertexType>
	bool MeshDecimator<VertexType>::isSubset(std::bitset<VF_NO_OF_FLAGS> a, std::bitset<VF_NO_OF_FLAGS> b)
	{
		bool result = true;

		for(int ct = 1; ct < 7; ct++) //Start at '1' to skip material flag
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

	//template <typename VertexType>
	bool MeshDecimator<PositionMaterialNormal>::canCollapseEdge(uint32_t uSrc, uint32_t uDst)
	{
		//A vertex will be locked if it has already been involved in a collapse this pass.
		if(vertexLocked[uSrc] || vertexLocked[uDst])
		{
			return false;
		}

		if(m_pInputMesh->m_vecVertices[uSrc].getMaterial() != m_pInputMesh->m_vecVertices[uDst].getMaterial())
		{
			return false;
		}

		//For now, don't collapse vertices on material edges...
		if(m_pInputMesh->m_vecVertices[uSrc].isOnMaterialEdge() || m_pInputMesh->m_vecVertices[uDst].isOnMaterialEdge())
		{
			if(true)
			{
				bool pass = false;						

				bool allMatch = false;

				// On the original undecimated mesh a material boundary vertex on a straight edge will
				// have four neighbours with the same material. If it's on a corner it will have a
				// different number. We only collapse straight edges to avoid changingthe shape of the
				// material boundary.
				if(m_vecNoOfNeighboursUsingMaterial[uSrc] == m_vecNoOfNeighboursUsingMaterial[uDst])
				{
					if(m_vecNoOfNeighboursUsingMaterial[uSrc] == 4)
					{
						allMatch = true;
					}
				}

				bool movementValid = false;
				Vector3DFloat movement = m_pInputMesh->m_vecVertices[uDst].getPosition() - m_pInputMesh->m_vecVertices[uSrc].getPosition();
				movement.normalise();
				if(movement.dot(Vector3DFloat(0,0,1)) > 0.999)
				{
					movementValid = true;
				}

				if(movement.dot(Vector3DFloat(0,1,0)) > 0.999)
				{
					movementValid = true;
				}

				if(movement.dot(Vector3DFloat(1,0,0)) > 0.999)
				{
					movementValid = true;
				}

				if(movement.dot(Vector3DFloat(0,0,-1)) > 0.999)
				{
					movementValid = true;
				}

				if(movement.dot(Vector3DFloat(0,-1,0)) > 0.999)
				{
					movementValid = true;
				}

				if(movement.dot(Vector3DFloat(-1,0,0)) > 0.999)
				{
					movementValid = true;
				}

				if(movementValid && allMatch)
				{
					pass = true;
				}

				if(!pass)
				{
					return false;
				}
			}
			else //Material collapses not allowed
			{
				return false;
			}
		}

		// Vertices on the geometrical edge of surface meshes need special handling. 
		// We check for this by whether any of the edge flags are set.
		if(m_pInputMesh->m_vecVertices[uSrc].m_bFlags.any() || m_pInputMesh->m_vecVertices[uDst].m_bFlags.any())
		{
			// Assume we can't collapse until we prove otherwise...
			bool bCollapseGeometryEdgePair = false;

			// We can collapse normal vertices onto edge vertices, and edge vertices
			// onto corner vertices, but not vice-versa. Hence we check whether all
			// the edge flags in the source vertex are also set in the destination vertex.
			if(isSubset(m_pInputMesh->m_vecVertices[uSrc].m_bFlags, m_pInputMesh->m_vecVertices[uDst].m_bFlags))
			{
				// In general adjacent regions surface meshes may collapse differently
				// and this can cause cracks. We solve this by only allowing the collapse
				// is the normals are exactly the same. We do not use the user provided
				// tolerence here (but do allow for floating point error).
				if(m_pInputMesh->m_vecVertices[uSrc].getNormal().dot(m_pInputMesh->m_vecVertices[uDst].getNormal()) > 0.999)
				{
					// Ok, this pair can collapse.
					bCollapseGeometryEdgePair = true;
				}
			}

			// Use the result.
			if(!bCollapseGeometryEdgePair)
			{
				return false;
			}
		}

		//Check the normals are within the threashold.
		if(m_pInputMesh->m_vecVertices[uSrc].getNormal().dot(m_pInputMesh->m_vecVertices[uDst].getNormal()) < fMinDotProductForCollapse)
		{
			return false;
		}

		////////////////////////////////////////////////////////////////////////////////
		//The last test is whether we will flip any of the faces
		if(collapseChangesFaceNormals(uSrc,uDst, 0.9f))
		{
			return false;
		}
		return true;
	}

	//template <typename VertexType>
	bool MeshDecimator<PositionMaterial>::canCollapseEdge(uint32_t uSrc, uint32_t uDst)
	{
		//A vertex will be locked if it has already been involved in a collapse this pass.
		if(vertexLocked[uSrc] || vertexLocked[uDst])
		{
			return false;
		}		

		if(m_vecInitialVertexMetadata[uSrc].isOnMaterialEdge)
		{
			if(m_vecInitialVertexMetadata[uSrc].isOnRegionEdge)
			{
				assert(false); //Shouldn't be on both edge types.
				return false;
			}
			else
			{
				return canCollapseMaterialEdge(uSrc, uDst);
			}
		}
		else
		{
			if(m_vecInitialVertexMetadata[uSrc].isOnRegionEdge)
			{
				return canCollapseRegionEdge(uSrc, uDst);
			}
			else
			{
				return canCollapseNormalEdge(uSrc, uDst);
			}
		}
	}

	template <typename VertexType>
	bool MeshDecimator<VertexType>::canCollapseNormalEdge(uint32_t uSrc, uint32_t uDst)
	{
		return !collapseChangesFaceNormals(uSrc, uDst, 0.999f);
	}

	template <typename VertexType>
	bool MeshDecimator<VertexType>::canCollapseRegionEdge(uint32_t uSrc, uint32_t uDst)
	{
		return false;

		if(m_vecInitialVertexMetadata[uDst].isOnRegionEdge)
		{

			int matchingCoordinates = 0;
			if(abs(m_pInputMesh->m_vecVertices[uSrc].getPosition().getX() - m_pInputMesh->m_vecVertices[uDst].getPosition().getX()) < 0.001)
			{
				matchingCoordinates++;
			}
			if(abs(m_pInputMesh->m_vecVertices[uSrc].getPosition().getY() - m_pInputMesh->m_vecVertices[uDst].getPosition().getY()) < 0.001)
			{
				matchingCoordinates++;
			}
			if(abs(m_pInputMesh->m_vecVertices[uSrc].getPosition().getZ() - m_pInputMesh->m_vecVertices[uDst].getPosition().getZ()) < 0.001)
			{
				matchingCoordinates++;
			}
			if(matchingCoordinates != 2)
			{
				return false;
			}

			if(m_vecInitialVertexMetadata[uSrc].trianglesUsingVertex.size() != m_vecInitialVertexMetadata[uDst].trianglesUsingVertex.size()) //Corner
			{
				return false;
			}

			if(m_vecInitialVertexMetadata[uSrc].normal.dot(m_vecInitialVertexMetadata[uDst].normal) < 0.999f)
			{
				return false;
			}

			return !collapseChangesFaceNormals(uSrc, uDst, 0.999f);
		}
		return false;
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
		//list<uint32_t> triangles = trianglesUsingVertexCurrently[v0];
		list<uint32_t> triangles = m_vecCurrentVertexMetadata[uSrc].trianglesUsingVertex;
		/*set<uint32_t> triangles;
		std::set_union(trianglesUsingVertex[v0].begin(), trianglesUsingVertex[v0].end(),
			trianglesUsingVertex[v1].begin(), trianglesUsingVertex[v1].end(),
			std::inserter(triangles, triangles.begin()));*/

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
}