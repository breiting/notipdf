#!/usr/bin/env python3

import argparse
import json
import re
from pathlib import Path


def extract_number_from_dirname(dirname: str) -> int | None:
    match = re.match(r"^(\d{3})_", dirname)
    if not match:
        return None
    return int(match.group(1))


def update_meta_file(meta_path: Path, dry_run: bool) -> bool:
    piece_dir = meta_path.parent.name
    number = extract_number_from_dirname(piece_dir)

    if number is None:
        print(f"SKIP  {meta_path}  (directory does not start with 000_)")
        return False

    try:
        data = json.loads(meta_path.read_text(encoding="utf-8"))
    except Exception as exc:
        print(f"ERROR {meta_path}  ({exc})")
        return False

    if "piece" not in data or not isinstance(data["piec[?1;2;4ce"], dict):
        data["piece"] = {}

    old_number = data["piece"].get("number")
    data["piece"]["number"] = number

    if old_number == number:
        print(f"OK    {meta_path}  number already {number}")
        return False

    print(f"FIX   {meta_path}  {old_number} -> {number}")

    if not dry_run:
        meta_path.write_text(
            json.dumps(data, indent=4, ensure_ascii=False) + "\n",
            encoding="utf-8",
        )

    return True


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Fix piece.number in meta.json files based on directory prefix."
    )
    parser.add_argument(
        "library_root",
        type=Path,
        help="Root directory of the notipdf library.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Only print changes without writing files.",
    )

    args = parser.parse_args()

    root = args.library_root.expanduser().resolve()

    if not root.exists():
        print(f"ERROR root does not exist: {root}")
        return 1

    changed = 0

    for meta_path in sorted(root.rglob("meta.json")):
        if update_meta_file(meta_path, args.dry_run):
            changed += 1

    print()
    if args.dry_run:
        print(f"Dry run complete. Files that would change: {changed}")
    else:
        print(f"Done. Files changed: {changed}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
