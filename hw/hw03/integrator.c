#include <stdio.h>
#include <string.h>
#include <zmq.h>

// Declaring functions
char *msg_recv(void *socket, int flag);
int msg_send(void *socket, char *msg);

int main()
{
    printf("Initializing server...\n");
    void *context = zmq_ctx_new();
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_bind(receiver, "tcp://*:5558");

    printf("Listening for messages\n");

    char *msg = msg_recv(receiver, 0);

    int n;
    sscanf(msg, "%d", &n);
    
    printf("===============\n");
    printf("Starting\n");
    printf("===============\n\n");

    printf("Calculating Sum of: %d!\n\n", n);

    unsigned long long result = 0;

    int i;
    for (i = 0; i < n; i++)
    {
        char *r = msg_recv(receiver, 0);

        printf("Received: %s\n", r);

        unsigned long long f;
        sscanf(r, "%llu", &f);

        result += f;       
    }

    printf("\nTotal : %llu\n", result);

    printf("\n===============\n");
    printf("Finished\n");
    printf("===============\n\n");

    zmq_close(receiver);
    zmq_ctx_destroy(context);

    return 0;
}

char *msg_recv(void *socket, int flag)
{
    char buffer[256];
    int size = zmq_recv(socket, buffer, 255, flag);
    if (size == -1)
        return NULL;
    buffer[size] = '\0';
    return strdup(buffer);
}

int msg_send(void *socket, char *msg)
{
    return zmq_send(socket, msg, strlen(msg), 0);
}