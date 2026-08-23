#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "../include/gamepad_core.h"
#include "../include/gamepad_types.h"
#include "../include/server_discovery.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static struct termios orig_termios;

void set_nonblock(void)
{
	tcgetattr(STDIN_FILENO, &orig_termios);

	struct termios t = orig_termios;
	t.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);

	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

void restore_terminal(void)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

int main(void)
{
	pthread_mutex_init(&server_state.lock, NULL);
	server_state.server_running = true;

	if (init_virtual_gamepad() < 0) {
		fprintf(stderr,
		        "Error: no access to /dev/uinput.\n"
		        "Run setup once: sudo ./install-uinput.sh\n");

		fprintf(stderr, "Failed to initialize Ponio gamepad\n");
		return 1;
	}

	pthread_t srv_thread;
	pthread_create(&srv_thread, NULL, server_thread, NULL);

	pthread_t discovery_tid;
	pthread_create(&discovery_tid, NULL, discovery_thread, NULL);
	pthread_detach(discovery_tid);

	// int should_exit = 0;
	// const char *greet_names[] = { "Hola", "Namaste", "Bonjour", "Konnichiwa" };

	set_nonblock();
	char key = 0;

	printf("\n========================================\n");
	printf("Ponio Gamepad Server Started!\n");
	printf("========================================\n");
	printf("TCP Port:       %d\n", PORT);
	// printf("UDP Port:       %d\n", PORT);
	printf("Discovery Port: 8889\n\n");
	printf("Press 'q' to EXIT :)\n");
	printf("========================================\n\n");

	while (key != 'q') {
		key = getchar();
	}

	printf("\nShutting down server...\n");
	server_state.server_running = false;

	if (server_state.server_socket >= 0) {
		shutdown(server_state.server_socket, SHUT_RDWR);
		close(server_state.server_socket);
	}

	pthread_join(srv_thread, NULL);

	cleanup_virtual_gamepad();
	pthread_mutex_destroy(&server_state.lock);

	printf("Server shut down cleanly\n");

	restore_terminal();
	// CloseWindow();
	return 0;
}
