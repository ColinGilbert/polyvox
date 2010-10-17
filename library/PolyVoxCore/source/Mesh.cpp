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

#include "Mesh.h"

#include "SurfaceMesh.h"

#include <vector>

namespace PolyVox
{
	void Mesh::buildFromMesh(SurfaceMesh<PositionMaterialNormal>* pMesh)
	{
		//First we copy the vertices across.
		//We also keep track of where each vertex went
		std::vector< std::set<MeshVertex*>::iterator > vertexMapper(pMesh->getNoOfVertices());

		for(int ct = 0; ct < pMesh->getNoOfVertices(); ct++)
		{
			MeshVertex* pMeshVertex = new MeshVertex;
			pMeshVertex->m_vertexData = pMesh->m_vecVertices[ct];
			vertexMapper[ct] = m_vertices.insert(pMeshVertex).first;
		}

		//Next, we add each triangle to the mesh
		for(int triCt = 0; triCt < pMesh->getNoOfIndices() / 3; triCt++)
		{
			int index0 = pMesh->m_vecTriangleIndices[triCt * 3];
			int index1 = pMesh->m_vecTriangleIndices[triCt * 3 + 1];
			int index2 = pMesh->m_vecTriangleIndices[triCt * 3 + 2];

			MeshVertex* meshVertex0 = *(vertexMapper[index0]);
			MeshVertex* meshVertex1 = *(vertexMapper[index1]);
			MeshVertex* meshVertex2 = *(vertexMapper[index2]);

			MeshEdge* v0v1 = new MeshEdge;
			v0v1->m_pSrc = meshVertex0;
			v0v1->m_pDest = meshVertex1;
			//meshVertex0->m_edges.insert(v0v1);

			MeshEdge* v1v2 = new MeshEdge;
			v1v2->m_pSrc = meshVertex1;
			v1v2->m_pDest = meshVertex2;
			//meshVertex1->m_edges.insert(v1v2);

			MeshEdge* v2v0 = new MeshEdge;
			v2v0->m_pSrc = meshVertex2;
			v2v0->m_pDest = meshVertex0;
			//meshVertex2->m_edges.insert(v2v0);

			v0v1->m_pNextEdge = v1v2;
			v1v2->m_pNextEdge = v2v0;
			v2v0->m_pNextEdge = v0v1;

			v0v1->m_pPreviousEdge = v2v0;
			v1v2->m_pPreviousEdge = v0v1;
			v2v0->m_pPreviousEdge = v1v2;

			v0v1->m_pOtherEdge = 0;
			v1v2->m_pOtherEdge = 0;
			v2v0->m_pOtherEdge = 0;

			m_edges.insert(v0v1);
			m_edges.insert(v1v2);
			m_edges.insert(v2v0);

			//Create the face
			MeshFace* meshFace = new MeshFace;

			v0v1->m_pFace = meshFace;
			v1v2->m_pFace = meshFace;
			v2v0->m_pFace = meshFace;

			meshFace->m_pEdge = v0v1; //Just use the first edge

			m_faces.insert(meshFace);

			//The the vertices they are used by the face
			//meshVertex0->m_faces.insert(meshFace);
			//meshVertex1->m_faces.insert(meshFace);
			//meshVertex2->m_faces.insert(meshFace);
		}

		//Loop over the edges and try to match up any pairs.
		/*for(std::set<MeshEdge*>::iterator edgeIter = m_edges.begin(); edgeIter != m_edges.end(); edgeIter++)
		{
			MeshEdge* pMeshEdge = *edgeIter;

			if(pMeshEdge->m_pOtherEdge == 0)
			{
				//Let's see if we can find the other edge somewhere.
				//Create an example of what we are looking for...
				MeshEdge otherEdgeExample;
				otherEdgeExample.m_pSrc = pMeshEdge->m_pDest;
				otherEdgeExample.m_pDest = pMeshEdge->m_pSrc;

				m_edges.find(
			}
		}*/

		/*int counter = 0;

		//Lastly, we loop over the edges and determine all those which should be adjacent
		for(std::set<MeshEdge*>::iterator outerIter = m_edges.begin(); outerIter != m_edges.end(); outerIter++)
		{
			std::cout << counter++ << std::endl;

			for(std::set<MeshEdge*>::iterator innerIter = m_edges.begin(); innerIter != m_edges.end(); innerIter++)
			{
				MeshEdge* edge0 = *outerIter;
				MeshEdge* edge1 = *innerIter;

				if((edge0->m_pSrc == edge1->m_pDest) && (edge0->m_pDest == edge1->m_pSrc))
				{
					edge0->m_pOtherEdge = edge1;
					edge1->m_pOtherEdge = edge0;
				}
			}
		}*/

		//Match up the edge pains
		matchEdgePairs();

		//Compute the edge costs
		computeEdgeCosts();

		//Check sanity
		isSane();
	}

