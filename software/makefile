all: captura.exe ataque.exe

captura.exe: captura.cpp rigol.h
	g++ captura.cpp -o captura.exe -O2 -Wall -lvisa32

ataque.exe: ataque.cpp
	g++ ataque.cpp -o ataque.exe -O2 -Wall -std=c++11

run:
	ataque.exe