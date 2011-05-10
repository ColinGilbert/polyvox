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

#include "Array.h"
#include "MaterialDensityPair.h"
#include "SurfaceMesh.h"
#include "PolyVoxImpl/MarchingCubesTables.h"
#include "VertexTypes.h"

namespace PolyVox
{
	template< template<typename> class VolumeType, typename VoxelType>
	const uint32_t ImprovedCubicSurfaceExtractor<VolumeType, VoxelType>::MaxQuadsSharingVertex = 4;

	template< template<typename> class VolumeType, typename VoxelType>
	ImprovedCubicSurfaceExtractor<VolumeType, VoxelType>::ImprovedCubicSurfaceExtractor(VolumeType<VoxelType>* volData, Region region, SurfaceMesh<PositionMaterial>* result, bool bMergeQuads)
		:m_volData(volData)
		,m_sampVolume(volData)
		,m_regSizeInVoxels(region)
		,m_meshCurrent(result)
		,m_bMergeQuads(bMergeQuads)
	{
		m_meshCurrent->clear();
	}

	template< template<typename> class VolumeType, typename VoxelType>
	void ImprovedCubicSurfaceExtractor<VolumeType, VoxelType>::execute()
	{
		uint32_t uArrayWidth = m_regSizeInVoxels.getUpperCorner().getX() - m_regSizeInVoxels.getLowerCorner().getX() + 2;
		uint32_t uArrayHeight = m_regSizeInVoxels.getUpperCorner().getY() - m_regSizeInVoxels.getLowerCorner().getY() + 2;

		uint32_t arraySize[3]= {uArrayWidth, uArrayHeight, MaxQuadsSharingVertex};
		m_previousSliceVertices.resize(arraySize);
		m_currentSliceVertices.resize(arraySize);
		memset(m_previousSliceVertices.getRawData(), 0xff, m_previousSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial));
		memset(m_currentSliceVertices.getRawData(), 0xff, m_currentSliceVertices.getNoOfElements() * sizeof(IndexAndMaterial));

		uint32_t uRegionWidth  = m_regSizeInVoxels.getUpperCorner().getX() - m_regSizeInVoxels.getLowerCorner().getX() + 1;
		uint32_t uRegionHeight = m_regSizeInVoxels.getUpperCorner().getY() - m_regSizeInVoxels.getLowerCorner().getY() + 1;
		uint32_t uRegionDepth  = m_regSizeInVoxels.getUpperCorner().getZ() - m_regSizeInVoxels.getLowerCorner().getZ() + 1;
		m_faces.resize(ArraySizes(uRegionWidth)(uRegionHeight)(uRegionDepth)(NoOfFaces));
		memset(m_faces.getRawData(), 0x00, m_faces.getNoOfElements() * sizeof(uint8_t)); //Note: hard-coded type uint8_t

		m_vecQuads[NegativeX].resize(m_regSizeInVoxels.getUpperCorner().getX() - m_regSizeInVoxels.getLowerCorner().getX() + 2);
		m_vecQuads[PositiveX].resize(m_regSizeInVoxels.getUpperCorner().getX() - m_regSizeInVoxels.getLowerCorner().getX() + 2);

		m_vecQuads[NegativeY].resize(m_regSizeInVoxels.getUpperCorner().getY() - m_regSizeInVoxels.getLowerCorner().getY() + 2);
		m_vecQuads[PositiveY].resize(m_regSizeInVoxels.getUpperCorner().getY() - m_regSizeInVoxels.getLowerCorner().getY() + 2);

