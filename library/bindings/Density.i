%module Density
%{
#include "Density.h"
%}

%include "Voxel.h"
%include "SurfaceExtractor.h"
%include "Density.h"

%template(Density8) PolyVox::Density<uint8_t>;
