#include <asio.hpp>
#include <iostream>
#include <string>

using asio::ip::tcp;

enum { max_length = 1024 };

class tcp_client {
public:
  tcp_client(asio::io_context &io_context)
      : io_context_(io_context), socket_(io_context) {}

  void connect(std::string addr, short port) {
    socket_.connect(tcp::endpoint(asio::ip::address::from_string(addr), port));
  }

  std::string send_recv(const std::string& msg) {
    // request/msg from client
    asio::write(socket_, asio::buffer(msg));

    // Get response from server
    asio::streambuf resp;
    size_t reply_len = asio::read_until(socket_, resp, '\n');
    std::istream is(&resp);
    std::string s;
    is >> s;
    return s;
  }

private:
  asio::io_context &io_context_;
  tcp::socket socket_;
};

int main() {
  asio::io_context io_context;

  tcp_client client(io_context);

  client.connect("127.0.0.1", 1234);
  const std::string cmd = "f1\n";
  std::cout << "Sending cmd: " << cmd << std::endl;

  auto resp = client.send_recv(cmd);
  std::cout << "Received: " << resp << std::endl;

  return 0;
}
