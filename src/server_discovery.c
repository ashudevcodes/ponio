#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/server_discovery.h"

#define DISCOVERY_PORT 8889
#define BUFFER_SIZE 1024
#define SERVER_NAME "Ponio Server"

void *
discovery_thread (void *arg)
{
  (void)arg;

  int sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof (client_addr);
  char buffer[BUFFER_SIZE];

  // Create UDP socket
  sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    {
      perror ("Discovery socket creation failed");
      return NULL;
    }

  // Allow broadcast
  int broadcast = 1;
  if (setsockopt (sock, SOL_SOCKET, SO_BROADCAST, &broadcast,
                  sizeof (broadcast))
      < 0)
    {
      perror ("Failed to set broadcast option");
      close (sock);
      return NULL;
    }

  // Bind discovery port
  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons (DISCOVERY_PORT);

  if (bind (sock, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0)
    {
      perror ("Discovery bind failed");
      close (sock);
      return NULL;
    }

  printf ("Discovery service listening on UDP port %d\n", DISCOVERY_PORT);

  while (1)
    {
      memset (buffer, 0, BUFFER_SIZE);
      int recv_len = recvfrom (sock, buffer, BUFFER_SIZE - 1, 0,
                               (struct sockaddr *)&client_addr, &client_len);

      if (recv_len > 0)
        {
          buffer[recv_len] = '\0';

          if (strcmp (buffer, "PONIO_DISCOVER") == 0)
            {
              // Respond with server info
              // Format: "PONIO_SERVER:name:port:protocol"
              char response[256];
              snprintf (response, sizeof (response), "PONIO_SERVER:%s:%d:tcp",
                        SERVER_NAME, PORT);

              int sent = sendto (sock, response, strlen (response), 0,
                                 (struct sockaddr *)&client_addr, client_len);

              if (sent > 0)
                {
                  printf ("Discovery response sent to %s:%d\n",
                          inet_ntoa (client_addr.sin_addr),
                          ntohs (client_addr.sin_port));
                }
            }
        }
    }

  close (sock);
  return NULL;
}
