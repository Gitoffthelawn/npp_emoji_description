// Embedded Unicode character-name lookup. See UnicodeNames.h.
//
// The lookup data lives in the RCDATA resource IDR_UNICODE_NAMES, produced by
// tools/unicode_names/gen_names.py. Blob layout (all little-endian):
//
//   Header (24 bytes):
//     char     magic[4]      = "UNAM"
//     uint8_t  formatVersion = 1
//     uint8_t  reserved
//     uint16_t rangeCount
//     uint32_t tokenCount
//     uint32_t poolSize       (bytes of the token pool)
//     uint32_t entryCount
//     uint32_t streamSize     (bytes of the name stream)
//   Sections, in order:
//     ranges[rangeCount]   : { u24 first, u24 last, u8 kind }   (7 bytes each)
//     tokenPool[poolSize]  : tokenCount NUL-terminated ASCII words, id order
//     codepoints[entryCount] : u24, sorted ascending (binary-search key)
//     offsets[entryCount]    : u24 byte offset of entry i into the name stream
//     stream[streamSize]     : per entry, LEB128 token ids; entry i spans
//                              [offsets[i], offsets[i+1]) (last ends at streamSize)
//
// A name is the entry's token ids decoded to words and joined with spaces.
// Algorithmic ranges (CJK / Tangut ideographs, Hangul syllables) are not stored
// per code point; their names are derived by formula from the range table.

#include "UnicodeNames.h"

#ifndef UNICODENAMES_TEST
#include <windows.h>
#endif
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>

namespace {

uint32_t rd24(const unsigned char* p)
{
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16);
}

uint32_t rd32(const unsigned char* p)
{
    return rd24(p) | (static_cast<uint32_t>(p[3]) << 24);
}

// Algorithmic name kinds, matching tools/unicode_names/gen_names.py.
enum { KIND_CJK = 0, KIND_TANGUT = 1, KIND_HANGUL = 2 };

struct NameTable
{
    bool ok = false;
    uint16_t rangeCount = 0;
    uint32_t entryCount = 0;
    uint32_t streamSize = 0;
    const unsigned char* ranges = nullptr;      // rangeCount * 7 bytes
    const unsigned char* codepoints = nullptr;  // entryCount * 3 bytes
    const unsigned char* offsets = nullptr;     // entryCount * 3 bytes
    const unsigned char* stream = nullptr;      // streamSize bytes
    std::vector<const char*> tokens;            // tokenCount pointers into the pool
};

NameTable g_table;
bool g_loaded = false;

// Parse and validate a name-table blob, populating g_table. Shared by the
// resource loader and the offline test harness (see tools/unicode_names).
bool parseTable(const unsigned char* data, size_t size)
{
    if (!data || size < 24 || std::memcmp(data, "UNAM", 4) != 0 || data[4] != 1)
        return false;

    uint16_t rangeCount = static_cast<uint16_t>(data[6] | (data[7] << 8));
    uint32_t tokenCount = rd32(data + 8);
    uint32_t poolSize   = rd32(data + 12);
    uint32_t entryCount = rd32(data + 16);
    uint32_t streamSize = rd32(data + 20);

    // Validate every section fits inside the resource before trusting offsets.
    uint64_t need = 24;
    uint64_t rangesOff = need;            need += static_cast<uint64_t>(rangeCount) * 7;
    uint64_t poolOff   = need;            need += poolSize;
    uint64_t cpOff     = need;            need += static_cast<uint64_t>(entryCount) * 3;
    uint64_t offOff    = need;            need += static_cast<uint64_t>(entryCount) * 3;
    uint64_t streamOff = need;            need += streamSize;
    if (need > size)
        return false;

    // Build the id -> word index by scanning the NUL-terminated pool.
    const char* pool = reinterpret_cast<const char*>(data + poolOff);
    g_table.tokens.reserve(tokenCount);
    uint32_t pos = 0;
    for (uint32_t i = 0; i < tokenCount && pos < poolSize; ++i) {
        g_table.tokens.push_back(pool + pos);
        while (pos < poolSize && pool[pos] != '\0')
            ++pos;
        ++pos;  // skip the NUL
    }
    if (g_table.tokens.size() != tokenCount)
        return false;

    g_table.rangeCount = rangeCount;
    g_table.entryCount = entryCount;
    g_table.streamSize = streamSize;
    g_table.ranges     = data + rangesOff;
    g_table.codepoints = data + cpOff;
    g_table.offsets    = data + offOff;
    g_table.stream     = data + streamOff;
    g_table.ok = true;
    return true;
}

#ifndef UNICODENAMES_TEST
void loadTable()
{
    g_loaded = true;

    // Resolve our own module so we read the resource from this DLL, not the host.
    HMODULE self = nullptr;
    if (!::GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&loadTable), &self) || !self)
        return;

    HRSRC res = ::FindResource(self, MAKEINTRESOURCE(IDR_UNICODE_NAMES), RT_RCDATA);
    if (!res)
        return;
    HGLOBAL handle = ::LoadResource(self, res);
    if (!handle)
        return;
    const unsigned char* data = static_cast<const unsigned char*>(::LockResource(handle));
    DWORD size = ::SizeofResource(self, res);
    parseTable(data, size);
}
#else
// In the offline test harness the blob is injected via parseTable() directly.
void loadTable() {}
#endif // UNICODENAMES_TEST

