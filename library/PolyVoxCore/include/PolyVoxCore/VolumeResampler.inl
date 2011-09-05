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
	template< template<typename> class SrcVolumeType, template<typename> class DestVolumeType, typename VoxelType>
	VolumeResampler<SrcVolumeType, DestVolumeType, VoxelType>::VolumeResampler(SrcVolumeType<VoxelType>* pVolSrc, Region regSrc, DestVolumeType<VoxelType>* pVolDst, Region regDst)
		:m_pVolSrc(pVolSrc)
		,m_regSrc(regSrc)
		,m_pVolDst(pVolDst)
		,m_regDst(regDst)
	{
	}

	template< template<typename> class SrcVolumeType, template<typename> class DestVolumeType, typename VoxelType>
	void VolumeResampler<SrcVolumeType, DestVolumeType, VoxelType>::execute()
	{
		int32_t uSrcWidth = m_regSrc.getUpperCorner().getX() - m_regSrc.getLowerCorner().getX() + 1;
		int32_t uSrcHeight = m_regSrc.getUpperCorner().getY() - m_regSrc.getLowerCorner().getY() + 1;
		int32_t uSrcDepth = m_regSrc.getUpperCorner().getZ() - m_regSrc.getLowerCorner().getZ() + 1;

		int32_t uDstWidth = m_regDst.getUpperCorner().getX() - m_regDst.getLowerCorner().getX() + 1;
		int32_t uDstHeight = m_regDst.getUpperCorner().getY() - m_regDst.getLowerCorner().getY() + 1;
		int32_t uDstDepth = m_regDst.getUpperCorner().getZ() - m_regDst.getLowerCorner().getZ() + 1;

		if((uSrcWidth == uDstWidth) && (uSrcHeight == uDstHeight) && (uSrcDepth == uDstDepth))
		{
			resampleSameSize();
		}
		else
		{
			resampleArbitrary();
		}
	}

	template< template<typename> class SrcVolumeType, template<typename> class DestVolumeType, typename VoxelType>
	void VolumeResampler<SrcVolumeType, DestVolumeType, VoxelType>::resampleSameSize()
	{
		for(int32_t sz = m_regSrc.getLowerCorner().getZ(), dz = m_regDst.getLowerCorner().getZ(); dz <= m_regDst.getUpperCorner().getZ(); sz++, dz++)
		{
			for(int32_t sy = m_regSrc.getLowerCorner().getY(), dy = m_regDst.getLowerCorner().getY(); dy <= m_regDst.getUpperCorner().getY(); sy++, dy++)
			{
				for(int32_t sx = m_regSrc.getLowerCorner().getX(), dx = m_regDst.getLowerCorner().getX(); dx <= m_regDst.getUpperCorner().getX(); sx++,dx++)
				{
					VoxelType voxel = m_pVolSrc->getVoxelAt(sx,sy,sz);
					m_pVolDst->setVoxelAt(dx,dy,dz,voxel);
				}
			}
		}
	}

	template< template<typename> class SrcVolumeType, template<typename> class DestVolumeType, typename VoxelType>
	void VolumeResampler<SrcVolumeType, DestVolumeType, VoxelType>::resampleArbitrary()
	{
		float srcWidth  = m_regSrc.getUpperCorner().getX() - m_regSrc.getLowerCorner().getX();
		float srcHeight = m_regSrc.getUpperCorner().getY() - m_regSrc.getLowerCorner().getY();
		float srcDepth  = m_regSrc.getUpperCorner().getZ() - m_regSrc.getLowerCorner().getZ();

		float dstWidth  = m_regDst.getUpperCorner().getX() - m_regDst.getLowerCorner().getX();
		float dstHeight = m_regDst.getUpperCorner().getY() - m_regDst.getLowerCorner().getY();
		float dstDepth  = m_regDst.getUpperCorner().getZ() - m_regDst.getLowerCorner().getZ();

		float fScaleX = srcWidth / dstWidth;
		float fScaleY = srcHeight / dstHeight;
		float fScaleZ = srcDepth / dstDepth;

		typename SrcVolumeType<VoxelType>::Sampler sampler(m_pVolSrc);

		for(int32_t dz = m_regDst.getLowerCorner().getZ(); dz <= m_regDst.getUpperCorner().getZ(); dz++)
		{
			for(int32_t dy = m_regDst.getLowerCorner().getY(); dy <= m_regDst.getUpperCorner().getY(); dy++)
			{
				for(int32_t dx = m_regDst.getLowerCorner().getX(); dx <= m_regDst.getUpperCorner().getX(); dx++)
				{
					float sx = (dx - m_regDst.getLowerCorner().getX()) * fScaleX;
					float sy = (dy - m_regDst.getLowerCorner().getY()) * fScaleY;
					float sz = (dz - m_regDst.getLowerCorner().getZ()) * fScaleZ;

					sx += m_regSrc.getLowerCorner().getX();
					sy += m_regSrc.getLowerCorner().getY();
					sz += m_regSrc.getLowerCorner().getZ();

					sampler.setPosition(sx,sy,sz);
					VoxelType voxel000 = sampler.peekVoxel0px0py0pz();
					VoxelType voxel001 = sampler.peekVoxel0px0py1pz();
					VoxelType voxel010 = sampler.peekVoxel0px1py0pz();
					VoxelType voxel011 = sampler.peekVoxel0px1py1pz();
					VoxelType voxel100 = sampler.peekVoxel1px0py0pz();
					VoxelType voxel101 = sampler.peekVoxel1px0py1pz();
					VoxelType voxel110 = sampler.peekVoxel1px1py0pz();
					VoxelType voxel111 = sampler.peekVoxel1px1py1pz();

					uint8_t voxel000Den = voxel000.getDensity();
					uint8_t voxel001Den = voxel001.getDensity();
					uint8_t voxel010Den = voxel010.getDensity();
					uint8_t voxel011Den = voxel011.getDensity();
					uint8_t voxel100Den = voxel100.getDensity();
					uint8_t voxel101Den = voxel101.getDensity();
					uint8_t voxel110Den = voxel110.getDensity();
					uint8_t voxel111Den = voxel111.getDensity();

					//FIXME - should accept all float parameters, but GCC complains?
					double dummy;
					sx = modf(sx, &dummy);
					sy = modf(sy, &dummy);
					sz = modf(sz, &dummy);

					uint8_t uInterpolatedDensity = trilinearlyInterpolate<float>(voxel000Den,voxel100Den,voxel010Den,voxel110Den,voxel001Den,voxel101Den,voxel011Den,voxel111Den,sx,sy,sz);

					VoxelType result;
					result.setDensity(uInterpolatedDensity);

					m_pVolDst->setVoxelAt(dx,dy,dz,result);
				}
			}
		}
	}
}
