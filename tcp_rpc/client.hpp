#pragma once
#include <tcp_rpc/common.hpp>

#include <asio.hpp>
#include <deque>
#include <iostream>
#include <string>

using asio::ip::tcp;

namespace tcp_rpc {

namespace detail {

class client {
public:
  client(asio::io_context &io_context) : socket_(io_context) {}

  void connect(std::string addr, short port) {
    if (!socket_.is_open()) {
      socket_.connect(
          tcp::endpoint(asio::ip::address::from_string(addr), port));
    }
  }

  // Send cmd to the server. All characters that are not alphanumeric will be
  // stripped. A new line character will be appened to the sent command.
  // Blocking version. (not async)
  std::string send_recv(std::string cmd) {
    cmd = _clean_cmd(cmd) + '\n';
    asio::write(socket_, asio::buffer(cmd));

    // Get response from server
    asio::streambuf resp;
    asio::read_until(socket_, resp, '\n');
    std::string s;
    std::istream is(&resp);
    is >> s;
    return s;
  }

private:
  tcp::socket socket_;
};
} // namespace detail

using detail::client;

} // namespace tcp_rpc
