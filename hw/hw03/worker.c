#include <stdio.h>
#include <string.h>
#include <zmq.h>

// Declaring functions
char *msg_recv(void *socket, int flag);
int msg_send(void *socket, char *msg);
unsigned long long factorial(int n);

int main()
{
    printf("Initializing server...\n");
    void *context = zmq_ctx_new();
    void *receiver = zmq_socket(context, ZMQ_PULL);
    zmq_bind(receiver, "tcp://localhost:5557");

    //  Socket to send start of batch message on
    void *sender = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sender, "tcp://localhost:5558");

    printf("===============\n");
    printf("Starting work\n");
    printf("===============\n\n");

    printf("Listening for messages\n\n");

    while (1)
    {
        char *msg = msg_recv(receiver, 0);

        int n;
        sscanf(msg, "%d", &n);

        unsigned long long f = factorial(n);

        printf("Factorial of %d = %llu\n", n, f);

        char bf[255];
        sprintf(bf, "%llu", f);
        msg_send(sender, bf);
    }

    zmq_close(receiver);
    zmq_close(sender);
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

unsigned long long factorial(int n)
{
    return n > 1 ? n * factorial(n - 1) : 1;
}