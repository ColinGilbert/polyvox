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
		else if((uSrcWidth == uDstWidth * 2) && (uSrcHeight == uDstHeight * 2) && (uSrcDepth == uDstDepth * 2))
		{
			resampleHalfSize();
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
	void VolumeResampler<SrcVolumeType, DestVolumeType, VoxelType>::resampleHalfSize()
	{
		for(int32_t sz = m_regSrc.getLowerCorner().getZ(), dz = m_regDst.getLowerCorner().getZ(); dz <= m_regDst.getUpperCorner().getZ(); sz+=2, dz++)
		{
			for(int32_t sy = m_regSrc.getLowerCorner().getY(), dy = m_regDst.getLowerCorner().getY(); dy <= m_regDst.getUpperCorner().getY(); sy+=2, dy++)
			{
				for(int32_t sx = m_regSrc.getLowerCorner().getX(), dx = m_regDst.getLowerCorner().getX(); dx <= m_regDst.getUpperCorner().getX(); sx+=2,dx++)
				{
					VoxelType voxel000 = m_pVolSrc->getVoxelAt(sx+0,sy+0,sz+0);
					VoxelType voxel001 = m_pVolSrc->getVoxelAt(sx+0,sy+0,sz+1);
					VoxelType voxel010 = m_pVolSrc->getVoxelAt(sx+0,sy+1,sz+0);
					VoxelType voxel011 = m_pVolSrc->getVoxelAt(sx+0,sy+1,sz+1);
					VoxelType voxel100 = m_pVolSrc->getVoxelAt(sx+1,sy+0,sz+0);
					VoxelType voxel101 = m_pVolSrc->getVoxelAt(sx+1,sy+0,sz+1);
					VoxelType voxel110 = m_pVolSrc->getVoxelAt(sx+1,sy+1,sz+0);
					VoxelType voxel111 = m_pVolSrc->getVoxelAt(sx+1,sy+1,sz+1);

					uint32_t averageDensity = 0;
					averageDensity += voxel000.getDensity();
					averageDensity += voxel001.getDensity();
					averageDensity += voxel010.getDensity();
					averageDensity += voxel011.getDensity();
					averageDensity += voxel100.getDensity();
					averageDensity += voxel101.getDensity();
					averageDensity += voxel110.getDensity();
					averageDensity += voxel111.getDensity();
					averageDensity /= 8;

					VoxelType result;
					result.setDensity(averageDensity);

					m_pVolDst->setVoxelAt(dx,dy,dz,result);
				}
			}
		}
	}

	template< template<typename> class SrcVolumeType, template<typename> class DestVolumeType, typename VoxelType>
	void VolumeResampler<SrcVolumeType, DestVolumeType, VoxelType>::resampleArbitrary()
	{
		//ARBITRARY RESAMPLING NOT YET IMPLEMENTED.
		assert(false); 
	}
}