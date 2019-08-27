#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <assert.h>
#include <errno.h>

// Declaring functions
char *msg_recv(void *socket, int flag);
int msg_send(void *socket, char *msg);
char *build_http_response_headers(int status_code, char *content_type, int lenght);
char *get_status_code(int status);
char *get_content_type(char *ext);
void close_connection(void *socket, uint8_t *id, size_t id_size);
char *get_file_extension(char *file);

int main(int argc, char **argv)
{
    printf("\n======================\n");
    printf("Initializing server...");
    printf("\n======================\n");

    void *ctx = zmq_ctx_new();

    /* Create ZMQ_STREAM socket */
    void *http_server = zmq_socket(ctx, ZMQ_STREAM);
    assert(http_server);

    int rc = zmq_bind(http_server, "tcp://*:8080");
    assert(rc == 0);

    void *tserver = zmq_socket(ctx, ZMQ_REQ);
    assert(tserver);
    int rt = zmq_connect(tserver, "tcp://localhost:5555");
    assert(rt == 0);

    printf("\n====================\n");
    printf("    Server ready");
    printf("\n====================\n\n");

    uint8_t id[256];
    size_t id_size = 256;
    char *line;
    int more;
    size_t more_size = sizeof(more);
    int length = 0;
    int status;

    while (1)
    {
        id_size = zmq_recv(http_server, id, 256, 0);
        printf("Request Received:\nId: %d\n", *id);

        char *request = msg_recv(http_server, 0);

        printf("%s\n\n", request);

        msg_send(tserver, request);

        line = msg_recv(tserver, 0);
        sscanf(line, "%d %d", &status, &length);

        char *resource = strtok(request, " ");
        resource = strtok(NULL, " ");

        char *ext = get_file_extension(resource);

        char *http_response = build_http_response_headers(status, get_content_type(ext), length);

        printf("Response Headers:\n%s", http_response);

        zmq_send(http_server, id, id_size, ZMQ_SNDMORE);
        zmq_send(http_server, http_response, strlen(http_response), ZMQ_SNDMORE);

        if (status != 200)
        {
            close_connection(http_server, id, id_size);
            free(request);
            free(line);
            continue;
        }

        free(line);

        while (1)
        {
            line = msg_recv(tserver, 0);
            zmq_send(http_server, id, id_size, ZMQ_SNDMORE);
            zmq_send(http_server, line, strlen(line), ZMQ_SNDMORE);
            free(line);

            zmq_getsockopt(tserver, ZMQ_RCVMORE, &more, &more_size);

            if (!more)
            {
                // Send a zero to close connection to client
                close_connection(http_server, id, id_size);
                break;
            }
        }
        free(request);
    }

    zmq_close(http_server);
    zmq_close(tserver);
    zmq_ctx_destroy(ctx);

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

void close_connection(void *socket, uint8_t *id, size_t id_size)
{
    zmq_send(socket, id, id_size, ZMQ_SNDMORE);
    zmq_send(socket, NULL, 0, ZMQ_SNDMORE);
}

char *get_file_extension(char *file)
{
    char *ext = strrchr(file, '.');
    return ext != NULL ? strdup(ext) : strdup("");
}

char *build_http_response_headers(int status_code, char *content_type, int lenght)
{
    char string[255] = "";
    char buffer[50];
    sprintf(buffer, "HTTP/1.1 %s\n", get_status_code(status_code));
    strcat(string, buffer);
    sprintf(buffer, "Content-Type: %s\n", content_type);
    strcat(string, buffer);
    sprintf(buffer, "Connection: close\n");
    strcat(string, buffer);
    sprintf(buffer, "Content-Length: %d\n", lenght);
    strcat(string, buffer);
    sprintf(buffer, "\n");
    strcat(string, buffer);
    return strdup(string);
}

char *get_content_type(char *ext)
{
    printf("Extension = %s\n", ext);
    if (strcmp(ext, ".php") == 0 || strcmp(ext, ".html") == 0)
        return "text/html";
    else if (strcmp(ext, ".json") == 0 || strcmp(ext, ".xml") == 0 || strcmp(ext, ".pdf") == 0)
    {
        char buf[15];
        sprintf(buf, "application/%s", ext + 1);
        return strdup(buf);
    }
    else if (strcmp(ext, ".png") == 0 || strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".gif") == 0)
    {
        char buf[15];
        sprintf(buf, "image/%s", ext + 1);
        return strdup(buf);
    }
    else
        return "text/plain";
}

char *get_status_code(int status)
{
    switch (status)
    {
    case 200:
        return "200 OK";
    case 400:
        return "400 Bad Request";
    case 404:
        return "404 Not Found";
    default:
        return "500 Internal Server Error";
    }
}