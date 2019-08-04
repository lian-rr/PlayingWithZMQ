#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>
#include <errno.h>
#include <limits.h>

// Function declarations
char *custom_recv_msg(void *socket);
int resp_msg(void *socket, char *msg);
int parseToLong(char *str);
char *parseToString(long num);

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

    long sum = 0;

    while (1)
    {
        char *received_msg = custom_recv_msg(responder);
        printf("Number received: %s\n", received_msg);

        sleep(1);

        long val = parseToLong(received_msg);

        if (val < 0 || val > 100)
        {
            printf("Invalid input received: %s. Sum value = %lu\n", received_msg, sum);
            resp_msg(responder, "Invalid input. Please enter a number between 0-100");
        }
        else
        {
            printf("Returning Sum: %lu + %lu\n", sum, val);
            sum += val;
            resp_msg(responder, parseToString(sum));
        }
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

int parseToLong(char *str)
{
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
    {
        return -1;
    }
    return val;
}

char *parseToString(long num)
{
    char buffer[255];
    sprintf(buffer, "%lu", num);
    return strdup(buffer);
}