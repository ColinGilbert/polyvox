#ifndef __MATERIALMAPSERIALIZER_H__
#define __MATERIALMAPSERIALIZER_H__

#include <OgreSerializer.h>
#include <OgreString.h>

#include <vector>

namespace Ogre
{
	class MaterialMap; // forward declaration

	class MaterialMapSerializer : public Ogre::Serializer
	{
	public:
	   MaterialMapSerializer ();
	   virtual ~MaterialMapSerializer ();

	   void importMaterialMap (Ogre::DataStreamPtr &stream, MaterialMap *pDest);

	private:
		void tokenize(const String& str, std::vector<String>& tokens, const String& delimiters = " ");
	};
}

#endif
