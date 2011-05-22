%module SimpleVolumeSampler
%{
#include "SimpleVolume.h"
%}

%include "SimpleVolume.h"

%template(SimpleVolumeSamplerMaterial8) PolyVox::SimpleVolumeSampler<PolyVox::Material8>;
%template(SimpleVolumeSamplerDensity8) PolyVox::SimpleVolumeSampler<PolyVox::Density8>;