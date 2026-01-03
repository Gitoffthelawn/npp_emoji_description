# Setting Up Visual Studio C++ Components for Building

This guide helps you install the necessary C++ components for Visual Studio 2022 to build the Emoji Description plugin.

## Automatic Installation (Recommended)

Open **PowerShell as Administrator** and run:

```powershell
& "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vs_installer.exe" modify --installPath "C:\Program Files\Microsoft Visual Studio\2022\Community" --add Microsoft.VisualStudio.Workload.NativeDesktop --quiet --norestart
```

This command will install:
- "Desktop development with C++" workload
- MSVC v143 build tools
- Windows SDK

## Manual Installation via GUI

1. Close Visual Studio (if open)

2. Launch Visual Studio Installer:
   - Press Win + S
   - Type "Visual Studio Installer"
   - Run the application

3. In Visual Studio Installer:
   - Find "Visual Studio Community 2022"
   - Click the **"Modify"** button

4. In the workloads list:
   - Check **"Desktop development with C++"**

5. On the right side in "Installation details", ensure these are selected:
   - ✓ MSVC v143 - VS 2022 C++ build tools (latest version)
   - ✓ Windows SDK (any 10.0.x or 11.0.x version)
   - ✓ C++ CMake tools for Windows

6. Click the **"Modify"** button at the bottom right

7. Wait for installation to complete (may take 5-15 minutes)

## Verify Installation

After installation, open "Developer Command Prompt for VS 2022" and run:

```bash
cl
```

Should output information about the Microsoft C/C++ compiler, for example:
```
Microsoft (R) C/C++ Optimizing Compiler Version 19.xx.xxxxx for x64
```

## After Installing Components

1. Return to the project folder:
```bash
cd <path_to_project>
```

2. Open the project in Visual Studio:
```bash
start vs.proj\EmojiDescription.vcxproj
```

3. Or build from command line:
   - Open "Developer Command Prompt for VS 2022"
   - Run:
   ```bash
   cd <path_to_project>
   msbuild vs.proj\EmojiDescription.vcxproj /p:Configuration=Release /p:Platform=x64
   ```

## Alternative: Using Build Tools Only

If you don't want to install the full Visual Studio, you can install only Build Tools:

1. Download: https://aka.ms/vs/17/release/vs_BuildTools.exe
2. Run the installer
3. Select "C++ build tools"
4. Install

Then use "Developer Command Prompt for VS 2022" to build the project.

## Troubleshooting

### "cl is not recognized as an internal or external command"

You're not in a Developer Command Prompt. Either:
- Use "Developer Command Prompt for VS 2022" from Start Menu
- Or run from PowerShell:
  ```powershell
  & "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1"
  ```

### "Cannot find vcvarsall.bat"

C++ components are not installed. Follow the installation steps above.

### MSBuild errors about missing props files

Visual Studio installation is incomplete. Try:
1. Open Visual Studio Installer
2. Click "Modify"
3. Ensure "Desktop development with C++" is checked
4. Click "Modify" again
5. Wait for installation to complete

## Required Components Summary

- **Visual Studio 2022** (Community, Professional, or Enterprise)
  - OR **Build Tools for Visual Studio 2022**
- **Workload**: Desktop development with C++
  - MSVC v143 compiler
  - Windows SDK (10 or 11)
  - C++ CMake tools (optional, for CMake builds)

## Useful Links

- Visual Studio Downloads: https://visualstudio.microsoft.com/downloads/
- Build Tools: https://aka.ms/vs/17/release/vs_BuildTools.exe
- VS Documentation: https://docs.microsoft.com/en-us/visualstudio/
