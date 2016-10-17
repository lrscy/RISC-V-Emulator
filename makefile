emulator: main.o elf.o hardware.o instruction.o
	g++ -std=c++11 -O2 -o $@ $^

instruction.o: instruction.cpp
	g++ -std=c++11 -O2 -c $<

hardware.o: hardware.cpp
	g++ -std=c++11 -O2 -c $<

elf.o: elf.cpp
	g++ -std=c++11 -O2 -c $<

main.o: main.cpp
	g++ -std=c++11 -O2 -c $<

clean:
	del *.o
