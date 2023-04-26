tests are designed to find bottle necks of using the objectlink protocol handling infrastructure: client node, remote node and the registries
they are to be started under performance profiler (e.g. Visual Studio Performance Profiler)
tests are not using any test framework - to exclude 3rd party calls
tests use generated implementation of apigear/performance_interface.module for test sinks and source, no other apigear functionality is necessary as tests don't use network layer. The objects are using fake object name to allow testing many objects in registry with one generated api.
Load should be performed in the network layer implementation - so not for this repository.

To allow building test set the PERFORMANCE_TESTS option in TopLEvel CMakeLists to TRUE

In case the sinks and sources need to be regenerated please do following:
1. Make sure the API file is in shape you want `objectlink-core-cpp\performance\apigear\performance_interface.module.yaml`
2. Make sure the solution file `objectlink-core-cpp\performance\apigear\performance.solution.yaml` generates the olink and stubs feature and it uses template-cpp14 
3. Not all folders are necessary - the testApi/apigear/ should only contain the utilities - the network layer is not necessary, also the CMakeLists.txt and ApigearConfig.cmake.in were changed to remove those directories.
4. The Cmake for  oink feature needs to be changed `objectlink-core-cpp\performance\testApi\modules\api_module\api\generated\olink\CMakeLists.txt` it should now link to `olink-core` and `apigear-utilities`
