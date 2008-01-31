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

#include "IntegralVector3.h"
#include "VolumeResource.h"
#include "VolumeIterator.h"

#include "VolumeSerializer.h"

#include "OgreVector3.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"

#include <iostream> //FIXME - remove this...

namespace Ogre
{
	VolumeResource::VolumeResource (Ogre::ResourceManager* creator, const Ogre::String &name, 
		Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual, 
		Ogre::ManualResourceLoader *loader) :
	Ogre::Resource (creator, name, handle, group, isManual, loader)
	{
		/* If you were storing a pointer to an object, then you would set that pointer to NULL here.
		*/

		/* For consistency with StringInterface, but we don't add any parameters here
		That's because the Resource implementation of StringInterface is to
		list all the options that need to be set before loading, of which 
		we have none as such. Full details can be set through scripts.
		*/ 
		createParamDictionary ("Volume");

		volume = new Volume();
	}

	VolumeResource::~VolumeResource()
	{
		unload ();
	}	

	// farm out to VolumeSerializer
	void VolumeResource::loadImpl ()
	{
		/* If you were storing a pointer to an object, then you would create that object with 'new' here.
		*/

		VolumeSerializer serializer;
		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton ().openResource (mName, mGroup, true, this);
		serializer.importVolume (stream, this->volume);
	}

	void VolumeResource::unloadImpl ()
	{
		/* If you were storing a pointer to an object, then you would check the pointer here,
		and if it is not NULL, you would destruct the object and set its pointer to NULL again.
		*/

		//mString.clear ();
	}

	size_t VolumeResource::calculateSize () const
	{
		//NOTE - I don't really know what this function is for, so am therefore
		//a bit vague on how to implement it. But here's my best guess...
		ulong uNonHomogeneousBlocks = 0;
		for(uint i = 0; i < OGRE_NO_OF_BLOCKS_IN_VOLUME; ++i)
		{
			//I think this is OK... If a block is in the homogeneous array it's ref count will be greater
			//than 1 as there will be the pointer in the volume and the pointer in the static homogeneous array.
			if(volume->mBlocks[i].unique())
			{
				++uNonHomogeneousBlocks;
			}
		}
		return uNonHomogeneousBlocks * OGRE_NO_OF_VOXELS_IN_BLOCK;
	}
}
