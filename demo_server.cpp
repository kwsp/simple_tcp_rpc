#include "tcp_server.h"

void f1() { std::cout << "f1 called!" << std::endl; }

int main() {

  try {
    const short port = 1234;
    asio::io_context io_context;
    tcp_rpc::server server(io_context, port);
    server.register_handler("f1", f1);
    io_context.run();

  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
