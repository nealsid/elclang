# elclang
An Emacs dynamic module to interface with libclang.

Compile with (only tested on macOS 12.1):

```
$ clang -I $(brew --prefix llvm)/include -I/Applications/Emacs.app/Contents/Resources/include elclang.c -Xlinker -dylib \
	-Xlinker -o -Xlinker elclang.dylib -Xlinker -lclang -Xlinker -L -Xlinker $(brew --prefix llvm)/lib
```

It doesn't do anything, but you can load it with:

```
$ /Applications/Emacs.app/Contents/MacOS/Emacs -nw -q -execute "(module-load \"elclang.dylib\")"
```

Then, once inside Emacs, do something like `M-: (elclang-initialize-build-tree "some build tree with compile_commands.json")`.
