%module Vector
%{
#include "Vector.h"
//#include "PolyVoxCore/include/PolyVoxCStdInt.h"
%}

%include "Vector.h"

%template(Vector3DFloat) PolyVox::Vector<3,float>;
%template(Vector3DDouble) PolyVox::Vector<3,double>;
%template(Vector3DInt8) PolyVox::Vector<3,int8_t>;
%template(Vector3DUint8) PolyVox::Vector<3,uint8_t>;
%template(Vector3DInt16) PolyVox::Vector<3,int16_t>;
%template(Vector3DUint16) PolyVox::Vector<3,uint16_t>;
%template(Vector3DInt32) PolyVox::Vector<3,int32_t>;
%template(Vector3DUint32) PolyVox::Vector<3,uint32_t>;

%rename(assign) Vector3DFloat::operator=;
