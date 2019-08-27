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
int read_file(char *buffer, char *file_name);
void send_content(void *socket, char *content, size_t chunk_size);

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
            //return with status 404
            msg_send(responder, "404 0");
            continue;
        }

        printf("File size: %zu\n", sb.st_size);

        char *content = malloc(sb.st_size);
        int r = read_file(content, full_path);
        if (r != 0)
        {
            printf("Error reading file in: %s", full_path);
            //return with status 500
            msg_send(responder, "500 0");
            continue;
        }

        //return with 200
        sprintf(length, "200 %zu", strlen(content));

        msg_sendmore(responder, length);

        // printf("Content:\n%s\n\n", content);

        send_content(responder, content, 255);

        //return file
        msg_send(responder, NULL);
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
    return zmq_send(socket, msg, strlen(msg), 0);
}

int msg_sendmore(void *socket, char *msg)
{
    return zmq_send(socket, msg, strlen(msg), ZMQ_SNDMORE);
}

int read_file(char *buffer, char *file_name)
{
    char ch;
    FILE *fp;
    char bf[1];

    fp = fopen(file_name, "r");
    if (fp == NULL)
        return -1;
    while ((ch = fgetc(fp)) != EOF)
    {
        sprintf(bf, "%c", ch);
        strcat(buffer, bf);
    }
    fclose(fp);
    return 0;
}

void send_content(void *socket, char *content, size_t chunk_size)
{
    char *buffer = malloc(chunk_size + 1);

    printf("%s\n\n", content + 3060);

    if (buffer)
    {
        int pos = 0;
        while (pos < strlen(content) -1)
        {
            size_t len = pos + chunk_size > strlen(content) ? (strlen(content) - pos - 1) : chunk_size;
            memcpy(buffer, content + pos, len);
            buffer[chunk_size] = '\0';

            printf("\nPos: %d. Len: %zu\n", pos, len);
            printf("%s", buffer);
            
            //send content
            msg_sendmore(socket, buffer);

            pos += len;
        }
        free(buffer);
    }
}