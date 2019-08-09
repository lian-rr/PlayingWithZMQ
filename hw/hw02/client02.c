#include <stdio.h>
#include <string.h>
#include <zmq.h>
#include <hashTable.h>

// Defining functions
char *custom_recv_msg(void *socket);
int send_msg(void *socket, char *msg);
void notify();
void get_numbers(struct table *t);
int mark_number(struct table *t, int key);
int get_random();

int main(int argc, char **argv)
{
    int result = game();
    if (result)
    {
        printf("Sending victory notification to the server\n");
        notify();

        printf("===============\n\n");
        printf("You are the winner...\n");
        printf("===============\n\n");
    }
    else
    {
        printf("===============\n\n");
        printf("You lost. Another player won before you...\n");
        printf("===============\n\n");
    }

    return 0;
}

int game()
{
    int victorius = 1;
    int remaining = 25;
    struct table *t = makeTable(remaining);
    printf("Preparing numbers...\n");
    get_numbers(t);
    printf("Numbers ready...\n");
    printf("=======================\n\n");
    printTable(t);
    printf("=======================\n\n");

    //  Prepare our context and publisher
    void *context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_connect(subscriber, "tcp://localhost:5556");

    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, NULL, 0);

    while (remaining)
    {
        char *string = custom_recv_msg(subscriber);

        int command;
        int number;
        sscanf(string, "%d %d", &command, &number);

        //if someone won
        if (!command)
        {
            victorius = 0;
            break;
        }

        printf("Number received: %d. Remaining: %d\n", number, remaining);
        int marked = mark_number(t, number);

        if (marked)
        {
            remaining--;
        }
    }
    if (victorius)
        printTable(t);

    zmq_close(subscriber);
    zmq_ctx_destroy(context);

    return victorius;
}

void notify()
{
    void *context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://localhost:5555");

    int result = zmq_send(requester, NULL, 0, 0);
    if (result != -1)
    {
        zmq_recv(requester, NULL, 0, 0);
    }

    zmq_close(requester);
    zmq_ctx_destroy(context);
}

void get_numbers(struct table *t)
{
    int i = 0;
    while (i < 25)
    {
        int rnd_val = get_random();
        if (lookup(t, rnd_val) == -1)
        {
            insert(t, rnd_val, 0);
            i++;
        }
    }
}

int mark_number(struct table *t, int key)
{
    int val = lookup(t, key);
    if (val == 1 || val == -1)
        return 0;
    insert(t, key, 1);
    return 1;
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