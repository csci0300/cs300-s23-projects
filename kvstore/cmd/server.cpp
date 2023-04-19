#include "server/server.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "kvstore/kvstore.hpp"
#include "repl/repl.hpp"
#include "server/printcommand.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2 || argc > 4) {
    cerr_color(RED,
               "\nIf on Concurrent Store:\n"
               "\t./server <port> [n_workers]\n"
               "If on Distributed Store:\n"
               "\t./server <port> <shardmaster_addr:port> [n_workers]");
    return EXIT_FAILURE;
  }

  std::shared_ptr<KvServer> server;

  std::string addr = get_host_address(argv[1]);
  std::string shardmaster_addr;
  uint64_t n_workers = N_WORKERS;
  // Part A: Concurrent Store
  if (argc == 3) {
    try {
      // Check if shardmaster is excluded, and # workers is specified
      n_workers = std::stoi(argv[2]);
    } catch (std::invalid_argument const& e) {
      // If stoi fails, 3rd argument is the shardmaster address
      shardmaster_addr = std::string(argv[3]);
    }
  } else if (argc == 4) {
    // Part B: Distributed Store
    shardmaster_addr = std::string(argv[2]);
    n_workers = std::stoi(argv[3]);
  }

  // If no shardmaster address specified, Concurrent Store; otherwise,
  // Distributed Store
  if (shardmaster_addr.empty()) {
    server = std::make_shared<KvServer>(addr, n_workers);
  } else {
    server = std::make_shared<KvServer>(addr, shardmaster_addr, n_workers);
  }

  int ret = server->start();
  if (ret < 0) {
    exit(EXIT_FAILURE);
  }

  Repl repl;
  // - `print <store|config>` (display store/config)
  PrintCommand pc{server};
  repl.add_command(pc);

  repl.run();

  server->stop();
}
