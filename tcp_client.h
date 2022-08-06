#include <asio.hpp>
#include <iostream>
#include <string>

using asio::ip::tcp;

namespace tcp_rpc {
class client {
  enum { max_length = 1024 };

public:
  client(asio::io_context &io_context)
      : io_context_(io_context), socket_(io_context) {}

  void connect(std::string addr, short port) {
    socket_.connect(tcp::endpoint(asio::ip::address::from_string(addr), port));
  }

  std::string send_recv(const std::string &msg) {
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
} // namespace tcp_rpc
