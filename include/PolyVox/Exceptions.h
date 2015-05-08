/*******************************************************************************
Copyright (c) 2005-2015 David Williams and Matthew Williams

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

#ifndef __PolyVox_Exceptions_H__
#define __PolyVox_Exceptions_H__

#include <stdexcept> // For base exception classes.
#include <string.h> // Exception constuctors take strings.

// These exceptions form part of the public API because lcient code may need to catch them.
// Note that our utility macros such as 'POLYVOX_THROW_IF' do not form part of the public API
// as they are only for our own internal use.
namespace PolyVox
{
	/// A general purpose exception to indicate that an operation cannot be peformed.
	class invalid_operation : public std::logic_error
	{
	public:
		explicit invalid_operation(const std::string& message)
			: logic_error(message.c_str()) {}

		explicit invalid_operation(const char *message)
			: logic_error(message) {}
	};

	/// Thrown to indicate that a function is deliberatly not implmented. For example, perhaps you called a function
	/// in a base class whereas you are supposed to use a derived class which implements the function, or perhaps the
	/// function is not defined for a particular template parameter. It may be that the function is required to
	/// compile sucessfully but it should not be called.
	class not_implemented : public std::logic_error
	{
	public:
		explicit not_implemented(const std::string& message)
			: logic_error(message.c_str()) {}

		explicit not_implemented(const char *message)
			: logic_error(message) {}
	};
}

#endif //__PolyVox_Exceptions_H__
