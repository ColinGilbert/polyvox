**************
Error Handling
**************
Error handling in PolyVox is provided by using the C++ exception mechanism. Exceptions can be thrown for a variety of reasons and your code should be prepared to handle them to prevent your application from crashing.

Most functions in PolyVox will validate their input parameters and throw an exception if the provided values do not meet the function's requirements (which should be specified in the API documentation). However, in certain performance critical cases we choose not to spend time validating the parameters and an exception will not be thrown, though we do still use an assertion if these are enabled.

The most notable example of this is when accessing volume data through the get/setVoxel() functions, as these are designed to be very fast. Validating an input position would require multiple conditional operations which we chose to avoid. Therefore, **accessing a voxel outside of a volume will cause undefined behaviour.** When reading voxels it is safer to use the function getVoxelWithWrapping() as this lets you specify how out-of-bounds voxels should be handled.

In addition to the C++ exception handling mechanism, PolyVox also makes use of assertions to verify the internal state of the library at various points. If you hit an assert in PolyVox then there is a good chance it is a bug in the library, as user errors should have been prevented by throwing an exceptions.