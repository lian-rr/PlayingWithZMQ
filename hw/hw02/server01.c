#include <stdio.h>
#include <string.h>
#include <zmq.h>

// Defining functions
char *custom_recv_msg(void *socket);
int send_msg(void *socket, char *msg);
int get_random();

int main(int argc, char **argv)
{
    //  Prepare our context and publisher
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind(publisher, "tcp://*:5556");
    // rc = zmq_bind(publisher, "ipc://bingo.ipc");

    while (1)
    {
        int rand_value = get_random();
        char rand_string[10];
        sprintf(rand_string, "%d", rand_value);
        printf("Sending number: %s\n", rand_string);
        int size = send_msg(publisher, rand_string);
        if(size == -1){
            printf("Error sending msg\n");
        }
        sleep(1);
    }

    zmq_close(publisher);
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
    return zmq_send(socket, msg, strlen(msg), 0);
}

int get_random()
{
    srand(time(NULL));
    return rand() % 100;
}