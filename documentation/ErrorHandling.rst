**************
Error Handling
**************
PolyVox includes a number of error handling features designed to help you identify and/or recover from problematic scenarios. This document describes these features and how they affect you as a user of the library.

Logging
=======
PolyVox has a simple logging mechanism which allows it to write messages with an associated severity (from Debug up to Fatal). It is possible to redirect the output of these logging functions so you can integrate them with your applications logging framework or suppress them completely.

The following functions are called at various points in the PolyVox codebase:

.. code-block :: c++

 void logDebug  (const std::string& message);
 void logInfo   (const std::string& message);
 void logWarning(const std::string& message);
 void logError  (const std::string& message);
 void logFatal  (const std::string& message);

Fatal messages are only issued in non-recoverable scenarios when the application is about to crash, and may provide the last peice of information you have about what went wrong. Error messages are issued when something has happened which prevents sucessful completion of a task, for example if you provide invalid parameters to a function (Error messages are also issued whenever an exception is thrown). Warning messages mean the system was able to continue but the results may not be what you expected. Info and Debug messages are both used for general information about what PolyVox is doing. The differentiating factor is that Debug is used if the output is very frequent so that it can be easily suppressed.

PolyVox defines a LogHandler function pointer which looks like this:

.. code-block :: c++

 typedef void (*LogHandler)(const std::string& message, LogLevel logLevel);

There is a function called 'defaultLogHandler()' which matches this signature and writes the messages to cout/cerr (note that it suppresses Debug messages). To redirect log messages you can write your own fuction which matches this signature and the apply it with setLogHandler:

.. code-block :: c++

 setLogHandler(&myLogHandler);

Note that you can disable logging completely by passing a value of '0' to setLogHandler().

Asserts
=======

Exceptions
==========
Error handling in PolyVox is provided by using the C++ exception mechanism. Exceptions can be thrown for a variety of reasons and your code should be prepared to handle them to prevent your application from crashing.

Most functions in PolyVox will validate their input parameters and throw an exception if the provided values do not meet the function's requirements (which should be specified in the API documentation). However, in certain performance critical cases we choose not to spend time validating the parameters and an exception will not be thrown, though we do still use an assertion if these are enabled.

The most notable example of this is when accessing volume data through the get/setVoxel() functions, as these are designed to be very fast. Validating an input position would require multiple conditional operations which we chose to avoid. Therefore, **accessing a voxel outside of a volume will cause undefined behaviour.** When reading voxels it is safer to use the function getVoxelWithWrapping() as this lets you specify how out-of-bounds voxels should be handled.

In addition to the C++ exception handling mechanism, PolyVox also makes use of assertions to verify the internal state of the library at various points. If you hit an assert in PolyVox then there is a good chance it is a bug in the library, as user errors should have been prevented by throwing an exceptions.

.. sourcecode :: python
 
 def wiki_rocks(text):
    formatter = lambda t: "funky"+t
    return formatter(text)