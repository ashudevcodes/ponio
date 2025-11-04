#include <arpa/inet.h>
#include <fcntl.h>
#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/gamepad_core.h"
#include "../include/gamepad_types.h"

// Global server state
ServerState server_state = { 0 };

int
init_virtual_gamepad (void)
{
  struct libevdev *dev;
  int err;

  dev = libevdev_new ();
  libevdev_set_name (dev, "Ponio");
  libevdev_set_id_bustype (dev, BUS_USB);
  libevdev_set_id_vendor (dev, 0x1234);
  libevdev_set_id_product (dev, 0x5678);
  libevdev_set_id_version (dev, 1);

  // button events
  libevdev_enable_event_type (dev, EV_KEY);
  libevdev_enable_event_code (dev, EV_KEY, BTN_A, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_B, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_X, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_Y, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_TL, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_TR, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_TL2, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_TR2, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_START, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_SELECT, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_DPAD_UP, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_DPAD_DOWN, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_DPAD_LEFT, NULL);
  libevdev_enable_event_code (dev, EV_KEY, BTN_DPAD_RIGHT, NULL);

  // Joystick axis
  libevdev_enable_event_type (dev, EV_ABS);

  struct input_absinfo absinfo = { .value = 0,
                                   .minimum = -32768,
                                   .maximum = 32767,
                                   .fuzz = 0,
                                   .flat = 0,
                                   .resolution = 0 };

  libevdev_enable_event_code (dev, EV_ABS, ABS_X, &absinfo);
  libevdev_enable_event_code (dev, EV_ABS, ABS_Y, &absinfo);
  libevdev_enable_event_code (dev, EV_ABS, ABS_RX, &absinfo);
  libevdev_enable_event_code (dev, EV_ABS, ABS_RY, &absinfo);

  // Create uinput device
  err = libevdev_uinput_create_from_device (dev, LIBEVDEV_UINPUT_OPEN_MANAGED,
                                            &server_state.uinput_dev);

  if (err != 0)
    {
      fprintf (stderr, "Error creating uinput device: %s\n", strerror (-err));
      libevdev_free (dev);
      return -1;
    }

  printf ("Ponio gamepad created: %s\n",
          libevdev_uinput_get_devnode (server_state.uinput_dev));

  return 0;
}

// Process gamepad command
void
process_command (const char *command)
{
  char cmd[64], value_str[64];
  int value;
  float x, y;

  if (sscanf (command, "%[^:]:%s", cmd, value_str) < 2)
    {
      return;
    }

  pthread_mutex_lock (&server_state.lock);
  server_state.total_commands++;

  // Button commands
  if (strncmp (cmd, "BTN_", 4) == 0)
    {
      value = atoi (value_str);

      if (strcmp (cmd, "BTN_A") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY, BTN_A,
                                       value);
        }
      else if (strcmp (cmd, "BTN_B") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY, BTN_B,
                                       value);
        }
      else if (strcmp (cmd, "BTN_X") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY, BTN_X,
                                       value);
        }
      else if (strcmp (cmd, "BTN_Y") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY, BTN_Y,
                                       value);
        }
      else if (strcmp (cmd, "BTN_L1") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY, BTN_TL,
                                       value);
        }
      else if (strcmp (cmd, "BTN_R1") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY, BTN_TR,
                                       value);
        }
      else if (strcmp (cmd, "BTN_L2") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY,
                                       BTN_TL2, value);
        }
      else if (strcmp (cmd, "BTN_R2") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY,
                                       BTN_TR2, value);
        }
      else if (strcmp (cmd, "BTN_START") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY,
                                       BTN_START, value);
        }
      else if (strcmp (cmd, "BTN_SELECT") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY,
                                       BTN_SELECT, value);
        }

      libevdev_uinput_write_event (server_state.uinput_dev, EV_SYN, SYN_REPORT,
                                   0);
    }
  // D-Pad commands
  else if (strncmp (cmd, "DPAD_", 5) == 0)
    {
      value = atoi (value_str);

      if (strcmp (cmd, "DPAD_UP") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY,
                                       BTN_DPAD_UP, value);
        }
      else if (strcmp (cmd, "DPAD_DOWN") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY,
                                       BTN_DPAD_DOWN, value);
        }
      else if (strcmp (cmd, "DPAD_LEFT") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY,
                                       BTN_DPAD_LEFT, value);
        }
      else if (strcmp (cmd, "DPAD_RIGHT") == 0)
        {
          libevdev_uinput_write_event (server_state.uinput_dev, EV_KEY,
                                       BTN_DPAD_RIGHT, value);
        }

      libevdev_uinput_write_event (server_state.uinput_dev, EV_SYN, SYN_REPORT,
                                   0);
    }
  // Joystick commands
  else if (strcmp (cmd, "LJOY") == 0 || strcmp (cmd, "RJOY") == 0)
    {
      if (sscanf (value_str, "%f,%f", &x, &y) == 2)
        {
          int axis_x = (int)(x * 32767);
          int axis_y = (int)(y * 32767);

          if (strcmp (cmd, "LJOY") == 0)
            {
              libevdev_uinput_write_event (server_state.uinput_dev, EV_ABS,
                                           ABS_X, axis_x);
              libevdev_uinput_write_event (server_state.uinput_dev, EV_ABS,
                                           ABS_Y, axis_y);
            }
          else
            {
              libevdev_uinput_write_event (server_state.uinput_dev, EV_ABS,
                                           ABS_RX, axis_x);
              libevdev_uinput_write_event (server_state.uinput_dev, EV_ABS,
                                           ABS_RY, axis_y);
            }

          libevdev_uinput_write_event (server_state.uinput_dev, EV_SYN,
                                       SYN_REPORT, 0);
        }
    }

  pthread_mutex_unlock (&server_state.lock);
}

