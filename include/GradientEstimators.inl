#pragma region License
/******************************************************************************
This file is part of the PolyVox library
Copyright (C) 2006  David Williams

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/
#pragma endregion

namespace PolyVox
{
	template <typename VoxelType>
	Vector3DFloat computeCentralDifferenceGradient(const VolumeIterator<VoxelType>& volIter)
	{
		//FIXME - bitwise way of doing this?
		VoxelType voxel1nx = volIter.peekVoxel1nx0py0pz() > 0 ? 1: 0;
		VoxelType voxel1px = volIter.peekVoxel1px0py0pz() > 0 ? 1: 0;

		VoxelType voxel1ny = volIter.peekVoxel0px1ny0pz() > 0 ? 1: 0;
		VoxelType voxel1py = volIter.peekVoxel0px1py0pz() > 0 ? 1: 0;

		VoxelType voxel1nz = volIter.peekVoxel0px0py1nz() > 0 ? 1: 0;
		VoxelType voxel1pz = volIter.peekVoxel0px0py1pz() > 0 ? 1: 0;

		return Vector3DFloat
		(
			static_cast<float>(voxel1px) - static_cast<float>(voxel1nx),
			static_cast<float>(voxel1py) - static_cast<float>(voxel1ny),
			static_cast<float>(voxel1pz) - static_cast<float>(voxel1nz)
		);
	}

	template <typename VoxelType>
	Vector3DFloat computeSmoothCentralDifferenceGradient(VolumeIterator<VoxelType>& volIter)
	{
		boost::uint16_t initialX = volIter.getPosX();
		boost::uint16_t initialY = volIter.getPosY();
		boost::uint16_t initialZ = volIter.getPosZ();

		//FIXME - bitwise way of doing this?
		volIter.setPosition(initialX-1, initialY, initialZ);
		float voxel1nx = volIter.getAveragedVoxel(1);
		volIter.setPosition(initialX+1, initialY, initialZ);
		float voxel1px = volIter.getAveragedVoxel(1);

		volIter.setPosition(initialX, initialY-1, initialZ);
		float voxel1ny = volIter.getAveragedVoxel(1);
		volIter.setPosition(initialX, initialY+1, initialZ);
		float voxel1py = volIter.getAveragedVoxel(1);

		volIter.setPosition(initialX, initialY, initialZ-1);
		float voxel1nz = volIter.getAveragedVoxel(1);
		volIter.setPosition(initialX, initialY, initialZ+1);
		float voxel1pz = volIter.getAveragedVoxel(1);

		return Vector3DFloat
		(
			voxel1px - voxel1nx,
			voxel1py - voxel1ny,
			voxel1pz - voxel1nz
		);
	}

	template <typename VoxelType>
	Vector3DFloat computeSobelGradient(const VolumeIterator<VoxelType>& volIter)
	{
		static const int weights[3][3][3] = {  {  {2,3,2}, {3,6,3}, {2,3,2}  },  {
			{3,6,3},  {6,0,6},  {3,6,3} },  { {2,3,2},  {3,6,3},  {2,3,2} } };

			const VoxelType pVoxel1nx1ny1nz = volIter.peekVoxel1nx1ny1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1ny0pz = volIter.peekVoxel1nx1ny0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1ny1pz = volIter.peekVoxel1nx1ny1pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx0py1nz = volIter.peekVoxel1nx0py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx0py0pz = volIter.peekVoxel1nx0py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx0py1pz = volIter.peekVoxel1nx0py1pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1py1nz = volIter.peekVoxel1nx1py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1py0pz = volIter.peekVoxel1nx1py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1nx1py1pz = volIter.peekVoxel1nx1py1pz() > 0 ? 1: 0;

			const VoxelType pVoxel0px1ny1nz = volIter.peekVoxel0px1ny1nz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1ny0pz = volIter.peekVoxel0px1ny0pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1ny1pz = volIter.peekVoxel0px1ny1pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px0py1nz = volIter.peekVoxel0px0py1nz() > 0 ? 1: 0;
			//const VoxelType pVoxel0px0py0pz = volIter.peekVoxel0px0py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px0py1pz = volIter.peekVoxel0px0py1pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1py1nz = volIter.peekVoxel0px1py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1py0pz = volIter.peekVoxel0px1py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel0px1py1pz = volIter.peekVoxel0px1py1pz() > 0 ? 1: 0;

			const VoxelType pVoxel1px1ny1nz = volIter.peekVoxel1px1ny1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1ny0pz = volIter.peekVoxel1px1ny0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1ny1pz = volIter.peekVoxel1px1ny1pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px0py1nz = volIter.peekVoxel1px0py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1px0py0pz = volIter.peekVoxel1px0py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px0py1pz = volIter.peekVoxel1px0py1pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1py1nz = volIter.peekVoxel1px1py1nz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1py0pz = volIter.peekVoxel1px1py0pz() > 0 ? 1: 0;
			const VoxelType pVoxel1px1py1pz = volIter.peekVoxel1px1py1pz() > 0 ? 1: 0;



			const int xGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) -
				weights[1][0][0] * ( pVoxel1nx1ny0pz) - weights[2][0][0] *
				( pVoxel1nx1ny1pz) - weights[0][1][0] * ( pVoxel1nx0py1nz) -
				weights[1][1][0] * ( pVoxel1nx0py0pz) - weights[2][1][0] *
				( pVoxel1nx0py1pz) - weights[0][2][0] * ( pVoxel1nx1py1nz) -
				weights[1][2][0] * ( pVoxel1nx1py0pz) - weights[2][2][0] *
				( pVoxel1nx1py1pz) + weights[0][0][2] * ( pVoxel1px1ny1nz) +
				weights[1][0][2] * ( pVoxel1px1ny0pz) + weights[2][0][2] *
				( pVoxel1px1ny1pz) + weights[0][1][2] * ( pVoxel1px0py1nz) +
				weights[1][1][2] * ( pVoxel1px0py0pz) + weights[2][1][2] *
				( pVoxel1px0py1pz) + weights[0][2][2] * ( pVoxel1px1py1nz) +
				weights[1][2][2] * ( pVoxel1px1py0pz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			const int yGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) -
				weights[1][0][0] * ( pVoxel1nx1ny0pz) - weights[2][0][0] *
				( pVoxel1nx1ny1pz) + weights[0][2][0] * ( pVoxel1nx1py1nz) +
				weights[1][2][0] * ( pVoxel1nx1py0pz) + weights[2][2][0] *
				( pVoxel1nx1py1pz) - weights[0][0][1] * ( pVoxel0px1ny1nz) -
				weights[1][0][1] * ( pVoxel0px1ny0pz) - weights[2][0][1] *
				( pVoxel0px1ny1pz) + weights[0][2][1] * ( pVoxel0px1py1nz) +
				weights[1][2][1] * ( pVoxel0px1py0pz) + weights[2][2][1] *
				( pVoxel0px1py1pz) - weights[0][0][2] * ( pVoxel1px1ny1nz) -
				weights[1][0][2] * ( pVoxel1px1ny0pz) - weights[2][0][2] *
				( pVoxel1px1ny1pz) + weights[0][2][2] * ( pVoxel1px1py1nz) +
				weights[1][2][2] * ( pVoxel1px1py0pz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			const int zGrad(- weights[0][0][0] * ( pVoxel1nx1ny1nz) +
				weights[2][0][0] * ( pVoxel1nx1ny1pz) - weights[0][1][0] *
				( pVoxel1nx0py1nz) + weights[2][1][0] * ( pVoxel1nx0py1pz) -
				weights[0][2][0] * ( pVoxel1nx1py1nz) + weights[2][2][0] *
				( pVoxel1nx1py1pz) - weights[0][0][1] * ( pVoxel0px1ny1nz) +
				weights[2][0][1] * ( pVoxel0px1ny1pz) - weights[0][1][1] *
				( pVoxel0px0py1nz) + weights[2][1][1] * ( pVoxel0px0py1pz) -
				weights[0][2][1] * ( pVoxel0px1py1nz) + weights[2][2][1] *
				( pVoxel0px1py1pz) - weights[0][0][2] * ( pVoxel1px1ny1nz) +
				weights[2][0][2] * ( pVoxel1px1ny1pz) - weights[0][1][2] *
				( pVoxel1px0py1nz) + weights[2][1][2] * ( pVoxel1px0py1pz) -
				weights[0][2][2] * ( pVoxel1px1py1nz) + weights[2][2][2] *
				( pVoxel1px1py1pz));

			return Vector3DFloat(static_cast<float>(xGrad),static_cast<float>(yGrad),static_cast<float>(zGrad));
	}
}
