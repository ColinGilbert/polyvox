#include "MaterialMapSerializer.h"
#include "MaterialMap.h"

#include <OgreStringConverter.h>

using namespace std;

namespace Ogre
{
	MaterialMapSerializer::MaterialMapSerializer ()
	{

	}

	MaterialMapSerializer::~MaterialMapSerializer ()
	{

	}

	void MaterialMapSerializer::importMaterialMap (Ogre::DataStreamPtr &stream, MaterialMap *pDest)
	{
		pDest->clearMaterials();
		while(!stream->eof())
		{
			String line = stream->getLine();
			vector<String> tokens;
			tokenize(line, tokens, " =");
			if(tokens.size() == 2)
			{
				//Get the index
				String sIndex = tokens[0];
				if(StringConverter::isNumber(sIndex))
				{
					int iIndex = StringConverter::parseInt(sIndex);
					if((iIndex > 0) && (iIndex < 256))
					{
						pDest->setMaterialAtIndex(iIndex, tokens[1]);
					}
				}
			}
		}
	}

	void MaterialMapSerializer::tokenize(const String& str, vector<String>& tokens, const String& delimiters)
	{
		// Skip delimiters at beginning.
		String::size_type lastPos = str.find_first_not_of(delimiters, 0);
		// Find first "non-delimiter".
		String::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (String::npos != pos || String::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	}
}
