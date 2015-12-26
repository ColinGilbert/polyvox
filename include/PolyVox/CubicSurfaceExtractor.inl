/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 David Williams and Matthew Williams
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/

#include "Impl/Timer.h"

namespace PolyVox
{
	template<typename VolumeType, typename MeshType, typename IsQuadNeeded>
	CubicSurfaceExtractor<VolumeType, MeshType, IsQuadNeeded>::CubicSurfaceExtractor(VolumeType* volData, Region region, MeshType* result, IsQuadNeeded isQuadNeeded, bool bMergeQuads)
	{
		// This extractor has a limit as to how large the extracted region can be, because the vertex positions are encoded with a single byte per component.
		int32_t maxReionDimensionInVoxels = 255;
		POLYVOX_THROW_IF(region.getWidthInVoxels() > maxReionDimensionInVoxels, std::invalid_argument, "Requested extraction region exceeds maximum dimensions");
		POLYVOX_THROW_IF(region.getHeightInVoxels() > maxReionDimensionInVoxels, std::invalid_argument, "Requested extraction region exceeds maximum dimensions");
		POLYVOX_THROW_IF(region.getDepthInVoxels() > maxReionDimensionInVoxels, std::invalid_argument, "Requested extraction region exceeds maximum dimensions");

		Timer timer;
		result->clear();

		//Used to avoid creating duplicate vertices.
		Array<3, IndexAndMaterial<VolumeType> > m_previousSliceVertices(region.getUpperX() - region.getLowerX() + 2, region.getUpperY() - region.getLowerY() + 2, MaxVerticesPerPosition);
		Array<3, IndexAndMaterial<VolumeType> > m_currentSliceVertices(region.getUpperX() - region.getLowerX() + 2, region.getUpperY() - region.getLowerY() + 2, MaxVerticesPerPosition);

		//During extraction we create a number of different lists of quads. All the 
		//quads in a given list are in the same plane and facing in the same direction.
		std::vector< std::list<Quad> > m_vecQuads[NoOfFaces];


		memset(m_previousSliceVertices.getRawData(), 0xff, m_previousSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial<VolumeType>));
		memset(m_currentSliceVertices.getRawData(), 0xff, m_currentSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial<VolumeType>));

		m_vecQuads[NegativeX].resize(region.getUpperX() - region.getLowerX() + 2);
		m_vecQuads[PositiveX].resize(region.getUpperX() - region.getLowerX() + 2);

		m_vecQuads[NegativeY].resize(region.getUpperY() - region.getLowerY() + 2);
		m_vecQuads[PositiveY].resize(region.getUpperY() - region.getLowerY() + 2);

		m_vecQuads[NegativeZ].resize(region.getUpperZ() - region.getLowerZ() + 2);
		m_vecQuads[PositiveZ].resize(region.getUpperZ() - region.getLowerZ() + 2);

		typename VolumeType::Sampler volumeSampler(volData);

		for (int32_t z = region.getLowerZ(); z <= region.getUpperZ(); z++)
		{
			uint32_t regZ = z - region.getLowerZ();

			for (int32_t y = region.getLowerY(); y <= region.getUpperY(); y++)
			{
				uint32_t regY = y - region.getLowerY();

				volumeSampler.setPosition(region.getLowerX(), y, z);

				for (int32_t x = region.getLowerX(); x <= region.getUpperX(); x++)
				{
					uint32_t regX = x - region.getLowerX();

					typename VolumeType::VoxelType material; //Filled in by callback
					typename VolumeType::VoxelType currentVoxel = volumeSampler.getVoxel();
					typename VolumeType::VoxelType negXVoxel = volumeSampler.peekVoxel1nx0py0pz();
					typename VolumeType::VoxelType negYVoxel = volumeSampler.peekVoxel0px1ny0pz();
					typename VolumeType::VoxelType negZVoxel = volumeSampler.peekVoxel0px0py1nz();

					// X
					if (isQuadNeeded(currentVoxel, negXVoxel, material))
					{
						uint32_t v0 = addVertex(regX, regY, regZ, material, m_previousSliceVertices, result);
						uint32_t v1 = addVertex(regX, regY, regZ + 1, material, m_currentSliceVertices, result);
						uint32_t v2 = addVertex(regX, regY + 1, regZ + 1, material, m_currentSliceVertices, result);
						uint32_t v3 = addVertex(regX, regY + 1, regZ, material, m_previousSliceVertices, result);

						m_vecQuads[NegativeX][regX].push_back(Quad(v0, v1, v2, v3));
					}

					if (isQuadNeeded(negXVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX, regY, regZ, material, m_previousSliceVertices, result);
						uint32_t v1 = addVertex(regX, regY, regZ + 1, material, m_currentSliceVertices, result);
						uint32_t v2 = addVertex(regX, regY + 1, regZ + 1, material, m_currentSliceVertices, result);
						uint32_t v3 = addVertex(regX, regY + 1, regZ, material, m_previousSliceVertices, result);

						m_vecQuads[PositiveX][regX].push_back(Quad(v0, v3, v2, v1));
					}

					// Y
					if (isQuadNeeded(currentVoxel, negYVoxel, material))
					{
						uint32_t v0 = addVertex(regX, regY, regZ, material, m_previousSliceVertices, result);
						uint32_t v1 = addVertex(regX + 1, regY, regZ, material, m_previousSliceVertices, result);
						uint32_t v2 = addVertex(regX + 1, regY, regZ + 1, material, m_currentSliceVertices, result);
						uint32_t v3 = addVertex(regX, regY, regZ + 1, material, m_currentSliceVertices, result);

						m_vecQuads[NegativeY][regY].push_back(Quad(v0, v1, v2, v3));
					}

					if (isQuadNeeded(negYVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX, regY, regZ, material, m_previousSliceVertices, result);
						uint32_t v1 = addVertex(regX + 1, regY, regZ, material, m_previousSliceVertices, result);
						uint32_t v2 = addVertex(regX + 1, regY, regZ + 1, material, m_currentSliceVertices, result);
						uint32_t v3 = addVertex(regX, regY, regZ + 1, material, m_currentSliceVertices, result);

						m_vecQuads[PositiveY][regY].push_back(Quad(v0, v3, v2, v1));
					}

					// Z
					if (isQuadNeeded(currentVoxel, negZVoxel, material))
					{
						uint32_t v0 = addVertex(regX, regY, regZ, material, m_previousSliceVertices, result);
						uint32_t v1 = addVertex(regX, regY + 1, regZ, material, m_previousSliceVertices, result);
						uint32_t v2 = addVertex(regX + 1, regY + 1, regZ, material, m_previousSliceVertices, result);
						uint32_t v3 = addVertex(regX + 1, regY, regZ, material, m_previousSliceVertices, result);

						m_vecQuads[NegativeZ][regZ].push_back(Quad(v0, v1, v2, v3));
					}

					if (isQuadNeeded(negZVoxel, currentVoxel, material))
					{
						uint32_t v0 = addVertex(regX, regY, regZ, material, m_previousSliceVertices, result);
						uint32_t v1 = addVertex(regX, regY + 1, regZ, material, m_previousSliceVertices, result);
						uint32_t v2 = addVertex(regX + 1, regY + 1, regZ, material, m_previousSliceVertices, result);
						uint32_t v3 = addVertex(regX + 1, regY, regZ, material, m_previousSliceVertices, result);

						m_vecQuads[PositiveZ][regZ].push_back(Quad(v0, v3, v2, v1));
					}

					volumeSampler.movePositiveX();
				}
			}

			m_previousSliceVertices.swap(m_currentSliceVertices);
			memset(m_currentSliceVertices.getRawData(), 0xff, m_currentSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial<VolumeType>));
		}

		for (uint32_t uFace = 0; uFace < NoOfFaces; uFace++)
		{
			std::vector< std::list<Quad> >& vecListQuads = m_vecQuads[uFace];

			for (uint32_t slice = 0; slice < vecListQuads.size(); slice++)
			{
				std::list<Quad>& listQuads = vecListQuads[slice];

				if (bMergeQuads)
				{
					//Repeatedly call this function until it returns
					//false to indicate nothing more can be done.
					while (performQuadMerging(listQuads, result)){}
				}

				typename std::list<Quad>::iterator iterEnd = listQuads.end();
				for (typename std::list<Quad>::iterator quadIter = listQuads.begin(); quadIter != iterEnd; quadIter++)
				{
					Quad& quad = *quadIter;
					result->addTriangle(quad.vertices[0], quad.vertices[1], quad.vertices[2]);
					result->addTriangle(quad.vertices[0], quad.vertices[2], quad.vertices[3]);
				}
			}
		}

		result->setOffset(region.getLowerCorner());
		result->removeUnusedVertices();

		POLYVOX_LOG_TRACE("Cubic surface extraction took ", timer.elapsedTimeInMilliSeconds(),
			"ms (Region size = ", m_regSizeInVoxels.getWidthInVoxels(), "x", m_regSizeInVoxels.getHeightInVoxels(),
			"x", m_regSizeInVoxels.getDepthInVoxels(), ")");
	}
}
