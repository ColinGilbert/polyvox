%module SimpleVolume
%{
#include "Material.h"
#include "Density.h"
#include "BaseVolume.h"
%}

%include "Material.h"
%include "Density.h"
%include "BaseVolume.h"

%template(BaseVolumeDensity8) PolyVox::BaseVolume<PolyVox::Density8>;

%template(BaseVolumeMaterial8) PolyVox::BaseVolume<PolyVox::Material8>;

%template(BaseVolumeMaterial16) PolyVox::BaseVolume<PolyVox::Material16>;

%template(BaseVolumeMaterialDensityPair44) PolyVox::BaseVolume<PolyVox::MaterialDensityPair44>;

%template(BaseVolumeMaterialDensityPair88) PolyVox::BaseVolume<PolyVox::MaterialDensityPair88>;
