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

#include "PolyVoxCore/Impl/Timer.h"

namespace PolyVox
{
	// We try to avoid duplicate vertices by checking whether a vertex has already been added at a given position.
	// However, it is possible that vertices have the same position but different materials. In this case, the
	// vertices are not true duplicates and both must be added to the mesh. As far as I can tell, it is possible to have
	// at most eight vertices with the same position but different materials. For example, this worst-case scenario
	// happens when we have a 2x2x2 group of voxels, all with different materials and some/all partially transparent.
	// The vertex position at the center of this group is then going to be used by all eight voxels all with different
	// materials.
	template<typename VolumeType, typename IsQuadNeeded>
	const uint32_t CubicSurfaceExtractor<VolumeType, IsQuadNeeded>::MaxVerticesPerPosition = 8;

	template<typename VolumeType, typename IsQuadNeeded>
	CubicSurfaceExtractor<VolumeType, IsQuadNeeded>::CubicSurfaceExtractor(VolumeType* volData, Region region, Mesh<CubicVertex<typename VolumeType::VoxelType> >* result, WrapMode eWrapMode, typename VolumeType::VoxelType tBorderValue, bool bMergeQuads, IsQuadNeeded isQuadNeeded)
		:m_volData(volData)
		,m_regSizeInVoxels(region)
		,m_meshCurrent(result)
		,m_bMergeQuads(bMergeQuads)
		,m_eWrapMode(eWrapMode)
		,m_tBorderValue(tBorderValue)
	{
		m_funcIsQuadNeededCallback = isQuadNeeded;
	}

