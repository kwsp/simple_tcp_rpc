#include <string>
#include <asio.hpp>
#include "tcp_client.h"


void send(tcp_rpc::client& client, const std::string& cmd) {
  std::cout << "Sending cmd: " << cmd << std::endl;
  auto resp = client.send_recv(cmd);
  std::cout << "Received: " << resp << std::endl;
}

int main() {
  asio::io_context io_context;
  tcp_rpc::client client(io_context);
  client.connect("127.0.0.1", 1234);

  send(client, "f1");
  send(client, "f1 arg1 arg2 arg3");
  send(client, "abcdlkasjdlakjsdlkajwlkajsdlkasjdlkajsdlkasjdlaksjdlaksjdlkajsdlaskjdaalksdjalksjdlaksjdlasjdlaksjdlaksjdlakjsdlaksjdlaksjdlaksjdlkasjdlkajsdlkajsdlkajsdlkajsdlkasjdlaksjdlaksjdlaskjdalskdjlaksjdlaksjdlaksjd");
  send(client, "f1");
  send(client, "12345 arg1");
  send(client, "67890");
  send(client, "f1");

  return 0;
}
