#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <zmq.h>
#include <assert.h>
#include <sys/stat.h>

char *msg_recv(void *socket, int flag);
int msg_send(void *socket, char *msg);
int msg_sendmore(void *socket, char *msg);
int serve_content(void *socket, size_t buffer_size, char *file_name, size_t file_size);

int main(void)
{
    printf("\n======================\n");
    printf("Initializing server...");
    printf("\n======================\n");

    void *context = zmq_ctx_new();

    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://*:5555");
    assert(responder);

    printf("\n====================\n");
    printf("    Server ready");
    printf("\n====================\n\n");

    char length[20];

    while (1)
    {
        printf("\n===========================\n\n");
        char *request;
        request = msg_recv(responder, 0);
        if (request == NULL)
            continue;

        printf("Received: %s\n", request);

        char *resource = strtok(request, " ");
        resource = strtok(NULL, " ");

        printf("Resource: %s\n", resource);

        char full_path[255];
        sprintf(full_path, "./content%s", resource);

        free(request);

        struct stat sb;

        if (stat(full_path, &sb) == -1)
        {
            printf("File not found\n");
            msg_sendmore(responder, "0");
            //return with status 404
            msg_send(responder, "404");
            continue;
        }

        printf("File size: %zu\n", sb.st_size);

        size_t buffer_size = 255;

        int r = serve_content(responder, buffer_size, full_path, sb.st_size);
        if (r != 0)
        {
            printf("Error reading file in: %s", full_path);
            //return with status 500
            msg_send(responder, "500");
            continue;
        }
    }

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

int msg_sendmore(void *socket, char *msg)
{
    return zmq_send(socket, msg, strlen(msg), ZMQ_SNDMORE);
}

int serve_content(void *socket, size_t buffer_size, char *file_name, size_t file_size)
{
    FILE *fp;
    char *buffer = malloc(buffer_size * sizeof(char));
    char *len = malloc(sizeof(char) * sizeof(file_size));

    fp = fopen(file_name, "r");
    if (fp == NULL)
        return -1;

    sprintf(len, "%zu", file_size);

    //send content lenght
    msg_sendmore(socket, len);

    //send 200
    msg_sendmore(socket, "200");

    while (fgets(buffer, buffer_size, fp) != NULL)
    {
        msg_sendmore(socket, buffer);
    }
    fclose(fp);
    msg_send(socket, NULL);
    return 0;
}