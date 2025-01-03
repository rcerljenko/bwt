# pbwt

## Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor

### Instalation

After git clone or downloaded and extracted zip file (assuming cmake is installed):

#### On Linux or MacOS

```sh
cd bwt/src
cmake -B build
cmake --build build
sudo cmake --install build
```

Install part is optional, but if it's done, build dir can be deleted.

#### On Windows

It's easier to use [CMake GUI](https://cmake.org/download/) for Windows to pick the right generator.

If genereator is "Visual Studio \*" you need to provide an extra config argument to the build and install commands:

```sh
cmake --build build --config Release
cmake --install build --config Release
```

**Important note:** On Windows, shared lib (\*.dll) needs to be copied to the same dir as \*.exe file after build process.

### Usage

```sh
pbwt [<input_file>] [OPTIONS]
```

For detailed help run `pbwt -h`.
