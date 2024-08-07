#!/bin/bash
set -e

meson setup --reconfigure --debug build 

# Link compile_commands.json
if [[ -f compile_commands.json ]]; then
  rm compile_commands.json
fi
ln -s ./build/compile_commands.json compile_commands.json
