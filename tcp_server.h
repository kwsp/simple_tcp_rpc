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

namespace tcp_rpc {
namespace detail {
using asio::ip::tcp;

typedef std::shared_ptr<tcp::socket> socket_ptr;
typedef std::function<void(void)> callable;
typedef std::unordered_map<std::string, callable> handlers;

inline std::string _clean_cmd(std::string cmd) {
  // Erase everything after '\n'
  cmd.erase(std::find(cmd.begin(), cmd.end(), '\n'), cmd.end());

  // Erase characters that are not alphanumeric
  cmd.erase(std::remove_if(cmd.begin(), cmd.end(),
                           [](char c) { return !std::isalnum(c); }),
            cmd.end());

  // Lowercase
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
  return cmd;
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
            // clean up cmd
            auto cmd = _clean_cmd(recv_buf_);

            // find and call handler for command
            auto it = handlers_.find(cmd);
            if (it != handlers_.end())
              it->second();
            else
              std::cerr << "Handler not found for cmd (" << cmd << ")\n";

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

  void register_handler(std::string cmd, std::function<void(void)> fn) {
    cmd = _clean_cmd(cmd);
    std::cout << "Registered handler for cmd (" << cmd << ")\n";
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
