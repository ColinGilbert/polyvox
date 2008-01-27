#ifndef __MATERIALMAP_H__
#define __MATERIALMAP_H__

#include <OgreResourceManager.h>

#include "TypeDef.h"

namespace Ogre
{
	class VOXEL_SCENE_MANAGER_API MaterialMap : public Ogre::Resource
	{
	   String mMaterials[256];	   

	protected:

	   // must implement these from the Ogre::Resource interface
	   void loadImpl ();
	   void unloadImpl ();
	   size_t calculateSize () const;

	public:

	   MaterialMap (Ogre::ResourceManager *creator, const Ogre::String &name, 
		   Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false, 
		   Ogre::ManualResourceLoader *loader = 0);

	   virtual ~MaterialMap ();

	   void clearMaterials();
	   String getMaterialAtIndex(uchar uIndex);
	   void setMaterialAtIndex(uchar uIndex, const String& materialName);

	   /*void setString (const Ogre::String &str);
	   const Ogre::String &getString () const;*/
	};

	class MaterialMapPtr : public Ogre::SharedPtr<MaterialMap> 
	{
	public:
	   MaterialMapPtr () : Ogre::SharedPtr<MaterialMap> () {}
	   explicit MaterialMapPtr (MaterialMap *rep) : Ogre::SharedPtr<MaterialMap> (rep) {}
	   MaterialMapPtr (const MaterialMapPtr &r) : Ogre::SharedPtr<MaterialMap> (r) {} 
	   MaterialMapPtr (const Ogre::ResourcePtr &r) : Ogre::SharedPtr<MaterialMap> ()
	   {
		   // lock & copy other mutex pointer
		   OGRE_LOCK_MUTEX (*r.OGRE_AUTO_MUTEX_NAME)
		   OGRE_COPY_AUTO_SHARED_MUTEX (r.OGRE_AUTO_MUTEX_NAME)
		   pRep = static_cast<MaterialMap*> (r.getPointer ());
		   pUseCount = r.useCountPointer ();
		   if (pUseCount)
		   {
			   ++ (*pUseCount);
		   }
	   }

	   /// Operator used to convert a ResourcePtr to a TextFilePtr
	   MaterialMapPtr& operator=(const Ogre::ResourcePtr& r)
	   {
		   if (pRep == static_cast<MaterialMap*> (r.getPointer ()))
			   return *this;
		   release ();
		   // lock & copy other mutex pointer
		   OGRE_LOCK_MUTEX (*r.OGRE_AUTO_MUTEX_NAME)
		   OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
		   pRep = static_cast<MaterialMap*> (r.getPointer());
		   pUseCount = r.useCountPointer ();
		   if (pUseCount)
		   {
			   ++ (*pUseCount);
		   }
		   return *this;
	   }
	};
}

#endif
