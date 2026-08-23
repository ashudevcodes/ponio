#ifndef GAMEPAD_CORE_H
#define GAMEPAD_CORE_H

int init_virtual_gamepad(void);

void process_command(const char *command);

void *handle_client(void *arg);

void *server_thread(void *arg);

void cleanup_virtual_gamepad(void);

#endif
