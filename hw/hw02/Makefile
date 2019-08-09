all: hw01 hw02

hw01:
	cc server01.c -o server01 -Llib -lzmq
	cc client01.c -o client01 -Llib -lzmq -Iinclude

hw02:
	cc server02.c -o server02 -Llib -lzmq -pthread	
	cc client02.c -o client02 -Llib -lzmq -Iinclude