#!/bin/bash

echo "Compiling src files."
make

if [ $? -eq 0 ]; then
  echo "Build complete. Binaries in build/"
else
  echo "Build failed."
fi
