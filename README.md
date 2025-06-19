# Hello World for EFI

[![QA](https://github.com/Tatsh/hello-world-efi/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/hello-world-efi/actions/workflows/qa.yml)
[![Tests](https://github.com/Tatsh/hello-world-efi/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/hello-world-efi/actions/workflows/tests.yml)
[![Coverage Status](https://coveralls.io/repos/github/Tatsh/hello-world-efi/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/hello-world-efi?branch=master)
![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/hello-world-efi)
![GitHub](https://img.shields.io/github/license/Tatsh/hello-world-efi)
![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/hello-world-efi/v0.0.1/master)

This is based on Roderick W. Smith's
[Creating a "Hello, World" Program](https://www.rodsbooks.com/efi-programming/hello.html). The
primary difference is the use of CMake as the build system.

## How to build

Optionally install [cdrtools](https://sourceforge.net/projects/cdrtools/) so an ISO can be
generated.

1. Have `cmake` in your PATH.
2. Make sure GNU EFI is installed on your system.
3. Clone this repository and have a terminal in the root of it.
4. `mkdir build`
5. `cd build`
6. `cmake -DWITH_GOP=1 -DWITH_TESTS=1 ..` (or `cmake -G Ninja ..` if you have Ninja installed).
7. `make` (or `ninja` if you used `-G Ninja` in the previous step)

If `hello.iso` was generated, use it on an UEFI system that has a shell (many do not) or emulator
such as VirtualBox.

## How to run in VirtualBox

1. Set up a new VM. It does not matter too much what settings you pick at this point.
2. In the VM settings, under _System_, check _Enable EFI (special OSes only)_.
3. For storage, 'insert' the `hello.iso` file.
4. Boot up the VM. You will see the EFI shell start to boot.
5. When the shell shows a prompt, type `fs0:`
6. Type `hello.efi`. You should see _Hello, world!_ and be returned to a prompt.

## How to run tests

In the `build` directory, run `ctest` or `./general_tests`. These run on the host operating system,
not in the EFI environment. `test.c` shows how to mock the gnu-efilib.
