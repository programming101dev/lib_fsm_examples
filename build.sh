#!/usr/bin/env bash

# --help / -h -> description, exit 0 (P101 uniform CLI help)
case " $* " in
  *" --help "*|*" -h "*)
    cat <<'P101_USAGE'
build.sh — takes no command-line options; run with no arguments.
P101_USAGE
    exit 0 ;;
esac

if [ ! -f "./CMakeLists.txt" ]; then
  echo "You must run ./generate-cmakelists.sh first"
  exit 1
fi

if [ ! -d "./build/" ]; then
  echo "You must run ./change-compiler.sh first"
  exit 1
fi

cmake --build build --clean-first
