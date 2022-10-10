#include <asio.hpp>
#include <iostream>
#include <string>

using asio::ip::tcp;

namespace tcp_rpc {
namespace detail {
inline std::string _clean_cmd(std::string cmd) {
  // Erase everything after '\n'
  cmd.erase(std::find(cmd.begin(), cmd.end(), '\n'), cmd.end());

  // Erase characters that are not alphanumeric or space
  cmd.erase(std::remove_if(cmd.begin(), cmd.end(),
                           [](char c) { return !(std::isalnum(c) || c == ' '); }),
            cmd.end());

  // Lowercase
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
  return cmd;
}
class client {
  enum { max_length = 1024 };

public:
  client(asio::io_context &io_context)
      : io_context_(io_context), socket_(io_context) {}

  void connect(std::string addr, short port) {
    socket_.connect(tcp::endpoint(asio::ip::address::from_string(addr), port));
  }

  // Send cmd to the server. All characters that are not alphanumeric will be stripped. 
  // A new line character will be appened to the sent command.
  std::string send_recv(std::string cmd) {
    cmd = _clean_cmd(cmd) + '\n';
    asio::write(socket_, asio::buffer(cmd));

    // Get response from server
    asio::streambuf resp;
    asio::read_until(socket_, resp, '\n');
    std::istream is(&resp);
    std::string s;
    is >> s;
    return s;
  }

private:
  asio::io_context &io_context_;
  tcp::socket socket_;
};
} // namespace detail

using detail::client;

} // namespace tcp_rpc
