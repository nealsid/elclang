#!/bin/bash

install_name_tool -change /usr/local/opt/llvm/lib/libclang.dylib /Users/nealsid/src/github/llvm-project-build/lib/libclang.dylib libelclang.dylib
