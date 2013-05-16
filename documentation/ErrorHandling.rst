**************
Error Handling
**************
PolyVox includes a number of error handling features designed to help you identify and/or recover from problematic scenarios. This document describes these features and how they affect you as a user of the library.

Logging
=======
PolyVox has a simple logging mechanism which allows it to write messages with an associated severity (from Debug up to Fatal). It is possible to redirect the output of these logging functions so you can integrate them with your applications logging framework or suppress them completely.

The following functions are called at various points in the PolyVox codebase:

.. sourcecode :: c++

 void logDebug  (const std::string& message);
 void logInfo   (const std::string& message);
 void logWarning(const std::string& message);
 void logError  (const std::string& message);
 void logFatal  (const std::string& message);

Fatal messages are only issued in non-recoverable scenarios when the application is about to crash, and may provide the last peice of information you have about what went wrong. Error messages are issued when something has happened which prevents sucessful completion of a task, for example if you provide invalid parameters to a function (Error messages are also issued whenever an exception is thrown). Warning messages mean the system was able to continue but the results may not be what you expected. Info and Debug messages are both used for general information about what PolyVox is doing. The differentiating factor is that Debug is used if the output is very frequent so that it can be easily suppressed.

PolyVox defines a LogHandler function pointer which looks like this:

.. sourcecode :: c++

 typedef void (*LogHandler)(const std::string& message, LogLevel logLevel);

There is a function called 'defaultLogHandler()' which matches this signature and writes the messages to cout/cerr (note that it suppresses Debug messages). To redirect log messages you can write your own fuction which matches this signature and the apply it with setLogHandler:

.. sourcecode :: c++

 setLogHandler(&myLogHandler);

Note that you can disable logging completely by passing a value of '0' to setLogHandler().

Exceptions
==========
Error handling in PolyVox is provided by using the C++ exception mechanism. Exceptions can be thrown for a variety of reasons and your code should be prepared to handle them to prevent your application from crashing. It is possible to disable the throwing of exeptions if they are not supported by your compiler.

Usage
-----
Most functions in PolyVox will validate their input parameters and throw an exception if the provided values do not meet the function's requirements (which should be specified in the API documentation). However, in certain performance critical cases we choose not to spend time validating the parameters and an exception will not be thrown, though we do still use an assertion if these are enabled.

The most notable example of this is when accessing volume data through the get/setVoxel() functions, as these are designed to be very fast. Validating an input position would require multiple conditional operations which we chose to avoid. Therefore, **accessing a voxel outside of a volume will cause undefined behaviour.** When reading voxels it is safer to use the function getVoxelWithWrapping() as this lets you specify how out-of-bounds voxels should be handled.

Disabling exceptions
--------------------
Some platforms may not support the use of C++ exceptions (older Android SDKs are the only place we have seen this) so you may need to disable them completely. This is highly undesirable but you may have no choice. To do this you should undefine 'POLYVOX_THROW_ENABLED' in Config.h (we may expose this through CMake in the future).

If exceptions are disabled then PolyVox will call a 'ThrowHandler' instead of throwing an exception. This is defined as follows:

.. sourcecode :: c++

 typedef void (*ThrowHandler)(std::exception& e, const char* file, int line);
 
The default throw handler will print an error and then terminate, but you can provide a custom throw handler matching the function definition above:

.. sourcecode :: c++

 setThrowHandler(&myThrowHandler);

Asserts
=======
In addition to the C++ exception handling mechanism, PolyVox also makes use of assertions to verify the internal state of the library at various points. This functionality is provided by out own POLYVOX_ASSERT() macro rather than the standard C++ assert() as this has a number of advantages described `here <http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/>`_.

Assertions inside PolyVox are enabled by defining 'POLYVOX_ASSERTS_ENABLED' in Config.h and again we may expose this through CMake. Note that the presence of assertions is independant of whether you are building a debug or release version of your application.

As a user you are not really expected to encounter an assertions inside PolyVox - they are mostly there for our purposes as developers of the library. So if you hit one in PolyVox then there is a good chance it is a bug in the library, as user errors should have been prevented by throwing an exceptions. Again, there are exceptions to this rule as some speed-critical functions (getVoxel(), etc) do not validate their parameters.
