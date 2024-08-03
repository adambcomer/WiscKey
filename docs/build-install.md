\page build-install Build & Install
\author Adam Comer <adambcomer@gmail.com>
\date August 3, 2024
\copyright Apache-2.0 License

## Dependencies

- [Meson](https://mesonbuild.com/)

## Optional Dependencies

- [Doxygen](https://www.doxygen.nl/index.html)
- [GraphViz](http://www.graphviz.org/)

## Build

This project uses the [Meson](https://mesonbuild.com/) build system.

### Release

Build the static library in release mode.

```shell
make init.release
make build
```

### Debug

Build the static library in debug mode.

```shell
make init
make build
```

## Test

Run the tests associated with this project.

```shell
make test
```

