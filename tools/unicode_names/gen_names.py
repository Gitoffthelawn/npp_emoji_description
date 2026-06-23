#!/usr/bin/env python3
# Generate src/UnicodeNames.bin from the pinned Unicode Character Database.
#
# The blob holds the formal Unicode name for every *individually named* code
# point in UnicodeData.txt. Algorithmically named ranges (CJK Unified
# Ideographs, Hangul Syllables, Tangut Ideographs) are NOT stored -- the
# runtime in src/UnicodeNames.cpp derives those names by formula, which is why
# the table stays small while still covering the whole of Unicode. Code points
# with no formal name (surrogates, Private Use) are intentionally absent.
#
# Format and lookup are documented in src/UnicodeNames.cpp. Names obey the
# Unicode Name Stability Policy (existing names never change), so bumping the
# pinned UCD version only ever adds rows.
#
# Usage:
#   python3 gen_names.py [UnicodeData.txt] [out.bin]
# Defaults: ./UnicodeData.txt -> ../../src/UnicodeNames.bin

import collections
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

# Algorithmic name kinds emitted into the range table. The runtime derives the
# name from the code point for these; the members are NOT stored individually.
KIND_CJK = 0      # "CJK UNIFIED IDEOGRAPH-<hex>"
KIND_TANGUT = 1   # "TANGUT IDEOGRAPH-<hex>"
KIND_HANGUL = 2   # "HANGUL SYLLABLE <jamo>"


def classify_range(marker):
    # marker is the "<...>" text without the trailing ", First"/", Last".
    if "CJK Ideograph" in marker:
        return KIND_CJK
    if "Tangut Ideograph" in marker:
        return KIND_TANGUT
    if "Hangul Syllable" in marker:
        return KIND_HANGUL
    # Surrogates and Private Use have no formal names -> not emitted.
    return None


# Parse UnicodeData.txt into:
#   entries -- individually named code points (formal Name property)
#   ranges  -- algorithmically named "<..., First>".."<..., Last>" blocks
# Marker rows and any other "<...>" pseudo-name (controls, Private Use,
# surrogates) carry no formal name and are not stored as entries.
def load_entries(path):
    entries = []
    ranges = []
    pending = None  # (kind, first_cp) awaiting its matching ", Last>"
    with open(path, "r", encoding="ascii") as f:
        for line in f:
            if not line.strip():
                continue
            fields = line.split(";")
            cp = int(fields[0], 16)
            name = fields[1]
            if name.endswith(", First>"):
                pending = (classify_range(name[1:-len(", First>")]), cp)
                continue
            if name.endswith(", Last>"):
                kind, first = pending
                pending = None
                if kind is not None:
                    ranges.append((first, cp, kind))
                continue
            if name.startswith("<"):
                # "<control>", "<Private Use>", ... -- no formal name.
                continue
            entries.append((cp, name))
    entries.sort(key=lambda e: e[0])
    ranges.sort(key=lambda r: r[0])
    return entries, ranges


def varint(value):
    out = bytearray()
    while True:
        b = value & 0x7F
        value >>= 7
        if value:
            out.append(b | 0x80)
        else:
            out.append(b)
            return out


def u24(value):
    if value > 0xFFFFFF:
        raise ValueError("value does not fit in 24 bits: %d" % value)
    return struct.pack("<I", value)[:3]


def build(entries, ranges):
    # Order the token dictionary by descending frequency so the most common
    # words ("LETTER", "WITH", "LATIN", ...) get the smallest ids and thus the
    # shortest varints in the name stream.
    freq = collections.Counter()
    for _, name in entries:
        freq.update(name.split(" "))
    tokens = [t for t, _ in freq.most_common()]
    tok_id = {t: i for i, t in enumerate(tokens)}

    pool = bytearray()
    for t in tokens:
        pool += t.encode("ascii")
        pool.append(0)

    codepoints = bytearray()
    offsets = bytearray()
    stream = bytearray()
    for cp, name in entries:
        codepoints += u24(cp)
        offsets += u24(len(stream))
        for t in name.split(" "):
            stream += varint(tok_id[t])

    range_tbl = bytearray()
    for first, last, kind in ranges:
        range_tbl += u24(first) + u24(last) + struct.pack("<B", kind)

    # Header: magic, format version, range count, token count, pool size,
    # entry count, name-stream size. Sections then follow in this order:
    #   ranges(u24,u24,u8) | token pool | codepoints(u24) | offsets(u24) | stream
    header = bytearray()
    header += b"UNAM"
    header += struct.pack("<BBH", 1, 0, len(ranges))  # formatVersion=1, reserved, rangeCount
    header += struct.pack("<I", len(tokens))
    header += struct.pack("<I", len(pool))
    header += struct.pack("<I", len(entries))
    header += struct.pack("<I", len(stream))

    return (bytes(header) + bytes(range_tbl) + bytes(pool)
            + bytes(codepoints) + bytes(offsets) + bytes(stream))


def main():
    src = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "UnicodeData.txt")
    out = sys.argv[2] if len(sys.argv) > 2 else os.path.join(HERE, "..", "..", "src", "UnicodeNames.bin")
    out = os.path.abspath(out)

    entries, ranges = load_entries(src)
    blob = build(entries, ranges)
    with open(out, "wb") as f:
        f.write(blob)
    print("wrote %s: %d entries, %d ranges, %d bytes (%.1f KB)"
          % (out, len(entries), len(ranges), len(blob), len(blob) / 1024.0))


if __name__ == "__main__":
    main()
