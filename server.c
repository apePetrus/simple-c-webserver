#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "server.h"

struct Server server_Constructor(int domain, int port, int service, int protocol, int backlog, u_long interface, void (*launch)(struct Server *server)) {
  struct Server server;

  server.domain = domain;
  server.port = port;
  server.service = service;
  server.protocol = protocol;
  server.backlog = backlog;

  server.address.sin_family = domain;
  server.address.sin_port = htons(port);
  server.address.sin_addr.s_addr = htonl(interface);

  /*
  * socket() creates a socket
  * Params:
  * specified domain (protocol family, e.g ipv4 or ipv6)
  * type of service (tcp or udp)
  * protocol (0 for automatic selection)
  * returns 0 if success, -1 on failure
  */
  server.socket = socket(domain, service, protocol);
  if (server.socket < 0) {
    perror("Failed to initialize/connect to socket...\n");
    exit(EXIT_FAILURE);
  }

  /*
  * bind() associates the socket with the ip address and port
  *  Params:
  *  socket (socket descriptor)
  *  addr (pointer to struct sockaddr containing IP address and port)
  *  addrlen (size of the structure)
  *  returns 0 if success, -1 on failure
  */
  if (bind(server.socket, (struct sockaddr*)&server.address, sizeof(server.address)) < 0) {
    perror("Failed to bind socket...\n");
    exit(EXIT_FAILURE);
  }

  /*
  * listen() marks the socket as a passive socket for incoming connection requests
  * Params:
  * socket (socker descriptor)
  * backlog (maximum length of the queue of pending connections)
  */
  if (listen(server.socket, server.backlog) < 0) {
    perror("Failed to start listening...\n");
    exit(EXIT_FAILURE);
  }

  server.launch = launch;
  return server;
}

void launch(struct Server *server) {
  char buffer[BUFFER_SIZE];
  while (1) {
    printf("=== WAITING FOR CONNECTION === \n");
    int addrlen = sizeof(server->address);
    /*
    * Function accept() extracts the first connection request, creates a new socket for the connection
    * Params:
    * socket (original socket for listening)
    * addr (pointer to struct sockaddr for client's address)
    * addrlen (pointer to socklen_t for the length of the client's address)
    * returns a new socket descriptor if successful, -1 on failure
    */
    int new_socket = accept(server->socket, (struct sockaddr*)&server->address, (socklen_t*)&addrlen);
    ssize_t bytesRead = read(new_socket, buffer, BUFFER_SIZE - 1);
    if (bytesRead >= 0) {
      buffer[bytesRead] = '\0'; // Null terminate the string
      puts(buffer);
    } else {
      perror("Error reading buffer...\n");
    }
    char *response = "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                     "<!DOCTYPE html>\r\n"
                     "<html>\r\n"
                     "<head>\r\n"
                     "<title>Testing Basic HTTP-SERVER</title>\r\n"
                     "</head>\r\n"
                     "<body>\r\n"
                     "<h1>Hello, Petrus!</h1>\r\n"
                     "</body>\r\n"
                     "</html>\r\n";
    write(new_socket, response, strlen(response));
    close(new_socket);
  }
}

