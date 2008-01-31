#ifndef __VOLUMEMANAGER_H__
#define __VOLUMEMANAGER_H__

#include <OgreResourceManager.h>
#include "VolumeResource.h"

namespace Ogre
{
	class VOXEL_SCENE_MANAGER_API VolumeManager : public Ogre::ResourceManager, public Ogre::Singleton<VolumeManager>
	{
	protected:

		// must implement this from ResourceManager's interface
		Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle, 
			const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader, 
			const Ogre::NameValuePairList *createParams);

	public:

		VolumeManager ();
		virtual ~VolumeManager ();

		virtual VolumeResourcePtr load (const Ogre::String &name, const Ogre::String &group);

		static VolumeManager &getSingleton ();
		static VolumeManager *getSingletonPtr ();
	};
}

#endif
