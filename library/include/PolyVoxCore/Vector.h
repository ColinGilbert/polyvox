#pragma region License
/******************************************************************************
This file is part of the PolyVox library
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
#pragma endregion

#ifndef __PolyVox_Vector_H__
#define __PolyVox_Vector_H__

#pragma region Headers
#include "PolyVoxForwardDeclarations.h"

#include <iostream>
#pragma endregion

namespace PolyVox
{
    ///Represents a vector in space.    
	template <uint32 Size, typename Type>
	class Vector
    {
    public:
        ///Constructor.
        Vector(Type x, Type y) throw();
		///Constructor.
		Vector(Type x, Type y, Type z) throw();
		///Constructor.
		Vector(Type x, Type y, Type z, Type w) throw();
		///Constructor
		Vector(void) throw();
        ///Copy Constructor.
        Vector(const Vector<Size,Type>& vector) throw();
		///Copy Constructor which performs casting.
		template <typename CastType> explicit Vector(const Vector<Size,CastType>& vector) throw();
        ///Destructor.
        ~Vector(void) throw();

        ///Assignment Operator.
        Vector<Size,Type>& operator=(const Vector<Size,Type>& rhs) throw();
        ///Equality Operator.
        bool operator==(const Vector<Size,Type>& rhs) const throw();
		///Comparison Operator.
        bool operator<(const Vector<Size,Type>& rhs) const throw();
        ///Addition and Assignment Operator.
        Vector<Size,Type>& operator+=(const Vector<Size,Type> &rhs) throw();
        ///Subtraction and Assignment Operator.
        Vector<Size,Type>& operator-=(const Vector<Size,Type> &rhs) throw();
        ///Multiplication and Assignment Operator.
        Vector<Size,Type>& operator*=(const Type& rhs) throw();
        ///Division and Assignment Operator.
        Vector<Size,Type>& operator/=(const Type& rhs) throw();

		///Element Access.
		Type getElement(uint32 index) const throw();
        ///Get the x component of the vector.
        Type getX(void) const throw();        
        ///Get the y component of the vector.
        Type getY(void) const throw();        
        ///Get the z component of the vector.
        Type getZ(void) const throw();        
		///Get the w component of the vector.
		Type getW(void) const throw();		

		///Element Access.
		void setElement(uint32 index, Type tValue) throw();
		///Element Access.
        void setElements(Type x, Type y) throw();
		///Element Access.
		void setElements(Type x, Type y, Type z) throw();
		///Element Access.
		void setElements(Type x, Type y, Type z, Type w) throw();
		///Set the x component of the vector.
		void setX(Type tX) throw();
		///Set the y component of the vector.
		void setY(Type tY) throw();
		///Set the z component of the vector.
		void setZ(Type tZ) throw();
		///Set the w component of the vector.
		void setW(Type tW) throw();

        ///Get the length of the vector.
        double length(void) const throw();
        ///Get the squared length of the vector.
        double lengthSquared(void) const throw();
        ///Find the angle between this vector and that which is passed as a parameter.
        double angleTo(const Vector<Size,Type>& vector) const throw();
        ///Find the cross product between this vector and the vector passed as a parameter.
        Vector<Size,Type> cross(const Vector<Size,Type>& vector) const throw();
        ///Find the dot product between this vector and the vector passed as a parameter.
        Type dot(const Vector<Size,Type>& rhs) const throw();
        ///Normalise the vector.
        void normalise(void) throw();

    private:
        //Values for the vector
		Type m_tElements[Size];
    };

    //Non-member overloaded operators. 
	///Addition operator.
	template <uint32 Size,typename Type>
	    Vector<Size,Type> operator+(const Vector<Size,Type>& lhs, const Vector<Size,Type>& rhs) throw();
	///Subtraction operator.
	template <uint32 Size,typename Type>
	    Vector<Size,Type> operator-(const Vector<Size,Type>& lhs, const Vector<Size,Type>& rhs) throw();
	///Multiplication operator.
	template <uint32 Size,typename Type>
	    Vector<Size,Type> operator*(const Vector<Size,Type>& lhs, const Type& rhs) throw();
	///Division operator.
	template <uint32 Size,typename Type>
	    Vector<Size,Type> operator/(const Vector<Size,Type>& lhs, const Type& rhs) throw();
    ///Stream insertion operator.
    template <uint32 Size, typename Type>
        std::ostream& operator<<(std::ostream& os, const Vector<Size,Type>& vector) throw();

	//Some handy typedefs
	///A 3D Vector of floats.
	typedef Vector<3,float> Vector3DFloat;
	///A 3D Vector of doubles.
    typedef Vector<3,double> Vector3DDouble;
	///A 3D Vector of signed 8-bit values.
	typedef Vector<3,int8> Vector3DInt8;
	///A 3D Vector of unsigned 8-bit values.
	typedef Vector<3,uint8> Vector3DUint8;
	///A 3D Vector of signed 16-bit values.
	typedef Vector<3,int16> Vector3DInt16;
	///A 3D Vector of unsigned 16-bit values.
	typedef Vector<3,uint16> Vector3DUint16;
	///A 3D Vector of signed 32-bit values.
	typedef Vector<3,int32> Vector3DInt32;
	///A 3D Vector of unsigned 32-bit values.
	typedef Vector<3,uint32> Vector3DUint32;



}//namespace Thermite

#include "Vector.inl"

#endif

