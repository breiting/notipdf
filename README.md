# notipdf 🎺📄

A minimalist PDF preprocessing tool for optimizing sheet music being used for e-ink devices.

## ✨ Motivation

**notipdf** is a small, focused tool to prepare sheet music PDFs for real-world use — especially on E-Ink devices like the PocketBook InkPad.

The core idea is simple:

> Simplicity is key.

No heavy frameworks.
No bloated UI.
No cloud.

Just:

- open a PDF
- select what you need
- export clean, optimized pages

## 🚀 Features

- 📄 Load and view PDFs (fast, lightweight)
- 🔍 Smooth pan & zoom (GLFW-based)
- 🖱️ Rectangle selection (single selection, moveable)
- 🔄 Page navigation (j/k or n/p)
- 🔁 Rotation support (90° steps)
- 📤 Export cropped regions as optimized PDFs
- 🧾 Automatic meta.json generation (structured for library use)
- 🎯 Optimized output for E-Ink devices (e.g. 1404×1872 @ 300 DPI)

## 🧱 Dependencies

You need:

- C++17 compatible compiler
- CMake (≥ 3.28)
- MuPDF (installed via system)
- (optional) ImageMagick for PDF writing

### macOS

Use Homebrew to install the required package(s).

```bash
brew install llvm cmake mupdf
```

### Linux

```bash
sudo apt libgumbo-dev libmujs-dev libharfbuzz-dev libopenjp2-7-dev libjpeg-dev libjbig2dec0-dev install libgl1-mesa-dev libglu1-mesa-dev libfreetype-dev libx11-dev libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev libmupdf-dev imagemagick
```

## 🛠️ Build Instructions

First, clone the repository to your local machine.

```bash
git clone https://github.com/breiting/notipdf
```

then build it:

```bash
cd notipdf

cmake -B build
cmake --build build
```

finally start the GUI application with a sample PDF file:

```bash
./build/notipdf input.pdf
```

## 🎮 Controls

- `j / n` → next page
- `k / p` → previous page
- `r` → rotate (counter-clockwise)
- `f` → fit to screen
- `q` → quit

### Workflow

Use the `Spacebar` to toggle between navigation and selection. Before starting the selection you can lock the aspect ratio using the `l` key (currently optimized for InkPad 4).

Make sure to switch to the selection mode and then use the mouse to drag a rectangle, selecting the area for export. If a selection is present, the mouse is used to move the selection box.

If you want to delete the selection use the `Delete` or `Backspace` key.

### Export

- `e` → open export dialog

Dialog fields:

- Book
- Piece number
- Title
- Author
- Instrument

Result:

```
library/
  marschbuch/
    006_47er/
      meta.json
      trompete_2.pdf
```

## ⚙️ Configuration

Config file location:

```bash
$XDG_CONFIG_HOME/notipdf/config.json
```

Example:

```json
{
  "output_directory": "/Users/you/notipdf/library",
  "default_book": 0,
  "default_instrument": 0,
  "optimize_for_eink": true,
  "threshold_black_white": false,
  "pdf_backend": "sips"
}
```

### PDF backends

- "sips" (macOS default)
- "magick" (cross-platform via ImageMagick)

## 📚 Data Model

- Book → Piece → Instrument
- `meta.json` is written per piece
- Multiple instruments per piece supported

Example:

```json
{
  "book": { "id": "marschbuch", "title": "Marschbuch" },
  "piece": { "number": 6, "id": "47er", "title": "47er" },
  "instruments": [
    { "id": "trompete_2", "title": "2. Trompete", "pdf": "trompete_2.pdf" }
  ]
}
```

## 📦 Libraries Used

- GLFW → window + input
- OpenGL → rendering
- MuPDF → PDF rendering
- Dear ImGui → minimal dialogs
- nlohmann/json → JSON handling

## 📄 License

MIT License

## 🎯 Philosophy

`notipdf` is intentionally small.

If something feels complicated, it’s probably wrong.

## 🙌 Final Note

Feel free to fork the repository and change it to your needs.
