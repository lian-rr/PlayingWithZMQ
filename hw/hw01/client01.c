#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>

// Function declarations
char *custom_recv_msg(void *socket);
int send_msg(void *socket, char *msg);

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("No arguments. Closing program.\n");
        return 1;
    }

    printf("Client Initialized\n");
    printf("------------------\n\n");
    // Initializing socket
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int i = 1;
    while (i < argc)
    {
        char *msg = argv[i];
        printf("Sending: %s\n", msg);
        int result = send_msg(requester, msg);
        if (result == -1)
        {
            printf("Error sending the message.\n");
            return 1;
        }
        char *reply = custom_recv_msg(requester);
        printf("Received: %s\n", reply);
        printf("------------------\n");
        free(reply);        
        i++;
    }

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

int send_msg(void *socket, char *msg)
{
    int size = zmq_send(socket, msg, strlen(msg), 0);
    return size;
}