::windres -i resource.rc -o obj/resource.o

gcc -o "bin/lazy input" main.c obj/resource.o -lgdi32 -mwindows
::gcc -o "bin/lazy input" main.c resource.o -lgdi32

"bin/lazy input.exe" 