# Changelog

All notable changes to Emoji Description plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.3.15] - 2026-06-22

### Changed
- The Unicode character name now covers the **entire** Unicode code space —
  supplementary planes, emoji, CJK / Tangut ideographs and Hangul syllables
  included — instead of only the Basic Multilingual Plane. Names are served from
  a table embedded in the plugin (generated from the pinned Unicode Character
  Database by `tools/unicode_names/gen_names.py`), so the lookup no longer
  depends on the system `getuname.dll` and behaves identically on every Windows
  version and architecture. Adds roughly 640 KB to the DLL.
- New default fields: code point, character name and HTML entity are shown out
  of the box; decimal, hexadecimal and the UTF-8 byte sequence are off by
  default. The character name is no longer opt-in now that it always resolves.
- The plugin menu now separates the master "Show Character Info" toggle from the
  per-field options with a divider.

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

[Unreleased]: https://github.com/Ruberoid/npp_emoji_description/compare/v0.3.15...HEAD
[0.3.15]: https://github.com/Ruberoid/npp_emoji_description/compare/v0.2.14...v0.3.15
[0.2.14]: https://github.com/Ruberoid/npp_emoji_description/compare/v0.1.0...v0.2.14
[0.1.0]: https://github.com/Ruberoid/npp_emoji_description/releases/tag/v0.1.0
