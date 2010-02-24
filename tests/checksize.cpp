/*******************************************************************************
Copyright (c) 2005-2009 David Williams
Copyright (c) 2010 Matt Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#include "Volume.h"

#ifdef WIN32
#include <windows.h>   // Standard Header For Most Programs
#endif

//Some namespaces we need
using namespace std;
using namespace PolyVox;

int main(int argc, char *argv[])
{
	const PolyVox::uint16_t g_uVolumeSideLength = 128;
	Volume<PolyVox::uint8_t> volData(g_uVolumeSideLength, g_uVolumeSideLength, g_uVolumeSideLength);

	cout << "Tidying memory...";
	volData.tidyUpMemory(0);
	cout << "done." << endl;
	
	if(volData.getWidth() == g_uVolumeSideLength && volData.getHeight() == g_uVolumeSideLength && volData.getDepth() == g_uVolumeSideLength)
	{
		cout << "Success" << endl;
		return EXIT_SUCCESS;
	}
	else
	{
		cout << "Failure" << endl;
		return EXIT_FAILURE;
	}
} 
