
@REM windres -i server-ui/src/resource.rc -o lib/resource.o

@REM rm -fr build
@REM mkdir build
cd build

cmake -G "Unix Makefiles" "../"

make 

cd ..