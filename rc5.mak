rc5:rc5.o
	g++ -o rc5 rc5.o
rc5.o:rc5.cpp
	g++ -c rc5.cpp
clean:
	/bin/rm -f rc5 rc5.o
