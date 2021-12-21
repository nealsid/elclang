cmake_minimum_required(VERSION 3.21)

# set the project name
project(elclang)

execute_process(COMMAND "brew" "--prefix" "llvm" OUTPUT_VARIABLE llvm-prefix OUTPUT_STRIP_TRAILING_WHITESPACE)

include_directories(${llvm-prefix}/include "/Applications/Emacs.app/Contents/Resources/include")
link_directories(${llvm-prefix}/lib)

# add the library
add_library(elclang SHARED elclang.cc emacs-functions.c emacs-module-main.c emacs-module-utilities.c)
target_link_libraries(elclang clang)
