#ifndef __PolyVox_GradientEstimators_H__
#define __PolyVox_GradientEstimators_H__

#include "VolumeIterator.h"

namespace PolyVox
{
	//FIXME - gradient can be expressed with ints.
	template <typename VoxelType>
	Vector3DFloat computeCentralDifferenceGradient(const VolumeIterator<VoxelType>& volIter);

	template <typename VoxelType>
	Vector3DFloat computeSobelGradient(const VolumeIterator<VoxelType>& volIter);
}

#include "GradientEstimators.inl"

#endif //__PolyVox_GradientEstimators_H__