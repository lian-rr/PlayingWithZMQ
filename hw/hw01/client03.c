#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>

// Function declarations
char *custom_recv_msg(void *socket);
int send_msg(void *socket, char *msg);
char *parseToString(int num);
int getRandom();

int main(int argc, char **argv)
{
    printf("Client Initialized\n");
    printf("------------------\n\n");
    // Initializing socket
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int val = getRandom();
    printf("Random generated with value: %d\n", val);
    char *msg = parseToString(val);
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

char *parseToString(int num)
{
    char buffer[255];
    sprintf(buffer, "%d", num);
    return strdup(buffer);
}

int getRandom()
{
    srand(time(NULL));
    return rand() % 100;
}