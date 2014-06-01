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

#ifndef __PolyVox_SurfaceExtractor_H__
#define __PolyVox_SurfaceExtractor_H__

#include "Impl/MarchingCubesTables.h"
#include "Impl/TypeDef.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/BaseVolume.h" //For wrap modes... should move these?
#include "PolyVoxCore/Mesh.h"
#include "PolyVoxCore/DefaultMarchingCubesController.h"
#include "PolyVoxCore/VertexTypes.h"

namespace PolyVox
{
#ifdef SWIG
	struct MarchingCubesVertex
#else
	template<typename _DataType>
	struct POLYVOX_API MarchingCubesVertex
#endif
	{
		typedef _DataType DataType;

		Vector3DUint16 position;
		uint16_t normal;
		DataType data;
	};

	Vector3DFloat decode(const Vector3DUint16& position)
	{
		Vector3DFloat result(position.getX(), position.getY(), position.getZ());
		result *= (1.0f / 256.0f); // Division is compile-time constant
		return result;
	}

	Vector3DFloat decode(const uint16_t normal)
	{
		uint16_t x = (normal >> 10) & 0x1F;
		uint16_t y = (normal >> 5) & 0x1F;
		uint16_t z = (normal) & 0x1F;

		Vector3DFloat result(x, y, z);
		result *= (1.0f / 15.5f); // Division is compile-time constant
		result -= Vector3DFloat(1.0f, 1.0f, 1.0f);

		return result;
	}

	template<typename DataType>
	Vertex<DataType> decode(const MarchingCubesVertex<DataType>& marchingCubesVertex)
	{
		Vertex<DataType> result;
		result.position = decode(marchingCubesVertex.position);
		result.normal = decode(marchingCubesVertex.normal);
		result.data = marchingCubesVertex.data; // Data is not encoded
		return result;
	}

	template< typename VolumeType, typename Controller = DefaultMarchingCubesController<typename VolumeType::VoxelType> >
	class MarchingCubesSurfaceExtractor
	{
	public:
		// This is a bit ugly - it seems that the C++03 syntax is different from the C++11 syntax? See this thread: http://stackoverflow.com/questions/6076015/typename-outside-of-template
		// Long term we should probably come back to this and if the #ifdef is still needed then maybe it should check for C++11 mode instead of MSVC? 
#if defined(_MSC_VER)
		MarchingCubesSurfaceExtractor(VolumeType* volData, Region region, Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> >* result, WrapMode eWrapMode = WrapModes::Border, typename VolumeType::VoxelType tBorderValue = VolumeType::VoxelType(), Controller controller = Controller());
#else
		MarchingCubesSurfaceExtractor(VolumeType* volData, Region region, Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> >* result, WrapMode eWrapMode = WrapModes::Border, typename VolumeType::VoxelType tBorderValue = typename VolumeType::VoxelType(), Controller controller = Controller());
#endif

		void execute();

	private:
		//Compute the cell bitmask for a particular slice in z.
		template<bool isPrevZAvail>
		uint32_t computeBitmaskForSlice(const Array2DUint8& pPreviousBitmask, Array2DUint8& pCurrentBitmask);

		//Compute the cell bitmask for a given cell.
		template<bool isPrevXAvail, bool isPrevYAvail, bool isPrevZAvail>
		void computeBitmaskForCell(const Array2DUint8& pPreviousBitmask, Array2DUint8& pCurrentBitmask, uint32_t uXRegSpace, uint32_t uYRegSpace);

		//Use the cell bitmasks to generate all the vertices needed for that slice
		void generateVerticesForSlice(const Array2DUint8& pCurrentBitmask,
			Array2DInt32& m_pCurrentVertexIndicesX,
			Array2DInt32& m_pCurrentVertexIndicesY,
			Array2DInt32& m_pCurrentVertexIndicesZ);

