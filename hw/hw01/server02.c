#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>
#include <time.h>

// Function declarations
int resp_msg(void *socket, char *msg);
char *getDate();

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
        zmq_recv(responder, NULL, 0, 0);
        char *date = getDate();
        printf("Request received. Responding with: %s\n", date);
        sleep(1);
        resp_msg(responder, date);
    }

    //closing socket
    zmq_close(responder);
    zmq_ctx_destroy(context);
    return 0;
}

int resp_msg(void *socket, char *msg)
{
    int size = zmq_send(socket, msg, strlen(msg), 0);
    return size;
}

char *getDate()
{
    char buffer[100];
    time_t t;
    struct tm *timeptr;
    time(&t);
    timeptr = localtime(&t);
    strftime(buffer, sizeof(buffer), "%a %m/%d/%Y %r", timeptr);
    return strdup(buffer);
}
