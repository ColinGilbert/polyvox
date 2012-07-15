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

#ifndef __PolyVox_MarchingCubesController_H__
#define __PolyVox_MarchingCubesController_H__

#include <limits>

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// This class provides a default implementation of a controller for the MarchingCubesSurfaceExtractor. It controls the behaviour of the
	/// MarchingCubesSurfaceExtractor and provides the required properties from the underlying voxel type.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// PolyVox does not enforce any requirements regarding what data must be present in a voxel, and instead allows any primitive or user-defined
	/// type to be used. However, the Marching Cubes algorithm does have some requirents about the underlying data in that conceptually it operates
	/// on a <i>density field</i>. In addition, the PolyVox implementation of the Marching Cubes algorithm also understands the idea of each voxel
	/// having a material which is copied into the vertex data.
	///
	/// Because we want the MarchingCubesSurfaceExtractor to work on <i>any</i> voxel type, we use a <i>Marching Cubes controller</i> (passed as
	/// a parameter of the MarchingCubesSurfaceExtractor) to expose the required properties. This parameter defaults to the DefaultMarchingCubesController.
	/// The main implementation of this class is designed to work with primitives data types, and the class is also specialised for the Material,
	/// Density and MaterialdensityPair classes.
	///
	/// If you create a custom class for your voxel data then you probably want to include a specialisation of DefaultMarchingCubesController,
	/// though you don't have to if you don't want to use the Marching Cubes algorithm or if you prefer to define a seperate Marching Cubes controller
	/// and pass it as an explicit parameter (rather than relying on the default).
	///
	/// For primitive types, the DefaultMarchingCubesController considers the value of the voxel to represent it's density and just returns a constant
	/// for the material. So you can, for example, run the MarchingCubesSurfaceExtractor on a volume of floats or ints.
	///
	/// It is possible to customise the behaviour of the controller by providing a threshold value through the constructor. The extracted surface
	/// will pass through the density value specified by the threshold, and so you should make sure that the threshold value you choose is between
	/// the minimum and maximum values found in your volume data. By default it is in the middle of the representable range of the underlying type.
	///
	/// \sa MarchingCubesSurfaceExtractor
	///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename VoxelType>
	class DefaultMarchingCubesController
	{
	public:
		typedef VoxelType DensityType;
		typedef float MaterialType;

		DefaultMarchingCubesController(void)
		{
			m_tThreshold = ((std::numeric_limits<DensityType>::min)() + (std::numeric_limits<DensityType>::max)()) / 2;
		}

		DefaultMarchingCubesController(DensityType tThreshold)
		{
			m_tThreshold = tThreshold;
		}

		DensityType convertToDensity(VoxelType voxel)
		{
			return voxel;
		}

		MaterialType convertToMaterial(VoxelType voxel)
		{
			return 1;
		}

		DensityType getThreshold(void)
		{
			// Returns a threshold value halfway between the min and max possible values.
			return m_tThreshold;
		}

	private:
		DensityType m_tThreshold;
	};
}

#endif