	void Mesh::matchEdgePairs(void)
	{
		std::set<MeshEdge*> matchedEdges;
		while(m_edges.empty() == false)
		{
			std::set<MeshEdge*>::iterator firstEdgeIter = m_edges.begin();
			MeshEdge* firstEdgePtr = *firstEdgeIter;
			m_edges.erase(firstEdgeIter);
			matchedEdges.insert(firstEdgePtr);

			//Attempt to find the opposite edge
			for(std::set<MeshEdge*>::iterator edgeIter = m_edges.begin(); edgeIter != m_edges.end(); edgeIter++)
			{
				MeshEdge* candidate = *edgeIter;
				if((firstEdgePtr->m_pSrc == candidate->m_pDest) && (firstEdgePtr->m_pDest == candidate->m_pSrc))
				{
					m_edges.erase(edgeIter);
					matchedEdges.insert(candidate);

					firstEdgePtr->m_pOtherEdge = candidate;
					candidate->m_pOtherEdge = firstEdgePtr;

					break;
				}
			}
		}

		m_edges = matchedEdges;
	}

	void Mesh::computeEdgeCosts(void)
	{
		for(std::set<MeshEdge*>::iterator edgeIter = m_edges.begin(); edgeIter != m_edges.end(); edgeIter++)
		{
			(*edgeIter)->computeEdgeCost(this);
		}
	}

	void Mesh::fillMesh(SurfaceMesh<PositionMaterialNormal>* pMesh)
	{
		pMesh->clear();

		for(std::set<MeshVertex*>::iterator vertItor = m_vertices.begin(); vertItor != m_vertices.end(); vertItor++)
		{
			MeshVertex* meshVertex = *vertItor;
			meshVertex->m_index = pMesh->addVertex(meshVertex->m_vertexData);
		}

		for(std::set<MeshFace*>::iterator faceItor = m_faces.begin(); faceItor != m_faces.end(); faceItor++)
		{
			MeshFace* meshFace = *faceItor;
			MeshVertex* v0 = meshFace->m_pEdge->m_pSrc;
			MeshVertex* v1 = meshFace->m_pEdge->m_pNextEdge->m_pSrc;
			MeshVertex* v2 = meshFace->m_pEdge->m_pNextEdge->m_pNextEdge->m_pSrc;

			pMesh->addTriangle(v0->m_index, v1->m_index, v2->m_index);
		}

		pMesh->m_vecLodRecords.clear();
		LodRecord lodRecord;
		lodRecord.beginIndex = 0;
		lodRecord.endIndex = pMesh->getNoOfIndices();
		pMesh->m_vecLodRecords.push_back(lodRecord);
	}

