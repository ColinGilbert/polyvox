%module Vector
%{
#include "PolyVoxCore/Vector.h"
//#include "PolyVoxCore/PolyVoxCStdInt.h"
%}

%include "PolyVoxCore/Vector.h"

%template(Vector3DFloat) PolyVox::Vector<3,float>;
%template(Vector3DDouble) PolyVox::Vector<3,double>;
//%template(Vector3DInt8) PolyVox::Vector<3,int8>;
//%template(Vector3DUint8) PolyVox::Vector<3,uint8>;
//%template(Vector3DInt16) PolyVox::Vector<3,int16>;
//%template(Vector3DUint16) PolyVox::Vector<3,uint16>;
//%template(Vector3DInt32) PolyVox::Vector<3,int32>;
//%template(Vector3DUint32) PolyVox::Vector<3,uint32>;

%rename(assign) Vector3DFloat::operator=;
