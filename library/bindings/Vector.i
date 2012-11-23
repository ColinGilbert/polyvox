%module Vector
%{
#include "Vector.h"
#include <sstream>
%}

%include "Vector.h"

#ifdef SWIGPYTHON
PROPERTY(PolyVox::Vector, x, getX, setX)
PROPERTY(PolyVox::Vector, y, getY, setY)
PROPERTY(PolyVox::Vector, z, getZ, setZ)
#endif

%extend PolyVox::Vector {
	PolyVox::Vector __add__(const PolyVox::Vector& rhs) {
		return *$self + rhs;
	}
	PolyVox::Vector __sub__(const PolyVox::Vector& rhs) {
		return *$self - rhs;
	}
	PolyVox::Vector __div__(const PolyVox::Vector& rhs) {
		return *$self / rhs;
	}
	PolyVox::Vector __div__(const StorageType& rhs) {
		return *$self / rhs;
	}
	PolyVox::Vector __mul__(const PolyVox::Vector& rhs) {
		return *$self * rhs;
	}
	PolyVox::Vector __mul__(const StorageType& rhs) {
		return *$self * rhs;
	}
	STR()
};

%template(Vector3DFloat) PolyVox::Vector<3,float,float>;
%template(Vector3DDouble) PolyVox::Vector<3,double,double>;
%template(Vector3DInt8) PolyVox::Vector<3,int8_t,int32_t>;
%template(Vector3DUint8) PolyVox::Vector<3,uint8_t,int32_t>;
%template(Vector3DInt16) PolyVox::Vector<3,int16_t,int32_t>;
%template(Vector3DUint16) PolyVox::Vector<3,uint16_t,int32_t>;
%template(Vector3DInt32) PolyVox::Vector<3,int32_t,int32_t>;
%template(Vector3DUint32) PolyVox::Vector<3,uint32_t,int32_t>;

//%rename(assign) Vector3DFloat::operator=;
