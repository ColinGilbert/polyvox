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

#include "OgrePrerequisites.h"
#include "OgreSceneManager.h"

#include "Constants.h"
#include "IndexedSurfacePatch.h"
#include "SurfacePatchRenderable.h"
#include "SurfaceTriangle.h"
//#include "AbstractSurfacePatch.h"
#include "TypeDef.h"
#include "Volume.h"
#include "SurfaceVertex.h"
#include "RegionGeometry.h"

#include <set>

namespace Ogre
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
		uchar getMaterialIndexAt(uint uX, uint uY, uint uZ);
		const String& getTypeName(void) const;
		uint getSideLength(void);


		//Setters
		void setNormalGenerationMethod(NormalGenerationMethod method);
		//void _findVisibleObjects(Camera* cam,  VisibleObjectsBoundsInfo *  visibleBounds, bool onlyShadowCasters);

		std::list<RegionGeometry> getChangedRegionGeometry(void);

		void setAllUpToDateFlagsTo(bool newUpToDateValue);
		void createSphereAt(Vector3 centre, Real radius, uchar value, bool painting);
		
		void generateLevelVolume(void);

		void generateMeshDataForRegion(uint regionX, uint regionY, uint regionZ, IndexedSurfacePatch* singleMaterialPatch, IndexedSurfacePatch* multiMaterialPatch) const;

		void doRegionGrowing(uint xStart, uint yStart, uint zStart, uchar value);

		bool containsPoint(Vector3 pos, float boundary);
		bool containsPoint(IntVector3 pos, uint boundary);

		

		bool surfaceUpToDate[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];
		bool regionIsHomogenous[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];

		//SurfacePatchRenderable* m_singleMaterialSurfaces[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];
		//SurfacePatchRenderable* m_multiMaterialSurfaces[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];

		Vector3 computeNormal(const Vector3& position, NormalGenerationMethod normalGenerationMethod) const;

	public:
		void markVoxelChanged(uint x, uint y, uint z);
		void markRegionChanged(uint firstX, uint firstY, uint firstZ, uint lastX, uint lastY, uint lastZ);


		static uint fileNo;

		bool useNormalSmoothing;
		uint normalSmoothingFilterSize;

		NormalGenerationMethod m_normalGenerationMethod;

		
		Volume* volumeData;

		bool m_bHaveGeneratedMeshes;

		//std::string m_aMaterialNames[256];		
	};
}

#endif
