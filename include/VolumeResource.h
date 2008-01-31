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
#ifndef __VolumeResource_H__
#define __VolumeResource_H__

#include "OgrePrerequisites.h"
#include "OgreSharedPtr.h"

#include "Block.h"
#include "Constants.h"
#include "TypeDef.h"
#include "IntegralVector3.h"
#include "Volume.h"

#include <OgreResourceManager.h>

namespace Ogre
{
	class VOXEL_SCENE_MANAGER_API VolumeResource : public Ogre::Resource
	{
	public:		
		VolumeResource (Ogre::ResourceManager *creator, const Ogre::String &name, 
			Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false, 
			Ogre::ManualResourceLoader *loader = 0);
		~VolumeResource();

	Volume* volume;

	protected:

		// must implement these from the Ogre::Resource interface
		void loadImpl ();
		void unloadImpl ();
		size_t calculateSize () const;
	};

	class VolumeResourcePtr : public Ogre::SharedPtr<VolumeResource> 
	{
	public:
		VolumeResourcePtr () : Ogre::SharedPtr<VolumeResource> () {}
		explicit VolumeResourcePtr (VolumeResource *rep) : Ogre::SharedPtr<VolumeResource> (rep) {}
		VolumeResourcePtr (const VolumeResourcePtr &r) : Ogre::SharedPtr<VolumeResource> (r) {} 
		VolumeResourcePtr (const Ogre::ResourcePtr &r) : Ogre::SharedPtr<VolumeResource> ()
		{
			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX (*r.OGRE_AUTO_MUTEX_NAME)
				OGRE_COPY_AUTO_SHARED_MUTEX (r.OGRE_AUTO_MUTEX_NAME)
				pRep = static_cast<VolumeResource*> (r.getPointer ());
			pUseCount = r.useCountPointer ();
			if (pUseCount)
			{
				++ (*pUseCount);
			}
		}

		/// Operator used to convert a ResourcePtr to a VolumeResourcePtr
		VolumeResourcePtr& operator=(const Ogre::ResourcePtr& r)
		{
			if (pRep == static_cast<VolumeResource*> (r.getPointer ()))
				return *this;
			release ();
			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX (*r.OGRE_AUTO_MUTEX_NAME)
				OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
				pRep = static_cast<VolumeResource*> (r.getPointer());
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
