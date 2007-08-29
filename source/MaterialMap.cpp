#include "MaterialMap.h"
#include "MaterialMapSerializer.h"

namespace Ogre
{
	MaterialMap::MaterialMap (Ogre::ResourceManager* creator, const Ogre::String &name, 
	   Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual, 
	   Ogre::ManualResourceLoader *loader) :
	   Ogre::Resource (creator, name, handle, group, isManual, loader)
	{
	   /* If you were storing a pointer to an object, then you would set that pointer to NULL here.
	   */

		clearMaterials();

	   /* For consistency with StringInterface, but we don't add any parameters here
	   That's because the Resource implementation of StringInterface is to
	   list all the options that need to be set before loading, of which 
	   we have none as such. Full details can be set through scripts.
	   */ 
	   createParamDictionary ("MaterialMap");
	}

	MaterialMap::~MaterialMap ()
	{
	   unload ();
	}

	void MaterialMap::clearMaterials()
	{
		for(uint ct = 0; ct < 256; ++ct)
		{
			mMaterials[ct] = "";
		}
	}

	// farm out to TextFileSerializer
	void MaterialMap::loadImpl ()
	{
	   /* If you were storing a pointer to an object, then you would create that object with 'new' here.
	   */

	   MaterialMapSerializer serializer;
	   Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton ().openResource (mName, mGroup, true, this);
	   serializer.importMaterialMap (stream, this);
	}

	void MaterialMap::unloadImpl ()
	{
	   /* If you were storing a pointer to an object, then you would check the pointer here,
	   and if it is not NULL, you would destruct the object and set its pointer to NULL again.
	   */

	   //mString.clear ();
	}

	size_t MaterialMap::calculateSize () const
	{
		size_t uSumOfLengths = 0;
		for(uint ct = 0; ct < 256; ++ct)
		{
			uSumOfLengths += mMaterials[ct].length();
		}
	   return uSumOfLengths;
	}

	String MaterialMap::getMaterialAtIndex(uchar uIndex)
	{
		return mMaterials[uIndex];
	}

	void MaterialMap::setMaterialAtIndex(uchar uIndex, const String& materialName)
	{
		mMaterials[uIndex] = materialName;
	}

	/*void MaterialMap::setString (const Ogre::String &str)
	{
	   mString = str;
	}

	const Ogre::String &MaterialMap::getString () const
	{
	   return mString;
	}*/
}
