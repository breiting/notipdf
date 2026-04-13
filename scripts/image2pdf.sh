#!/bin/sh

magick input.jpg \
  -colorspace Gray \
  -contrast-stretch 0x20% \
  -resize 1404x1872 \
  -density 300 \
  output.pdf
