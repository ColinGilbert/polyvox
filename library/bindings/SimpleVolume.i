%module SimpleVolume
%{
#include "Material.h"
#include "Density.h"
#include "SimpleVolume.h"
%}

%include "Material.h"
%include "Density.h"
%include "SimpleVolume.h"

%template(SimpleVolumeMaterial8) PolyVox::SimpleVolume<PolyVox::Material8>;
%template(SimpleVolumeDensity8) PolyVox::SimpleVolume<PolyVox::Density8>;