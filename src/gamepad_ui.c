#include "raylib.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/gamepad_core.h"
#include "../include/gamepad_types.h"
#include "../include/server_discovery.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void
draw_ui (const char *greet_as_name[])
{
  BeginDrawing ();
  ClearBackground ((Color){ 20, 20, 30, 255 });

  DrawRectangleGradientH (0, 0, SCREEN_WIDTH, 60, (Color){ 30, 60, 90, 255 },
                          (Color){ 60, 30, 90, 255 });
  DrawText ("Ponio Gamepad Server", 20, 15, 32, WHITE);
  DrawText ("Press Q to exit", SCREEN_WIDTH - 180, 20, 16, LIGHTGRAY);

  DrawRectangle (20, 70, SCREEN_WIDTH - 40, 120, (Color){ 45, 45, 45, 255 });
  DrawRectangleLines (20, 70, SCREEN_WIDTH - 40, 120, GREEN);

  DrawText (TextFormat ("Server Status: RUNNING"), 40, 85, 20, GREEN);
  DrawText (
      TextFormat ("TCP Port: %d | UDP Port: %d | Discovery: 8889", PORT, PORT),
      40, 110, 16, SKYBLUE);
  DrawText (TextFormat ("Connected Clients: %d / %d",
                        server_state.client_count, MAX_CLIENTS),
            40, 135, 18, YELLOW);
  DrawText (TextFormat ("Total Commands: %d", server_state.total_commands), 40,
            160, 18, SKYBLUE);

  DrawText ("Connected Clients:", 20, 210, 22, WHITE);

  int y_offset = 240;
  pthread_mutex_lock (&server_state.lock);

  for (int i = 0; i < MAX_CLIENTS; i++)
  {
	if (server_state.clients[i].active)
	{
	  Color box_color = (Color){ 50, 50, 70, 255 };
	  DrawRectangle (20, y_offset, SCREEN_WIDTH - 40, 80, box_color);
	  DrawRectangleLines (20, y_offset, SCREEN_WIDTH - 40, 80, BLUE);

	  DrawText (TextFormat ("Client #%d - %s", i + 1, greet_as_name[i]),
			 40, y_offset + 10, 18, WHITE);
	  DrawText (TextFormat ("Local IP and Port: %s:%d", server_state.clients[i].ip,
						 server_state.clients[i].port),
			 40, y_offset + 35, 16, LIGHTGRAY);
	  DrawText (TextFormat ("Commands: %d | Last: %s",
						 server_state.clients[i].commands_received,
						 server_state.clients[i].last_command[0]
						 ? server_state.clients[i].last_command
						 : "None"),
			 40, y_offset + 58, 14, GRAY);

	  y_offset += 90;
	}
	break;
  }

  pthread_mutex_unlock (&server_state.lock);

  if (server_state.client_count == 0)
    {
      DrawText ("No clients connected. Waiting for connections...", 40,
                y_offset, 18, DARKGRAY);
      DrawText ("Scan from Ponio Android app to auto-discover this server", 40,
                y_offset + 25, 14, DARKGRAY);
    }

  EndDrawing ();
}

int
main (void)
{
  pthread_mutex_init (&server_state.lock, NULL);
  server_state.server_running = true;

  if (init_virtual_gamepad () < 0)
    {
      fprintf (stderr, "Failed to initialize Ponio gamepad\n");
      return 1;
    }

  pthread_t srv_thread;
  pthread_create (&srv_thread, NULL, server_thread, NULL);

  pthread_t discovery_tid;
  pthread_create (&discovery_tid, NULL, discovery_thread, NULL);
  pthread_detach (discovery_tid);

  printf ("\n========================================\n");
  printf ("Ponio Gamepad Server Started!\n");
  printf ("========================================\n");
  printf ("TCP Port:       %d\n", PORT);
  printf ("UDP Port:       %d\n", PORT);
  printf ("Discovery Port: 8889\n");
  printf ("========================================\n\n");

  InitWindow (SCREEN_WIDTH, SCREEN_HEIGHT, "Ponio Gamepad Server");
  SetTargetFPS (30);

  int should_exit = 0;
  const char *greet_names[] = { "Hola", "Namaste", "Bonjour", "Konnichiwa" };

  while (!WindowShouldClose () && !should_exit)
    {
      if (IsKeyPressed (KEY_Q))
        {
          should_exit = 1;
        }
      else
        {
          draw_ui (greet_names);
        }
    }

  printf ("\nShutting down server...\n");
  server_state.server_running = false;

  if (server_state.server_socket >= 0)
    {
      shutdown (server_state.server_socket, SHUT_RDWR);
      close (server_state.server_socket);
    }

  pthread_join (srv_thread, NULL);

  cleanup_virtual_gamepad ();
  pthread_mutex_destroy (&server_state.lock);

  printf ("Server shut down cleanly\n");

  CloseWindow ();
  return 0;
}
