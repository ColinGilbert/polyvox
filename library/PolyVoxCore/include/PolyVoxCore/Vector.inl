/*******************************************************************************
Copyright (c) 2005-2009 David Williams

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

namespace PolyVox
{
    //-------------------------- Constructors, etc ---------------------------------
	/**
	Creates a Vector object but does not initialise it.
	*/
	template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::Vector(void)
	{
	}

	/**
    Creates a Vector object and initialises it with given values.
    \param x x component to set.
    */
    template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType>::Vector(StorageType tFillValue)
    {
		for(uint32_t ct = 0; ct < Size; ct++)
		{
			m_tElements[ct] = tFillValue;
		}
    }

    /**
    Creates a Vector object and initialises it with given values.
    \param x x component to set.
    \param y y component to set.
    */
    template <uint32_t Size,typename StorageType, typename OperationType>
    Vector<Size,StorageType,OperationType>::Vector(StorageType x, StorageType y)
    {
#ifndef SWIGPYTHON // SWIG instantiates all constructors, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size == 2, "This constructor should only be used for vectors with two elements.");
#endif

		m_tElements[0] = x;
		m_tElements[1] = y;
    }

	/**
	Creates a Vector3D object and initialises it with given values.
	\param x x component to set.
	\param y y component to set.
	\param z z component to set.
	*/
	template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType>::Vector(StorageType x, StorageType y, StorageType z)
	{
#ifndef SWIGPYTHON // SWIG instantiates all constructors, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size == 3, "This constructor should only be used for vectors with three elements.");
#endif

		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;

	}

	/**
	Creates a Vector3D object and initialises it with given values.
	\param x x component to set.
	\param y y component to set.
	\param z z component to set.
	\param w w component to set.
	*/
	template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType>::Vector(StorageType x, StorageType y, StorageType z, StorageType w)
	{
#ifndef SWIGPYTHON // SWIG instantiates all constructors, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size == 4, "This constructor should only be used for vectors with four elements.");
#endif

		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;
		m_tElements[3] = w;
	}

    /**
    Copy constructor builds object based on object passed as parameter.
    \param vector A reference to the Vector to be copied.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::Vector(const Vector<Size, StorageType, OperationType>& vector)
    {
		std::memcpy(m_tElements, vector.m_tElements, sizeof(StorageType) * Size);
    }

	/**
	This copy constructor allows casting between vectors with different data types.
	It is now possible to use code such as:
	
	Vector3DDouble v3dDouble(1.0,2.0,3.0);
	Vector3DFloat v3dFloat = static_cast<Vector3DFloat>(v3dDouble); //Casting

	\param vector A reference to the Vector to be copied.
	*/
	template <uint32_t Size, typename StorageType, typename OperationType>
	template <typename CastType>
		Vector<Size, StorageType, OperationType>::Vector(const Vector<Size, CastType>& vector)
	{
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] = static_cast<StorageType>(vector.getElement(ct));
		}
	}

    /**
    Destroys the Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>::~Vector(void)
    {
		// We put the static_asserts in the destructor because there is one one of these,
		// where as there are multiple constructors.

		// Force a vector to have a length greater than one. There is no need for a
		// vector with one element, and supporting this would cause confusion over the
		// behaviour of the constructor taking a single value, as this fills all elements
		// to that value rather than just the first one.
		static_assert(Size > 1, "Vector must have a length greater than one.");
    }

    /**
    Assignment operator copies each element of first Vector to the second.
    \param rhs Vector to assign to.
    \return A reference to the result to allow chaining.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator=(const Vector<Size, StorageType, OperationType>& rhs)
    {
        if(this == &rhs)
		{
			return *this;
		}
        std::memcpy(m_tElements, rhs.m_tElements, sizeof(StorageType) * Size);
        return *this;
    }

    /**
    Checks whether two Vectors are equal.
    \param rhs The Vector to compare to.
    \return true if the Vectors match.
    \see operator!=
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline bool Vector<Size, StorageType, OperationType>::operator==(const Vector<Size, StorageType, OperationType> &rhs) const
    {
		bool equal = true;
        for(uint32_t ct = 0; ct < Size; ++ct)
		{
			if(m_tElements[ct] != rhs.m_tElements[ct])
			{
				equal = false;
				break;
			}
		}
		return equal;
    }

	/**
    Checks whether two Vectors are not equal.
    \param rhs The Vector to compare to.
    \return true if the Vectors do not match.
    \see operator==
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline bool Vector<Size, StorageType, OperationType>::operator!=(const Vector<Size, StorageType, OperationType> &rhs) const
    {
		return !(*this == rhs); //Just call equality operator and invert the result.
    }

	/**
    Checks whether this vector is less than the parameter. The metric is
	meaningless but it allows Vectors to me used as key in sdt::map, etc.
    \param rhs The Vector to compare to.
    \return true if this is less than the parameter
    \see operator!=
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline bool Vector<Size, StorageType, OperationType>::operator<(const Vector<Size, StorageType, OperationType> &rhs) const
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			if (m_tElements[ct] < rhs.m_tElements[ct])
				return true;
			if (rhs.m_tElements[ct] < m_tElements[ct])
				return false;
		}
		return false;
    }    

    /**
    Addition operator adds corresponding elements of the two Vectors.
    \param rhs Vector to add
    \return The resulting Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator+=(const Vector<Size, StorageType, OperationType>& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] += rhs.m_tElements[ct];
		}
        return *this;
    }

	/**
    Subtraction operator subtracts corresponding elements of one Vector from the other.
    \param rhs Vector to subtract
    \return The resulting Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator-=(const Vector<Size, StorageType, OperationType>& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] -= rhs.m_tElements[ct];
		}
        return *this;
    }

	/**
    Multiplication operator multiplies corresponding elements of the two Vectors.
    \param rhs Vector to multiply by
    \return The resulting Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator*=(const Vector<Size, StorageType, OperationType>& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] *= rhs.m_tElements[ct];
		}
        return *this;
    }

	/**
    Division operator divides corresponding elements of one Vector by the other.
    \param rhs Vector to divide by
    \return The resulting Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator/=(const Vector<Size, StorageType, OperationType>& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] /= rhs.m_tElements[ct];
		}
        return *this;
    }

    /**
    Multiplication operator multiplies each element of the Vector by a number.
    \param rhs the number the Vector is multiplied by.
    \return The resulting Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator*=(const StorageType& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] *= rhs;
		}
        return *this;
    }

    /**
	Division operator divides each element of the Vector by a number.
	\param rhs the number the Vector is divided by.
	\return The resulting Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType>& Vector<Size, StorageType, OperationType>::operator/=(const StorageType& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] /= rhs;
		}
        return *this;
    }

	/**
    Addition operator adds corresponding elements of the two Vectors.
	\param lhs Vector to add to.
    \param rhs Vector to add.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType> operator+(const Vector<Size,StorageType,OperationType>& lhs, const Vector<Size,StorageType,OperationType>& rhs)
	{
		Vector<Size,StorageType,OperationType> result = lhs;
		result += rhs;
		return result;
	}

	/**
    Subtraction operator subtracts corresponding elements of one Vector from the other.
	\param lhs Vector to subtract from.
    \param rhs Vector to subtract.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType> operator-(const Vector<Size,StorageType,OperationType>& lhs, const Vector<Size,StorageType,OperationType>& rhs)
	{
		Vector<Size,StorageType,OperationType> result = lhs;
		result -= rhs;
		return result;
	}

	/**
    Multiplication operator mulitplies corresponding elements of the two Vectors.
	\param lhs Vector to multiply.
    \param rhs Vector to multiply by.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType> operator*(const Vector<Size,StorageType,OperationType>& lhs, const Vector<Size,StorageType,OperationType>& rhs)
	{
		Vector<Size,StorageType,OperationType> result = lhs;
		result *= rhs;
		return result;
	}

	/**
    Division operator divides corresponding elements of one Vector by the other.
	\param lhs Vector to divide.
    \param rhs Vector to divide by.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType> operator/(const Vector<Size,StorageType,OperationType>& lhs, const Vector<Size,StorageType,OperationType>& rhs)
	{
		Vector<Size,StorageType,OperationType> result = lhs;
		result /= rhs;
		return result;
	}

	/**
    Multiplication operator multiplies each element of the Vector by a number.
	\param lhs the Vector to multiply.
    \param rhs the number the Vector is multiplied by.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType> operator*(const Vector<Size,StorageType,OperationType>& lhs, const StorageType& rhs)
	{
		Vector<Size,StorageType,OperationType> result = lhs;
		result *= rhs;
		return result;
	}

	/**
	Division operator divides each element of the Vector by a number.
	\param lhs the Vector to divide.
	\param rhs the number the Vector is divided by.
	\return The resulting Vector.
    */
	template <uint32_t Size,typename StorageType, typename OperationType>
	Vector<Size,StorageType,OperationType> operator/(const Vector<Size,StorageType,OperationType>& lhs, const StorageType& rhs)
	{
		Vector<Size,StorageType,OperationType> result = lhs;
		result /= rhs;
		return result;
	}

    /**
    Enables the Vector to be used intuitively with output streams such as cout.
    \param os The output stream to write to.
    \param vector The Vector to write to the stream.
    \return A reference to the output stream to allow chaining.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	std::ostream& operator<<(std::ostream& os, const Vector<Size, StorageType, OperationType>& vector)
    {
        os << "(";
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			os << vector.getElement(ct);
			if(ct < (Size-1))
			{
				os << ",";
			}
		}
		os << ")";
        return os;
    }		

	/**
	Returns the element at the given position.
	\param index The index of the element to return.
	\return The element.
	*/
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getElement(uint32_t index) const
	{
		assert(index < Size);
		return m_tElements[index];
	}

    /**
    \return A const reference to the X component of a 1, 2, 3, or 4 dimensional Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getX(void) const
    {
        return m_tElements[0]; // This is fine, a Vector always contains at least two elements.
    }	

	/**
	\return A const reference to the Y component of a 2, 3, or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getY(void) const
    {
        return m_tElements[1]; // This is fine, a Vector always contains at least two elements.
    }	

	/**
	\return A const reference to the Z component of a 3 or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getZ(void) const
    {
#ifndef SWIGPYTHON // SWIG instantiates all getters, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size >= 3, "You can only get the 'z' component from a vector with at least three elements.");
#endif

        return m_tElements[2];
    }	

	/**
	\return A const reference to the W component of a 4 dimensional Vector.
	*/
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline StorageType Vector<Size, StorageType, OperationType>::getW(void) const
	{
#ifndef SWIGPYTHON // SWIG instantiates all getters, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size >= 4, "You can only get the 'w' component from a vector with at least four elements.");
#endif

		return m_tElements[3];
	}  

	/**
	\param index The index of the element to set.
	\param tValue The new value for the element.
	*/
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setElement(uint32_t index, StorageType tValue)
	{
		assert(index < Size);
		m_tElements[index] = tValue;
	}

	/**
    Sets several elements of a vector at once.
    \param x x component to set.
    \param y y component to set.
    */
    template <uint32_t Size,typename StorageType, typename OperationType>
	inline void Vector<Size,StorageType,OperationType>::setElements(StorageType x, StorageType y)
    {
		// This is fine, a Vector always contains at least two elements.
		m_tElements[0] = x;
		m_tElements[1] = y;
    }

	/**
	Sets several elements of a vector at once.
	\param x x component to set.
	\param y y component to set.
	\param z z component to set.
	*/
	template <uint32_t Size,typename StorageType, typename OperationType>
	inline void Vector<Size,StorageType,OperationType>::setElements(StorageType x, StorageType y, StorageType z)
	{
#ifndef SWIGPYTHON // SWIG instantiates all setters, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size >= 3, "You can only use this version of setElements() on a vector with at least three elements.");
#endif
		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;
	}

	/**
	Sets several elements of a vector at once.
	\param x x component to set.
	\param y y component to set.
	\param z z component to set.
	\param w w component to set.
	*/
	template <uint32_t Size,typename StorageType, typename OperationType>
	inline void Vector<Size,StorageType,OperationType>::setElements(StorageType x, StorageType y, StorageType z, StorageType w)
	{
#ifndef SWIGPYTHON // SWIG instantiates all setters, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size >= 4, "You can only use this version of setElements() on a vector with at least four elements.");
#endif
		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;
		m_tElements[3] = w;
	}

	/**
	\param tX The new value for the X component of a 1, 2, 3, or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setX(StorageType tX)
    {
        m_tElements[0] = tX; // This is fine, a Vector always contains at least two elements.
    }

	/**
	\param tY The new value for the Y component of a 2, 3, or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setY(StorageType tY)
    {
        m_tElements[1] = tY; // This is fine, a Vector always contains at least two elements.
    }

	/**
	\param tZ The new value for the Z component of a 3 or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setZ(StorageType tZ)
    {
#ifndef SWIGPYTHON // SWIG instantiates all setters, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size >= 3, "You can only set the 'w' component from a vector with at least three elements.");
#endif
        m_tElements[2] = tZ;
    }

	/**
	\param tW The new value for the W component of a 4 dimensional Vector.
	*/
	template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::setW(StorageType tW)
    {
#ifndef SWIGPYTHON // SWIG instantiates all setters, unless we can find a way around that. Should we use SWIGIMPORT here, and then %import this file rather then %include it?
		static_assert(Size >= 4, "You can only set the 'w' component from a vector with at least four elements.");
#endif
        m_tElements[3] = tW;
    }

	/**
	\note This function does not make much sense on integer Vectors.
    \return Length of the Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline float Vector<Size, StorageType, OperationType>::length(void) const
    {
        return sqrt(static_cast<float>(lengthSquared()));
    }

    /**
    \return Squared length of the Vector.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline OperationType Vector<Size, StorageType, OperationType>::lengthSquared(void) const
    {
		OperationType tLengthSquared = static_cast<OperationType>(0);
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			tLengthSquared += static_cast<OperationType>(m_tElements[ct]) * static_cast<OperationType>(m_tElements[ct]);
		}
		return tLengthSquared;
    }

    /**
    This function is commutative, such that a.angleTo(b) == b.angleTo(a). The angle
    returned is in radians and varies between 0 and 3.14(pi). It is always positive.

	\note This function does not make much sense on integer Vectors.

    \param vector The Vector to find the angle to.
    \return The angle between them in radians.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline float Vector<Size, StorageType, OperationType>::angleTo(const Vector<Size, StorageType, OperationType>& vector) const
    {
        return acos(static_cast<float>(dot(vector)) / (vector.length() * this->length()));
    }

    /**
    This function is used to calculate the cross product of two Vectors.
    The cross product is the Vector which is perpendicular to the two
    given Vectors. It is worth remembering that, unlike the dot product,
    it is not commutative. E.g a.b != b.a. The cross product obeys the 
	right-hand rule such that if the two vectors are given by the index
	finger and middle finger respectively then the cross product is given
	by the thumb.
    \param vector The vector to cross with this
    \return The value of the cross product.
    \see dot()
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline Vector<Size, StorageType, OperationType> Vector<Size, StorageType, OperationType>::cross(const Vector<Size, StorageType, OperationType>& vector) const
    {
        StorageType i = vector.getZ() * this->getY() - vector.getY() * this->getZ();
        StorageType j = vector.getX() * this->getZ() - vector.getZ() * this->getX();
        StorageType k = vector.getY() * this->getX() - vector.getX() * this->getY();
        return Vector<Size, StorageType, OperationType>(i,j,k);
    }

    /**
    Calculates the dot product of the Vector and the parameter.
    This function is commutative, such that a.dot(b) == b.dot(a).
    \param rhs The Vector to find the dot product with.
    \return The value of the dot product.
    \see cross()
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline OperationType Vector<Size, StorageType, OperationType>::dot(const Vector<Size, StorageType, OperationType>& rhs) const
    {
        OperationType dotProduct = static_cast<OperationType>(0);
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			dotProduct += static_cast<OperationType>(m_tElements[ct]) * static_cast<OperationType>(rhs.m_tElements[ct]);
		}
		return dotProduct;
    }

    /**
    Divides the i, j, and k components by the length to give a Vector of length 1.0.

	\note This function does not make much sense on integer Vectors.
    */
    template <uint32_t Size, typename StorageType, typename OperationType>
	inline void Vector<Size, StorageType, OperationType>::normalise(void)
    {
        StorageType tLength = static_cast<StorageType>(this->length());
		//FIXME - throw div by zero exception?
		if(tLength < 0.0001f)
		{
			return;
		}
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] /= tLength;
		}
    }
}//namespace PolyVox
