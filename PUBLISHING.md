# Publishing Emoji Description to Notepad++ Plugin Admin

This guide explains how to publish the Emoji Description plugin to the official Notepad++ Plugin Admin repository.

## Automated Release Process

This project uses **GitHub Actions** to automatically create releases. No manual archive creation or file uploads needed!

### Quick Release (TL;DR)

```bash
# 1. Update version in src/EmojiDescription.rc
# 2. Update CHANGELOG.md
# 3. Commit changes
git add .
git commit -m "Bump version to 0.2.0"
git push

# 4. Create and push tag
git tag -a v0.2.0 -m "Release version 0.2.0"
git push origin v0.2.0

# Done! GitHub Actions will:
# - Build binaries for x86, x64, and ARM64
# - Create ZIP archives
# - Create GitHub Release
# - Attach all files
```

## Detailed Release Process

### Step 1: Prepare the Release

1. **Update version number** in `src/EmojiDescription.rc`:
```cpp
#define VERSION_VALUE "0.2.0\0"
#define VERSION_DIGITALVALUE 0, 2, 0, 0
```

2. **Update CHANGELOG.md**:
```markdown
## [0.2.0] - 2026-01-XX

### Added
- New feature description

### Fixed
- Bug fix description
```

3. **Commit changes**:
```bash
git add src/EmojiDescription.rc CHANGELOG.md
git commit -m "Bump version to 0.2.0"
git push
```

### Step 2: Create Release Tag

```bash
# Create annotated tag
git tag -a v0.2.0 -m "Release version 0.2.0

- Feature 1
- Feature 2
- Bug fix 1"

# Push tag to GitHub
git push origin v0.2.0
```

### Step 3: Wait for Automation

GitHub Actions will automatically (takes ~3-5 minutes):

1. ✅ Build plugin for all platforms (x86, x64, ARM64)
2. ✅ Create ZIP archives:
   - `EmojiDescription_x64_v0.2.0.zip`
   - `EmojiDescription_x86_v0.2.0.zip`
   - `EmojiDescription_arm64_v0.2.0.zip`
3. ✅ Create GitHub Release
4. ✅ Attach ZIP files to release

**Monitor progress**: https://github.com/Ruberoid/npp_emoji_description/actions

**Check release**: https://github.com/Ruberoid/npp_emoji_description/releases

### Step 4: Verify Release

