#!/bin/sh
# Host-side tests: the value decoder (C++) and the definitions parser and codegen (Python).
set -eu
cd "$(dirname "$0")/.."

build_dir=$(mktemp -d)
trap 'rm -rf "$build_dir"' EXIT

c++ -std=c++17 -Wall -Wextra -Werror -I components/bsb \
  tests/test_value_decoder.cpp components/bsb/bsbValueDecoder.cpp components/bsb/bsbDumpFormat.cpp \
  -o "$build_dir/test_value_decoder"
"$build_dir/test_value_decoder"

PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover tests
