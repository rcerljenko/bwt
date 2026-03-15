# pbwt

## Portable Multithreaded Burrows-Wheeler Transform + Move To Front + Run Length Encoding Compressor/Decompressor

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

**Important note:** When building a shared library on Windows, without install part, shared lib (\*.dll) needs to be copied to the same dir as \*.exe file after build process.

List of possible options you can pass to the CMake generator command:

| Option | Default | Description |
| --- | --- | --- |
| `BWT_NO_MTF` | `OFF` | Pass `ON` to build without MTF pass |
| `BUILD_BWT_LIBRARY` | `ON` | Pass `OFF` to build a main program only (without library part) |
| `BUILD_SHARED_LIBS` | `ON` | Pass `OFF` to build a static library instead of a shared library |

So, for example, if you want to build without MTF pass you can simply pass that flag to the CMake generator command:

```sh
cmake -B build -DBWT_NO_MTF=ON
```

### Usage

```sh
pbwt [OPTIONS] [<input_file>]
```

For detailed help run `pbwt -h`.
