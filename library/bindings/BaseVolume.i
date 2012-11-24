%module SimpleVolume
%{
#include "Material.h"
#include "Density.h"
#include "BaseVolume.h"
%}

%include "Material.h"
%include "Density.h"
%include "BaseVolume.h"

VOLUMETYPES(BaseVolume, PolyVox::BaseVolume)
