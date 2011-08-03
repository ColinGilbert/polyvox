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
	LowPassFilter<SrcVolumeType, DestVolumeType, VoxelType>::LowPassFilter(SrcVolumeType<VoxelType>* pVolSrc, Region regSrc, DestVolumeType<VoxelType>* pVolDst, Region regDst)
		:m_pVolSrc(pVolSrc)
		,m_regSrc(regSrc)
		,m_pVolDst(pVolDst)
		,m_regDst(regDst)
	{
	}

	template< template<typename> class SrcVolumeType, template<typename> class DestVolumeType, typename VoxelType>
	void LowPassFilter<SrcVolumeType, DestVolumeType, VoxelType>::execute()
	{
		int32_t iSrcMinX = m_regSrc.getLowerCorner().getX();
		int32_t iSrcMinY = m_regSrc.getLowerCorner().getY();
		int32_t iSrcMinZ = m_regSrc.getLowerCorner().getZ();

		int32_t iSrcMaxX = m_regSrc.getUpperCorner().getX();
		int32_t iSrcMaxY = m_regSrc.getUpperCorner().getY();
		int32_t iSrcMaxZ = m_regSrc.getUpperCorner().getZ();

		int32_t iDstMinX = m_regDst.getLowerCorner().getX();
		int32_t iDstMinY = m_regDst.getLowerCorner().getY();
		int32_t iDstMinZ = m_regDst.getLowerCorner().getZ();

		//int32_t iDstMaxX = m_regDst.getUpperCorner().getX();
		//int32_t iDstMaxY = m_regDst.getUpperCorner().getY();
		//int32_t iDstMaxZ = m_regDst.getUpperCorner().getZ();

		SrcVolumeType<VoxelType>::Sampler srcSampler(m_pVolSrc);

		for(int32_t iSrcZ = iSrcMinZ, iDstZ = iDstMinZ; iSrcZ <= iSrcMaxZ; iSrcZ++, iDstZ++)
		{
			for(int32_t iSrcY = iSrcMinY, iDstY = iDstMinY; iSrcY <= iSrcMaxY; iSrcY++, iDstY++)
			{
				for(int32_t iSrcX = iSrcMinX, iDstX = iDstMinX; iSrcX <= iSrcMaxX; iSrcX++, iDstX++)
				{
					//VoxelType tSrcVoxel = m_pVolSrc->getVoxelAt(iSrcX, iSrcY, iSrcZ);
					srcSampler.setPosition(iSrcX, iSrcY, iSrcZ);

					VoxelType tSrcVoxel = srcSampler.getVoxel();

					uint32_t uDensity = 0;
					uDensity += srcSampler.peekVoxel1nx1ny1nz().getDensity();
					uDensity += srcSampler.peekVoxel1nx1ny0pz().getDensity();
					uDensity += srcSampler.peekVoxel1nx1ny1pz().getDensity();
					uDensity += srcSampler.peekVoxel1nx0py1nz().getDensity();
					uDensity += srcSampler.peekVoxel1nx0py0pz().getDensity();
					uDensity += srcSampler.peekVoxel1nx0py1pz().getDensity();
					uDensity += srcSampler.peekVoxel1nx1py1nz().getDensity();
					uDensity += srcSampler.peekVoxel1nx1py0pz().getDensity();
					uDensity += srcSampler.peekVoxel1nx1py1pz().getDensity();

					uDensity += srcSampler.peekVoxel0px1ny1nz().getDensity();
					uDensity += srcSampler.peekVoxel0px1ny0pz().getDensity();
					uDensity += srcSampler.peekVoxel0px1ny1pz().getDensity();
					uDensity += srcSampler.peekVoxel0px0py1nz().getDensity();
					uDensity += srcSampler.peekVoxel0px0py0pz().getDensity();
					uDensity += srcSampler.peekVoxel0px0py1pz().getDensity();
					uDensity += srcSampler.peekVoxel0px1py1nz().getDensity();
					uDensity += srcSampler.peekVoxel0px1py0pz().getDensity();
					uDensity += srcSampler.peekVoxel0px1py1pz().getDensity();

					uDensity += srcSampler.peekVoxel1px1ny1nz().getDensity();
					uDensity += srcSampler.peekVoxel1px1ny0pz().getDensity();
					uDensity += srcSampler.peekVoxel1px1ny1pz().getDensity();
					uDensity += srcSampler.peekVoxel1px0py1nz().getDensity();
					uDensity += srcSampler.peekVoxel1px0py0pz().getDensity();
					uDensity += srcSampler.peekVoxel1px0py1pz().getDensity();
					uDensity += srcSampler.peekVoxel1px1py1nz().getDensity();
					uDensity += srcSampler.peekVoxel1px1py0pz().getDensity();
					uDensity += srcSampler.peekVoxel1px1py1pz().getDensity();

					uDensity /= 27;

					tSrcVoxel.setDensity(uDensity);
					m_pVolDst->setVoxelAt(iSrcX, iSrcY, iSrcZ, tSrcVoxel);
				}
			}
		}
	}
}
