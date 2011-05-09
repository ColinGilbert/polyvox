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

#ifndef __PolyVox_ImprovedCubicSurfaceExtractor_H__
#define __PolyVox_ImprovedCubicSurfaceExtractor_H__

#include "Array.h"
#include "PolyVoxForwardDeclarations.h"
#include "LargeVolume.h"

#include "PolyVoxImpl/TypeDef.h"

namespace PolyVox
{
	struct IndexAndMaterial
	{
		int32_t iIndex    : 24;
		int32_t uMaterial : 8;
	};

	enum FaceNames
	{
		PositiveX,
		PositiveY,
		PositiveZ,
		NegativeX,
		NegativeY,
		NegativeZ,
		NoOfFaces
	};

	struct Quad
	{
		uint32_t vertices[4];
		uint8_t material; //Shouldn't hard code to uint8_t type?
	};

	template< template<typename> class VolumeType, typename VoxelType>
	class ImprovedCubicSurfaceExtractor
	{
	public:
		ImprovedCubicSurfaceExtractor(VolumeType<VoxelType>* volData, Region region, SurfaceMesh<PositionMaterial>* result);

		void execute();

		int32_t addVertex(float fX, float fY, float fZ, uint8_t uMaterial, Array<3, IndexAndMaterial>& existingVertices);

	private:
		//The volume data and a sampler to access it.
		VolumeType<VoxelType>* m_volData;
		typename VolumeType<VoxelType>::Sampler m_sampVolume;

		//Information about the region we are currently processing
		Region m_regSizeInVoxels;

		//The surface patch we are currently filling.
		SurfaceMesh<PositionMaterial>* m_meshCurrent;

		//Array<4, IndexAndMaterial> m_vertices;
		Array<3, IndexAndMaterial> m_previousSliceVertices;
		Array<3, IndexAndMaterial> m_currentSliceVertices;

		Array<4, uint8_t> m_faces;

		std::vector< std::list<Quad> > m_vecNegXQuads;
		std::vector< std::list<Quad> > m_vecPosXQuads;

		std::vector< std::list<Quad> > m_vecNegYQuads;
		std::vector< std::list<Quad> > m_vecPosYQuads;

		std::vector< std::list<Quad> > m_vecNegZQuads;
		std::vector< std::list<Quad> > m_vecPosZQuads;

		//Although we try to avoid creating multiple vertices at the same location, sometimes this is unavoidable
		//if they have different materials. For example, four different materials next to each other would mean
		//four quads (though more triangles) sharing the vertex. As far as I can tell, four is the worst case scenario.
		static const uint32_t MaxQuadsSharingVertex;

		////////////////////////////////////////////////////////////////////////////////
		// Decimation
		////////////////////////////////////////////////////////////////////////////////

		/*class Triangle
		{
			uint32_t v0;
			uint32_t v1;
			uint32_t v2;
		}

		std::vector<Triangle> triangles;
		std::vector< std::vector<uint32_t> > trianglesUsingVertex;*/

		//std::vector<Quad> decimate(const std::vector<Quad>& quads);

		bool decimate(std::list<Quad>& quads);

		Quad mergeQuads(Quad q1, Quad q2);

		bool canMergeQuads(Quad q1, Quad q2);

		int32_t quadContainsVertex(const Quad& quad, uint32_t uVertexIndex);

	};
}

#include "ImprovedCubicSurfaceExtractor.inl"

#endif
