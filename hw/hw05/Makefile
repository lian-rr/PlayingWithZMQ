all: hw

hw:
	gcc client.c -o client -Llib -lzmq
	gcc worker.c -o worker -Llib -lzmq
	gcc broker.c -o broker -Llib -lzmq

clean:
	rm client
	rm worker
	rm broker