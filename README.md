# pbwt

## Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor

### Build & Install

After git clone or downloaded and extracted zip file (assuming [CMake](https://cmake.org/download/) is installed):

```sh
cd bwt/src
cmake -B build
cmake --build build
sudo cmake --install build --strip
```

Install part is optional, but if it's done, build dir can be deleted.

When using multi-config genereators such as "Xcode" or "Visual Studio \*" you need to provide an extra config argument to the build and install commands:

```sh
cmake --build build --config Release
sudo cmake --install build --strip --config Release
```

**Important note:** On Windows, without install part, shared lib (\*.dll) needs to be copied to the same dir as \*.exe file after build process.

### Usage

```sh
pbwt [OPTIONS] [<input_file>]
```

For detailed help run `pbwt -h`.
