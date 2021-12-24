#!/bin/bash
# Meant to be run from build subdirectory of repository.

lldb /Applications/Emacs.app/Contents/MacOS/Emacs-x86_64-10_14 -- --script ../test-script.el
