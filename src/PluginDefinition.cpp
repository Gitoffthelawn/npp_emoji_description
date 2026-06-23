//this file is part of notepad++
//Copyright (C)2022 Don HO <don.h@free.fr>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "UnicodeNames.h"
#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <string>
#include <sstream>
#include <iomanip>

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Plugin state
//
static bool g_showCharInfo = true;

// Which fields are shown by default: code point, character name and HTML entity;
// decimal, hex and the UTF-8 byte sequence are off. The character name is now
// served from the embedded Unicode table (see UnicodeNames.cpp), so it is always
// available and shown by default.
// Order: { codePoint, name, decimal, hex, html, utf8 }.
static DisplayFields g_fields = { true, true, false, false, true, false };

// Full path to the plugin .ini, resolved once from the Notepad++ config dir.
static TCHAR g_iniPath[MAX_PATH] = { 0 };

//
// Initialize your plugin data here
// It will be called while plugin loading
void pluginInit(HANDLE /*hModule*/)
{
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Insert a plugin-menu separator (defined below).
static bool setSeparator(size_t index);

// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
    // Restore persisted settings before the menu check states are initialized.
    loadConfig();

    setCommand(0, TEXT("Show Character Info"),    toggleShowCharInfo,   NULL, g_showCharInfo);
    setSeparator(1);  // divide the master on/off toggle from the per-field options
    setCommand(2, TEXT("Show: Code Point (U+)"),  toggleFieldCodePoint, NULL, g_fields.codePoint);
    setCommand(3, TEXT("Show: Character Name"),   toggleFieldName,      NULL, g_fields.name);
    setCommand(4, TEXT("Show: Decimal"),          toggleFieldDecimal,   NULL, g_fields.decimal);
    setCommand(5, TEXT("Show: Hexadecimal"),      toggleFieldHex,       NULL, g_fields.hex);
    setCommand(6, TEXT("Show: HTML Entity"),      toggleFieldHtml,      NULL, g_fields.html);
    setCommand(7, TEXT("Show: UTF-8 Bytes"),      toggleFieldUtf8,      NULL, g_fields.utf8);
    setCommand(8, TEXT("About"),                  aboutDialog,          NULL, false);
}

//
// Resolve the .ini path inside the Notepad++ plugin config directory.
//
static void initConfigPath()
{
    TCHAR configDir[MAX_PATH] = { 0 };
    ::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)configDir);
    ::PathAppend(configDir, TEXT("EmojiDescription.ini"));
    _tcsncpy_s(g_iniPath, MAX_PATH, configDir, _TRUNCATE);
}

//
// Load persisted settings (falling back to defaults on first run).
//
void loadConfig()
{
    initConfigPath();
    g_showCharInfo     = ::GetPrivateProfileInt(TEXT("Display"), TEXT("Enabled"),   1, g_iniPath) != 0;
    g_fields.codePoint = ::GetPrivateProfileInt(TEXT("Fields"),  TEXT("CodePoint"), 1, g_iniPath) != 0;
    g_fields.name      = ::GetPrivateProfileInt(TEXT("Fields"),  TEXT("Name"),      1, g_iniPath) != 0;
    g_fields.decimal   = ::GetPrivateProfileInt(TEXT("Fields"),  TEXT("Decimal"),   0, g_iniPath) != 0;
    g_fields.hex       = ::GetPrivateProfileInt(TEXT("Fields"),  TEXT("Hex"),       0, g_iniPath) != 0;
    g_fields.html      = ::GetPrivateProfileInt(TEXT("Fields"),  TEXT("Html"),      1, g_iniPath) != 0;
    g_fields.utf8      = ::GetPrivateProfileInt(TEXT("Fields"),  TEXT("Utf8"),      0, g_iniPath) != 0;
}

//
// Persist the current settings.
//
void saveConfig()
{
    if (!g_iniPath[0])
        return;

    ::WritePrivateProfileString(TEXT("Display"), TEXT("Enabled"),   g_showCharInfo     ? TEXT("1") : TEXT("0"), g_iniPath);
    ::WritePrivateProfileString(TEXT("Fields"),  TEXT("CodePoint"), g_fields.codePoint ? TEXT("1") : TEXT("0"), g_iniPath);
    ::WritePrivateProfileString(TEXT("Fields"),  TEXT("Name"),      g_fields.name      ? TEXT("1") : TEXT("0"), g_iniPath);
    ::WritePrivateProfileString(TEXT("Fields"),  TEXT("Decimal"),   g_fields.decimal   ? TEXT("1") : TEXT("0"), g_iniPath);
    ::WritePrivateProfileString(TEXT("Fields"),  TEXT("Hex"),       g_fields.hex       ? TEXT("1") : TEXT("0"), g_iniPath);
    ::WritePrivateProfileString(TEXT("Fields"),  TEXT("Html"),      g_fields.html      ? TEXT("1") : TEXT("0"), g_iniPath);
    ::WritePrivateProfileString(TEXT("Fields"),  TEXT("Utf8"),      g_fields.utf8      ? TEXT("1") : TEXT("0"), g_iniPath);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
}

