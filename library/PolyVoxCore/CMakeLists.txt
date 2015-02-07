# Copyright (c) 2008-2012 Matt Williams
# Copyright (c) 2008-2012 David Williams
# 
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
# 
# 1. The origin of this software must not be misrepresented; you must not
# claim that you wrote the original software. If you use this software
# in a product, an acknowledgment in the product documentation would be
# appreciated but is not required.
# 
# 2. Altered source versions must be plainly marked as such, and must not be
# misrepresented as being the original software.
# 
# 3. This notice may not be removed or altered from any source
# distribution.

PROJECT(PolyVoxCore)

#Projects source files
#SET(CORE_SRC_FILES
	#source/AStarPathfinder.cpp
#)

#Projects headers files
SET(CORE_INC_FILES
	include/PolyVoxCore/AmbientOcclusionCalculator.h
	include/PolyVoxCore/AmbientOcclusionCalculator.inl
	include/PolyVoxCore/Array.h
	include/PolyVoxCore/AStarPathfinder.h
	include/PolyVoxCore/AStarPathfinder.inl
	include/PolyVoxCore/BaseVolume.h
	include/PolyVoxCore/BaseVolume.inl
	include/PolyVoxCore/BaseVolumeSampler.inl
	include/PolyVoxCore/CubicSurfaceExtractor.h
	include/PolyVoxCore/CubicSurfaceExtractor.inl
	include/PolyVoxCore/DefaultIsQuadNeeded.h
	include/PolyVoxCore/DefaultMarchingCubesController.h
	include/PolyVoxCore/Density.h
	include/PolyVoxCore/FilePager.h
	include/PolyVoxCore/GradientEstimators.h
	include/PolyVoxCore/GradientEstimators.inl
	include/PolyVoxCore/Interpolation.h
	include/PolyVoxCore/IteratorController.h
	include/PolyVoxCore/IteratorController.inl
	include/PolyVoxCore/LargeVolume.h
	include/PolyVoxCore/LowPassFilter.h
	include/PolyVoxCore/LowPassFilter.inl
	include/PolyVoxCore/MarchingCubesSurfaceExtractor.h
	include/PolyVoxCore/MarchingCubesSurfaceExtractor.inl
	include/PolyVoxCore/Material.h
	include/PolyVoxCore/MaterialDensityPair.h
	include/PolyVoxCore/Mesh.h
	include/PolyVoxCore/Mesh.inl
	include/PolyVoxCore/PagedVolume.h
	include/PolyVoxCore/PagedVolume.inl
	include/PolyVoxCore/PagedVolumeChunk.inl
	include/PolyVoxCore/PagedVolumeSampler.inl
	include/PolyVoxCore/PolyVoxForwardDeclarations.h
	include/PolyVoxCore/Picking.h
	include/PolyVoxCore/Picking.inl
	include/PolyVoxCore/RawVolume.h
	include/PolyVoxCore/RawVolume.inl
	include/PolyVoxCore/RawVolumeSampler.inl
	include/PolyVoxCore/Raycast.h
	include/PolyVoxCore/Raycast.inl
	include/PolyVoxCore/Region.h
	include/PolyVoxCore/Region.inl
	include/PolyVoxCore/SimpleVolume.h
	include/PolyVoxCore/Vector.h
	include/PolyVoxCore/Vector.inl
	include/PolyVoxCore/Vertex.h
	include/PolyVoxCore/VolumeResampler.h
	include/PolyVoxCore/VolumeResampler.inl
	include/PolyVoxCore/VoxelFilters.h
	include/PolyVoxCore/VoxelFilters.inl
)

#SET(IMPL_SRC_FILES
	#source/Impl/ErrorHandling.cpp
	#source/Impl/Logging.cpp
	#source/Impl/MarchingCubesTables.cpp
	#source/Impl/RandomUnitVectors.cpp
	#source/Impl/RandomVectors.cpp
	#source/Impl/Timer.cpp
	#source/Impl/Utility.cpp
#)

SET(IMPL_INC_FILES
	include/PolyVoxCore/Impl/AStarPathfinderImpl.h
    include/PolyVoxCore/Impl/Config.h
	include/PolyVoxCore/Impl/ErrorHandling.h
	include/PolyVoxCore/Impl/Logging.h
	include/PolyVoxCore/Impl/MarchingCubesTables.h
	include/PolyVoxCore/Impl/RandomUnitVectors.h
	include/PolyVoxCore/Impl/RandomVectors.h
	include/PolyVoxCore/Impl/Timer.h
	include/PolyVoxCore/Impl/TypeDef.h
	include/PolyVoxCore/Impl/Utility.h
)

#NOTE: The following line should be uncommented when building shared libs.

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
#SOURCE_GROUP("Source Files" FILES ${CORE_SRC_FILES})
SOURCE_GROUP("Header Files" FILES ${CORE_INC_FILES})

#SOURCE_GROUP("Source Files\\Impl" FILES ${IMPL_SRC_FILES})
SOURCE_GROUP("Header Files\\Impl" FILES ${IMPL_INC_FILES})

# Although we don't build anything for PolyVox, we still add this custom target (which
# doesn't do anything) so that we can browse the source code from within Visual Studio.
ADD_CUSTOM_TARGET(PolyVoxCore SOURCES ${CORE_INC_FILES})
SET_PROPERTY(TARGET PolyVoxCore PROPERTY FOLDER "Library")
