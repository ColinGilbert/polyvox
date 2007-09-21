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
#include "MaterialMap.h"
#include "SurfacePatchRenderable.h"
#include "SurfaceTriangle.h"
#include "SurfacePatch.h"
#include "TypeDef.h"
#include "Volume.h"
#include "SurfaceVertex.h"

#include <set>

namespace Ogre
{	
	/// Factory for default scene manager
	class VOXEL_SCENE_MANAGER_API PolyVoxSceneManagerFactory : public SceneManagerFactory
	{	
	public:
		/// Factory type name
		static const String FACTORY_TYPE_NAME;
		SceneManager* createInstance(const String& instanceName);
		void destroyInstance(SceneManager* instance);
	protected:
		void initMetaData(void) const;
	};

	/// Voxel scene manager
	class VOXEL_SCENE_MANAGER_API PolyVoxSceneManager : public SceneManager
	{
	public:
		//Constructors, etc
		PolyVoxSceneManager(const String& name);
		~PolyVoxSceneManager();

		//Getters
		uchar getMaterialIndexAt(uint uX, uint uY, uint uZ);
		const String& getTypeName(void) const;
		uint getSideLength(void);


		//Setters
		//void setMaterialNameForIndex(uchar uIndex, std::string sMaterialName);
		void setNormalGenerationMethod(NormalGenerationMethod method);
		void _findVisibleObjects(Camera* cam,  VisibleObjectsBoundsInfo *  visibleBounds, bool onlyShadowCasters);

		void setAllUpToDateFalse(void);
		void createSphereAt(Vector3 centre, Real radius, uchar value, bool painting);

		bool loadScene(const String& filename);
		bool saveScene(const String& filename);
		
		void generateLevelVolume(void);

		std::map<uchar, SurfacePatch> generateMeshDataForRegion(uint regionX, uint regionY, uint regionZ) const;

		void doRegionGrowing(uint xStart, uint yStart, uint zStart, uchar value);

		bool containsPoint(Vector3 pos, float boundary);
		bool containsPoint(IntVector3 pos, uint boundary);


		
		std::map<UIntVector3, SceneNode*> sceneNodes;
		//std::map<uchar,ManualObject*> m_mapManualObjects[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];
		bool surfaceUpToDate[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];
		bool regionIsHomogenous[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];

		std::map<uchar,SurfacePatchRenderable*> m_mapSurfaces[OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS][OGRE_VOLUME_SIDE_LENGTH_IN_REGIONS];

		void igniteVoxel(UIntVector3 voxelToIgnite);

	private:
		void markVoxelChanged(uint x, uint y, uint z);
		void markRegionChanged(uint firstX, uint firstY, uint firstZ, uint lastX, uint lastY, uint lastZ);



		static uint fileNo;

		bool useNormalSmoothing;
		uint normalSmoothingFilterSize;

		NormalGenerationMethod m_normalGenerationMethod;

		VolumePtr volumeData;
		MaterialMapPtr materialMap;

		std::queue<UIntVector3> m_queueVoxelsToBurn;

		bool m_bHaveGeneratedMeshes;

		//std::string m_aMaterialNames[256];		
	};
}

#endif
