/******************************************************************************
This file is part of a voxel plugin for OGRE
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
#ifndef __PolyVoxSceneManager_H__
#define __PolyVoxSceneManager_H__

#include <list>

#include "boost/cstdint.hpp"

#include "Constants.h"
#include "IndexedSurfacePatch.h"
#include "SurfaceTriangle.h"
//#include "AbstractSurfacePatch.h"
#include "TypeDef.h"
#include "Volume.h"
#include "SurfaceVertex.h"
#include "RegionGeometry.h"

#include "Vector.h"

#include <set>

namespace PolyVox
{	
	enum NormalGenerationMethod
	{
		SIMPLE,
		CENTRAL_DIFFERENCE,
		SOBEL
	};

	/// Voxel scene manager
	class VOXEL_SCENE_MANAGER_API PolyVoxSceneManager
	{
	public:
		//Constructors, etc
		PolyVoxSceneManager();
		~PolyVoxSceneManager();

		//Getters
		boost::uint8_t getMaterialIndexAt(boost::uint16_t uX, boost::uint16_t uY, boost::uint16_t uZ);
		const std::string& getTypeName(void) const;
		boost::uint16_t getSideLength(void);


		//Setters
		void setNormalGenerationMethod(NormalGenerationMethod method);
		//void _findVisibleObjects(Camera* cam,  VisibleObjectsBoundsInfo *  visibleBounds, bool onlyShadowCasters);

		std::list<RegionGeometry> getChangedRegionGeometry(void);

		void setAllUpToDateFlagsTo(bool newUpToDateValue);
		void createSphereAt(Vector3DFloat centre, float radius, boost::uint8_t value, bool painting);
		
		void generateLevelVolume(void);

		void generateMeshDataForRegion(boost::uint16_t regionX,boost:: uint16_t regionY, boost::uint16_t regionZ, IndexedSurfacePatch* singleMaterialPatch, IndexedSurfacePatch* multiMaterialPatch) const;

		bool containsPoint(Vector3DFloat pos, float boundary);
		bool containsPoint(IntVector3 pos, boost::uint16_t boundary);

		

		bool surfaceUpToDate[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];
		bool regionIsHomogenous[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];

		//SurfacePatchRenderable* m_singleMaterialSurfaces[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];
		//SurfacePatchRenderable* m_multiMaterialSurfaces[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];

		Vector3DFloat computeNormal(const Vector3DFloat& position, NormalGenerationMethod normalGenerationMethod) const;

	public:
		void markVoxelChanged(boost::uint16_t x, boost::uint16_t y, boost::uint16_t z);
		void markRegionChanged(boost::uint16_t firstX, boost::uint16_t firstY, boost::uint16_t firstZ, boost::uint16_t lastX, boost::uint16_t lastY, boost::uint16_t lastZ);


		static boost::uint16_t fileNo;

		bool useNormalSmoothing;
		boost::uint16_t normalSmoothingFilterSize;

		NormalGenerationMethod m_normalGenerationMethod;

		
		Volume* volumeData;

		bool m_bHaveGeneratedMeshes;

		//std::string m_aMaterialNames[256];		
	};
}

#endif
