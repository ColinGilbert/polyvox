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

#include "PolyVox/Impl/Timer.h"

namespace PolyVox
{
	template<typename VolumeType, typename MeshType, typename IsQuadNeeded>
	CubicSurfaceExtractor<VolumeType, MeshType, IsQuadNeeded>::CubicSurfaceExtractor(VolumeType* volData, Region region, MeshType* result, IsQuadNeeded isQuadNeeded, bool bMergeQuads)
		:m_volData(volData)
		,m_regSizeInVoxels(region)
		,m_meshCurrent(result)
		,m_previousSliceVertices(m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2, m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2, MaxVerticesPerPosition)
		,m_currentSliceVertices(m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2, m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2, MaxVerticesPerPosition)
		,m_bMergeQuads(bMergeQuads)
	{
		m_funcIsQuadNeededCallback = isQuadNeeded;

		// This extractor has a limit as to how large the extracted region can be, because the vertex positions are encoded with a single byte per component.
		int32_t maxReionDimension = 256;
		POLYVOX_THROW_IF(region.getWidthInVoxels() > maxReionDimension, std::invalid_argument, "Requested extraction region exceeds maximum dimensions");
		POLYVOX_THROW_IF(region.getHeightInVoxels() > maxReionDimension, std::invalid_argument, "Requested extraction region exceeds maximum dimensions");
		POLYVOX_THROW_IF(region.getDepthInVoxels() > maxReionDimension, std::invalid_argument, "Requested extraction region exceeds maximum dimensions");
	}

	template<typename VolumeType, typename MeshType, typename IsQuadNeeded>
	void CubicSurfaceExtractor<VolumeType, MeshType, IsQuadNeeded>::execute()
	{
		Timer timer;
		m_meshCurrent->clear();

		//uint32_t uArrayWidth = m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2;
		//uint32_t uArrayHeight = m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2;

		//uint32_t arraySize[3]= {uArrayWidth, uArrayHeight, MaxVerticesPerPosition};
		//m_previousSliceVertices.resize(arraySize);
		//m_currentSliceVertices.resize(arraySize);
		memset(m_previousSliceVertices.getRawData(), 0xff, m_previousSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial<VolumeType>));
		memset(m_currentSliceVertices.getRawData(), 0xff, m_currentSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial<VolumeType>));

		m_vecQuads[NegativeX].resize(m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2);
		m_vecQuads[PositiveX].resize(m_regSizeInVoxels.getUpperX() - m_regSizeInVoxels.getLowerX() + 2);

		m_vecQuads[NegativeY].resize(m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2);
		m_vecQuads[PositiveY].resize(m_regSizeInVoxels.getUpperY() - m_regSizeInVoxels.getLowerY() + 2);

		m_vecQuads[NegativeZ].resize(m_regSizeInVoxels.getUpperZ() - m_regSizeInVoxels.getLowerZ() + 2);
		m_vecQuads[PositiveZ].resize(m_regSizeInVoxels.getUpperZ() - m_regSizeInVoxels.getLowerZ() + 2);

		typename VolumeType::Sampler volumeSampler(m_volData);	
		
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
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v1 = addVertex(regX    , regY    , regZ + 1, material, m_currentSliceVertices, m_meshCurrent);
						uint32_t v2 = addVertex(regX    , regY + 1, regZ + 1, material, m_currentSliceVertices, m_meshCurrent);
						uint32_t v3 = addVertex(regX    , regY + 1, regZ    , material, m_previousSliceVertices, m_meshCurrent);

						m_vecQuads[NegativeX][regX].push_back(Quad(v0, v1, v2, v3));
					}

					if(m_funcIsQuadNeededCallback(negXVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v1 = addVertex(regX    , regY    , regZ + 1, material, m_currentSliceVertices, m_meshCurrent);
						uint32_t v2 = addVertex(regX    , regY + 1, regZ + 1, material, m_currentSliceVertices, m_meshCurrent);
						uint32_t v3 = addVertex(regX    , regY + 1, regZ    , material, m_previousSliceVertices, m_meshCurrent);

						m_vecQuads[PositiveX][regX].push_back(Quad(v0, v3, v2, v1));
					}

					// Y
					if(m_funcIsQuadNeededCallback(currentVoxel, negYVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v1 = addVertex(regX + 1, regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v2 = addVertex(regX + 1, regY    , regZ + 1, material, m_currentSliceVertices, m_meshCurrent);
						uint32_t v3 = addVertex(regX    , regY    , regZ + 1, material, m_currentSliceVertices, m_meshCurrent);

						m_vecQuads[NegativeY][regY].push_back(Quad(v0, v1, v2, v3));
					}

					if(m_funcIsQuadNeededCallback(negYVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v1 = addVertex(regX + 1, regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v2 = addVertex(regX + 1, regY    , regZ + 1, material, m_currentSliceVertices, m_meshCurrent);
						uint32_t v3 = addVertex(regX    , regY    , regZ + 1, material, m_currentSliceVertices, m_meshCurrent);

						m_vecQuads[PositiveY][regY].push_back(Quad(v0, v3, v2, v1));
					}

					// Z
					if(m_funcIsQuadNeededCallback(currentVoxel, negZVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v1 = addVertex(regX    , regY + 1, regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v2 = addVertex(regX + 1, regY + 1, regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v3 = addVertex(regX + 1, regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);

						m_vecQuads[NegativeZ][regZ].push_back(Quad(v0, v1, v2, v3));
					}

					if(m_funcIsQuadNeededCallback(negZVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX    , regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v1 = addVertex(regX    , regY + 1, regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v2 = addVertex(regX + 1, regY + 1, regZ    , material, m_previousSliceVertices, m_meshCurrent);
						uint32_t v3 = addVertex(regX + 1, regY    , regZ    , material, m_previousSliceVertices, m_meshCurrent);

						m_vecQuads[PositiveZ][regZ].push_back(Quad(v0, v3, v2, v1));
					}

					volumeSampler.movePositiveX();
				}
			}

			m_previousSliceVertices.swap(m_currentSliceVertices);
			memset(m_currentSliceVertices.getRawData(), 0xff, m_currentSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial<VolumeType>));
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
					while (performQuadMerging(listQuads, m_meshCurrent)){}
				}

				typename std::list<Quad>::iterator iterEnd = listQuads.end();
				for(typename std::list<Quad>::iterator quadIter = listQuads.begin(); quadIter != iterEnd; quadIter++)
				{
					Quad& quad = *quadIter;				
					m_meshCurrent->addTriangle(quad.vertices[0], quad.vertices[1],quad.vertices[2]);
					m_meshCurrent->addTriangle(quad.vertices[0], quad.vertices[2],quad.vertices[3]);
				}			
			}
		}

		m_meshCurrent->setOffset(m_regSizeInVoxels.getLowerCorner());
		m_meshCurrent->removeUnusedVertices();

		POLYVOX_LOG_TRACE("Cubic surface extraction took ", timer.elapsedTimeInMilliSeconds(),
			"ms (Region size = ", m_regSizeInVoxels.getWidthInVoxels(), "x", m_regSizeInVoxels.getHeightInVoxels(),
			"x", m_regSizeInVoxels.getDepthInVoxels(), ")");
	}
}
