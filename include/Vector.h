#ifndef __PolyVox_Vector_H__
#define __PolyVox_Vector_H__

#include <iostream>

#include "boost/cstdint.hpp"
#include <boost/operators.hpp>

namespace PolyVox
{
	//template <boost::uint32_t Size, typename Type> class Matrix; //Forward declaration

    ///Represents a vector in space.    
	template <boost::uint32_t Size, typename Type>
	class Vector
		: boost::addable< Vector<Size,Type>          // Vector + Vector
		, boost::subtractable< Vector<Size,Type>     // Vector - Vector
		, boost::dividable2< Vector<Size,Type>, Type    // Vector / Type
		, boost::multipliable2< Vector<Size,Type>, Type // Vector * Type, Type * Vector
		, boost::equality_comparable1< Vector<Size,Type> // Vector != Vector
		> > > > >
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
        ///Addition and Assignment Operator.
        Vector<Size,Type>& operator+=(const Vector<Size,Type> &rhs) throw();
        ///Subtraction and Assignment Operator.
        Vector<Size,Type>& operator-=(const Vector<Size,Type> &rhs) throw();
        ///Multiplication and Assignment Operator.
        Vector<Size,Type>& operator*=(const Type& rhs) throw();
        ///Division and Assignment Operator.
        Vector<Size,Type>& operator/=(const Type& rhs) throw();
		///Element Access
		Type operator()(boost::uint32_t index) const throw();		

        ///Get the x component of the vector.
        Type x(void) const throw();        
        ///Get the y component of the vector.
        Type y(void) const throw();        
        ///Get the z component of the vector.
        Type z(void) const throw();        
		///Get the w component of the vector.
		Type w(void) const throw();		

		///Element Access
		void set(boost::uint32_t index, Type tValue) throw();
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
	//!Multiplication operator.
	//template <boost::uint32_t Size,typename Type>
		//Vector<Size,Type> operator*(const Vector<Size,Type>& lhs, const Matrix<Size,Type>& rhs) throw();
	//!Multiplication operator.
	//template <boost::uint32_t Size,typename Type>
	    //Vector<Size,Type> operator*(const Matrix<Size,Type>& lhs, const Vector<Size,Type>& rhs) throw();	
    ///Stream insertion operator.
    template <boost::uint32_t Size, typename Type>
        std::ostream& operator<<(std::ostream& os, const Vector<Size,Type>& vector) throw();

	//Some handy typedefs
	typedef Vector<2,float> Vector2DFloat;
    typedef Vector<2,double> Vector2DDouble;
	typedef Vector<2,boost::int32_t> Vector2DInt32;
	typedef Vector<2,boost::uint32_t> Vector2DUint32;
	typedef Vector<3,float> Vector3DFloat;
    typedef Vector<3,double> Vector3DDouble;
	typedef Vector<3,boost::int32_t> Vector3DInt32;
	typedef Vector<3,boost::uint32_t> Vector3DUint32;
}//namespace Thermite

#include "Vector.inl"

#endif

