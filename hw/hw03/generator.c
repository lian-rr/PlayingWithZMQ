#include <stdio.h>
#include <string.h>
#include <zmq.h>

// Declaring functions
char *msg_recv(void *socket, int flag);
int msg_send(void *socket, char *msg);
void send_msg_to_integrator(void *socket, int n);
void send_msgs_to_workers(void *socket, int n);

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        printf("Enter a number to calculate the factorial!\n");
        return 1;
    }

    printf("Initializing server...\n");
    void *context = zmq_ctx_new();

    //  Socket to send messages on
    void *sender = zmq_socket(context, ZMQ_PUSH);
    zmq_bind(sender, "tcp://*:5557");

    // //  Socket to send start of batch message on
    void *sink = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(sink, "tcp://localhost:5558");

    printf("Press Enter to start ");
    getchar();

    int n;
    sscanf(argv[1], "%d", &n);

    printf("\n===============\n");
    printf("Synq with Integrator\n");
    printf("===============\n\n");

    send_msg_to_integrator(sink, n);

    printf("\n===============\n");
    printf("Starting work\n");
    printf("===============\n\n");

    send_msgs_to_workers(sender, n);

    printf("\n===============\n");
    printf("Finished work\n");
    printf("===============\n\n");

    zmq_close(sink);
    zmq_close(sender);
    zmq_ctx_destroy(context);

    printf("Closing app\n");
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

void send_msg_to_integrator(void *socket, int n)
{
    char msg[1];
    sprintf(msg, "%d", n);
    printf("Sending to Integrator the number: %d ------> ", n);
    int r = msg_send(socket, msg);
    printf("%s\n", r != -1 ? "Successful" : "Failed");
}

void send_msgs_to_workers(void *socket, int n)
{
    int i;
    for (i = 1; i <= n; i++)
    {
        char msg[2];
        sprintf(msg, "%d", i);
        printf("Sending to worker the number: %d ------> ", i);
        int r = msg_send(socket, msg);
        printf("%s\n", r != -1 ? "Successful" : "Failed");
    }
}