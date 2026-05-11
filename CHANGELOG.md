<!-- markdownlint-configure-file {"MD024": { "siblings_only": true } } -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project
adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.0.4] - 2026-05-10

### Added

- Interactive prompts in the application: it now greets, prompts for a keypress before listing GOP
  modes and framebuffer information, prompts again before exiting, and clears the screen on the way
  out so output remains visible.

### Changed

- `hello.iso` is now a genuine UEFI-bootable ISO. It embeds a FAT image containing
  `EFI/BOOT/BOOTX64.EFI` and exposes it through El Torito, so UEFI firmware (such as OVMF) boots
  the application automatically without any manual `fs0:` step.
- Building `hello.iso` now requires `mtools` (`mformat`, `mmd`, and `mcopy`) along with one of
  `xorrisofs`, `mkisofs`, or `genisoimage`; if any are missing, the `hello.iso` target is skipped
  with a status message.
- Updated `README.md` to document the new build dependencies and the auto-boot QEMU flow.

### Removed

- The arbitrary `SetMode(12)` call that wiped earlier output before the user could read it.

## [0.0.3] - 2026-05-07

### Added

- Tests and Coverage badges to `README.md`.
- Gated GitHub release publishing workflow.
- Build attestation and artefact upload to the CMake workflow.

### Changed

- CMake workflow names release assets with the version.
- Bumped GitHub Actions: `actions/checkout` 5 → 6, `actions/upload-artifact` 6 → 7,
  `actions/attest-build-provenance` 3 → 4, `softprops/action-gh-release` 2 → 3, and
  `github/codeql-action` 3 → 4.
- Bumped development dependencies: `cspell`, `prettier`, `prettier-plugin-sort-json`, and
  `markdownlint-cli2`.
- Refreshed project scaffolding.

### Fixed

- Bumped transitive npm dependencies (`flatted`, `js-yaml`, and `picomatch`) to address advisories.

### Removed

- `cdrtools` PPA dependency from CI workflows.

## [0.0.2] - 2025-06-19

### Added

- Commitizen configuration.
- Jekyll `_config.yml` for GitHub Pages.
- pre-commit configuration.

### Changed

- Refreshed and expanded the test suite.
- Updated `README.md`.
- Refreshed project scaffolding.

### Fixed

- Several test failures.
- Removed a dead store.
- Bumped `braces` to address an advisory.

## [0.0.1] - 2023-10-01

First version.

[unreleased]: https://github.com/Tatsh/hello-world-efi/compare/v0.0.4...HEAD
[0.0.4]: https://github.com/Tatsh/hello-world-efi/compare/v0.0.3...v0.0.4
[0.0.3]: https://github.com/Tatsh/hello-world-efi/compare/v0.0.2...v0.0.3
[0.0.2]: https://github.com/Tatsh/hello-world-efi/compare/v0.0.1...v0.0.2
[0.0.1]: https://github.com/Tatsh/hello-world-efi/releases/tag/v0.0.1
