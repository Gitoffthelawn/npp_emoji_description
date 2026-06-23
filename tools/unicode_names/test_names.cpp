// Offline verification for the embedded Unicode name table.
//
// Includes src/UnicodeNames.cpp with UNICODENAMES_TEST defined (so the Windows
// resource loader is replaced by direct blob injection), loads the generated
// UnicodeNames.bin, and checks every individually named code point against the
// source UnicodeData.txt plus spot checks for the algorithmic ranges.
//
// Build & run (Linux/macOS, no Windows needed):
//   c++ -std=c++17 -DUNICODENAMES_TEST -I../../src test_names.cpp -o /tmp/unt && \
//     /tmp/unt UnicodeData.txt ../../src/UnicodeNames.bin

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <string>
#include <vector>

// --- CRT shims so the production code compiles off-Windows ------------------
#define _TRUNCATE ((size_t)-1)
static void unt_copy(wchar_t* d, size_t n, const wchar_t* s)
{
    if (!n) return;
    size_t i = 0;
    for (; s[i] && i + 1 < n; ++i) d[i] = s[i];
    d[i] = 0;
}
#define wcsncpy_s(d, n, s, t) unt_copy((d), (n), (s))
#define _snwprintf_s(b, n, t, ...) swprintf((b), (n), __VA_ARGS__)

#include "UnicodeNames.cpp"

static std::wstring widen(const std::string& s)
{
    std::wstring w;
    for (unsigned char c : s) w += static_cast<wchar_t>(c);
    return w;
}

static int g_fail = 0;
static void expect(uint32_t cp, bool got, const std::wstring& gotName, bool wantOk, const std::wstring& want)
{
    if (got != wantOk || (got && gotName != want)) {
        wprintf(L"FAIL U+%04X: got=%d \"%ls\"  want=%d \"%ls\"\n",
                cp, (int)got, gotName.c_str(), (int)wantOk, want.c_str());
        if (++g_fail > 40) { wprintf(L"...too many failures\n"); }
    }
}

static bool lookup(uint32_t cp, std::wstring& out)
{
    wchar_t buf[256];
    bool ok = lookupUnicodeName(cp, buf, 256);
    out = ok ? buf : L"";
    return ok;
}

int main(int argc, char** argv)
{
    const char* ucd = argc > 1 ? argv[1] : "UnicodeData.txt";
    const char* bin = argc > 2 ? argv[2] : "../../src/UnicodeNames.bin";

    FILE* f = fopen(bin, "rb");
    if (!f) { fprintf(stderr, "cannot open %s\n", bin); return 2; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    std::vector<unsigned char> blob(sz);
    if (fread(blob.data(), 1, sz, f) != (size_t)sz) { fprintf(stderr, "read error\n"); return 2; }
    fclose(f);

    if (!parseTable(blob.data(), blob.size())) {
        fprintf(stderr, "parseTable failed\n");
        return 2;
    }

    // Verify every individually named entry round-trips to its UCD name.
    FILE* u = fopen(ucd, "r");
    if (!u) { fprintf(stderr, "cannot open %s\n", ucd); return 2; }
    char line[512];
    long checked = 0;
    while (fgets(line, sizeof line, u)) {
        char* semi = strchr(line, ';');
        if (!semi) continue;
        *semi = 0;
        char* name = semi + 1;
        char* semi2 = strchr(name, ';');
        if (semi2) *semi2 = 0;
        if (name[0] == '<') continue;  // ranges / pseudo-names
        uint32_t cp = (uint32_t)strtoul(line, nullptr, 16);
        std::wstring got;
        bool ok = lookup(cp, got);
        expect(cp, ok, got, true, widen(name));
        ++checked;
    }
    fclose(u);

    // Algorithmic ranges and negative cases.
    struct { uint32_t cp; bool ok; const wchar_t* name; } spot[] = {
        { 0x4E00,  true,  L"CJK UNIFIED IDEOGRAPH-4E00" },
        { 0x3400,  true,  L"CJK UNIFIED IDEOGRAPH-3400" },
        { 0x20000, true,  L"CJK UNIFIED IDEOGRAPH-20000" },
        { 0x2EBF0, true,  L"CJK UNIFIED IDEOGRAPH-2EBF0" },
        { 0x17000, true,  L"TANGUT IDEOGRAPH-17000" },
        { 0x18D00, true,  L"TANGUT IDEOGRAPH-18D00" },
        { 0xAC00,  true,  L"HANGUL SYLLABLE GA" },
        { 0xAC01,  true,  L"HANGUL SYLLABLE GAG" },
        { 0xD7A3,  true,  L"HANGUL SYLLABLE HIH" },
        { 0x1F600, true,  L"GRINNING FACE" },        // 4-byte emoji, table entry
        { 0xE000,  false, L"" },                       // Private Use, no name
        { 0xD800,  false, L"" },                       // surrogate, no name
        { 0x0378,  false, L"" },                       // unassigned
    };
    for (auto& s : spot) {
        std::wstring got;
        bool ok = lookup(s.cp, got);
        expect(s.cp, ok, got, s.ok, s.name);
    }

    wprintf(L"checked %ld table entries + %zu spot cases; failures=%d\n",
            checked, sizeof(spot) / sizeof(spot[0]), g_fail);
    return g_fail ? 1 : 0;
}
