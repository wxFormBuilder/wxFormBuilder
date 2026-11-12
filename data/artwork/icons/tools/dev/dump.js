// Prints bitmaps (PNG or XPM) at native size as character grids ('.' = transparent) with their
// palette (RRGGBBAA), to read the exact pixels of an icon while redrawing it.
// Usage: node dump.js <bitmap> [<bitmap> ...]   (output on stdout)
'use strict';
const { readXpm, bitmapSvg } = require('./sheet.js');
const { renderPng, decodePng } = require('../raster.js');

const files = process.argv.slice(2);
if (!files.length) { console.error('Usage: node dump.js <bitmap> [<bitmap> ...]'); process.exit(2); }
const CHARS = '123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@#$%&*+=?<>!^~:;';
const hex = v => v.toString(16).padStart(2, '0');

async function main() {
  for (const file of files) {
    // PNGs of any colour type are decoded by rendering them 1:1.
    const { w, h, px } = file.endsWith('.xpm') ? readXpm(file) : decodePng(await renderPng(bitmapSvg(file)));
    const palette = new Map(), rows = [];
    for (let y = 0; y < h; y++) {
      let line = '';
      for (let x = 0; x < w; x++) {
        const [r, g, b, a] = px.subarray((y * w + x) * 4, (y * w + x) * 4 + 4);
        if (a < 16) { line += '.'; continue; }
        const key = hex(r) + hex(g) + hex(b) + hex(a);
        if (!palette.has(key)) palette.set(key, palette.size < CHARS.length ? CHARS[palette.size] : '"');
        line += palette.get(key);
      }
      rows.push(line);
    }
    let out = `== ${file} (${w}x${h})\n`, i = 0;
    for (const [key, ch] of palette) { out += `${ch}=${key} `; if (++i % 8 === 0) out += '\n'; }
    out += `\n   ${Array.from({ length: w }, (_, x) => x % 10).join('')}\n`;
    rows.forEach((line, y) => { out += `${String(y).padStart(2)} ${line}\n`; });
    process.stdout.write(out);
  }
}

main().catch(e => { console.error(e.message || e); process.exitCode = 1; });
