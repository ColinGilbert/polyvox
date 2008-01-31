#include "VolumeManager.h"

#include "OgreLogManager.h" //FIXME - shouldn't realy need this in this class?'

namespace Ogre
{
	template<> VolumeManager *Ogre::Singleton<VolumeManager>::ms_Singleton = 0;

	VolumeManager *VolumeManager::getSingletonPtr ()
	{
		return ms_Singleton;
	}

	VolumeManager &VolumeManager::getSingleton ()
	{  
		assert (ms_Singleton);  
		return (*ms_Singleton);
	}

	VolumeManager::VolumeManager ()
	{
		mResourceType = "Volume";

		// low, because it will likely reference other resources
		mLoadOrder = 30.0f;

		// this is how we register the ResourceManager with OGRE
		Ogre::ResourceGroupManager::getSingleton ()._registerResourceManager (mResourceType, this);
	}

	VolumeManager::~VolumeManager()
	{
		// and this is how we unregister it
		Ogre::ResourceGroupManager::getSingleton ()._unregisterResourceManager (mResourceType);
	}

	VolumeResourcePtr VolumeManager::load (const Ogre::String &name, const Ogre::String &group)
	{
		Ogre::LogManager::getSingleton().logMessage("DAVID - calling getByName");
		VolumeResourcePtr textf = getByName (name);
		Ogre::LogManager::getSingleton().logMessage("DAVID - done getByName");

		if (textf.isNull ())
		{
			textf = create (name, group);
		}

		textf->load ();

		return textf;
	}

	Ogre::Resource *VolumeManager::createImpl (const Ogre::String &name, Ogre::ResourceHandle handle, 
												const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader, 
												const Ogre::NameValuePairList *createParams)
	{
		return new VolumeResource (this, name, handle, group, isManual, loader);
	}
}
