# Emoji Description for Notepad++

**Version 0.3.15**

A Notepad++ plugin that automatically displays detailed character encoding information in the status bar for the character under the cursor.

[![CI Build](https://github.com/Ruberoid/npp_emoji_description/actions/workflows/CI_build.yml/badge.svg)](https://github.com/Ruberoid/npp_emoji_description/actions/workflows/CI_build.yml)
[![Release](https://github.com/Ruberoid/npp_emoji_description/actions/workflows/release.yml/badge.svg)](https://github.com/Ruberoid/npp_emoji_description/actions/workflows/release.yml)
[![GitHub release](https://img.shields.io/github/v/release/Ruberoid/npp_emoji_description)](https://github.com/Ruberoid/npp_emoji_description/releases)
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

GitHub: https://github.com/Ruberoid/npp_emoji_description

## Features

When you move the cursor through text, the plugin automatically displays for the current character:
- **Unicode code point** (U+XXXX format)
- **Unicode character name** (e.g. `EM DASH`, `NO-BREAK SPACE`, `GRINNING FACE`) — handy for telling look-alike characters apart; covers the entire Unicode code space including emoji; off by default
- **Decimal** value
- **Hexadecimal** value
- **HTML entity** (&#XXXX; format)
- **UTF-8 byte sequence** (hex bytes)

Each field can be toggled individually from the `Plugins → Emoji Description`
menu, so you can show just the information you care about. Your choices are
remembered between sessions.

## Supported Characters

- All ASCII characters
- All Unicode characters (including Cyrillic, Chinese, etc.)
- Emoji and other characters outside the Basic Multilingual Plane (BMP)
- Full support for multi-byte UTF-8 sequences

## Output Examples

For character `A`:
```
U+0041 | Dec: 65 | Hex: 0x41 | HTML: &#65; | UTF-8: 0x41
```

For emoji `😀`:
```
U+1F600 | Dec: 128512 | Hex: 0x1F600 | HTML: &#128512; | UTF-8: 0xF0 0x9F 0x98 0x80
```

For Cyrillic character `Я`:
```
U+042F | Dec: 1071 | Hex: 0x42F | HTML: &#1071; | UTF-8: 0xD0 0xAF
```

## Installation

### Option 1: From Release (Recommended)

1. Download the latest release from [Releases page](https://github.com/Ruberoid/npp_emoji_description/releases)
2. Choose the appropriate ZIP file for your Notepad++ version:
   - `EmojiDescription_x64_*.zip` for 64-bit Notepad++
   - `EmojiDescription_x86_*.zip` for 32-bit Notepad++
   - `EmojiDescription_arm64_*.zip` for ARM64 Notepad++
3. Extract the DLL file
4. Copy it into a `EmojiDescription` subfolder of your Notepad++ `plugins` directory, so that the final path is `plugins\EmojiDescription\EmojiDescription.dll`. The `plugins` directory depends on how Notepad++ was installed:
   - **Standard installer** (e.g. `C:\Program Files\Notepad++`): `C:\Program Files\Notepad++\plugins\EmojiDescription\`
   - **Per-user / "Don't use %APPDATA%" installs and portable builds**: `%APPDATA%\Notepad++\plugins\EmojiDescription\`

   If unsure, check `Settings → Import → Import plugin(s)...` or look at where your existing plugins live. You may need administrator rights to write into `C:\Program Files`.
5. Restart Notepad++

### Option 2: Build from Source

See [Building from Source](#building-from-source) section below.

## Usage

After installation, the plugin starts working automatically. Access plugin commands via `Plugins → Emoji Description`:

- **Show Character Info** - Toggle the whole display on/off (enabled by default)
- **Show: Code Point / Character Name / Decimal / Hexadecimal / HTML Entity / UTF-8 Bytes** - Toggle each field individually (Character Name is off by default)
- **About** - Plugin information

Simply move the cursor through text, and character information will be displayed in the status bar at the bottom of the Notepad++ window.

## Building from Source

### Requirements
- Visual Studio 2017 or later with "Desktop development with C++" workload
- Windows SDK
- (Optional) CMake 3.15 or later for CMake-based builds

### Build Option 1: Visual Studio (Recommended)

1. Open `vs.proj\EmojiDescription.vcxproj` in Visual Studio
2. Select `Release` configuration and `x64` platform (or `Win32` for 32-bit)
3. Build → Build Solution (or press `Ctrl+Shift+B`)
4. Compiled DLL will be in `bin64\` (or `bin\` for x86)

### Build Option 2: MSBuild from Developer Command Prompt

1. Open "Developer Command Prompt for VS 2022" (or your VS version)
2. Navigate to project folder:
```bash
cd <path_to_project>
```
3. Build:
```bash
msbuild vs.proj\EmojiDescription.vcxproj /p:Configuration=Release /p:Platform=x64
```

### Build Option 3: CMake

1. Create build directory:
```bash
mkdir build
cd build
```

2. Open "Developer Command Prompt for VS 2022"
3. Generate project:
```bash
cmake ..
```

4. Build:
```bash
cmake --build . --config Release
```

## Development

Repository: https://github.com/Ruberoid/npp_emoji_description

### Project Structure
```
EmojiDescription/
├── src/
│   ├── PluginDefinition.h       - Header with declarations
│   ├── PluginDefinition.cpp     - Main plugin logic
│   ├── EmojiDescription.cpp     - DLL entry point
│   ├── EmojiDescription.rc      - Resources (version, description)
│   ├── PluginInterface.h        - Notepad++ plugin interface
│   ├── Scintilla.h              - Scintilla API
│   └── ...
├── vs.proj/
│   └── EmojiDescription.vcxproj - Visual Studio project
├── .github/workflows/
│   ├── CI_build.yml             - CI for builds
│   └── release.yml              - Automated releases
├── CMakeLists.txt               - CMake configuration
└── README.md                    - This file
```

### Key Functions

- `updateCharacterInfo()` - Updates character info under cursor (src/PluginDefinition.cpp:194)
- `decodeUtf8Char()` - Decodes UTF-8 character to Unicode code point (src/PluginDefinition.cpp:91)
- `formatCharacterCodes()` - Formats all code representations (src/PluginDefinition.cpp:136)
- `pluginBeNotified()` - Handles notifications from Notepad++ and Scintilla (src/PluginDefinition.cpp:243)

## CI/CD

The project uses GitHub Actions for automated builds and releases:

- **CI Build**: Runs on every commit to verify compilation for all platforms
- **Release**: Automatically creates releases with binaries when a version tag is pushed

See [PUBLISHING.md](PUBLISHING.md) for details on creating releases.

## License

This plugin is distributed under the GPL v2 license, same as Notepad++.

See [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Support

If you encounter any issues or have suggestions, please [open an issue](https://github.com/Ruberoid/npp_emoji_description/issues) on GitHub.

## Author

**Ruberoid**
- GitHub: [@Ruberoid](https://github.com/Ruberoid)
- Repository: https://github.com/Ruberoid/npp_emoji_description

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history and changes.
