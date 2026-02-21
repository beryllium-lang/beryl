# Beryllium

This is a hobby programming language. It is WIP.

## Dependencies

For the compiler:

- C++20
- LLVM API
- Make

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

There are only Makefiles for macOS homebrew clang, macOS Apple Clang, and Linux, but the code itself is cross-platform.
Linux:

```sh
cd linux
make
```

The executable will be located at ./beryl.

macOS Apple Clang:

```sh
cd macOS-apple-clang
make
```

The executable will be located at ./beryl.

macOS Homebrew Clang:

```sh
cd macOS-homebrew-clang
make
```

The executable will be located at ./beryl.

For Windows, figure it out yourself.

Package manager:

```sh
cd mineraloil
go build -o mineraloil main.go
```

The executable is at ./mineraloil.