//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit)
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//
// Insert a separator line in the plugin menu at the given index. Notepad++
// renders a FuncItem with a NULL function pointer as a menu separator.
//
static bool setSeparator(size_t index)
{
    if (index >= nbFunc)
        return false;

    funcItem[index]._itemName[0] = TEXT('\0');
    funcItem[index]._pFunc = NULL;
    funcItem[index]._init2Check = false;
    funcItem[index]._pShKey = NULL;

    return true;
}

//
// Decode UTF-8 character at given position
//
uint32_t decodeUtf8Char(const unsigned char* text, int& bytesRead)
{
    bytesRead = 0;
    if (!text || !text[0])
        return 0;

    unsigned char firstByte = text[0];

    // 1-byte character (ASCII)
    if ((firstByte & 0x80) == 0) {
        bytesRead = 1;
        return firstByte;
    }

    // 2-byte character
    if ((firstByte & 0xE0) == 0xC0) {
        if ((text[1] & 0xC0) != 0x80)
            return 0;
        bytesRead = 2;
        return ((firstByte & 0x1F) << 6) | (text[1] & 0x3F);
    }

    // 3-byte character
    if ((firstByte & 0xF0) == 0xE0) {
        if ((text[1] & 0xC0) != 0x80 || (text[2] & 0xC0) != 0x80)
            return 0;
        bytesRead = 3;
        return ((firstByte & 0x0F) << 12) | ((text[1] & 0x3F) << 6) | (text[2] & 0x3F);
    }

    // 4-byte character (includes emoji)
    if ((firstByte & 0xF8) == 0xF0) {
        if ((text[1] & 0xC0) != 0x80 || (text[2] & 0xC0) != 0x80 || (text[3] & 0xC0) != 0x80)
            return 0;
        bytesRead = 4;
        return ((firstByte & 0x07) << 18) | ((text[1] & 0x3F) << 12) |
               ((text[2] & 0x3F) << 6) | (text[3] & 0x3F);
    }

    return 0;
}

//
// Look up the Unicode character name (e.g. "EM DASH", "GRINNING FACE") for a
// code point. Backed by the name table embedded in the DLL (see UnicodeNames.h
// / UnicodeNames.cpp), so it covers the entire Unicode code space -- including
// supplementary planes and emoji -- with no dependency on the OS. Returns false
// when the code point has no formal Unicode name.
//
bool getUnicodeName(uint32_t codepoint, TCHAR* output, size_t outputSize)
{
    return lookupUnicodeName(codepoint, output, outputSize);
}

//
// Append the UTF-8 byte sequence for a code point to the stream.
//
static void appendUtf8Bytes(std::wstringstream& ss, uint32_t codepoint)
{
    unsigned char bytes[4];
    int count;

    if (codepoint < 0x80) {
        bytes[0] = static_cast<unsigned char>(codepoint);
        count = 1;
    }
    else if (codepoint < 0x800) {
        bytes[0] = static_cast<unsigned char>(0xC0 | (codepoint >> 6));
        bytes[1] = static_cast<unsigned char>(0x80 | (codepoint & 0x3F));
        count = 2;
    }
    else if (codepoint < 0x10000) {
        bytes[0] = static_cast<unsigned char>(0xE0 | (codepoint >> 12));
        bytes[1] = static_cast<unsigned char>(0x80 | ((codepoint >> 6) & 0x3F));
        bytes[2] = static_cast<unsigned char>(0x80 | (codepoint & 0x3F));
        count = 3;
    }
    else {
        bytes[0] = static_cast<unsigned char>(0xF0 | (codepoint >> 18));
        bytes[1] = static_cast<unsigned char>(0x80 | ((codepoint >> 12) & 0x3F));
        bytes[2] = static_cast<unsigned char>(0x80 | ((codepoint >> 6) & 0x3F));
        bytes[3] = static_cast<unsigned char>(0x80 | (codepoint & 0x3F));
        count = 4;
    }

    for (int i = 0; i < count; ++i) {
        if (i > 0)
            ss << L" ";
        ss << L"0x" << std::uppercase << std::hex << std::setfill(L'0') << std::setw(2) << (int)bytes[i];
    }
}

//
// Format the enabled character-info fields for display.
//
void formatCharacterCodes(uint32_t codepoint, TCHAR* output, size_t outputSize)
{
    if (!codepoint) {
        _sntprintf_s(output, outputSize, _TRUNCATE, TEXT("No character"));
        return;
    }

    std::wstringstream ss;
    bool first = true;

    // Separator helper: " | " between fields, nothing before the first one.
    auto sep = [&]() {
        if (!first)
            ss << L" | ";
        first = false;
    };

    if (g_fields.codePoint) {
        sep();
        ss << L"U+" << std::uppercase << std::hex << std::setfill(L'0') << std::setw(4) << codepoint;
    }

    if (g_fields.name) {
        TCHAR nameBuf[256];
        if (getUnicodeName(codepoint, nameBuf, 256)) {
            sep();
            ss << nameBuf;
        }
    }

    if (g_fields.decimal) {
        sep();
        ss << L"Dec: " << std::dec << codepoint;
    }

    if (g_fields.hex) {
        sep();
        ss << L"Hex: 0x" << std::uppercase << std::hex << codepoint;
    }

    if (g_fields.html) {
        sep();
        ss << L"HTML: &#" << std::dec << codepoint << L";";
    }

    if (g_fields.utf8) {
        sep();
        ss << L"UTF-8: ";
        appendUtf8Bytes(ss, codepoint);
    }

    if (first) {
        // No fields enabled — give the user a hint rather than a blank bar.
        _sntprintf_s(output, outputSize, _TRUNCATE, TEXT("Emoji Description: no fields enabled (see Plugins menu)"));
        return;
    }

    std::wstring result = ss.str();
    _tcsncpy_s(output, outputSize, result.c_str(), _TRUNCATE);
}

