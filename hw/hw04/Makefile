all: clean hw

hw:
	gcc http_server.c -o http_server -Llib -lzmq
	gcc zmq_server.c -o zmq_server -Llib -lzmq

clean:
	rm http_server
	rm zmq_server