// Handle client connection
void *
handle_client (void *arg)
{
  int client_index = *(int *)arg;
  free (arg);

  Client *client = &server_state.clients[client_index];
  char buffer[BUFFER_SIZE];

  printf ("Client connected from %s:%d\n", client->ip, client->port);

  while (server_state.server_running && client->active)
    {
      memset (buffer, 0, BUFFER_SIZE);
      int bytes_read = recv (client->socket, buffer, BUFFER_SIZE - 1, 0);

      if (bytes_read <= 0)
        {
          break;
        }

      buffer[bytes_read] = '\0';

      // Process each line in the buffer
      char *line = strtok (buffer, "\n");
      while (line != NULL)
        {
          if (strlen (line) > 0)
            {
              pthread_mutex_lock (&server_state.lock);
              strncpy (client->last_command, line,
                       sizeof (client->last_command) - 1);
              client->commands_received++;
              pthread_mutex_unlock (&server_state.lock);

              if (strncmp (line, "DISCONNECT", 10) != 0
                  && strncmp (line, "CONNECT", 7) != 0)
                {
                  process_command (line);
                }
            }
          line = strtok (NULL, "\n");
        }
    }

  pthread_mutex_lock (&server_state.lock);
  close (client->socket);
  client->active = 0;
  server_state.client_count--;
  printf ("Client disconnected: %s:%d\n", client->ip, client->port);
  pthread_mutex_unlock (&server_state.lock);

  return NULL;
}

// Server thread
void *
server_thread (void *arg)
{
  (void)arg;

  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof (client_addr);

  // Create socket
  server_state.server_socket = socket (AF_INET, SOCK_STREAM, 0);
  if (server_state.server_socket < 0)
    {
      perror ("Socket creation failed");
      return NULL;
    }

  // Set socket options
  int opt = 1;
  setsockopt (server_state.server_socket, SOL_SOCKET, SO_REUSEADDR, &opt,
              sizeof (opt));

  // Bind
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons (PORT);

  if (bind (server_state.server_socket, (struct sockaddr *)&server_addr,
            sizeof (server_addr))
      < 0)
    {
      perror ("Bind failed");
      close (server_state.server_socket);
      return NULL;
    }

  // Listen
  if (listen (server_state.server_socket, MAX_CLIENTS) < 0)
    {
      perror ("Listen failed");
      close (server_state.server_socket);
      return NULL;
    }

  printf ("Server listening on port %d\n", PORT);

  // Accept connections
  while (server_state.server_running)
    {
      int client_socket
          = accept (server_state.server_socket,
                    (struct sockaddr *)&client_addr, &client_len);

      if (client_socket < 0)
        {
          if (server_state.server_running)
            {
              perror ("Accept failed");
            }
          continue;
        }

      pthread_mutex_lock (&server_state.lock);

      // Find free slot
      int slot = -1;
      for (int i = 0; i < MAX_CLIENTS; i++)
        {
          if (!server_state.clients[i].active)
            {
              slot = i;
              break;
            }
        }

      if (slot >= 0)
        {
          server_state.clients[slot].socket = client_socket;
          server_state.clients[slot].active = 1;
          server_state.clients[slot].commands_received = 0;
          server_state.clients[slot].last_command[0] = '\0';
          server_state.clients[slot].port = ntohs (client_addr.sin_port);
          inet_ntop (AF_INET, &client_addr.sin_addr,
                     server_state.clients[slot].ip, INET_ADDRSTRLEN);
          server_state.client_count++;

          int *index = malloc (sizeof (int));
          *index = slot;
          printf ("%s", server_state.clients->last_command);

          pthread_t thread;
          pthread_create (&thread, NULL, handle_client, index);
          pthread_detach (thread);
        }
      else
        {
          printf ("Max clients reached, rejecting connection\n");
          close (client_socket);
        }

      pthread_mutex_unlock (&server_state.lock);
    }

  close (server_state.server_socket);
  return NULL;
}

// Cleanup virtual gamepad
void
cleanup_virtual_gamepad (void)
{
  if (server_state.uinput_dev)
    {
      libevdev_uinput_destroy (server_state.uinput_dev);
      server_state.uinput_dev = NULL;
    }
}
