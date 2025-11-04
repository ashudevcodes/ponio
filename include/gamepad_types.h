#ifndef GAMEPAD_TYPES_H
#define GAMEPAD_TYPES_H

#include <netinet/in.h>
#include <pthread.h>

#define PORT 8888
#define MAX_CLIENTS 4
#define BUFFER_SIZE 1024

struct libevdev_uinput;

typedef struct
{
  int socket;
  char ip[INET_ADDRSTRLEN];
  int port;
  int active;
  char last_command[128];
  int commands_received;
} Client;

typedef struct
{
  int server_socket;
  Client clients[MAX_CLIENTS];
  int client_count;
  int total_commands;
  int server_running;
  struct libevdev_uinput *uinput_dev;
  pthread_mutex_t lock;
} ServerState;

extern ServerState server_state;

#endif
