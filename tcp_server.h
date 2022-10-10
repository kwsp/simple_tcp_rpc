#pragma once

#include <algorithm>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <cctype>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace tcp_rpc {
namespace detail {
using asio::ip::tcp;

typedef std::shared_ptr<tcp::socket> socket_ptr;
typedef std::function<void(const std::vector<std::string>&)> handler_t;
typedef std::unordered_map<std::string, handler_t> handlers;

// Split string s with delimiter ch
inline std::vector<std::string> split(const std::string& s, const char ch = ' ') {
  size_t i = s.find(ch);
  size_t start_i = 0;
  std::vector<std::string> res;

  while (i != std::string::npos) {
    res.push_back(s.substr(start_i, i - start_i));
    start_i = i + 1;
    i = s.find(ch, start_i);
  }
  res.push_back(s.substr(start_i, std::min(i, s.size()) - start_i + 1));
  return res;
}

inline std::string _clean_cmd(std::string cmd) {
  // Erase everything after '\n'
  cmd.erase(std::find(cmd.begin(), cmd.end(), '\n'), cmd.end());

  // Erase characters that are not alphanumeric
  cmd.erase(std::remove_if(cmd.begin(), cmd.end(),
                           [](char c) { return !std::isalnum(c); }),
            cmd.end());


  // Lowercase the command, but not the arguments
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
  return cmd;
}

inline std::vector<std::string> _clean_resp(std::string resp) {
  // Erase everything after '\n'
  resp.erase(std::find(resp.begin(), resp.end(), '\n'), resp.end());

  // Erase characters that are not alphanumeric or space
  resp.erase(std::remove_if(resp.begin(), resp.end(),
                           [](char c) { return !(std::isalnum(c) || c == ' '); }),
            resp.end());

  // Lowercase the command, but not the arguments
  auto parts = split(resp);
  auto& cmd = parts[0];
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
  return parts;

}

class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket socket, handlers &handlers_)
      : socket_(std::move(socket)), handlers_(handlers_) {}

  void start() { do_read(); }

private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        asio::buffer(recv_buf_, max_length),
        [this, self](std::error_code ec, std::size_t length) {
          if (!ec) {
            // clean up response
            auto resp = _clean_resp(recv_buf_);
            auto& cmd = resp[0];

            // find and call handler for command
            auto it = handlers_.find(cmd);
            if (it != handlers_.end())
              it->second(resp);
            else
              std::cerr << "Handler not found for cmd \"" << cmd << "\"\n";

            std::strcpy(send_buf_, "OK\n");
            do_write(std::strlen(send_buf_));
          } else {
            std::strcpy(send_buf_, "ERROR\n");
            do_write(std::strlen(send_buf_));
          }
        });
  }

  void do_write(std::size_t length) {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(send_buf_, length),
                      [this, self](std::error_code ec, std::size_t /*length*/) {
                        if (!ec) {
                          do_read();
                        }
                      });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char recv_buf_[max_length];
  char send_buf_[max_length];

  handlers &handlers_;
};

class server {
public:
  server(asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
        socket_(io_context) {
    do_accept();
  }

  void register_handler(std::string cmd, handler_t fn) {
    cmd = _clean_cmd(cmd);
    std::cout << "Registered handler for cmd \"" << cmd << "\"\n";
    handlers_[cmd] = fn;
  }

private:
  void do_accept() {
    acceptor_.async_accept(socket_, [this](std::error_code ec) {
      if (!ec) {
        std::make_shared<session>(std::move(socket_), handlers_)->start();
      }

      do_accept();
    });
  }

  tcp::acceptor acceptor_;
  tcp::socket socket_;

  handlers handlers_;
};
} // namespace detail

using detail::server;

} // namespace tcp_rpc
