# Hello World for EFI

<!-- WISWA-GENERATED-README:START -->

[![C](https://img.shields.io/badge/C-00599C?logo=c)](<https://en.wikipedia.org/wiki/C_(programming_language)>)
[![GitHub tag (with filter)](https://img.shields.io/github/v/tag/Tatsh/hello-world-efi)](https://github.com/Tatsh/hello-world-efi/tags)
[![License](https://img.shields.io/github/license/Tatsh/hello-world-efi)](https://github.com/Tatsh/hello-world-efi/blob/master/LICENSE.txt)
[![GitHub commits since latest release (by SemVer including pre-releases)](https://img.shields.io/github/commits-since/Tatsh/hello-world-efi/v0.0.3/master)](https://github.com/Tatsh/hello-world-efi/compare/v0.0.3...master)
[![QA](https://github.com/Tatsh/hello-world-efi/actions/workflows/qa.yml/badge.svg)](https://github.com/Tatsh/hello-world-efi/actions/workflows/qa.yml)
[![Dependabot](https://img.shields.io/badge/Dependabot-enabled-blue?logo=dependabot)](https://github.com/dependabot)
[![pages-build-deployment](https://github.com/Tatsh/hello-world-efi/actions/workflows/pages/pages-build-deployment/badge.svg)](https://tatsh.github.io/hello-world-efi/)
[![Stargazers](https://img.shields.io/github/stars/Tatsh/hello-world-efi?logo=github&style=flat)](https://github.com/Tatsh/hello-world-efi/stargazers)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
[![CMake](https://img.shields.io/badge/CMake-6E6E6E?logo=cmake)](https://cmake.org/)
[![Prettier](https://img.shields.io/badge/Prettier-black?logo=prettier)](https://prettier.io/)
[![Tests](https://github.com/Tatsh/hello-world-efi/actions/workflows/tests.yml/badge.svg)](https://github.com/Tatsh/hello-world-efi/actions/workflows/tests.yml)
[![Coverage Status](https://coveralls.io/repos/github/Tatsh/hello-world-efi/badge.svg?branch=master)](https://coveralls.io/github/Tatsh/hello-world-efi?branch=master)

[![@Tatsh](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fpublic.api.bsky.app%2Fxrpc%2Fapp.bsky.actor.getProfile%2F%3Factor=did%3Aplc%3Auq42idtvuccnmtl57nsucz72&query=%24.followersCount&label=Follow+%40Tatsh&logo=bluesky&style=social)](https://bsky.app/profile/Tatsh.bsky.social)
[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-Tatsh-black?logo=buymeacoffee)](https://buymeacoffee.com/Tatsh)
[![Libera.Chat](https://img.shields.io/badge/Libera.Chat-Tatsh-black?logo=liberadotchat)](irc://irc.libera.chat/Tatsh)
[![Mastodon Follow](https://img.shields.io/mastodon/follow/109370961877277568?domain=hostux.social&style=social)](https://hostux.social/@Tatsh)
[![Patreon](https://img.shields.io/badge/Patreon-Tatsh2-F96854?logo=patreon)](https://www.patreon.com/Tatsh2)

<!-- WISWA-GENERATED-README:STOP -->

This is based on Roderick W. Smith's
[Creating a "Hello, World" Program](https://www.rodsbooks.com/efi-programming/hello.html). The
primary difference is the use of CMake as the build system.

## How to build

Optionally install [mtools](https://www.gnu.org/software/mtools/) (for `mformat`, `mmd`, and
`mcopy`) plus one of [xorriso](https://www.gnu.org/software/xorriso/),
[cdrtools](https://sourceforge.net/projects/cdrtools/), or
[genisoimage](https://wiki.debian.org/genisoimage) so a UEFI-bootable ISO can be generated. All
four tools must be present, otherwise the `hello.iso` target is skipped.

1. Have `cmake` in your PATH.
2. Make sure GNU EFI is installed on your system.
3. Clone this repository and have a terminal in the root of it.
4. `mkdir build`
5. `cd build`
6. `cmake -DWITH_GOP=1 -DWITH_TESTS=1 ..` (or `cmake -G Ninja ..` if you have Ninja installed).
7. `make` (or `ninja` if you used `-G Ninja` in the previous step)

If `hello.iso` was generated, use it on an UEFI system that has a shell (many do not) or emulator
such as QEMU.

## How to run in QEMU

1. Install `qemu-system-x86_64` and an OVMF firmware package (for example `edk2-ovmf` on Fedora
   and Gentoo, or `ovmf` on Debian and Ubuntu).
2. From the directory containing `hello.iso`, run:

   ```shell
   qemu-system-x86_64 -bios /usr/share/edk2-ovmf/OVMF_CODE.fd -cdrom hello.iso
   ```

   Adjust the `-bios` path to match where your distribution installs `OVMF_CODE.fd`.

3. The firmware locates `\EFI\BOOT\BOOTX64.EFI` on the El Torito FAT image inside the ISO and runs
   it automatically. _Hello, world!_ appears, and the program returns to the firmware boot
   manager.
4. Close the QEMU window (or press <kbd>Ctrl</kbd>+<kbd>A</kbd> then <kbd>X</kbd> if you launched
   with `-nographic`) to exit.

## How to run tests

In the `build` directory, run `ctest` or `./general_tests`. These run on the host operating system,
not in the EFI environment. `test.c` demonstrates how to mock the gnu-efilib with CMocka. Tests can
only be built with GCC.
