%module SurfaceMesh
%{
#include "Region.h"
#include "VertexTypes.h"
#include "SurfaceMesh.h"
%}

%include "Region.h"
%include "VertexTypes.h"
%include "SurfaceMesh.h"

%template(SurfaceMeshPositionMaterial) PolyVox::SurfaceMesh<PolyVox::PositionMaterial>;
%template(SurfaceMeshPositionMaterialNormal) PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>;