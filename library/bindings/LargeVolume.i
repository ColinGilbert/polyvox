%module LargeVolume
%{
#include "Material.h"
#include "Density.h"
#include "LargeVolume.h"
%}

%include "Material.h"
%include "Density.h"
%include "LargeVolume.h"

VOLUMETYPES(LargeVolume, PolyVox::LargeVolume)
