# notipdf 🎺📄

A minimalist PDF preprocessing tool for optimizing sheet music being used for e-ink devices.

## ✨ Motivation

notipdf is a small, focused tool to prepare sheet music PDFs for real-world use — especially on E-Ink devices like the PocketBook InkPad.

The core idea is simple:

Simplicity is key.

No heavy frameworks.
No bloated UI.
No cloud.

Just:

- open a PDF
- select what you need
- export clean, optimized pages

Built for practical use during rehearsals, performances, and marching situations.

## 🚀 Features

- 📄 Load and view PDFs (fast, lightweight)
- 🔍 Smooth pan & zoom (GLFW-based)
- 🖱️ Rectangle selection (single selection, moveable)
- 🔄 Page navigation
- 🔁 Rotation support (90° steps)
- 📤 Export cropped regions as optimized PDFs
- 🧾 Automatic meta.json generation (structured for library use)
- 🎯 Optimized output for E-Ink devices (e.g. 1404×1872 @ 300 DPI)

## 🧱 Dependencies

You need:

- C++17 compatible compiler
- CMake (≥ 3.28)
- MuPDF (installed via system)
- (optional) ImageMagick (magick) for PDF writing

### macOS (Homebrew)

```bash
brew install mupdf
brew install imagemagick # optional (sips is used by default)
```

### Linux (example: Ubuntu)

```bash
sudo apt libgumbo-dev libmujs-dev libharfbuzz-dev libopenjp2-7-dev libjpeg-dev libjbig2dec0-dev install libgl1-mesa-dev libglu1-mesa-dev libfreetype-dev libx11-dev libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev libmupdf-dev
sudo apt install imagemagick # required
```

## 🛠️ Build Instructions

### macOS 🍎

```bash
git clone <your-repo-url>
cd notipdf

cmake -B build
cmake --build build

./build/notipdf input.pdf
```

### Linux 🐧

```bash
git clone <your-repo-url>
cd notipdf

cmake -B build
cmake --build build

./build/notipdf input.pdf
```

## 🎮 Controls

### Navigation

- j / n → next page
- k / p → previous page
- r → rotate (counter-clockwise)
- f → fit to screen
- q → quit

### Interaction

- 🖱️ Left Mouse
- no selection → create selection
- selection exists → move selection
- Space → toggle mode (pan / select)
- Delete → remove selection

## Export

- e → open export dialog

Dialog fields:

- Book
- Piece number
- Title
- Part (instrument)

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

````bash
$XDG_CONFIG_HOME/notipdf/config.json
```A

Example:

```json
{
    "output_directory": "/Users/you/notipdf/library",
    "default_book": 0,
    "default_part": 0,
    "optimize_for_eink": true,
    "threshold_black_white": false,
    "pdf_backend": "sips"
}
````

### PDF backends

- "sips" (macOS default)
- "magick" (cross-platform via ImageMagick)

## 📚 Data Model

- Book → Piece → Part
- `meta.json` is written per piece
- Multiple parts per piece supported

Example:

```json
{
  "book": { "id": "marschbuch", "title": "Marschbuch" },
  "piece": { "number": 6, "id": "47er", "title": "47er" },
  "parts": [
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

notipdf is intentionally small.

If something feels complicated, it’s probably wrong.

## 🙌 Final Note

This tool was built for real musicians —
for situations where you don’t want to fight software,
but just play.
