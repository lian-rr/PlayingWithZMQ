#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>

// Function declarations
char *custom_recv_msg(void *socket);
int resp_msg(void *socket, char *msg);

int main()
{
    // Initializing socket
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://*:5555");

    printf("Socket Initialized\n");
    printf("Listening\n");
    printf("============================\n\n");
    //work
    while (1)
    {
        char *received_msg = custom_recv_msg(responder);
        printf("Message received: %s\n", received_msg);
        sleep(1);
        resp_msg(responder, received_msg);
        free(received_msg);
    }

    //closing socket
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}

char *custom_recv_msg(void *socket)
{
    char buffer[256];
    int size = zmq_recv(socket, buffer, 255, 0);
    if (size == -1)
        return NULL;
    buffer[size] = '\0';
    return strdup(buffer);
}

int resp_msg(void *socket, char *msg)
{
    return zmq_send(socket, msg, strlen(msg), 0);
}