	template<typename VolumeType, typename IsQuadNeeded>
	void CubicSurfaceExtractor<VolumeType, IsQuadNeeded>::execute()
	{
		Timer timer;
		m_meshCurrent->clear();

		uint32_t uArrayWidth = m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2;
		uint32_t uArrayHeight = m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2;

		uint32_t arraySize[3]= {uArrayWidth, uArrayHeight, MaxVerticesPerPosition};
		m_previousSliceVertices.resize(arraySize);
		m_currentSliceVertices.resize(arraySize);
		memset(m_previousSliceVertices.getRawData(), 0xff, m_previousSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial));
		memset(m_currentSliceVertices.getRawData(), 0xff, m_currentSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial));

		m_vecQuads[NegativeX].resize(m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2);
		m_vecQuads[PositiveX].resize(m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2);

		m_vecQuads[NegativeY].resize(m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2);
		m_vecQuads[PositiveY].resize(m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2);

		m_vecQuads[NegativeZ].resize(m_regSizeInVoxels.getUpperZ() - m_regSizeInVoxels.getLowerZ() + 2);
		m_vecQuads[PositiveZ].resize(m_regSizeInVoxels.getUpperZ() - m_regSizeInVoxels.getLowerZ() + 2);

		typename VolumeType::Sampler volumeSampler(m_volData);	
		volumeSampler.setWrapMode(m_eWrapMode, m_tBorderValue);
		
		for(int32_t z = m_regSizeInVoxels.getLowerZ(); z <= m_regSizeInVoxels.getUpperZ(); z++)
		{
			uint32_t regZ = z - m_regSizeInVoxels.getLowerZ();

			for(int32_t y = m_regSizeInVoxels.getLowerY(); y <= m_regSizeInVoxels.getUpperY(); y++)
			{
				uint32_t regY = y - m_regSizeInVoxels.getLowerY();

				volumeSampler.setPosition(m_regSizeInVoxels.getLowerX(),y,z);

				for(int32_t x = m_regSizeInVoxels.getLowerX(); x <= m_regSizeInVoxels.getUpperX(); x++)
				{
					uint32_t regX = x - m_regSizeInVoxels.getLowerX();						

					typename VolumeType::VoxelType material; //Filled in by callback
					typename VolumeType::VoxelType currentVoxel = volumeSampler.getVoxel();
					typename VolumeType::VoxelType negXVoxel = volumeSampler.peekVoxel1nx0py0pz();
					typename VolumeType::VoxelType negYVoxel = volumeSampler.peekVoxel0px1ny0pz();
					typename VolumeType::VoxelType negZVoxel = volumeSampler.peekVoxel0px0py1nz();

					// X
					if(m_funcIsQuadNeededCallback(currentVoxel, negXVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices);
						uint32_t v1 = addVertex(regX    , regY    , regZ + 1, material, m_currentSliceVertices);
						uint32_t v2 = addVertex(regX    , regY + 1, regZ + 1, material, m_currentSliceVertices);
						uint32_t v3 = addVertex(regX    , regY + 1, regZ    , material, m_previousSliceVertices);

						m_vecQuads[NegativeX][regX].push_back(Quad(v0, v1, v2, v3));
					}

					if(m_funcIsQuadNeededCallback(negXVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices);
						uint32_t v1 = addVertex(regX    , regY    , regZ + 1, material, m_currentSliceVertices);
						uint32_t v2 = addVertex(regX    , regY + 1, regZ + 1, material, m_currentSliceVertices);
						uint32_t v3 = addVertex(regX    , regY + 1, regZ    , material, m_previousSliceVertices);

						m_vecQuads[PositiveX][regX].push_back(Quad(v0, v3, v2, v1));
					}

					// Y
					if(m_funcIsQuadNeededCallback(currentVoxel, negYVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices);
						uint32_t v1 = addVertex(regX + 1, regY    , regZ    , material, m_previousSliceVertices);
						uint32_t v2 = addVertex(regX + 1, regY    , regZ + 1, material, m_currentSliceVertices);
						uint32_t v3 = addVertex(regX    , regY    , regZ + 1, material, m_currentSliceVertices);

						m_vecQuads[NegativeY][regY].push_back(Quad(v0, v1, v2, v3));
					}

					if(m_funcIsQuadNeededCallback(negYVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices);
						uint32_t v1 = addVertex(regX + 1, regY    , regZ    , material, m_previousSliceVertices);
						uint32_t v2 = addVertex(regX + 1, regY    , regZ + 1, material, m_currentSliceVertices);
						uint32_t v3 = addVertex(regX    , regY    , regZ + 1, material, m_currentSliceVertices);

						m_vecQuads[PositiveY][regY].push_back(Quad(v0, v3, v2, v1));
					}

					// Z
					if(m_funcIsQuadNeededCallback(currentVoxel, negZVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices);
						uint32_t v1 = addVertex(regX    , regY + 1, regZ    , material, m_previousSliceVertices);
						uint32_t v2 = addVertex(regX + 1, regY + 1, regZ    , material, m_previousSliceVertices);
						uint32_t v3 = addVertex(regX + 1, regY    , regZ    , material, m_previousSliceVertices);

						m_vecQuads[NegativeZ][regZ].push_back(Quad(v0, v1, v2, v3));
					}

					if(m_funcIsQuadNeededCallback(negZVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices);
						uint32_t v1 = addVertex(regX    , regY + 1, regZ    , material, m_previousSliceVertices);
						uint32_t v2 = addVertex(regX + 1, regY + 1, regZ    , material, m_previousSliceVertices);
						uint32_t v3 = addVertex(regX + 1, regY    , regZ    , material, m_previousSliceVertices);

						m_vecQuads[PositiveZ][regZ].push_back(Quad(v0, v3, v2, v1));
					}

					volumeSampler.movePositiveX();
				}
			}

			m_previousSliceVertices.swap(m_currentSliceVertices);
			memset(m_currentSliceVertices.getRawData(), 0xff, m_currentSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial));
		}

		for(uint32_t uFace = 0; uFace < NoOfFaces; uFace++)
		{
			std::vector< std::list<Quad> >& vecListQuads = m_vecQuads[uFace];

			for(uint32_t slice = 0; slice < vecListQuads.size(); slice++)
			{
				std::list<Quad>& listQuads = vecListQuads[slice];

				if(m_bMergeQuads)
				{
					//Repeatedly call this function until it returns
					//false to indicate nothing more can be done.
					while(performQuadMerging(listQuads)){}
				}

				typename std::list<Quad>::iterator iterEnd = listQuads.end();
				for(typename std::list<Quad>::iterator quadIter = listQuads.begin(); quadIter != iterEnd; quadIter++)
				{
					Quad& quad = *quadIter;				
					m_meshCurrent->addTriangleCubic(quad.vertices[0], quad.vertices[1],quad.vertices[2]);
					m_meshCurrent->addTriangleCubic(quad.vertices[0], quad.vertices[2],quad.vertices[3]);
				}			
			}
		}

		m_meshCurrent->m_Region = m_regSizeInVoxels;
		m_meshCurrent->removeUnusedVertices();

		m_meshCurrent->m_vecLodRecords.clear();
		LodRecord lodRecord;
		lodRecord.beginIndex = 0;
		lodRecord.endIndex = m_meshCurrent->getNoOfIndices();
		m_meshCurrent->m_vecLodRecords.push_back(lodRecord);

		POLYVOX_LOG_TRACE("Cubic surface extraction took " << timer.elapsedTimeInMilliSeconds()
			<< "ms (Region size = " << m_regSizeInVoxels.getWidthInVoxels() << "x" << m_regSizeInVoxels.getHeightInVoxels()
			<< "x" << m_regSizeInVoxels.getDepthInVoxels() << ")");
	}

	template<typename VolumeType, typename IsQuadNeeded>
	int32_t CubicSurfaceExtractor<VolumeType, IsQuadNeeded>::addVertex(uint32_t uX, uint32_t uY, uint32_t uZ, typename VolumeType::VoxelType uMaterialIn, Array<3, IndexAndMaterial>& existingVertices)
	{
		for(uint32_t ct = 0; ct < MaxVerticesPerPosition; ct++)
		{
			IndexAndMaterial& rEntry = existingVertices[uX][uY][ct];

			if(rEntry.iIndex == -1)
			{
				//No vertices matched and we've now hit an empty space. Fill it by creating a vertex. The 0.5f offset is because vertices set between voxels in order to build cubes around them.
				CubicVertex<typename VolumeType::VoxelType> cubicVertex;
				cubicVertex.position.setElements(static_cast<float>(uX)-0.5f, static_cast<float>(uY)-0.5f, static_cast<float>(uZ)-0.5f);
				cubicVertex.material = uMaterialIn;
				rEntry.iIndex = m_meshCurrent->addVertex(cubicVertex);
				rEntry.uMaterial = uMaterialIn;

				return rEntry.iIndex;
			}

			//If we have an existing vertex and the material matches then we can return it.
			if(rEntry.uMaterial == uMaterialIn)
			{
				return rEntry.iIndex;
			}
		}

		// If we exit the loop here then apparently all the slots were full but none of them matched.
		// This shouldn't ever happen, so if it does it is probably a bug in PolyVox. Please report it to us!
		POLYVOX_THROW(std::runtime_error, "All slots full but no matches during cubic surface extraction. This is probably a bug in PolyVox");
		return -1; //Should never happen.
	}

	template<typename VolumeType, typename IsQuadNeeded>
	bool CubicSurfaceExtractor<VolumeType, IsQuadNeeded>::performQuadMerging(std::list<Quad>& quads)
	{
		bool bDidMerge = false;
		for(typename std::list<Quad>::iterator outerIter = quads.begin(); outerIter != quads.end(); outerIter++)
		{
			typename std::list<Quad>::iterator innerIter = outerIter;
			innerIter++;
			while(innerIter != quads.end())
			{
				Quad& q1 = *outerIter;
				Quad& q2 = *innerIter;

				bool result = mergeQuads(q1,q2);

				if(result)
				{
					bDidMerge = true;
					innerIter = quads.erase(innerIter);
				}
				else
				{
					innerIter++;
				}
			}
		}

		return bDidMerge;
	}

	template<typename VolumeType, typename IsQuadNeeded>
	bool CubicSurfaceExtractor<VolumeType, IsQuadNeeded>::mergeQuads(Quad& q1, Quad& q2)
	{
		//All four vertices of a given quad have the same material,
		//so just check that the first pair of vertices match.
		if(m_meshCurrent->getVertices()[q1.vertices[0]].material == m_meshCurrent->getVertices()[q2.vertices[0]].material)
		{
			//Now check whether quad 2 is adjacent to quad one by comparing vertices.
			//Adjacent quads must share two vertices, and the second quad could be to the
			//top, bottom, left, of right of the first one. This gives four combinations to test.
			if((q1.vertices[0] == q2.vertices[1]) && ((q1.vertices[3] == q2.vertices[2])))
			{
				q1.vertices[0] = q2.vertices[0];
				q1.vertices[3] = q2.vertices[3];
				return true;
			}
			else if((q1.vertices[3] == q2.vertices[0]) && ((q1.vertices[2] == q2.vertices[1])))
			{
				q1.vertices[3] = q2.vertices[3];
				q1.vertices[2] = q2.vertices[2];
				return true;
			}
			else if((q1.vertices[1] == q2.vertices[0]) && ((q1.vertices[2] == q2.vertices[3])))
			{
				q1.vertices[1] = q2.vertices[1];
				q1.vertices[2] = q2.vertices[2];
				return true;
			}
			else if((q1.vertices[0] == q2.vertices[3]) && ((q1.vertices[1] == q2.vertices[2])))
			{
				q1.vertices[0] = q2.vertices[0];
				q1.vertices[1] = q2.vertices[1];
				return true;
			}
		}
		
		//Quads cannot be merged.
		return false;
	}
}
