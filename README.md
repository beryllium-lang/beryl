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

## Building

First, clone the repository:

```sh
git clone www.github.com/arvillacl16-bit/BerylliumLang.git
cd BerylliumLang
```

Compiler:

```sh
meson setup builddir
meson compile -C builddir
```

The executable is at ./builddir/beryl

Package manager:

```sh
cd mineraloil
go build -o mineraloil main.go
```

The executable is at ./mineraloil.
