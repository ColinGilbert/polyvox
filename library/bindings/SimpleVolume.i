%module SimpleVolume
%{
#include "Material.h"
#include "Density.h"
#include "SimpleVolume.h"
%}

%include "Material.h"
%include "Density.h"
%include "SimpleVolume.h"

%template(SimpleVolumeDensity8) PolyVox::SimpleVolume<PolyVox::Density8>;

%template(SimpleVolumeMaterial8) PolyVox::SimpleVolume<PolyVox::Material8>;

%template(SimpleVolumeMaterial16) PolyVox::SimpleVolume<PolyVox::Material16>;

%template(SimpleVolumeMaterialDensityPair44) PolyVox::SimpleVolume<PolyVox::MaterialDensityPair44>;

%template(SimpleVolumeMaterialDensityPair88) PolyVox::SimpleVolume<PolyVox::MaterialDensityPair88>;
