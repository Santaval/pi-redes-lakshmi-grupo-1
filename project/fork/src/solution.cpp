// Copyright <year> <You>
#include <iostream>
#include <string>
#include <memory>
#include "fork/includes/Fork.hpp"

/**
 * @brief Start program execution.
 *
 * @return Status code to the operating system, 0 means success.
 */
int main(int argc, char* argv[]) {
  // Start the Fork server to handle client requests
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port> <ip_address>\n";
    return 1;
  }
  int port = std::stoi(argv[1]);
  if (port <= 0 || port > 65535) {
    std::cerr << "Error: Invalid port number. Must be between 1 and 65535.\n";
    return 1;
  }
  Fork* fork = new Fork(port, "127.0.0.1");
  fork->start("127.0.0.1");
  delete fork; // Clean up the allocated memory
  
  return 0;
}
