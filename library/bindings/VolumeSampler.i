%module VolumeSampler
%{
#include "VolumeSampler.h"
%}

%include "VolumeSampler.h"

%template(VolumeSamplerMaterial8) PolyVox::VolumeSampler<PolyVox::Material8>;
%template(VolumeSamplerDensity8) PolyVox::VolumeSampler<PolyVox::Density8>;