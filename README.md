# pbwt

## Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor

[![CMake CI](https://github.com/rcerljenko/bwt/actions/workflows/cmake.yml/badge.svg)](https://github.com/rcerljenko/bwt/actions/workflows/cmake.yml)

### Build & Install

After git clone or downloaded and extracted zip file (assuming [CMake](https://cmake.org/download/) is installed):

```sh
cd src
cmake -B build
cmake --build build
sudo cmake --install build --strip
```

Install part is optional, but if it's done, build dir can be deleted.

When using multi-config generators such as "Xcode" or "Visual Studio \*" you need to provide an extra config argument to the build and install commands:

```sh
cmake --build build --config Release
sudo cmake --install build --strip --config Release
```

By default, CMake will build a shared library. To build a static library you can pass a custom flag to the generator command:

```sh
cmake -B build -DBUILD_SHARED_LIBS=OFF
```

**Important note:** When building a shared library on Windows, without install part, shared lib (\*.dll) needs to be copied to the same dir as \*.exe file after build process.

### Usage

```sh
pbwt [OPTIONS] [<input_file>]
```

For detailed help run `pbwt -h`.
