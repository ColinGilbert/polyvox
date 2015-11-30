/*******************************************************************************
Copyright (c) 2010 Matt Williams

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

#include "TestVolumeSubclass.h"

#include "PolyVox/Array.h"

#include "PolyVox/BaseVolume.h"
#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/Material.h"
#include "PolyVox/Vector.h"

#include <QtTest>

using namespace PolyVox;

template <typename VoxelType>
class VolumeSubclass : public BaseVolume<VoxelType>
{
public:
	//There seems to be some descrepency between Visual Studio and GCC about how the following class should be declared.
	//There is a work around (see also See http://goo.gl/qu1wn) given below which appears to work on VS2010 and GCC, but
	//which seems to cause internal compiler errors on VS2008 when building with the /Gm 'Enable Minimal Rebuild' compiler
	//option. For now it seems best to 'fix' it with the preprocessor insstead, but maybe the workaround can be reinstated
	//in the future
	//typedef BaseVolume<VoxelType> VolumeOfVoxelType; //Workaround for GCC/VS2010 differences. See http://goo.gl/qu1wn
	//class Sampler : public VolumeOfVoxelType::template Sampler< VolumeSubclass<VoxelType> >
	#if defined(_MSC_VER)
		class Sampler : public BaseVolume<VoxelType>::Sampler< VolumeSubclass<VoxelType> > //This line works on VS2010
	#else
		class Sampler : public BaseVolume<VoxelType>::template Sampler< VolumeSubclass<VoxelType> > //This line works on GCC
	#endif
	{
	public:
		Sampler(VolumeSubclass<VoxelType>* volume)
			:BaseVolume<VoxelType>::template Sampler< VolumeSubclass<VoxelType> >(volume)
		{
			this->mVolume = volume;
		}
		//~Sampler();
	};

	/// Constructor for creating a fixed size volume.
	VolumeSubclass(const Region& regValid)
		:BaseVolume<VoxelType>()
		, mVolumeData(regValid.getWidthInVoxels(), regValid.getHeightInVoxels(), regValid.getDepthInVoxels())
	{
		//mVolumeData.resize(ArraySizes(this->getWidth())(this->getHeight())(this->getDepth()));
	}
	/// Destructor
	~VolumeSubclass() {};

	/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
	VoxelType getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if ((uXPos >= 0) && (uXPos < static_cast<int32_t>(mVolumeData.getDimension(0))) &&
			(uYPos >= 0) && (uYPos < static_cast<int32_t>(mVolumeData.getDimension(1))) &&
			(uZPos >= 0) && (uZPos < static_cast<int32_t>(mVolumeData.getDimension(2))))
		{
			return mVolumeData(uXPos, uYPos, uZPos);
		}
		else
		{
			return VoxelType();
		}
	}

	/// Gets a voxel at the position given by a 3D vector
	VoxelType getVoxel(const Vector3DInt32& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	/// Sets the value used for voxels which are outside the volume
	void setBorderValue(const VoxelType& tBorder) { }
	/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
	bool setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		if( (uXPos >= 0) && (uXPos < static_cast<int32_t>(mVolumeData.getDimension(0))) &&
			(uYPos >= 0) && (uYPos < static_cast<int32_t>(mVolumeData.getDimension(1))) &&
			(uZPos >= 0) && (uZPos < static_cast<int32_t>(mVolumeData.getDimension(2))))
		{
			mVolumeData(uXPos, uYPos, uZPos) = tValue;
			return true;
		}
		else
		{
			return false;
		}
	}
	/// Sets the voxel at the position given by a 3D vector
	bool setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue) { return setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue); }

	/// Calculates approximatly how many bytes of memory the volume is currently using.
	uint32_t calculateSizeInBytes(void) { return 0; }

private:	
	Array<3, VoxelType> mVolumeData;
};

void TestVolumeSubclass::testExtractSurface()
{
	Region region(0, 0, 0, 16, 16, 16);
	VolumeSubclass<Material8> volumeSubclass(region);

	for (int32_t z = 0; z < region.getDepthInVoxels() / 2; z++)
	{
		for (int32_t y = 0; y < region.getHeightInVoxels(); y++)
		{
			for (int32_t x = 0; x < region.getWidthInVoxels(); x++)
			{
				Material8 mat(1);
				volumeSubclass.setVoxel(Vector3DInt32(x,y,z),mat);
			}
		}
	}

	auto result = extractCubicMesh(&volumeSubclass, region);

	QCOMPARE(result.getNoOfVertices(), static_cast<uint32_t>(8));
}

QTEST_MAIN(TestVolumeSubclass)
