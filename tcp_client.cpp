#include <asio.hpp>
#include <iostream>
#include <string>

using asio::ip::tcp;

enum { max_length = 1024 };

int main() {
  asio::io_context io_context;

  // create socket
  tcp::socket socket(io_context);
  socket.connect(
      tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 1234));

  // request/msg from client
  const std::string msg = "f1\n";
  asio::write(socket, asio::buffer(msg));

  // Get response from server
  asio::streambuf resp;
  size_t reply_len = asio::read_until(socket, resp, '\n');
  std::istream is(&resp);
  std::string s;
  is >> s;
  std::cout << s << std::endl;

  return 0;
}
