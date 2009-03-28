%module Vector
%{
#include "PolyVoxCore/Vector.h"
%}

%include "PolyVoxCore/Vector.h"

%template(VectorTest) PolyVox::Vector<3,float>;

%rename(assign) VectorTest::operator=;
