# elclang

`elclang` is an Emacs dynamic module to interface with libclang.  It only builds on macOS 12 right now, because it references header files for building Emacs modules directly from the Emacs.app bundle.  It also requires LLVM installed by Homebrew (there will probably be forthcoming changes to support building against LLVM/libclang from a different directory in order use different or custom versions), as well as Ninja.  However, because I am too lazy to upgrade Emacs on my laptop, I can say it has been tested on both Emacs 26 & Emacs 27.

Compile with (tested on macOS 12.2 beta 21D5025f):

```
$ git clone https://github.com/nealsid/elclang
$ cd elclang && mkdir build && cd build
$ cmake ../
$ ninja elclang
```

It doesn't do anything, but you can load it with:

```
$ /Applications/Emacs.app/Contents/MacOS/Emacs -nw -q -execute "(module-load \"elclang.dylib\")"
```

I suggest starting a new Emacs because there is no way to unload modules currently (also, it really isn't useful right now).  Once inside Emacs, you can do something like `M-: (elclang-initialize-build-tree "some build tree with compile_commands.json")`.
