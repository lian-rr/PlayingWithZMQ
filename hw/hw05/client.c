#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
#include <assert.h>

char *msg_recv(void *socket, int flag);
int msg_send(void *socket, char *msg);
int get_random();

int main()
{

  void *context = zmq_ctx_new();

  //  Socket to talk to server
  void *requester = zmq_socket(context, ZMQ_REQ);
  int rc = zmq_connect(requester, "tcp://localhost:5559");
  assert(rc == 0);

  int i;
  for (i = 0; i < 10; i++)
  {
    int rand_value = get_random();
    char rand_string[10];
    sprintf(rand_string, "%d", rand_value);

    printf("Number generated: %s\n", rand_string);

    msg_send(requester, rand_string);
    zmq_send(requester, NULL, 0, 0);
    char *string = msg_recv(requester, 0);
    printf("Factorial of %d is: %s\n", rand_value, string);
    free(string);
  }
  zmq_close(requester);
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

int get_random()
{
  srand(time(NULL));
  return rand() % 20;
}