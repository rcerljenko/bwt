# pbwt

## Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor

### Instructions

After git clone or downloaded and extracted zip file (assuming cmake is installed):

#### On Linux

**Shared lib:**

```sh
cd bwt/src/libbwt
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

**Main program:**

```sh
cd bwt/src
mkdir build
cd build
cmake ..
make
sudo make install
```

Install parts are optional, but if they're done, build dirs can be deleted.

#### On Windows

It's easier to use CMake GUI for Windows to pick the right generator.
If generator is "MinGW Makefiles" then all the steps above are valid except install and ldconfig parts.
If genereator is "Visual Studio \*" then the Solution file (\*.sln) needs to be built with it.
**Important note:** On Windows, shared lib (\*.dll) needs to be copied to the same dir as \*.exe file after build process.

#### Usage

```sh
pbwt [<input_file>] [OPTIONS]
```

For detailed help run ```pbwt -h```.
