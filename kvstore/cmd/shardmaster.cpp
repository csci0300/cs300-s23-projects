#include "shardmaster/shardmaster.hpp"

#include <iostream>

#include "repl/repl.hpp"
#include "shardmaster/querycommand.hpp"
#include "shardmaster/static_shardmaster.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    cerr_color(RED, "Usage: ./shardmaster <PORT>");
    exit(EXIT_FAILURE);
  }

  // Get shardmaster address, for servers to connect
  std::string addr = get_host_address(argv[1]);
  std::shared_ptr<Shardmaster> shardmaster =
      std::make_shared<StaticShardmaster>(addr);

  int ret = shardmaster->start();
  if (ret < 0) {
    cerr_color(RED, "Failed to start Shardmaster on address ", addr);
    exit(EXIT_FAILURE);
  }

  Repl repl;
  // Add query, ... commands
  QueryCommand qc{shardmaster};
  repl.add_command(qc);

  repl.run();

  shardmaster->stop();

  return 0;
}
