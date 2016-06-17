#pbwt
Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor

Instructions:

After git clone or downloaded and extracted zip file (assuming cmake is installed):

On Linux:

Shared lib:

cd bwt/libbwt
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig

Main program:

cd bwt
mkdir build
cd build
cmake ..
make
sudo make install

Install parts are optional, but if they're done, build dirs can be deleted.


On Windows:

It's easier to use CMake gui for Windows to pick the right generator.
If the generator is "MinGW Makefiles" then all the steps above are valid except install and ldconfig parts.
If the genereator is "Visual Stuio*" then the Soulution file (*.sln) needs to be builded with it.
Important note: On Windows, shared lib (*.dll) needs to be copied to the same dir as *.exe file after build process.