//
// Update character information in status bar
//
void updateCharacterInfo()
{
    if (!g_showCharInfo)
        return;

    // Get current scintilla handle
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;

    HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    // Get current cursor position
    Sci_Position pos = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);

    // Get document length to check bounds
    Sci_Position docLength = ::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0);
    if (pos >= docLength) {
        ::SendMessage(nppData._nppHandle, NPPM_SETSTATUSBAR, STATUSBAR_DOC_TYPE, (LPARAM)TEXT("End of document"));
        return;
    }

    // Get text range (read up to 4 bytes for UTF-8)
    unsigned char buffer[5] = {0};
    Sci_TextRangeFull tr;
    tr.chrg.cpMin = pos;
    tr.chrg.cpMax = pos + 4;
    if (tr.chrg.cpMax > docLength)
        tr.chrg.cpMax = docLength;
    tr.lpstrText = (char*)buffer;

    ::SendMessage(curScintilla, SCI_GETTEXTRANGEFULL, 0, (LPARAM)&tr);

    // Decode UTF-8 character
    int bytesRead = 0;
    uint32_t codepoint = decodeUtf8Char(buffer, bytesRead);

    // Format output
    TCHAR statusText[512];
    formatCharacterCodes(codepoint, statusText, 512);

    // Display in status bar
    ::SendMessage(nppData._nppHandle, NPPM_SETSTATUSBAR, STATUSBAR_DOC_TYPE, (LPARAM)statusText);
}

//
// Notification handler
//
void pluginBeNotified(SCNotification *notifyCode)
{
    switch (notifyCode->nmhdr.code)
    {
        case SCN_UPDATEUI:
            // Update when cursor moves or selection changes
            if (notifyCode->updated & (SC_UPDATE_SELECTION | SC_UPDATE_CONTENT))
            {
                updateCharacterInfo();
            }
            break;

        case NPPN_BUFFERACTIVATED:
            // Update when switching between documents
            updateCharacterInfo();
            break;
    }
}

//
// Message handler
//
LRESULT pluginMessageProc(UINT /*Message*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return TRUE;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//

void toggleShowCharInfo()
{
    g_showCharInfo = !g_showCharInfo;

    // Update menu check state
    ::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[0]._cmdID, g_showCharInfo);
    saveConfig();

    if (g_showCharInfo) {
        updateCharacterInfo();
    } else {
        ::SendMessage(nppData._nppHandle, NPPM_SETSTATUSBAR, STATUSBAR_DOC_TYPE, (LPARAM)TEXT(""));
    }
}

//
// Flip a single field flag: update its menu check, persist, and refresh the bar.
//
static void toggleField(bool& flag, int menuIndex)
{
    flag = !flag;
    ::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[menuIndex]._cmdID, flag);
    saveConfig();
    if (g_showCharInfo)
        updateCharacterInfo();
}

// menuIndex values match the setCommand() indices in commandMenuInit(), which
// are offset by the separator at index 1.
void toggleFieldCodePoint() { toggleField(g_fields.codePoint, 2); }
void toggleFieldName()      { toggleField(g_fields.name,      3); }
void toggleFieldDecimal()   { toggleField(g_fields.decimal,   4); }
void toggleFieldHex()       { toggleField(g_fields.hex,       5); }
void toggleFieldHtml()      { toggleField(g_fields.html,      6); }
void toggleFieldUtf8()      { toggleField(g_fields.utf8,      7); }

void aboutDialog()
{
    ::MessageBox(NULL,
        TEXT("Emoji Description v0.3.15\n\n")
        TEXT("Displays detailed character encoding information in the status bar.\n\n")
        TEXT("Shows for any character:\n")
        TEXT("- Unicode code point (U+XXXX)\n")
        TEXT("- Unicode character name (e.g. EM DASH)\n")
        TEXT("- Decimal and Hexadecimal values\n")
        TEXT("- HTML entity (&#XXXX;)\n")
        TEXT("- UTF-8 byte sequence\n\n")
        TEXT("Each field can be toggled from the Plugins menu.\n")
        TEXT("Supports all Unicode characters including emoji!\n\n")
        TEXT("https://github.com/Ruberoid/npp_emoji_description"),
        TEXT("About Emoji Description"),
        MB_OK | MB_ICONINFORMATION);
}