	void Mesh::removeEdge(MeshEdge* pMeshEdge)
	{
		//Get the src and dest vertices
		MeshVertex* pSrc = pMeshEdge->m_pSrc;
		MeshVertex* pDest = pMeshEdge->m_pDest;

		//Get the other half of the edge
		MeshEdge* pOtherEdge = pMeshEdge->m_pOtherEdge;

		//Get the faces
		MeshFace* pMeshFace = pMeshEdge->m_pFace;
		MeshFace* pOtherFace = 0;
		if(pOtherEdge != 0)
		{
			pOtherFace = pOtherEdge->m_pFace;
		}

		//Remove the faces (the edges keep pointers to the faces, but those edges will be deleted soon anyway...)
		m_faces.erase(pMeshFace);
		delete pMeshFace;
		pMeshFace = 0;

		if(pOtherFace)
		{
			m_faces.erase(pOtherFace);
			delete pOtherFace;
			pOtherFace = 0;
		}

		//Erase the edges
		m_edges.erase(pMeshEdge);
		m_edges.erase(pMeshEdge->m_pNextEdge);
		m_edges.erase(pMeshEdge->m_pPreviousEdge);

		/*pMeshEdge->m_pSrc->m_edges.erase(pMeshEdge);
		pMeshEdge->m_pNextEdge->m_pSrc->m_edges.erase(pMeshEdge->m_pNextEdge);
		pMeshEdge->m_pPreviousEdge->m_pSrc->m_edges.erase(pMeshEdge->m_pPreviousEdge);*/

		delete pMeshEdge->m_pNextEdge;
		pMeshEdge->m_pNextEdge = 0;
		delete pMeshEdge->m_pPreviousEdge;
		pMeshEdge->m_pPreviousEdge = 0;
		delete pMeshEdge;
		//pMeshEdge = 0;


		if(pOtherEdge)
		{
			m_edges.erase(pOtherEdge);
			m_edges.erase(pOtherEdge->m_pNextEdge);
			m_edges.erase(pOtherEdge->m_pPreviousEdge);

			/*pOtherEdge->m_pSrc->m_edges.erase(pOtherEdge);
			pOtherEdge->m_pNextEdge->m_pSrc->m_edges.erase(pOtherEdge->m_pNextEdge);
			pOtherEdge->m_pPreviousEdge->m_pSrc->m_edges.erase(pOtherEdge->m_pPreviousEdge);*/

			delete pOtherEdge->m_pNextEdge;
			pOtherEdge->m_pNextEdge = 0;
			delete pOtherEdge->m_pPreviousEdge;
			pOtherEdge->m_pPreviousEdge = 0;
			delete pOtherEdge;
			//pOtherEdge = 0;
		}

		//Find the affected edges
		/*std::set<MeshEdge*> affectedEdges = edgesStartingatEitherVertex;
		for(std::set<MeshEdge*>::iterator edgeIter = edgesStartingatEitherVertex.begin(); edgeIter != edgesStartingatEitherVertex.end(); edgeIter++)
		{
			affectedEdges.insert((*edgeIter)->m_pNextEdge);
			affectedEdges.insert((*edgeIter)->m_pPreviousEdge);
			//if((*edgeIter)->m_pOtherEdge != 0)
			//{
			//	affectedEdges.insert((*edgeIter)->m_pOtherEdge);
			//}
		}*/

		//Update the source and destinations
		for(std::set<MeshEdge*>::iterator edgeIter = m_edges.begin(); edgeIter != m_edges.end(); edgeIter++)
		{
			MeshEdge* pEdge = *edgeIter;
			if(pEdge->m_pSrc == pSrc)
			{
				pEdge->m_pSrc = pDest;
			}
			if(pEdge->m_pDest == pSrc)
			{
				pEdge->m_pDest = pDest;
			}
		}

		/*for(std::set<MeshVertex*>::iterator vertexIter = m_vertices.begin(); vertexIter != m_vertices.end(); vertexIter++)
		{
			(*vertexIter)->m_edges.clear();
		}

		for(std::set<MeshEdge*>::iterator edgeIter = m_edges.begin(); edgeIter != m_edges.end(); edgeIter++)
		{
			MeshEdge* pEdge = *edgeIter;
			pEdge->m_pSrc->m_edges.insert(pEdge);
		}*/


		/*for(std::set<MeshEdge*>::iterator edgeIter = affectedEdges.begin(); edgeIter != affectedEdges.end(); edgeIter++)
		{
			MeshEdge* pEdge = *edgeIter;
			if(pEdge->m_pSrc == pSrc)
			{
				pEdge->m_pSrc = pDest;
			}
			if(pEdge->m_pDest == pSrc)
			{
				pEdge->m_pDest = pDest;
			}
		}*/

		//Deal with the vertices
		/*pSrc->m_edges.erase(pMeshEdge);
		if(pOtherEdge)
		{
			pDest->m_edges.erase(pOtherEdge);
		}*/

		//Find the edges starting at either vertex
		/*std::set<MeshEdge*> edgesStartingatEitherVertex;
		std::set_union(pSrc->m_edges.begin(), pSrc->m_edges.end(),
			pDest->m_edges.begin(), pDest->m_edges.end(),
			std::inserter(edgesStartingatEitherVertex, edgesStartingatEitherVertex.begin()));*/

		//Remove the old vertex
		//pDest->m_edges = edgesStartingatEitherVertex;
		m_vertices.erase(pSrc);
		delete pSrc; //Don't set to 0 yet as we will use the value

		//Update any pairings
		matchEdgePairs();
		/*for(std::set<MeshEdge*>::iterator outerIter = affectedEdges.begin(); outerIter != affectedEdges.end(); outerIter++)
		{
			for(std::set<MeshEdge*>::iterator innerIter = affectedEdges.begin(); innerIter != affectedEdges.end(); innerIter++)
			{
				MeshEdge* edge0 = *outerIter;
				MeshEdge* edge1 = *innerIter;

				if((edge0->m_pSrc == edge1->m_pDest) && (edge0->m_pDest == edge1->m_pSrc))
				{
					edge0->m_pOtherEdge = edge1;
					edge1->m_pOtherEdge = edge0;
				}
			}
		}*/

		//Recompute the affected edge costs
		computeEdgeCosts();
		/*for(std::set<MeshEdge*>::iterator edgeIter = affectedEdges.begin(); edgeIter != affectedEdges.end(); edgeIter++)
		{
			(*edgeIter)->computeEdgeCost();
		}*/
	}