		m_vecQuads[NegativeZ].resize(m_regSizeInVoxels.getUpperCorner().getZ() - m_regSizeInVoxels.getLowerCorner().getZ() + 2);
		m_vecQuads[PositiveZ].resize(m_regSizeInVoxels.getUpperCorner().getZ() - m_regSizeInVoxels.getLowerCorner().getZ() + 2);
		
		
		for(int32_t z = m_regSizeInVoxels.getLowerCorner().getZ(); z <= m_regSizeInVoxels.getUpperCorner().getZ() + 1; z++)
		{
			for(int32_t y = m_regSizeInVoxels.getLowerCorner().getY(); y <= m_regSizeInVoxels.getUpperCorner().getY() + 1; y++)
			{
				for(int32_t x = m_regSizeInVoxels.getLowerCorner().getX(); x <= m_regSizeInVoxels.getUpperCorner().getX() + 1; x++)
				{
					// these are always positive anyway
					uint32_t regX = x - m_regSizeInVoxels.getLowerCorner().getX();
					uint32_t regY = y - m_regSizeInVoxels.getLowerCorner().getY();
					uint32_t regZ = z - m_regSizeInVoxels.getLowerCorner().getZ();

					bool finalX = (x == m_regSizeInVoxels.getUpperCorner().getX() + 1);
					bool finalY = (y == m_regSizeInVoxels.getUpperCorner().getY() + 1);
					bool finalZ = (z == m_regSizeInVoxels.getUpperCorner().getZ() + 1);

					VoxelType currentVoxel = m_volData->getVoxelAt(x,y,z);
					bool currentVoxelIsSolid = currentVoxel.getDensity() >= VoxelType::getThreshold();

					VoxelType negXVoxel = m_volData->getVoxelAt(x-1,y,z);
					bool negXVoxelIsSolid = negXVoxel.getDensity()  >= VoxelType::getThreshold();

					if((currentVoxelIsSolid != negXVoxelIsSolid) && (finalY == false) && (finalZ == false))
					{
						int material = (std::max)(currentVoxel.getMaterial(), negXVoxel.getMaterial());

						// Check to ensure that when a voxel solid/non-solid change is right on a region border, the vertices are generated on the solid side of the region border
						if(((currentVoxelIsSolid > negXVoxelIsSolid) && finalX == false) || ((currentVoxelIsSolid < negXVoxelIsSolid) && regX != 0))
						{
							uint32_t v0 = addVertex(regX - 0.5f, regY - 0.5f, regZ - 0.5f, material, m_previousSliceVertices);
							uint32_t v1 = addVertex(regX - 0.5f, regY - 0.5f, regZ + 0.5f, material, m_currentSliceVertices);	
							uint32_t v2 = addVertex(regX - 0.5f, regY + 0.5f, regZ + 0.5f, material, m_currentSliceVertices);							
							uint32_t v3 = addVertex(regX - 0.5f, regY + 0.5f, regZ - 0.5f, material, m_previousSliceVertices);

							if(currentVoxelIsSolid > negXVoxelIsSolid)
							{
								Quad quad;
								quad.vertices[0] = v0;
								quad.vertices[1] = v1;
								quad.vertices[2] = v2;
								quad.vertices[3] = v3;
								quad.material = material;

								m_vecQuads[NegativeX][regX].push_back(quad);
							}
							else											
							{
								Quad quad;
								quad.vertices[0] = v0;
								quad.vertices[1] = v3;
								quad.vertices[2] = v2;
								quad.vertices[3] = v1;
								quad.material = material;

								m_vecQuads[PositiveX][regX].push_back(quad);
							}

						}
					}

					VoxelType negYVoxel = m_volData->getVoxelAt(x,y-1,z);
					bool negYVoxelIsSolid = negYVoxel.getDensity()  >= VoxelType::getThreshold();

					if((currentVoxelIsSolid != negYVoxelIsSolid) && (finalX == false) && (finalZ == false))
					{
						int material = (std::max)(currentVoxel.getMaterial(),negYVoxel.getMaterial());

						if(((currentVoxelIsSolid > negYVoxelIsSolid) && finalY == false) || ((currentVoxelIsSolid < negYVoxelIsSolid) && regY != 0))
						{
							uint32_t v0 = addVertex(regX - 0.5f, regY - 0.5f, regZ - 0.5f, material, m_previousSliceVertices);
							uint32_t v1 = addVertex(regX - 0.5f, regY - 0.5f, regZ + 0.5f, material, m_currentSliceVertices);							
							uint32_t v2 = addVertex(regX + 0.5f, regY - 0.5f, regZ + 0.5f, material, m_currentSliceVertices);
							uint32_t v3 = addVertex(regX + 0.5f, regY - 0.5f, regZ - 0.5f, material, m_previousSliceVertices);

							if(currentVoxelIsSolid > negYVoxelIsSolid)
							{
								//NOTE: For some reason y windong is opposite of X and Z. Investigate this...
								Quad quad;
								quad.vertices[0] = v0;
								quad.vertices[1] = v3;
								quad.vertices[2] = v2;
								quad.vertices[3] = v1;
								quad.material = material;

								m_vecQuads[NegativeY][regY].push_back(quad);
							}
							else
							{
								//NOTE: For some reason y windong is opposite of X and Z. Investigate this...
								Quad quad;
								quad.vertices[0] = v0;
								quad.vertices[1] = v1;
								quad.vertices[2] = v2;
								quad.vertices[3] = v3;
								quad.material = material;

								m_vecQuads[PositiveY][regY].push_back(quad);
							}
						}
					}

					VoxelType negZVoxel = m_volData->getVoxelAt(x,y,z-1);
					bool negZVoxelIsSolid = negZVoxel.getDensity()  >= VoxelType::getThreshold();

					if((currentVoxelIsSolid != negZVoxelIsSolid) && (finalX == false) && (finalY == false))
					{
						int material = (std::max)(currentVoxel.getMaterial(), negZVoxel.getMaterial());

						if(((currentVoxelIsSolid > negZVoxelIsSolid) && finalZ == false) || ((currentVoxelIsSolid < negZVoxelIsSolid) && regZ != 0))
						{
							uint32_t v0 = addVertex(regX - 0.5f, regY - 0.5f, regZ - 0.5f, material, m_previousSliceVertices);
							uint32_t v1 = addVertex(regX - 0.5f, regY + 0.5f, regZ - 0.5f, material, m_previousSliceVertices);
							uint32_t v2 = addVertex(regX + 0.5f, regY + 0.5f, regZ - 0.5f, material, m_previousSliceVertices);
							uint32_t v3 = addVertex(regX + 0.5f, regY - 0.5f, regZ - 0.5f, material, m_previousSliceVertices);
							
							
	
							if(currentVoxelIsSolid > negZVoxelIsSolid)
							{
								Quad quad;
								quad.vertices[0] = v0;
								quad.vertices[1] = v1;
								quad.vertices[2] = v2;
								quad.vertices[3] = v3;
								quad.material = material;

								m_vecQuads[NegativeZ][regZ].push_back(quad);
							}
							else
							{
								Quad quad;
								quad.vertices[0] = v0;
								quad.vertices[1] = v3;
								quad.vertices[2] = v2;
								quad.vertices[3] = v1;
								quad.material = material;

								m_vecQuads[PositiveZ][regZ].push_back(quad);
							}
						}
					}
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
					while(decimate(listQuads)){}
				}

				std::list<Quad>::iterator iterEnd = listQuads.end();
				for(std::list<Quad>::iterator quadIter = listQuads.begin(); quadIter != iterEnd; quadIter++)
				{
					Quad& quad = *quadIter;				
					m_meshCurrent->addTriangleCubic(quad.vertices[0], quad.vertices[1],quad.vertices[2]);
					m_meshCurrent->addTriangleCubic(quad.vertices[0], quad.vertices[2],quad.vertices[3]);
				}			
			}
		}

