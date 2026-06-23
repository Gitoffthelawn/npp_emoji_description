// Embedded Unicode character-name lookup.
//
// The full table of formal Unicode names is generated from the pinned Unicode
// Character Database by tools/unicode_names/gen_names.py into UnicodeNames.bin,
// which is embedded in the DLL as an RCDATA resource (see EmojiDescription.rc).
// This replaces the old getuname.dll lookup and covers the whole code space,
// including supplementary planes and emoji.

#define IDR_UNICODE_NAMES 1001

#ifndef RC_INVOKED

#include <cstdint>
#include <cstddef>

// Look up the formal Unicode name (e.g. "EM DASH", "GRINNING FACE") for a code
// point. Names for algorithmic ranges (CJK ideographs, Hangul syllables, Tangut
// ideographs) are derived by formula. Returns false when the code point has no
// formal name (unassigned, surrogate, Private Use, control). On success, output
// holds a NUL-terminated wide string (truncated to outputSize if needed).
bool lookupUnicodeName(uint32_t codepoint, wchar_t* output, size_t outputSize);

#endif // RC_INVOKED