		////////////////////////////////////////////////////////////////////////////////
		// NOTE: These two functions are in the .h file rather than the .inl due to an apparent bug in VC2010.
		//See http://stackoverflow.com/questions/1484885/strange-vc-compile-error-c2244 for details.
		////////////////////////////////////////////////////////////////////////////////
		Vector3DFloat computeCentralDifferenceGradient(const typename VolumeType::Sampler& volIter)
		{
			//FIXME - Should actually use DensityType here, both in principle and because the maths may be
			//faster (and to reduce casts). So it would be good to add a way to get DensityType from a voxel.
			//But watch out for when the DensityType is unsigned and the difference could be negative.
			float voxel1nx = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx0py0pz()));
			float voxel1px = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px0py0pz()));

			float voxel1ny = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1ny0pz()));
			float voxel1py = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1py0pz()));

			float voxel1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py1nz()));
			float voxel1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py1pz()));

			return Vector3DFloat
			(
				voxel1nx - voxel1px,
				voxel1ny - voxel1py,
				voxel1nz - voxel1pz
			);
		}

		Vector3DFloat computeSobelGradient(const typename VolumeType::Sampler& volIter)
		{
			static const int weights[3][3][3] = {  {  {2,3,2}, {3,6,3}, {2,3,2}  },  {
				{3,6,3},  {6,0,6},  {3,6,3} },  { {2,3,2},  {3,6,3},  {2,3,2} } };

				//FIXME - Should actually use DensityType here, both in principle and because the maths may be
				//faster (and to reduce casts). So it would be good to add a way to get DensityType from a voxel.
				//But watch out for when the DensityType is unsigned and the difference could be negative.
				const float pVoxel1nx1ny1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1ny1nz()));
				const float pVoxel1nx1ny0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1ny0pz()));
				const float pVoxel1nx1ny1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1ny1pz()));
				const float pVoxel1nx0py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx0py1nz()));
				const float pVoxel1nx0py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx0py0pz()));
				const float pVoxel1nx0py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx0py1pz()));
				const float pVoxel1nx1py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1py1nz()));
				const float pVoxel1nx1py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1py0pz()));
				const float pVoxel1nx1py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1nx1py1pz()));

				const float pVoxel0px1ny1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1ny1nz()));
				const float pVoxel0px1ny0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1ny0pz()));
				const float pVoxel0px1ny1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1ny1pz()));
				const float pVoxel0px0py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py1nz()));
				//const float pVoxel0px0py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py0pz()));
				const float pVoxel0px0py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px0py1pz()));
				const float pVoxel0px1py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1py1nz()));
				const float pVoxel0px1py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1py0pz()));
				const float pVoxel0px1py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel0px1py1pz()));

				const float pVoxel1px1ny1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1ny1nz()));
				const float pVoxel1px1ny0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1ny0pz()));
				const float pVoxel1px1ny1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1ny1pz()));
				const float pVoxel1px0py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px0py1nz()));
				const float pVoxel1px0py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px0py0pz()));
				const float pVoxel1px0py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px0py1pz()));
				const float pVoxel1px1py1nz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1py1nz()));
				const float pVoxel1px1py0pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1py0pz()));
				const float pVoxel1px1py1pz = static_cast<float>(m_controller.convertToDensity(volIter.peekVoxel1px1py1pz()));

				const float xGrad(- weights[0][0][0] * pVoxel1nx1ny1nz -
					weights[1][0][0] * pVoxel1nx1ny0pz - weights[2][0][0] *
					pVoxel1nx1ny1pz - weights[0][1][0] * pVoxel1nx0py1nz -
					weights[1][1][0] * pVoxel1nx0py0pz - weights[2][1][0] *
					pVoxel1nx0py1pz - weights[0][2][0] * pVoxel1nx1py1nz -
					weights[1][2][0] * pVoxel1nx1py0pz - weights[2][2][0] *
					pVoxel1nx1py1pz + weights[0][0][2] * pVoxel1px1ny1nz +
					weights[1][0][2] * pVoxel1px1ny0pz + weights[2][0][2] *
					pVoxel1px1ny1pz + weights[0][1][2] * pVoxel1px0py1nz +
					weights[1][1][2] * pVoxel1px0py0pz + weights[2][1][2] *
					pVoxel1px0py1pz + weights[0][2][2] * pVoxel1px1py1nz +
					weights[1][2][2] * pVoxel1px1py0pz + weights[2][2][2] *
					pVoxel1px1py1pz);

				const float yGrad(- weights[0][0][0] * pVoxel1nx1ny1nz -
					weights[1][0][0] * pVoxel1nx1ny0pz - weights[2][0][0] *
					pVoxel1nx1ny1pz + weights[0][2][0] * pVoxel1nx1py1nz +
					weights[1][2][0] * pVoxel1nx1py0pz + weights[2][2][0] *
					pVoxel1nx1py1pz - weights[0][0][1] * pVoxel0px1ny1nz -
					weights[1][0][1] * pVoxel0px1ny0pz - weights[2][0][1] *
					pVoxel0px1ny1pz + weights[0][2][1] * pVoxel0px1py1nz +
					weights[1][2][1] * pVoxel0px1py0pz + weights[2][2][1] *
					pVoxel0px1py1pz - weights[0][0][2] * pVoxel1px1ny1nz -
					weights[1][0][2] * pVoxel1px1ny0pz - weights[2][0][2] *
					pVoxel1px1ny1pz + weights[0][2][2] * pVoxel1px1py1nz +
					weights[1][2][2] * pVoxel1px1py0pz + weights[2][2][2] *
					pVoxel1px1py1pz);

				const float zGrad(- weights[0][0][0] * pVoxel1nx1ny1nz +
					weights[2][0][0] * pVoxel1nx1ny1pz - weights[0][1][0] *
					pVoxel1nx0py1nz + weights[2][1][0] * pVoxel1nx0py1pz -
					weights[0][2][0] * pVoxel1nx1py1nz + weights[2][2][0] *
					pVoxel1nx1py1pz - weights[0][0][1] * pVoxel0px1ny1nz +
					weights[2][0][1] * pVoxel0px1ny1pz - weights[0][1][1] *
					pVoxel0px0py1nz + weights[2][1][1] * pVoxel0px0py1pz -
					weights[0][2][1] * pVoxel0px1py1nz + weights[2][2][1] *
					pVoxel0px1py1pz - weights[0][0][2] * pVoxel1px1ny1nz +
					weights[2][0][2] * pVoxel1px1ny1pz - weights[0][1][2] *
					pVoxel1px0py1nz + weights[2][1][2] * pVoxel1px0py1pz -
					weights[0][2][2] * pVoxel1px1py1nz + weights[2][2][2] *
					pVoxel1px1py1pz);

				//Note: The above actually give gradients going from low density to high density.
				//For our normals we want the the other way around, so we switch the components as we return them.
				return Vector3DFloat(-xGrad,-yGrad,-zGrad);
		}
		////////////////////////////////////////////////////////////////////////////////
		// End of compiler bug workaroumd.
		////////////////////////////////////////////////////////////////////////////////

		//Use the cell bitmasks to generate all the indices needed for that slice
		void generateIndicesForSlice(const Array2DUint8& pPreviousBitmask,
			const Array2DInt32& m_pPreviousVertexIndicesX,
			const Array2DInt32& m_pPreviousVertexIndicesY,
			const Array2DInt32& m_pPreviousVertexIndicesZ,
			const Array2DInt32& m_pCurrentVertexIndicesX,
			const Array2DInt32& m_pCurrentVertexIndicesY);

		//The volume data and a sampler to access it.
		VolumeType* m_volData;
		typename VolumeType::Sampler m_sampVolume;

		//Used to return the number of cells in a slice which contain triangles.
		uint32_t m_uNoOfOccupiedCells;

		//The surface patch we are currently filling.
		Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> >* m_meshCurrent;

		//Information about the region we are currently processing
		Region m_regSizeInVoxels;
		Region m_regSizeInCells;
		/*Region m_regSizeInVoxelsCropped;
		Region m_regSizeInVoxelsUncropped;
		Region m_regVolumeCropped;*/
		Region m_regSlicePrevious;
		Region m_regSliceCurrent;

		//Used to convert arbitrary voxel types in densities and materials.
		Controller m_controller;

		//Our threshold value
		typename Controller::DensityType m_tThreshold;
	};

	template< typename VolumeType, typename Controller>
	Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > extractMarchingCubesMesh(VolumeType* volData, Region region, WrapMode eWrapMode, typename VolumeType::VoxelType tBorderValue, Controller controller)
	{
		Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > result;
		MarchingCubesSurfaceExtractor<VolumeType, Controller> extractor(volData, region, &result, eWrapMode, tBorderValue, controller);
		extractor.execute();
		return result;
	}

	template< typename VolumeType>
	// This is a bit ugly - it seems that the C++03 syntax is different from the C++11 syntax? See this thread: http://stackoverflow.com/questions/6076015/typename-outside-of-template
	// Long term we should probably come back to this and if the #ifdef is still needed then maybe it should check for C++11 mode instead of MSVC? 
#if defined(_MSC_VER)
	Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > extractMarchingCubesMesh(VolumeType* volData, Region region, WrapMode eWrapMode = WrapModes::Border, typename VolumeType::VoxelType tBorderValue = VolumeType::VoxelType())
#else
	Mesh<MarchingCubesVertex<typename VolumeType::VoxelType> > extractMarchingCubesMesh(VolumeType* volData, Region region, WrapMode eWrapMode = WrapModes::Border, typename VolumeType::VoxelType tBorderValue = typename VolumeType::VoxelType())
#endif
	{
		DefaultMarchingCubesController<typename VolumeType::VoxelType> controller;
		return extractMarchingCubesMesh(volData, region, eWrapMode, tBorderValue, controller);
	}
}

#include "PolyVoxCore/MarchingCubesSurfaceExtractor.inl"

#endif