		m_meshCurrent->m_Region = m_regSizeInVoxels;

		m_meshCurrent->m_vecLodRecords.clear();
		LodRecord lodRecord;
		lodRecord.beginIndex = 0;
		lodRecord.endIndex = m_meshCurrent->getNoOfIndices();
		m_meshCurrent->m_vecLodRecords.push_back(lodRecord);
	}

	template< template<typename> class VolumeType, typename VoxelType>
	int32_t ImprovedCubicSurfaceExtractor<VolumeType, VoxelType>::addVertex(float fX, float fY, float fZ, uint8_t uMaterialIn, Array<3, IndexAndMaterial>& existingVertices)
	{
		uint32_t uX = static_cast<uint32_t>(fX + 0.75f);
		uint32_t uY = static_cast<uint32_t>(fY + 0.75f);

		for(uint32_t ct = 0; ct < MaxQuadsSharingVertex; ct++)
		{
			IndexAndMaterial& rEntry = existingVertices[uX][uY][ct];

			int32_t iIndex = static_cast<int32_t>(rEntry.iIndex);
			uint8_t uMaterial = static_cast<uint8_t>(rEntry.uMaterial);

			//If we have an existing vertex and the material matches then we can return it.
			if((iIndex != -1) && (uMaterial == uMaterialIn))
			{
				return iIndex;
			}
			else
			{
				//No vertices matched and we've now hit an empty space. Fill it by creating a vertex.
				uint32_t temp = m_meshCurrent->addVertex(PositionMaterial(Vector3DFloat(fX, fY, fZ), uMaterialIn));

				//Note - Slightly dodgy casting taking place here. No proper way to convert to 24-bit int though?
				//If problematic in future then fix IndexAndMaterial to contain variables rather than bitfield.
				rEntry.iIndex = temp;
				rEntry.uMaterial = uMaterialIn;

				return temp;
			}
		}

		//If we exit the loop here then apparently all the slots were full but none of
		//them matched. I don't think this can happen so let's put an assert to make sure.
		assert(false);
		return 0;
	}

	template< template<typename> class VolumeType, typename VoxelType>
	bool ImprovedCubicSurfaceExtractor<VolumeType, VoxelType>::decimate(std::list<Quad>& quads)
	{
		for(std::list<Quad>::iterator outerIter = quads.begin(); outerIter != quads.end(); outerIter++)
		{
			std::list<Quad>::iterator innerIter = outerIter;
			innerIter++;
			while(innerIter != quads.end())
			{
				Quad q1 = *outerIter;
				Quad q2 = *innerIter;

				if(canMergeQuads(q1, q2))
				{
					//std::cout << "Can merge" << std::endl;
					Quad result = mergeQuads(q1,q2);

					quads.erase(innerIter);
					quads.erase(outerIter);

					quads.push_back(result);

					return true;
				}

				innerIter++;
			}
		}

		return false;
		//std::cout << "Can not merge" << std::endl;
	}

	template< template<typename> class VolumeType, typename VoxelType>
	Quad ImprovedCubicSurfaceExtractor<VolumeType, VoxelType>::mergeQuads(const Quad& q1, const Quad& q2)
	{
		Quad result;
		result.material = q1.material;

		for(uint32_t vertex = 0; vertex < 4; vertex++)
		{
			uint32_t quad1vertex = q1.vertices[vertex];
			uint32_t quad2vertex = q2.vertices[vertex];

			if(quadContainsVertex(q2, quad1vertex) != -1)
			{
				result.vertices[vertex] = quad2vertex;
			}
			else
			{
				result.vertices[vertex] = quad1vertex;
			}
		}

		return result;
	}

	template< template<typename> class VolumeType, typename VoxelType>
	bool ImprovedCubicSurfaceExtractor<VolumeType, VoxelType>::canMergeQuads(const Quad& q1, const Quad& q2)
	{
		if(q1.material != q2.material)
		{
			return false;
		}

		uint32_t uNoOfMatchingVertices = 0;
		for(uint32_t uQuad1Index = 0; uQuad1Index < 4; uQuad1Index++)
		{
			if(quadContainsVertex(q2, q1.vertices[uQuad1Index]) != -1)
			{
				uNoOfMatchingVertices++;
			}
		}

		return uNoOfMatchingVertices == 2;
	}

	template< template<typename> class VolumeType, typename VoxelType>
	int32_t ImprovedCubicSurfaceExtractor<VolumeType, VoxelType>::quadContainsVertex(const Quad& quad, uint32_t uVertexIndex)
	{
		for(uint32_t ct = 0; ct < 4; ct++)
		{
			if(quad.vertices[ct] == uVertexIndex)
			{
				//We've found a matching vertex.
				return ct;
			}
		}

		//Vertex not found.
		return -1;
	}
}