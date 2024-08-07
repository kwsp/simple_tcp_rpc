#include "tcp_rpc/tcp_rpc.hpp"
#include <string>
#include <vector>

void f1(const std::vector<std::string> &args) {
  std::cout << "f1 called with args (";
  for (size_t i = 1; i < args.size(); i++) {
    std::cout << args[i] << ", ";
  }
  std::cout << ")\n";
}

int main() {
  const short port = 1234;
  asio::io_context io_context;
  tcp_rpc::server server(io_context, port);
  server.register_handler("f1", f1);

  try {
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
