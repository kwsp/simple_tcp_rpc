#pragma once
#include <string>

namespace tcp_rpc::detail {

inline std::string _clean_cmd(std::string cmd) {
  // Erase everything after '\n'
  cmd.erase(std::find(cmd.begin(), cmd.end(), '\n'), cmd.end());

  // Erase characters that are not alphanumeric or space
  cmd.erase(
      std::remove_if(cmd.begin(), cmd.end(),
                     [](char c) { return !(std::isalnum(c) || c == ' '); }),
      cmd.end());

  // Lowercase
  std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
  return cmd;
}

} // namespace tcp_rpc::detail