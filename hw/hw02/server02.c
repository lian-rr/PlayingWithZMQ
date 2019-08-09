#include <stdio.h>
#include <string.h>
#include <zmq.h>
#include <pthread.h>

// Defining functions
void control();
void *serve_game();
int send_msg(void *socket, char *msg);
int get_random();

//global variable
int run = 1;
pthread_mutex_t lock;

int main(int argc, char **argv)
{
    pthread_t thread_id;

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("Error initializing mutex\n");
        return 1;
    }

    if (pthread_create(&thread_id, NULL, serve_game, NULL) != 0)
    {
        printf("Error serving the game\n.");
        return 1;
    }

    control();
    pthread_join(thread_id, NULL);
    printf("===============\n\n");
    printf("Game finnished. Closing program\n");
    printf("===============\n\n");
    return 0;
}

void control()
{
    void *context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://*:5555");

    while (run)
    {
        zmq_recv(responder, NULL, 0, 0);

        //set lock
        pthread_mutex_lock(&lock);
        //change run
        run = 0;

        //remove lock
        pthread_mutex_unlock(&lock);

        zmq_send(responder, NULL, 0, 0);
    }
    //closing socket
    zmq_close(responder);
    zmq_ctx_destroy(context);
}

void *serve_game()
{
    printf("===============\n\n");
    printf("Serving Game\n");
    printf("===============\n\n");

    //  Prepare our context and publisher
    void *context = zmq_ctx_new();
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind(publisher, "tcp://*:5556");
    // rc = zmq_bind(publisher, "ipc://bingo.ipc");

    while (run)
    {
        int rand_value = get_random();
        char rand_string[10];
        sprintf(rand_string, "%d %d", 1, rand_value);
        printf("Sending number: %s\n", rand_string);
        int size = send_msg(publisher, rand_string);
        if (size == -1)
        {
            printf("Error sending msg\n");
        }
        sleep(1);
    }

    char msg[10];
    sprintf(msg, "%d %d", 0, 0);
    printf("Sending End Command: %s\n", msg);
    send_msg(publisher, msg);

    zmq_close(publisher);
    zmq_ctx_destroy(context);
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