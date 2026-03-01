# Beryllium

This is a hobby programming language. It is WIP.

## Dependencies

For the compiler:

- C++20
- LLVM API
- Meson

For the package manager:

- Go
- Cobra

For the Beryllium runtime:

- C compiler

## Building

First, clone the repository:

```sh
git clone www.github.com/arvillacl16-bit/beryllium-lang.git
cd beryllium-lang
```

Compiler:

```sh
cd compiler
meson setup builddir
meson compile -C builddir
```

The executable is at ./builddir/beryl

Package manager:

```sh
cd mineraloil
go build -o mineraloil main.go
```

Runtime:

The C file is at ./start/start.c. It is a single file that can be compiled directly with no issues.

The executable is at ./mineraloil.
