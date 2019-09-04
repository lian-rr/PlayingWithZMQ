#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <assert.h>

char *msg_recv(void *socket, int flag);
int msg_send(void *socket, char *msg);
int msg_sendmore(void *socket, char *msg);
void free_data(void *data, void *hint);

int main()
{
    printf("\n======================\n");
    printf("Initializing server...");
    printf("\n======================\n");

    void *context = zmq_ctx_new();
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    void *backend = zmq_socket(context, ZMQ_DEALER);

    int fr = zmq_bind(frontend, "tcp://*:5559");
    assert(fr == 0);

    int br = zmq_bind(backend, "tcp://*:5560");
    assert(br == 0);

    //  Initialize poll set
    zmq_pollitem_t items[] = {
        {frontend, 0, ZMQ_POLLIN, 0},
        {backend, 0, ZMQ_POLLIN, 0}};

    printf("\n====================\n");
    printf("    Server ready");
    printf("\n====================\n\n");

    //Cache
    unsigned long long cache[20] = {0};

    int n;

    while (1)
    {
        int more;
        zmq_msg_t message;

        int pr = zmq_poll(items, 2, -1);
        assert(pr >= 0);
        if (items[0].revents & ZMQ_POLLIN)
        {
            while (1)
            {

                //Get message
                zmq_msg_init(&message);

                zmq_msg_recv(&message, frontend, 0);

                size_t more_size = sizeof(more);

                zmq_getsockopt(frontend, ZMQ_RCVMORE, &more, &more_size);

                sscanf(zmq_msg_data(&message), "%d", &n);

                if (n < 19)
                {
                    printf("Number received %d\n", n);

                    if (cache[n] == 0)
                    {
                        printf("Not in cache\n");
                        zmq_msg_send(&message, backend, more ? ZMQ_SNDMORE : 0);
                    }
                    else
                    {
                        char *buffer = malloc(255 * sizeof(char));
                        sprintf(buffer, "%llu", cache[n]);

                        zmq_msg_t msg;
                        int rc = zmq_msg_init_data(&msg, buffer, strlen(buffer), free_data, NULL);
                        assert(rc == 0);
                        zmq_msg_send(&msg, frontend, more ? ZMQ_SNDMORE : 0);

                        zmq_msg_close(&msg);
                    }
                }
                zmq_msg_close(&message);

                if (!more)
                    break; // Last message part
            }
        }
        if (items[1].revents & ZMQ_POLLIN)
        {
            while (1)
            {
                zmq_msg_init(&message);
                zmq_msg_recv(&message, backend, 0);

                size_t more_size = sizeof(more);

                zmq_getsockopt(backend, ZMQ_RCVMORE, &more, &more_size);

                if (strlen(zmq_msg_data(&message)) > 0)
                    printf("Response: %s\n", (char *)zmq_msg_data(&message));

                unsigned long long f;
                sscanf(zmq_msg_data(&message), "%llu", &f);

                cache[n] = f;

                zmq_msg_send(&message, frontend, more ? ZMQ_SNDMORE : 0);
                zmq_msg_close(&message);
                if (!more)
                    break; // Last message part
            }
        }
    }

    zmq_close(frontend);
    zmq_close(backend);
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

int msg_sendmore(void *socket, char *msg)
{
    return zmq_send(socket, msg, strlen(msg), ZMQ_SNDMORE);
}

void free_data(void *data, void *hint)
{
    free(data);
}