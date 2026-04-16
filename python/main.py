import argparse
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from pypdf import PdfReader, PdfWriter


@dataclass
class PieceEntry:
    number: int
    title: str
    pdf_path: Path


def load_json(path: Path) -> dict[str, Any]:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:
        raise RuntimeError(f"Fehler beim Lesen von {path}: {exc}") from exc


def folder_prefix_number(path: Path) -> int | None:
    """
    Extrahiert z. B. aus '001_da_kaiser...' die 1.
    """
    match = re.match(r"^(\d+)", path.name)
    if match:
        return int(match.group(1))
    return None


def collect_entries(book_dir: Path) -> list[PieceEntry]:
    entries: list[PieceEntry] = []

    for child in sorted(book_dir.iterdir()):
        if not child.is_dir():
            continue

        meta_file = child / "meta.json"
        if not meta_file.exists():
            continue

        data = load_json(meta_file)

        piece = data.get("piece", {})
        instruments = data.get("instruments", [])

        piece_title = piece.get("title")
        if not piece_title:
            print(f"Warnung: Kein piece.title in {meta_file}", file=sys.stderr)
            continue

        piece_number = piece.get("number")
        if not isinstance(piece_number, int):
            piece_number = folder_prefix_number(child)

        if piece_number is None:
            print(
                f"Warnung: Keine gueltige Nummer fuer {child.name} gefunden",
                file=sys.stderr,
            )
            continue

        selected_pdf: Path | None = None

        for inst in instruments:
            pdf_name = inst.get("pdf")

            candidate = child / pdf_name
            if not candidate.exists():
                print(
                    f"Warnung: PDF fehlt: {candidate}",
                    file=sys.stderr,
                )
                continue

            selected_pdf = candidate
            break

        if selected_pdf is None:
            # Dieses Stück gibt es für die gewünschte Stimme nicht
            continue

        entries.append(
            PieceEntry(
                number=piece_number,
                title=str(piece_title),
                pdf_path=selected_pdf,
            )
        )

    entries.sort(key=lambda e: (e.number, e.title.casefold()))
    return entries


def sanitize_filename(value: str) -> str:
    value = value.strip().replace(" ", "_")
    value = re.sub(r"[^A-Za-z0-9_\-]+", "", value)
    return value or "output"


def build_pdf(
    book_dir: Path,
    output_file: Path | None = None,
    top_bookmark_title: str | None = None,
) -> Path:
    entries = collect_entries(book_dir)

    # Optionalen Buchtitel aus einem meta.json lesen
    if top_bookmark_title is None:
        for child in sorted(book_dir.iterdir()):
            meta_file = child / "meta.json"
            if meta_file.exists():
                data = load_json(meta_file)
                top_bookmark_title = data.get("book", {}).get("title")
                if top_bookmark_title:
                    break

    writer = PdfWriter()
    current_page = 0

    parent_outline = None
    if top_bookmark_title:
        # Root-Bookmark fuer das ganze Buch
        parent_outline = writer.add_outline_item(str(top_bookmark_title), 0)

    for entry in entries:
        try:
            reader = PdfReader(str(entry.pdf_path))
        except Exception as exc:
            print(
                f"Warnung: Konnte PDF nicht lesen: {entry.pdf_path} ({exc})",
                file=sys.stderr,
            )
            continue

        start_page = current_page

        for page in reader.pages:
            writer.add_page(page)
            current_page += 1

        bookmark_title = f"{entry.number:03d} {entry.title}"
        try:
            if parent_outline is not None:
                writer.add_outline_item(
                    bookmark_title, start_page, parent=parent_outline
                )
            else:
                writer.add_outline_item(bookmark_title, start_page)
        except Exception as exc:
            print(
                f"Warnung: Konnte Bookmark fuer {entry.pdf_path.name} nicht setzen ({exc})",
                file=sys.stderr,
            )

    if len(writer.pages) == 0:
        raise RuntimeError("Es wurden keine PDF-Seiten hinzugefuegt.")

    if output_file is None:
        output_name = f"{sanitize_filename(book_dir.name)}.pdf"
        output_file = book_dir / output_name

    with output_file.open("wb") as f:
        writer.write(f)

    return output_file


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Erzeugt ein Marschbuch-PDF fuer eine bestimmte Stimme mit Bookmarks."
    )
    parser.add_argument(
        "book_dir",
        type=Path,
        help="Pfad zum Buch-Verzeichnis, das die Stueck-Unterordner enthaelt",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        help="Zieldatei fuer das fertige PDF",
    )
    parser.add_argument(
        "--no-book-root-bookmark",
        action="store_true",
        help="Kein oberstes Bookmark mit dem Buchtitel anlegen",
    )

    args = parser.parse_args()

    book_dir: Path = args.book_dir
    if not book_dir.exists() or not book_dir.is_dir():
        print(f"Fehler: Ungueltiges Buch-Verzeichnis: {book_dir}", file=sys.stderr)
        return 1

    try:
        output = build_pdf(
            book_dir=book_dir,
            output_file=args.output,
            top_bookmark_title=None if not args.no_book_root_bookmark else "",
        )
    except Exception as exc:
        print(f"Fehler: {exc}", file=sys.stderr)
        return 1

    print(f"Fertig: {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
