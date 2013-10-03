/*******************************************************************************
Copyright (c) 2005-2013 David Williams

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

#ifndef __PolyVox_MinizWrapper_H__
#define __PolyVox_MinizWrapper_H__

// The miniz library is suplied as a single '.c' file, but this is messy for a project like PolyVox
// because it consists mostly of headers. Many of our headers may want to make use of Miniz code which
// means we need to be careful about #including 'miniz.c' multiple times and getting linker errors.
// We simplify this situation with this 'MinizWrapper.h' and 'MinizWrapper.c' which include 'miniz.c'
// with only declarations and definitions respectively. 'MinizWrapper.cpp' only gets compiled once,
// and all other parts of PolyVox can simply include 'MinizWrapper.h' to get the declarations.

// Diable things we don't need, and in particular the zlib compatible names which
// would cause conflicts if a user application is using both PolyVox and zlib.
#define MINIZ_NO_STDIO
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_TIME
#define MINIZ_NO_ZLIB_APIS
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#define MINIZ_NO_MALLOC

// Include only the declarations of the functions in miniz.c. Don't include
// the actual definitions, as this 'MinizWrapper.h' may be included from multiple
// locations and we want to avoid linker errors regarding multiple definitions.
#define MINIZ_HEADER_FILE_ONLY
#include "PolyVoxCore/Impl/miniz.c"

#endif //__PolyVox_MinizWrapper_H__