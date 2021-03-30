ef2:cpp.o rc.o
	g++ cpp.o rc.o -mwindows -static -O3 -DNDEBUG -o ef2
cpp.o:ef2.cpp
	g++ -c ef2.cpp -fexec-charset=GBK -finput-charset=UTF-8 -o cpp.o
rc.o:resource.rc
	windres resource.rc rc.o

.PHONY : clean
clean:
	-rm ef2.exe cpp.o rc.o