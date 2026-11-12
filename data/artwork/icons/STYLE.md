# Icon style guide

Binding rules for the vector icons in this directory. They are redraws of the bitmap icons in
`resources/icons`, `plugins/*/icons` and `sdk/plugin_interface`, not traced pixel copies.

## Pixel icons (16, 22 and 32 px)

1. **viewBox = native bitmap size**: a 22×22 bitmap becomes `viewBox="0 0 22 22" width="22" height="22"`,
   a 16×16 bitmap `viewBox="0 0 16 16"`. Never rescale between grids; a 16 px icon is drawn on its own grid.
2. **Every contour uses `stroke-width="1"`** and lies on pixel centres, i.e. on .5 coordinates
   (`M2.5 6.5H8.5V14.5H2.5Z`), so it is sharp at 1× and 2×. A frame around pixel columns x0..x1 and
   rows y0..y1 is `M(x0+.5) (y0+.5)H(x1+.5)V(y1+.5)H(x0+.5)Z`.
3. **Elements of 2 px or more** (bars, thick frames, 2×2 dots, glyphs) are filled areas with integer
   edges, not thicker strokes. Rings and frames are `fill-rule="evenodd"` areas.
4. **Areas use gradients** (linear or radial) instead of reproducing pixels or dithering. Areas of
   2 px or less may be flat. Colours are taken from the original bitmap.
5. **No gaps when scaling**: a fill reaches the centre of the contour on top of it (same path for fill
   and stroke) or runs underneath neighbouring elements. Content inside a frame is drawn as
   fill → content → frame contour last (`fill="none"`). Adjacent areas overlap instead of touching.
6. **Minimal paths**: `H`/`V`/relative commands, parts of the same paint merged into one `<path>`,
   no needless groups or attributes, no `<text>` (lettering is paths).
7. **Unique ids**: gradient ids are prefixed with `<folder>-<icon>-` (e.g. `common-list_box-b`)
   because file names repeat between folders and SVGs may be inlined into one document.
8. **Geometry from the bitmap**: positions and sizes follow the original pixel grid; recurring motifs
   (mouse pointer, magnifier, spin buttons, window frames, "ok" lettering, …) are drawn identically
   everywhere. Blurred or dithered originals are rebuilt cleanly; 16 px icons may drop detail.
9. No comments, metadata or editor attributes in the SVG files.

## Application icons (resources/icons, sdk)

These icons are also rendered for HiDPI (150 %, 200 %) from the same SVG, so they are drawn as
vectors, not as pixel copies:

- **Flat style**: colours come from the shared materials in `tools/palette.js` (paper, metal, blue,
  yellow, red, …); each material is a subtle two-stop gradient from top to bottom with a fixed
  contour colour. No gloss highlights, glow spots or ground shadows.
- **Sharp at 100 %**: horizontal and vertical edges and 1 px contours stay on the pixel grid
  (rule 2); curves, diagonals and organic shapes are real curves instead of pixel steps.
- **Lettering** is drawn as letter shapes, not as pixel blocks. Small lettering uses 1 px lines with
  square ends (e.g. "php"); glyph-like symbols may be stroked centre lines of any width when they use
  `stroke-linecap="round"` (e.g. the question mark of `sdk/plugin_interface/default`, a 3 px line
  over a 5 px dark line as outline). This is the only exception to rule 2.
- **Logo** inside an application icon (e.g. `project`) is the 16 px logo of the platform icons,
  including its pixel lettering, so the brand looks the same everywhere.
- Check icons at 100, 150 and 200 %, not only against the original bitmap.

## Logo and platform icons

- The 512 px master `resources/icons/logo.svg` is the 32-unit logo grid scaled by 16. Squares have a
  12 px contour; the lettering is 24 px strong with a 6 px outline whose round joins give the
  corners a slight rounding. `logo32x32.svg` uses the same geometry 1:1, `logo16x16.svg` and the
  logo part of `project.svg` use whole-pixel squares and 5-row pixel lettering.
- Platform icons are generated from the same geometry. Every raster size up to 32 px has a
  pixel-hinted source in `platform/sources`; larger sizes are rendered from the masters.
- Stroke widths other than 1 occur only in the logo files and the platform masters.

## Example (22 px)

```xml
<svg xmlns="http://www.w3.org/2000/svg" width="22" height="22" viewBox="0 0 22 22">
<defs><linearGradient id="layout-sizer-b" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#ecf7ff"/><stop offset="1" stop-color="#59a6ff"/></linearGradient></defs>
<path d="M11.5 4.5H19.5V14.5H11.5Z" fill="url(#layout-sizer-b)" stroke="#1b6cd0" stroke-width="1"/>
<path d="M1 16H21V18H1Z" fill="#4c4465"/>
</svg>
```
