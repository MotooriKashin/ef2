ef2:c.o rc.o
	g++ c.o rc.o -s -mwindows -fexec-charset=GBK -finput-charset=UTF-8 -static -O3 -DNDEBUG -o ef2
c.o:ef2.cpp
	g++ -c ef2.cpp -s -mwindows -fexec-charset=GBK -finput-charset=UTF-8 -static -O3 -DNDEBUG -o c.o
rc.o:resource.rc
	windres resource.rc rc.o

.PHONY : clean
clean:
	-rm ef2.exe c.o rc.o