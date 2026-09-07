#!/usr/bin/env python3
"""Check Realgam map dialogue line width against the field textbox."""

import argparse
import re
import sys
from pathlib import Path


DEFAULT_MAX_PX = 216
DEFAULT_WARN_PX = 170


def read_normal_font_widths(root):
    fonts_c = (root / "src" / "fonts.c").read_text(encoding="utf-8")
    match = re.search(
        r"gFontNormalLatinGlyphWidths\[\]\s*=\s*\{(?P<body>.*?)\};",
        fonts_c,
        re.S,
    )
    if match is None:
        raise RuntimeError("Could not find gFontNormalLatinGlyphWidths in src/fonts.c")
    return [int(value) for value in re.findall(r"\d+", match.group("body"))]


def read_charmap(root):
    mapping = {}
    charmap = (root / "charmap.txt").read_text(encoding="utf-8")
    for line in charmap.splitlines():
        match = re.match(r"'(.+)'\s*=\s*([0-9A-F]{2})\b", line)
        if match is not None and len(match.group(1)) == 1:
            mapping[match.group(1)] = int(match.group(2), 16)
    return mapping


PLACEHOLDER_WIDTHS = {
    "PLAYER": "PLAYER",
    "STR_VAR_1": "999999",
    "STR_VAR_2": "999999",
    "STR_VAR_3": "999999",
}


def string_width(text, charmap, widths):
    width = 0
    index = 0
    length = len(text)
    while index < length:
        char = text[index]
        if char == "{":
            end = text.find("}", index + 1)
            if end != -1:
                placeholder = text[index + 1 : end]
                width += string_width(
                    PLACEHOLDER_WIDTHS.get(placeholder, "PLAYER"),
                    charmap,
                    widths,
                )
                index = end + 1
                continue
        glyph = charmap.get(char)
        width += widths[glyph] if glyph is not None and glyph < len(widths) else 6
        index += 1
    return width


def split_dialogue_line(text):
    parts = []
    current = []
    i = 0
    while i < len(text):
        char = text[i]
        if char == "\\" and i + 1 < len(text) and text[i + 1] in "nlp":
            parts.append(("".join(current), text[i + 1]))
            current = []
            i += 2
            continue
        if char == "$":
            parts.append(("".join(current), "$"))
            current = []
            i += 1
            continue
        current.append(char)
        i += 1
    if current:
        parts.append(("".join(current), None))
    return parts


def first_word(text):
    stripped = text.lstrip()
    if not stripped:
        return ""
    return stripped.split(" ", 1)[0]


def extract_dialogue(path):
    label = None
    for line_no, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        label_match = re.match(r"([A-Za-z0-9_]+):", line)
        if label_match is not None:
            label = label_match.group(1)
            continue

        string_match = re.search(r'\.string\s+"(.*)"', line)
        if label is not None and string_match is not None:
            for text, separator in split_dialogue_line(string_match.group(1)):
                if text:
                    yield label, line_no, text, separator


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path.cwd())
    parser.add_argument("--max-px", type=int, default=DEFAULT_MAX_PX)
    parser.add_argument("--warn-px", type=int, default=DEFAULT_WARN_PX)
    args = parser.parse_args()

    root = args.root.resolve()
    widths = read_normal_font_widths(root)
    charmap = read_charmap(root)
    scripts = sorted((root / "data" / "maps").glob("REALGAM_*/scripts.inc"))

    problems = []
    for script in scripts:
        lines = list(extract_dialogue(script))
        for index, (label, line_no, text, separator) in enumerate(lines):
            width = string_width(text, charmap, widths)
            relpath = script.relative_to(root)
            if width > args.max_px:
                problems.append(
                    f"{relpath}:{line_no}: over {args.max_px}px ({width}px): {label}: {text}"
                )

            if separator in ("n", "l") and index + 1 < len(lines):
                next_label, _, next_text, _ = lines[index + 1]
                word = first_word(next_text)
                if next_label == label and word:
                    candidate = f"{text} {word}"
                    candidate_width = string_width(candidate, charmap, widths)
                    if width < args.warn_px and candidate_width <= args.max_px:
                        problems.append(
                            f"{relpath}:{line_no}: underfilled ({width}px, can fit '{word}'): "
                            f"{label}: {text}"
                        )

    if problems:
        print("Realgam dialogue width issues:")
        for problem in problems:
            print(problem)
        return 1

    print(f"Checked {len(scripts)} Realgam script files; dialogue widths look good.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
