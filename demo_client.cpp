#include "tcp_client.h"

int main() {
  asio::io_context io_context;

  tcp_rpc::client client(io_context);

  client.connect("127.0.0.1", 1234);
  const std::string cmd = "f1\n";
  std::cout << "Sending cmd: " << cmd << std::endl;

  auto resp = client.send_recv(cmd);
  std::cout << "Received: " << resp << std::endl;

  return 0;
}
