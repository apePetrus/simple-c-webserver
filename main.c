#include "server.h"

int main() {
  /*
  * AF_INET == IPV4
  * 80 == Server Port
  * SOCK_STREAM == TCP
  * 0 is the protocol to bbe used, usually 0 for automatic selection
  */
  struct Server server = server_Constructor(AF_INET, 80, SOCK_STREAM, 0, 10, INADDR_ANY, launch);
  server.launch(&server);
  return 0;
}
