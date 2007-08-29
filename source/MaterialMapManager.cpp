#include "MaterialMapManager.h"

namespace Ogre
{
	template<> MaterialMapManager *Ogre::Singleton<MaterialMapManager>::ms_Singleton = 0;

	MaterialMapManager *MaterialMapManager::getSingletonPtr ()
	{
	   return ms_Singleton;
	}

	MaterialMapManager &MaterialMapManager::getSingleton ()
	{  
	   assert (ms_Singleton);  
		  return (*ms_Singleton);
	}

	MaterialMapManager::MaterialMapManager ()
	{
	   mResourceType = "MaterialMap";

	   // low, because it will likely reference other resources
	   mLoadOrder = 30.0f;

	   // this is how we register the ResourceManager with OGRE
	   Ogre::ResourceGroupManager::getSingleton ()._registerResourceManager (mResourceType, this);
	}

	MaterialMapManager::~MaterialMapManager()
	{
	   // and this is how we unregister it
	   Ogre::ResourceGroupManager::getSingleton ()._unregisterResourceManager (mResourceType);
	}

	MaterialMapPtr MaterialMapManager::load (const Ogre::String &name, const Ogre::String &group)
	{
	   MaterialMapPtr textf = getByName (name);

	   if (textf.isNull ())
	   {
		   textf = create (name, group);
	   }

	   textf->load ();

	   return textf;
	}

	Ogre::Resource *MaterialMapManager::createImpl (const Ogre::String &name, Ogre::ResourceHandle handle, 
	   const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader, 
	   const Ogre::NameValuePairList *createParams)
	{
	   return new MaterialMap (this, name, handle, group, isManual, loader);
	}
}
