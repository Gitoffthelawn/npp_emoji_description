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

#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

//
// All difinitions of plugin interface
//
#include "PluginInterface.h"

//-------------------------------------//
//-- STEP 1. DEFINE YOUR PLUGIN NAME --//
//-------------------------------------//
// Here define your plugin name
//
const TCHAR NPP_PLUGIN_NAME[] = TEXT("Emoji Description");

//-----------------------------------------------//
//-- STEP 2. DEFINE YOUR PLUGIN COMMAND NUMBER --//
//-----------------------------------------------//
//
// Here define the number of your plugin commands
//
const int nbFunc = 9;

//
// Which information fields are shown in the status bar.
// Toggled individually from the plugin menu and persisted to the plugin config.
//
struct DisplayFields
{
    bool codePoint;   // U+XXXX
    bool name;        // Unicode character name (e.g. EM DASH)
    bool decimal;     // Dec: NNN
    bool hex;         // Hex: 0xNN
    bool html;        // HTML: &#NNN;
    bool utf8;        // UTF-8: 0x.. byte sequence
};


//
// Initialization of your plugin data
// It will be called while plugin loading
//
void pluginInit(HANDLE hModule);

//
// Cleaning of your plugin
// It will be called while plugin unloading
//
void pluginCleanUp();

//
//Initialization of your plugin commands
//
void commandMenuInit();

//
//Clean up your plugin commands allocation (if any)
//
void commandMenuCleanUp();

//
// Function which sets your command
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false);


//
// Your plugin command functions
//
void toggleShowCharInfo();
void toggleFieldCodePoint();
void toggleFieldName();
void toggleFieldDecimal();
void toggleFieldHex();
void toggleFieldHtml();
void toggleFieldUtf8();
void aboutDialog();

//
// Notification handler
//
void pluginBeNotified(SCNotification *notifyCode);

//
// Message handler
//
LRESULT pluginMessageProc(UINT Message, WPARAM wParam, LPARAM lParam);

//
// Helper functions
//
void updateCharacterInfo();
uint32_t decodeUtf8Char(const unsigned char* text, int& bytesRead);
void formatCharacterCodes(uint32_t codepoint, TCHAR* output, size_t outputSize);
bool getUnicodeName(uint32_t codepoint, TCHAR* output, size_t outputSize);

//
// Configuration persistence (plugin config dir .ini)
//
void loadConfig();
void saveConfig();

#endif //PLUGINDEFINITION_H