	void Mesh::decimateAll(void)
	{
		for(int ct = 0; ct < 100; ct++)
		{
			std::cout << ct << std::endl;
			decimateOne();
			//isSane();
		}
		/*int ct = 0;
		while(decimateOne())
		{
			std::cout << ct++ << std::endl;
		}*/

		//decimateOne();
	}

	bool Mesh::decimateOne(void)
	{
		for(std::set<MeshEdge*>::iterator edgeIter = m_edges.begin(); edgeIter != m_edges.end(); edgeIter++)
		{
			/*for(int ct = 0; ct < 30; ct++)
			{
				edgeIter++;
			}*/

			MeshEdge* pMeshEdge = *edgeIter;
			if(pMeshEdge->m_fCost < 1.0)
			{
				removeEdge(pMeshEdge);
				return true;
			}
		}

		return false;
	}

	bool Mesh::isSane(void)
	{
		for(std::set<MeshVertex*>::iterator vertexIter = m_vertices.begin(); vertexIter != m_vertices.end(); vertexIter++)
		{
			//This would be eaiser if we just propergated exceptions?
			if((*vertexIter)->isSane() == false)
			{
				std::cout << "SANITY CHECK FAIL: Problem found in vertex set" << std::endl;
				return false;
			}
		}

		for(std::set<MeshFace*>::iterator faceIter = m_faces.begin(); faceIter != m_faces.end(); faceIter++)
		{
			//This would be eaiser if we just propergated exceptions?
			if((*faceIter)->isSane() == false)
			{
				std::cout << "SANITY CHECK FAIL: Problem found in face set" << std::endl;
				return false;
			}
		}

		for(std::set<MeshEdge*>::iterator edgeIter = m_edges.begin(); edgeIter != m_edges.end(); edgeIter++)
		{
			//This would be eaiser if we just propergated exceptions?
			if((*edgeIter)->isSane() == false)
			{
				std::cout << "SANITY CHECK FAIL: Problem found in edge set" << std::endl;
				return false;
			}
		}

		std::cout << "SANITY CHECK SUCCESS" << std::endl;
		return true;
	}
}