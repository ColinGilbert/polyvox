%module Volume
%{
#include "Volume.h"
%}

%include "Volume.h"

%template(VolumeUint16) PolyVox::Volume<unsigned int>;
