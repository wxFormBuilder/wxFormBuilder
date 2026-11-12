// Renders bitmaps and SVGs side by side for comparison while drawing an icon. Each pair shows the
// bitmap enlarged 7x pixel-exact, the SVG enlarged 7x as vector and the SVG at its own size.
// Usage: node preview.js <out.png> <bitmap1> <svg1> [<bitmap2> <svg2> ...]
'use strict';
const path = require('path');
const { bitmapSize, svgSize, bitmapImage, svgImage, rect, text, writeSheet } = require('./sheet.js');

const [out, ...rest] = process.argv.slice(2);
if (!out || !rest.length || rest.length % 2) {
  console.error('Usage: node preview.js <out.png> <bitmap1> <svg1> [<bitmap2> <svg2> ...]'); process.exit(2);
}
const COLUMNS = 3, WIDTH = 1200, GAP = 4, PAD = 4, LABEL = 16, SPACE = 8, ZOOM = 7;
const CELL = (WIDTH - (COLUMNS + 1) * GAP) / COLUMNS;

async function main() {
  const cells = [];
  for (let i = 0; i < rest.length; i += 2) {
    const bmp = rest[i], svg = rest[i + 1], size = bitmapSize(bmp), canvas = svgSize(svg);
    const dir = path.basename(path.dirname(bmp)) === 'icons' ? path.basename(path.dirname(path.dirname(bmp))) : path.basename(path.dirname(bmp));
    cells.push({ bmp, svg, size, canvas, label: `${dir}/${path.basename(bmp)}`, height: Math.max(size, canvas) * ZOOM });
  }
  let body = '', y = GAP;
  for (let i = 0; i < cells.length; i += COLUMNS) {
    const row = cells.slice(i, i + COLUMNS), inner = Math.max(...row.map(c => c.height));
    for (const [col, c] of row.entries()) {
      const x0 = GAP + col * (CELL + GAP), top = y + PAD + LABEL;
      let x = x0 + PAD;
      body += rect(x0, y, CELL, inner + LABEL + 2 * PAD, '#ffffff') + text(x, y + PAD + 11, c.label, 11);
      body += bitmapImage(c.bmp, x, top, c.size * ZOOM); x += c.size * ZOOM + SPACE;
      body += await svgImage(c.svg, x, top, c.canvas * ZOOM); x += c.canvas * ZOOM + SPACE;
      body += await svgImage(c.svg, x, top, c.canvas);
    }
    y += inner + LABEL + 2 * PAD + GAP;
  }
  await writeSheet(out, WIDTH, y, rect(0, 0, WIDTH, y, '#c8c8c8') + body);
  console.log('written', path.resolve(out));
}

main().catch(e => { console.error(e.message || e); process.exitCode = 1; });