1. Go to [Releases page](https://github.com/Ruberoid/npp_emoji_description/releases)
2. Verify the new release appears with all three ZIP files
3. Download and test one of the archives

## Publishing to Notepad++ Plugin List

Once you have a stable release, you can submit the plugin to the official Notepad++ Plugin List.

### Prerequisites

- ✅ Stable release published on GitHub
- ✅ GPL v2 compatible license
- ✅ Public source code repository
- ✅ Binaries hosted on GitHub Releases
- ✅ Both x86 and x64 builds available

### Submission Process

#### 1. Generate Plugin GUID

Generate a unique GUID for your plugin:

**PowerShell**:
```powershell
[guid]::NewGuid()
```

**Online**: https://www.guidgenerator.com/

Save this GUID - you'll use it for all versions.

#### 2. Fork Plugin List Repository

Fork the official repository:
https://github.com/notepad-plus-plus/nppPluginList

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/nppPluginList.git
cd nppPluginList

# Create branch
git checkout -b add-emoji-description
```

#### 3. Add Plugin Entry

**Edit `src/pl.x64.json`**:

Add your plugin entry (keep alphabetical order by `folder-name`):

```json
{
    "folder-name": "EmojiDescription",
    "display-name": "Emoji Description",
    "version": "0.1.0",
    "id": "YOUR-GUID-HERE",
    "repository": "https://github.com/Ruberoid/npp_emoji_description",
    "description": "Displays detailed character encoding information in the status bar. Shows Unicode code point, decimal/hexadecimal values, HTML entity, and UTF-8 byte sequence for any character including emoji.",
    "author": "Ruberoid",
    "homepage": "https://github.com/Ruberoid/npp_emoji_description",
    "x64-download": "https://github.com/Ruberoid/npp_emoji_description/releases/download/v0.1.0/EmojiDescription_x64_v0.1.0.zip"
}
```

**Edit `src/pl.x86.json`**:

Add the same entry with x86 download URL:

```json
{
    "folder-name": "EmojiDescription",
    "display-name": "Emoji Description",
    "version": "0.1.0",
    "id": "YOUR-GUID-HERE",
    "repository": "https://github.com/Ruberoid/npp_emoji_description",
    "description": "Displays detailed character encoding information in the status bar. Shows Unicode code point, decimal/hexadecimal values, HTML entity, and UTF-8 byte sequence for any character including emoji.",
    "author": "Ruberoid",
    "homepage": "https://github.com/Ruberoid/npp_emoji_description",
    "x86-download": "https://github.com/Ruberoid/npp_emoji_description/releases/download/v0.1.0/EmojiDescription_x86_v0.1.0.zip"
}
```

#### 4. Validate JSON

```bash
# Install dependencies (if needed)
npm install

# Validate JSON
npm test
```

Fix any validation errors before proceeding.

#### 5. Submit Pull Request

```bash
# Commit changes
git add src/pl.x64.json src/pl.x86.json
git commit -m "Add Emoji Description plugin v0.1.0"

# Push to your fork
git push origin add-emoji-description
```

**Create Pull Request** on GitHub with this template:

```markdown
## Plugin Information
- **Name**: Emoji Description
- **Version**: 0.1.0
- **Author**: Ruberoid
- **Repository**: https://github.com/Ruberoid/npp_emoji_description

## Description
Displays detailed character encoding information in the status bar for any character under the cursor, with full emoji support.

## Features
- Unicode code point (U+XXXX)
- Decimal and hexadecimal values
- HTML entity format
- UTF-8 byte sequence
- Works with all Unicode characters including emoji

## Checklist
- [x] Plugin binaries hosted on GitHub Releases
- [x] Both x86 and x64 versions provided
- [x] JSON entries added to both pl.x64.json and pl.x86.json
- [x] Plugin follows Notepad++ plugin guidelines
- [x] Source code publicly available
- [x] GPL v2 compatible license
- [x] JSON validation passed (`npm test`)
```

#### 6. Wait for Review

Notepad++ maintainers will review your submission. They may:
- Request changes
- Ask questions
- Test the plugin
- Merge if everything is OK

**Review time**: Usually 1-7 days

## Updating Plugin Version

When releasing a new version:

### 1. Create New Release

Follow [Step 1-3](#detailed-release-process) above to create a new release with updated version.

### 2. Update Plugin List

Fork/clone nppPluginList again (or pull latest changes):

```bash
cd nppPluginList
git checkout main
git pull upstream main
git checkout -b update-emoji-description-v0.2.0
```

Update version and download URLs in both JSON files:

```json
{
    "version": "0.2.0",
    "x64-download": "https://github.com/Ruberoid/npp_emoji_description/releases/download/v0.2.0/EmojiDescription_x64_v0.2.0.zip"
}
```

Submit PR with title: `Update Emoji Description to v0.2.0`

## Requirements for Acceptance

- ✅ GPL v2 or compatible license
- ✅ Public source code
- ✅ Binaries on GitHub Releases
- ✅ Both x86 and x64 builds
- ✅ Valid plugin interface implementation
- ✅ No malware or suspicious code
- ✅ Clear documentation
- ✅ Stable release (not alpha/beta for first submission)

## Useful Links

- **Plugin List Repo**: https://github.com/notepad-plus-plus/nppPluginList
- **Plugin Template**: https://github.com/npp-plugins/plugintemplate
- **Plugin Development**: https://npp-user-manual.org/docs/plugin-communication/
- **Plugin List Guidelines**: https://github.com/notepad-plus-plus/nppPluginList#submission-guidelines

## Troubleshooting

### Release workflow failed?

Check GitHub Actions logs:
https://github.com/Ruberoid/npp_emoji_description/actions

Common issues:
- Missing permissions (add `permissions: contents: write`)
- Build errors (check MSBuild logs)
- Invalid tag format (must be `v*.*.*`)

### JSON validation failed?

```bash
cd nppPluginList
npm test
```

Fix syntax errors, missing commas, or invalid URLs.

### Download links not working?

Ensure:
- Release is published (not draft)
- ZIP files are attached to release
- URLs match exactly: `https://github.com/Ruberoid/npp_emoji_description/releases/download/v0.1.0/EmojiDescription_x64_v0.1.0.zip`