void copyOut(const std::wstring& name, wchar_t* output, size_t outputSize)
{
    ::wcsncpy_s(output, outputSize, name.c_str(), _TRUNCATE);
}

// Build "HANGUL SYLLABLE <jamo>" for a precomposed syllable (Unicode 3.12).
bool hangulName(uint32_t cp, uint32_t base, wchar_t* output, size_t outputSize)
{
    static const wchar_t* kL[] = {
        L"G", L"GG", L"N", L"D", L"DD", L"R", L"M", L"B", L"BB", L"S",
        L"SS", L"", L"J", L"JJ", L"C", L"K", L"T", L"P", L"H" };
    static const wchar_t* kV[] = {
        L"A", L"AE", L"YA", L"YAE", L"EO", L"E", L"YEO", L"YE", L"O", L"WA",
        L"WAE", L"OE", L"YO", L"U", L"WEO", L"WE", L"WI", L"YU", L"EU", L"YI", L"I" };
    static const wchar_t* kT[] = {
        L"", L"G", L"GG", L"GS", L"N", L"NJ", L"NH", L"D", L"L", L"LG", L"LM",
        L"LB", L"LS", L"LT", L"LP", L"LH", L"M", L"B", L"BS", L"S", L"SS", L"NG",
        L"J", L"C", L"K", L"T", L"P", L"H" };
    const uint32_t kVCount = 21, kTCount = 28, kNCount = kVCount * kTCount;

    uint32_t s = cp - base;
    std::wstring name = L"HANGUL SYLLABLE ";
    name += kL[s / kNCount];
    name += kV[(s % kNCount) / kTCount];
    name += kT[s % kTCount];
    copyOut(name, output, outputSize);
    return true;
}

// Names for algorithmically named ranges (CJK/Tangut ideographs, Hangul).
bool algorithmicName(uint32_t cp, wchar_t* output, size_t outputSize)
{
    for (uint16_t i = 0; i < g_table.rangeCount; ++i) {
        const unsigned char* r = g_table.ranges + static_cast<size_t>(i) * 7;
        uint32_t first = rd24(r);
        uint32_t last  = rd24(r + 3);
        if (cp < first || cp > last)
            continue;
        switch (r[6]) {
            case KIND_CJK:
                _snwprintf_s(output, outputSize, _TRUNCATE, L"CJK UNIFIED IDEOGRAPH-%04X", cp);
                return true;
            case KIND_TANGUT:
                _snwprintf_s(output, outputSize, _TRUNCATE, L"TANGUT IDEOGRAPH-%04X", cp);
                return true;
            case KIND_HANGUL:
                return hangulName(cp, first, output, outputSize);
            default:
                return false;
        }
    }
    return false;
}

// Find the entry index whose code point equals cp, or -1.
int findEntry(uint32_t cp)
{
    uint32_t lo = 0, hi = g_table.entryCount;
    while (lo < hi) {
        uint32_t mid = lo + (hi - lo) / 2;
        uint32_t v = rd24(g_table.codepoints + static_cast<size_t>(mid) * 3);
        if (v < cp)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (lo < g_table.entryCount &&
        rd24(g_table.codepoints + static_cast<size_t>(lo) * 3) == cp)
        return static_cast<int>(lo);
    return -1;
}

// Decode the name stored at table entry `index` (token ids -> spaced words).
void decodeEntry(int index, wchar_t* output, size_t outputSize)
{
    uint32_t start = rd24(g_table.offsets + static_cast<size_t>(index) * 3);
    uint32_t end = (static_cast<uint32_t>(index) + 1 < g_table.entryCount)
                       ? rd24(g_table.offsets + (static_cast<size_t>(index) + 1) * 3)
                       : g_table.streamSize;

    std::wstring name;
    const unsigned char* p = g_table.stream + start;
    const unsigned char* e = g_table.stream + end;
    bool first = true;
    while (p < e) {
        uint32_t id = 0;
        int shift = 0;
        while (p < e) {
            unsigned char b = *p++;
            id |= static_cast<uint32_t>(b & 0x7F) << shift;
            if (!(b & 0x80))
                break;
            shift += 7;
        }
        if (id >= g_table.tokens.size())
            break;
        if (!first)
            name += L' ';
        first = false;
        for (const char* t = g_table.tokens[id]; *t; ++t)
            name += static_cast<wchar_t>(static_cast<unsigned char>(*t));
    }
    copyOut(name, output, outputSize);
}

} // namespace

bool lookupUnicodeName(uint32_t codepoint, wchar_t* output, size_t outputSize)
{
    if (!output || outputSize == 0)
        return false;
    if (!g_loaded)
        loadTable();
    if (!g_table.ok)
        return false;

    if (algorithmicName(codepoint, output, outputSize))
        return true;

    int index = findEntry(codepoint);
    if (index < 0)
        return false;
    decodeEntry(index, output, outputSize);
    return true;
}
