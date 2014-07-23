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
#include "PolyVoxCore/Vertex.h"

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

		// Each component of the position is stored using 8.8 fixed-point encoding.
		Vector3DUint16 encodedPosition;

		// Each component of the normal is encoded using 5 bits of this variable.
		// The 16 bits are -xxxxxyyyyyzzzzz (note the left-most bit is currently 
		// unused). Some extra shifting and scaling is required to make it signed.
		uint16_t encodedNormal;

		// User data
		DataType data;
	};

	/// Decodes a position from a MarchingCubesVertex
	inline Vector3DFloat decode(const Vector3DUint16& encodedPosition)
	{
		Vector3DFloat result(encodedPosition.getX(), encodedPosition.getY(), encodedPosition.getZ());
		result *= (1.0f / 256.0f); // Division is compile-time constant
		return result;
	}

	/*inline uint16_t encodeNormal(const Vector3DFloat& normal)
	{
		Vector3DFloat v3dNormal = normal;
		v3dNormal += Vector3DFloat(1.0f, 1.0f, 1.0f);
		uint16_t encodedX = static_cast<uint16_t>(roundToNearestInteger(v3dNormal.getX() * 15.5f));
		uint16_t encodedY = static_cast<uint16_t>(roundToNearestInteger(v3dNormal.getY() * 15.5f));
		uint16_t encodedZ = static_cast<uint16_t>(roundToNearestInteger(v3dNormal.getZ() * 15.5f));
		POLYVOX_ASSERT(encodedX < 32, "Encoded value out of range");
		POLYVOX_ASSERT(encodedY < 32, "Encoded value out of range");
		POLYVOX_ASSERT(encodedZ < 32, "Encoded value out of range");
		uint16_t encodedNormal = (encodedX << 10) | (encodedY << 5) | encodedZ;
		return encodedNormal;
	}

	/// Decodes a normal from a MarchingCubesVertex
	inline Vector3DFloat decode(const uint16_t encodedNormal)
	{
		// Get normal components in the range 0 to 31
		uint16_t x = (encodedNormal >> 10) & 0x1F;
		uint16_t y = (encodedNormal >> 5) & 0x1F;
		uint16_t z = (encodedNormal) & 0x1F;

		// Build the resulting vector
		Vector3DFloat result(x, y, z);

		// Convert to range 0.0 to 2.0
		result *= (1.0f / 15.5f); // Division is compile-time constant

		// Convert to range -1.0 to 1.0
		result -= Vector3DFloat(1.0f, 1.0f, 1.0f);

		return result;
	}*/

	// Returns ±1
	float signNotZero(float v)
	{
		return v >= 0.0 ? +1.0 : -1.0;
	}

	Vector2DFloat signNotZero(Vector2DFloat v)
	{
		return Vector2DFloat((v.getX() >= 0.0) ? +1.0 : -1.0, (v.getY() >= 0.0) ? +1.0 : -1.0);
	}

	// Assume normalized input. Output is on [-1, 1] for each component.
	Vector2DFloat float32x3_to_oct(Vector3DFloat v)
	{
		// Project the sphere onto the octahedron, and then onto the xy plane
		Vector2DFloat p(v.getX(), v.getY());			
		p = p * (1.0f / (abs(v.getX()) + abs(v.getY()) + abs(v.getZ())));

		float refX = ((1.0f - abs(p.getY())) * signNotZero(p.getX()));
		float refY = ((1.0f - abs(p.getX())) * signNotZero(p.getY()));

		Vector2DFloat ref(refX, refY);

		// Reflect the folds of the lower hemisphere over the diagonals
		return (v.getZ() <= 0.0) ? ref : p;
	}

	Vector3DFloat oct_to_float32x3(Vector2DFloat e)
	{
		Vector3DFloat v = Vector3DFloat(e.getX(), e.getY(), 1.0 - abs(e.getX()) - abs(e.getY()));

		float refX = ((1.0f - abs(v.getY())) * signNotZero(v.getX()));
		float refY = ((1.0f - abs(v.getX())) * signNotZero(v.getY()));

		Vector2DFloat ref(refX, refY);

		if (v.getZ() < 0.0f)
		{
			//v.xy = (1.0 - abs(v.yx)) * signNotZero(v.xy);
			v.setX(refX);
			v.setY(refY);
		}

		v.normalise();

		return v;
	}

	inline uint16_t encodeNormal(const Vector3DFloat& normal)
	{
		Vector2DFloat floatResult = float32x3_to_oct(normal);

		floatResult += Vector2DFloat(1.0f, 1.0f); // To range 0.0f to 2.0f
		floatResult *= Vector2DFloat(127.5f, 127.5f); // To range 0.0f to 255.0f

		uint16_t resultX = static_cast<uint16_t>(floatResult.getX() + 0.5f);
		uint16_t resultY = static_cast<uint16_t>(floatResult.getY() + 0.5f);

		resultX &= 0xFF;
		resultY &= 0xFF;

		return (resultX << 8) | resultY;
	}

	inline Vector3DFloat decode(const uint16_t& encodedNormal)
	{
		uint16_t x = (encodedNormal >> 8) & 0xFF;
		uint16_t y = (encodedNormal     ) & 0xFF;
		Vector2DFloat floatNormal(x, y);

		floatNormal /= Vector2DFloat(127.5f, 127.5f);
		floatNormal -= Vector2DFloat(1.0f, 1.0f);

		return oct_to_float32x3(floatNormal);
	}

	/// Decodes a MarchingCubesVertex by converting it into a regular Vertex which can then be directly used for rendering.
	template<typename DataType>
	Vertex<DataType> decode(const MarchingCubesVertex<DataType>& marchingCubesVertex)
	{
		Vertex<DataType> result;
		result.position = decode(marchingCubesVertex.encodedPosition);
		result.normal = decode(marchingCubesVertex.encodedNormal);
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
