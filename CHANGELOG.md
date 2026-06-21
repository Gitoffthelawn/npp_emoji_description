# Changelog

All notable changes to Emoji Description plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.2.14] - 2026-06-21

### Added
- Per-field display toggles in the plugin menu — show only the information you
  care about (e.g. just the UTF-8 bytes), hiding code point / decimal / hex /
  HTML entity individually (#4)
- Optional Unicode character name in the status bar (e.g. `EM DASH`, `NO-BREAK
  SPACE`) to distinguish look-alike characters, off by default. Uses the system
  `getuname.dll` lookup and covers Basic Multilingual Plane characters (#4)
- Settings are persisted to `EmojiDescription.ini` in the Notepad++ plugin
  config directory and restored on startup

### Fixed
- Documented the `C:\Program Files\Notepad++\plugins` install path for standard
  installer-based setups (#3)

### Changed
- CI/release builds pinned to the `windows-2022` runner so the v143 (VS2022)
  toolset remains available

## [0.1.0] - 2026-01-03

### Added
- Initial release of Emoji Description plugin
- Display character encoding information in Notepad++ status bar
- Show Unicode code point (U+XXXX format)
- Show decimal representation
- Show hexadecimal representation
- Show HTML entity (&#XXXX; format)
- Show UTF-8 byte sequence
- Support for all Unicode characters including emoji
- Toggle on/off functionality via plugin menu
- About dialog with plugin information

### Technical
- Built with Visual Studio 2022
- Supports x86, x64, and ARM64 architectures
- Uses Scintilla API for text editor integration
- GPL v2 licensed

[Unreleased]: https://github.com/Ruberoid/npp_emoji_description/compare/v0.2.14...HEAD
[0.2.14]: https://github.com/Ruberoid/npp_emoji_description/compare/v0.1.0...v0.2.14
[0.1.0]: https://github.com/Ruberoid/npp_emoji_description/releases/tag/v0.1.0
