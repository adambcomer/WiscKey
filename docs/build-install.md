\page build-install Build & Install
\author Adam Comer <adambcomer@gmail.com>
\date May 15, 2020
\copyright Apache-2.0 License

## Dependencies

- [CMake](https://cmake.org/)

## Optional Dependencies

- [Doxygen](https://www.doxygen.nl/index.html)
- [GraphViz](http://www.graphviz.org/)

## Build

This project uses [CMake](https://cmake.org/) to generate the build files.

### Release

Build the static library in release mode.

```shell
cmake -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build --config Release
```

### Debug

Build the static library in debug mode.

```shell
cmake -B ./build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build --config Debug
```

## Test

Run the tests associated with this project.

Note: The tests can only be executed under a "Debug" build.

```shell
ctest --test-dir ./build
```

## Install

Install the static library and headers to your machine.

```shell
cmake --install ./build
```