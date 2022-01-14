windres -i resource.rc -o obj/resource.o

gcc -o "bin/prod/lazy input" main.c obj/resource.o -lgdi32 -mwindows
