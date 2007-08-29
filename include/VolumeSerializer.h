#ifndef __VOLUMESERIALIZER_H__
#define __VOLUMESERIALIZER_H__

#include <OgreSerializer.h>

namespace Ogre
{
	class Volume; // forward declaration

	class VolumeSerializer : public Ogre::Serializer
	{
	public:
		VolumeSerializer ();
		virtual ~VolumeSerializer ();

		//void exportVolume (const Volume *pText, const Ogre::String &fileName);
		void importVolume (Ogre::DataStreamPtr &stream, Volume *pDest);
	};
}

#endif
