#ifndef SOCKUTILS_H 
#define SOCKUTILS_H 

extern int create_listen_socket(int domain, int type, int protocol, char* ip, int port, int backlog);
extern int accept_socket(int listen_sock);
extern void write_msg(int client_sock, const void* buffer);

#endif