# Beryllium

This is a hobby programming language. It is WIP.

## Dependencies

- C++20
- LLVM API
- Meson

For the Beryllium runtime:

- C compiler

## Building

First, clone the repository:

```sh
git clone www.github.com/arvillacl16-bit/beryllium-lang.git
cd beryllium-lang
```

To build:

```sh
meson setup builddir --buildtype=release
meson compile -C builddir
```

If you want to install system-wide:

```sh
meson install -C builddir
```

Runtime:

The C file is at ./start/start.c. It is a single file that can be compiled directly with no issues. However, you do have to pass a flag to stop it from trying to link the file.
