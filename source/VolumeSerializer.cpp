#include "VolumeSerializer.h"
#include "Volume.h"

#include "VolumeIterator.h"

#include "OgreLogManager.h"
#include "OgreStringConverter.h"

namespace Ogre
{
	VolumeSerializer::VolumeSerializer ()
	{

	}

	VolumeSerializer::~VolumeSerializer ()
	{

	}

	/*void VolumeSerializer::exportVolume (const Volume *pText, const Ogre::String &fileName)
	{
		std::ofstream outFile;
		outFile.open (fileName.c_str(), std::ios::out);
		outFile << pText->getString ();
		outFile.close ();
	}*/

	void VolumeSerializer::importVolume (Ogre::DataStreamPtr &stream, Volume *pDest)
	{
		//pDest->setString (stream->getAsString ());
		//Volume vol;

		//Read volume dimensions
		uchar volumeWidth = 0;
		uchar volumeHeight = 0;
		uchar volumeDepth = 0;
		stream->read(reinterpret_cast<void*>(&volumeWidth), sizeof(volumeWidth));
		stream->read(reinterpret_cast<void*>(&volumeHeight), sizeof(volumeHeight));
		stream->read(reinterpret_cast<void*>(&volumeDepth), sizeof(volumeDepth));
		/*if(stream->fail())
		{
			LogManager::getSingleton().logMessage("Failed to read dimentions");
			return false;
		} */

		//Read data
		VolumeIterator volIter(*pDest);
		for(uint z = 0; z < OGRE_VOLUME_SIDE_LENGTH; ++z)
		{
			for(uint y = 0; y < OGRE_VOLUME_SIDE_LENGTH; ++y)
			{
				for(uint x = 0; x < OGRE_VOLUME_SIDE_LENGTH; ++x)
				{
					uchar value = 0;
					stream->read(reinterpret_cast<void*>(&value), sizeof(value)); //FIXME - check for error here
					/*if(value != 0)
					{
						LogManager::getSingleton().logMessage("Value is " + StringConverter::toString(int(value)));
					}*/
					volIter.setVoxelAt(x,y,z,value);
					if(z < 24)
					{
						//if(x % 32 < 16)
							volIter.setVoxelAt(x,y,z,4);
						//else
							//volIter.setVoxelAt(x,y,z,5);
					}
					else
						volIter.setVoxelAt(x,y,z,0);
				}
			}
			volIter.setVoxelAt(130,130,23,0);

			//Periodically see if we can tidy the memory to avoid excessive usage during loading.
			if(z%OGRE_BLOCK_SIDE_LENGTH == OGRE_BLOCK_SIDE_LENGTH-1)
			{
				pDest->tidy(); //FIXME - we don't actually have to tidy the whole volume here - just the part we loaded since the last call to tidy.
			}
		}

		/*vol.load(stream->getName());*/
		//pDest->setVolume(vol);
	}
}
