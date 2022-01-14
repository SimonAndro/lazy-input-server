
windres -i resource.rc -o lib/resource.o

:: rm -fr bin
:: mkdir bin
rm -fr build
mkdir build
cd build

cmake -G "Unix Makefiles" "../"

make 

cd ..