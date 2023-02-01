<p align="center"><img src="https://docs.apigear.io/img/icon.png" alt="ApiGear Logo" width="100"/></p>

# ObjectLinkCPP 
> An object link protocol for C++ 14

This repository contains the implementation of the [ObjectLink core protocol](https://docs.apigear.io/docs/advanced/objectlink/intro) which can be used by C++ based templates.
General ApiGear documentation can be found at [docs.apigear.io](https://docs.apigear.io/)

Unit tests on Windows, MacOS, Linux: ![Unit test results](https://github.com/apigear-io/objectlink-core-cpp/actions/workflows/cmake-test.yml/badge.svg?branch=main)

## Compilation and distribution

### Use of STL types
:warning: In this library we use STL classes in public interfaces.

* **We generally assume that this library is built from source.**
* Otherwise, it has to be ensured that for each build configuration the compiler version etc. matches the one which originally built the library.