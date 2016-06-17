#pbwt
##Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor

###Instructions:

After git clone or downloaded and extracted zip file (assuming cmake is installed):<br />

###On Linux:

**Shared lib:**<br /><br />

cd bwt/libbwt<br />
mkdir build<br />
cd build<br />
cmake ..<br />
make<br />
sudo make install<br />
sudo ldconfig<br /><br />

**Main program:**<br /><br />

cd bwt<br />
mkdir build<br />
cd build<br />
cmake ..<br />
make<br />
sudo make install<br /><br />

Install parts are optional, but if they're done, build dirs can be deleted.<br /><br />


###On Windows:

It's easier to use CMake gui for Windows to pick the right generator.<br />
If the generator is "MinGW Makefiles" then all the steps above are valid except install and ldconfig parts.<br />
If the genereator is "Visual Stuio*" then the Soulution file (*.sln) needs to be builded with it.<br />
Important note: On Windows, shared lib (*.dll) needs to be copied to the same dir as *.exe file after build process.