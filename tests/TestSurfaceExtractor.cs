using System.Diagnostics;

public class test
{
	public static void Main()
	{
		Region r = new Region(new Vector3Dint32_t(0,0,0), new Vector3Dint32_t(31,31,31));
		SimpleVolumeuint8 vol = new SimpleVolumeuint8(r);
		//Set one single voxel to have a reasonably high density
		vol.setVoxelAt(new Vector3Dint32_t(5, 5, 5), 200);
		SurfaceMeshPositionMaterialNormal mesh = new SurfaceMeshPositionMaterialNormal();
		MarchingCubesSurfaceExtractorSimpleVolumeuint8 extractor = new MarchingCubesSurfaceExtractorSimpleVolumeuint8(vol, r, mesh);
		extractor.execute();
		
		Debug.Assert(mesh.getNoOfVertices() == 6);
		
		Vector3Dint32_t v1 = new Vector3Dint32_t(1,2,3);
		Vector3Dint32_t v2 = new Vector3Dint32_t(6,8,12);
		Vector3Dint32_t v3 = v1 + v2;
		
		Vector3Dint32_t v11 = new Vector3Dint32_t(1,2,3);
		
		Debug.Assert(v3.getX() == 7);
		Debug.Assert((v3*5).getX() == 35);
		Debug.Assert(v1.Equals(v11));
		Debug.Assert(v1 != v11);
		Debug.Assert(!v1.Equals(v2));
		Debug.Assert(!v1.Equals(null));

		System.Console.WriteLine("Success");
	}
}
