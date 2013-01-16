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
#ifdef SWIGPYTHON
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
#endif
	STR()
};

%feature("pythonprepend") PolyVox::Vector::operator< %{
	import warnings
	warnings.warn("deprecated", DeprecationWarning)
%}

//%csattributes PolyVox::Vector::operator< "[System.Obsolete(\"deprecated\")]"

%define VECTOR3(StorageType,OperationType,ReducedStorageType)
%ignore PolyVox::Vector<3,StorageType,OperationType>::Vector(ReducedStorageType,ReducedStorageType,ReducedStorageType,ReducedStorageType);
%ignore PolyVox::Vector<3,StorageType,OperationType>::Vector(ReducedStorageType,ReducedStorageType);
%ignore PolyVox::Vector<3,StorageType,OperationType>::getW() const;
%ignore PolyVox::Vector<3,StorageType,OperationType>::setW(ReducedStorageType);
%ignore PolyVox::Vector<3,StorageType,OperationType>::setElements(ReducedStorageType,ReducedStorageType,ReducedStorageType,ReducedStorageType);
%template(Vector3D ## StorageType) PolyVox::Vector<3,StorageType,OperationType>;
%enddef

VECTOR3(float,float,float)
VECTOR3(double,double,double)
VECTOR3(int8_t,int32_t,signed char)
VECTOR3(uint8_t,int32_t,unsigned char)
VECTOR3(int16_t,int32_t,signed short)
VECTOR3(uint16_t,int32_t,unsigned short)
VECTOR3(int32_t,int32_t,signed int)
VECTOR3(uint32_t,int32_t,unsigned int)

//%rename(assign) Vector3DFloat::operator=;
