#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>

// Function declarations
char *custom_recv_msg(void *socket);

int main(int argc, char **argv)
{
    printf("Client Initialized\n");
    // Initializing socket
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");
    
    int result = zmq_send(requester, NULL, 0, 0);
    if (result == -1)
    {
        printf("Error sending the message.\n");
        return 1;
    }
    char *reply = custom_recv_msg(requester);
    printf("Server date: %s\n", reply);
    free(reply);

    zmq_close(requester);
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
