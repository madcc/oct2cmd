cflags=-g

tg:main.o circleBuf.o
	g++ $(cflags) -o tg main.o circleBuf.o
circleBuf.o: circleBuf.cpp circleBuf.h
	g++ $(cflags)  -c circleBuf.cpp 
main.o: main.cpp circleBuf.h
	g++ $(cflags) -c main.cpp  

clean:
	del *.o tg.exe

