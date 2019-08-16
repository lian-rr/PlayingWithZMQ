all: clean hw01

hw01:
	gcc generator.c -o generator -Llib -lzmq
	gcc worker.c -o worker -Llib -lzmq
	gcc integrator.c -o integrator -Llib -lzmq

clean:
	rm generator
	rm integrator
	rm worker