#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <assert.h>

char *msg_recv(void *socket, int flag);
int msg_send(void *socket, char *msg);
unsigned long long factorial(int n);

int main()
{
    void *context = zmq_ctx_new();

    //  Socket to talk to clients
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_connect(responder, "tcp://localhost:5560");
    while (1)
    {
        // Wait for next request from client
        char *string = msg_recv(responder, 0);
        printf("Received request: [%s]\n", string);

        int n;
        sscanf(string, "%d", &n);
        free(string);

        unsigned long long f = factorial(n);

        printf("Factorial of %d is %llu\n", n, f);

        char buffer[255];
        sprintf(buffer, "%llu", f);

        //  Send reply back to client
        int s = msg_send(responder, buffer);
        printf("Size: %d\n", s);
        
    }
    //  We never get here, but clean up anyhow
    zmq_close(responder);
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
    if (msg == NULL)
        return zmq_send(socket, NULL, 0, 0);
    return zmq_send(socket, msg, strlen(msg), 0);
}

unsigned long long factorial(int n)
{
    return n > 1 ? n * factorial(n - 1) : 1;
}