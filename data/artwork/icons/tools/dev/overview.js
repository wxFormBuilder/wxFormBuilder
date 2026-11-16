// Builds one large overview PNG of the whole icon set: every bitmap icon of the repository
// (resources/icons, sdk/plugin_interface, plugins/*/icons) next to its SVG from <svgRoot>
// (default: the data/artwork/icons directory). Each cell shows the bitmap at its native size and
// enlarged pixel-exact, then the SVG at its own canvas size and enlarged as vector.
// Usage: node overview.js <out.png> [<svgRoot>]
'use strict';
const fs = require('fs');
const path = require('path');
const { esc, bitmapSize, svgSize, bitmapImage, svgImage, rect, text, writeSheet } = require('./sheet.js');

const outPng = process.argv[2];
if (!outPng) { console.error('Usage: node overview.js <out.png> [<svgRoot>]'); process.exit(2); }
const svgRoot = process.argv[3] || path.resolve(__dirname, '..', '..');
const repoRoot = path.resolve(svgRoot, '..', '..', '..');

const COLUMNS = 6, WIDTH = 1580, MARGIN = 6, GAP = 4, PAD = 4, LABEL = 18, SPACE = 6;
const CELL = (WIDTH - 2 * MARGIN - (COLUMNS - 1) * GAP) / COLUMNS;
// Enlargement of the second image of each pair: small icons 3x, 32 px icons 2x, larger ones not at all.
const zoomOf = size => (size > 64 ? 1 : size > 24 ? 2 : 3);

const bitmapsIn = dir => fs.readdirSync(dir).filter(f => /\.(png|xpm)$/.test(f)).sort((a, b) => a.localeCompare(b));
const groups = [['resources/icons', 'Application'], ['sdk/plugin_interface', 'SDK'],
  ...fs.readdirSync(path.join(repoRoot, 'plugins'))
    .filter(d => fs.existsSync(path.join(repoRoot, 'plugins', d, 'icons')))
    .map(d => [`plugins/${d}/icons`, `Plugin ${d}`])]
  .map(([rel, title]) => ({ rel, title, files: bitmapsIn(path.join(repoRoot, rel)) }));

async function main() {
  let body = text(10, 28, 'wxFormBuilder icons: bitmap and SVG', 20) +
    text(10, 48, 'Per icon: the bitmap (native size and enlarged pixel-exact), then the SVG (own canvas size and enlarged ' +
      'as vector). "22px → 24px" marks a 22 px bitmap whose SVG uses the 24 px canvas.', 12, '#555555');
  let y = 58, count = 0;
  for (const { rel, title, files } of groups) {
    body += text(10, y + 22, `${title} (${rel}) – ${files.length}`, 16);
    y += 30;
    const cells = files.map(f => {
      const name = f.replace(/\.(png|xpm)$/, ''), bmp = path.join(repoRoot, rel, f), svg = path.join(svgRoot, rel, `${name}.svg`);
      const size = bitmapSize(bmp), zoom = zoomOf(size), canvas = zoom > 1 ? svgSize(svg) : size;
      return { name, bmp, svg, size, zoom, canvas, height: Math.max(size, canvas) * zoom };
    });
    for (let i = 0; i < cells.length; i += COLUMNS) {
      const row = cells.slice(i, i + COLUMNS), inner = Math.max(...row.map(c => c.height));
      for (const [col, c] of row.entries()) {
        const x0 = MARGIN + col * (CELL + GAP), bottom = y + PAD + LABEL + inner;
        let x = x0 + PAD;
        body += rect(x0, y, CELL, inner + LABEL + 2 * PAD, '#f2f2f2', 3) +
          `<text x="${x}" y="${y + PAD + 11}" font-size="12">${esc(c.name)}<tspan dx="6" font-size="11" fill="#777777">` +
          `${c.size}px${c.canvas !== c.size ? ` → ${c.canvas}px` : ''}</tspan></text>`;
        body += bitmapImage(c.bmp, x, bottom - c.size, c.size); x += c.size + SPACE;
        if (c.zoom > 1) { body += bitmapImage(c.bmp, x, bottom - c.size * c.zoom, c.size * c.zoom); x += c.size * c.zoom + SPACE; }
        body += rect(x, y + PAD + LABEL, 1, inner, '#cccccc'); x += 1 + SPACE;
        body += await svgImage(c.svg, x, bottom - c.canvas, c.canvas); x += c.canvas + SPACE;
        if (c.zoom > 1) body += await svgImage(c.svg, x, bottom - c.canvas * c.zoom, c.canvas * c.zoom);
        count++;
      }
      y += inner + LABEL + 2 * PAD + GAP;
    }
    y += 6;
  }
  await writeSheet(outPng, WIDTH, y + 6, body);
  console.log(`${count} icons, ${WIDTH} x ${y + 6} px -> ${path.resolve(outPng)}`);
}

main().catch(e => { console.error(e.message || e); process.exitCode = 1; });
