#ifndef __MATERIALMAPMANAGER_H__
#define __MATERIALMAPMANAGER_H__

#include <OgreResourceManager.h>
#include "MaterialMap.h"
#include "TypeDef.h"

namespace Ogre
{
	class VOXEL_SCENE_MANAGER_API MaterialMapManager : public Ogre::ResourceManager, public Ogre::Singleton<MaterialMapManager>
	{
	protected:

	   // must implement this from ResourceManager's interface
	   Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle, 
		   const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader, 
		   const Ogre::NameValuePairList *createParams);

	public:

	   MaterialMapManager ();
	   virtual ~MaterialMapManager ();

	   virtual MaterialMapPtr load (const Ogre::String &name, const Ogre::String &group);

	   static MaterialMapManager &getSingleton ();
	   static MaterialMapManager *getSingletonPtr ();
	};
}

#endif
