%module SurfaceExtractor
%{
#include "SimpleVolume.h"
#include "SurfaceExtractor.h"
#include "PolyVoxCore/Material.h"

namespace PolyVox
{
	class SurfaceExtractorSimpleVolumeMaterial8 : public SurfaceExtractor<SimpleVolume, Material8>
	{
		public:
			SurfaceExtractorSimpleVolumeMaterial8(SimpleVolume<Material8>* volData, Region region, SurfaceMesh<PositionMaterialNormal>* result)
			: SurfaceExtractor<SimpleVolume, Material8>(volData, region, result) {}
			void execute() { SurfaceExtractor<SimpleVolume, Material8>::execute(); }
	};
};
%}

%include "SimpleVolume.h"
%include "SurfaceExtractor.h"

namespace PolyVox
{
	class SurfaceExtractorSimpleVolumeMaterial8 : public SurfaceExtractor<SimpleVolume, Material8>
	{
		public:
			SurfaceExtractorSimpleVolumeMaterial8(SimpleVolume<Material8>* volData, Region region, SurfaceMesh<PositionMaterialNormal>* result);
			void execute();
	};
};
