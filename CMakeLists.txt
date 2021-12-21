cmake_minimum_required(VERSION 3.21)

# set the project name
project(elclang)

execute_process(COMMAND "brew" "--prefix" "llvm" OUTPUT_VARIABLE llvm-prefix OUTPUT_STRIP_TRAILING_WHITESPACE)

include_directories(${llvm-prefix}/include "/Applications/Emacs.app/Contents/Resources/include")
link_directories(${llvm-prefix}/lib)

# add the executable
add_library(elclang elclang.cc emacs-functions.c emacs-module-main.c emacs-module-utilities.c)
