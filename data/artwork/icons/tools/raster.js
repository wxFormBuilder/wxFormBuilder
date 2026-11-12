// Rasteriser of the icon tools (module, no output): renders SVG text to PNG with resvg.
// The WebAssembly build of resvg is used on purpose: it runs the same code on every operating system
// and CPU, so the raster icons checked into the repository are reproducible byte for byte on Windows,
// Linux (including WSL) and macOS. Install it once with `npm ci` in this directory (see ../SETUP.md).
'use strict';
const fs = require('fs');
const zlib = require('zlib');

let resvg;
async function load() {
  if (!resvg) {
    let mod;
    try {
      mod = require('@resvg/resvg-wasm');
    } catch (e) {
      throw new Error('The package @resvg/resvg-wasm is not installed. Run "npm ci" in data/artwork/icons/tools ' +
        '(see data/artwork/icons/SETUP.md).');
    }
    await mod.initWasm(fs.readFileSync(require.resolve('@resvg/resvg-wasm/index_bg.wasm')));
    resvg = mod;
  }
  return resvg;
}

// Renders SVG text and returns the PNG file content (8-bit RGBA, straight alpha).
//   width: output width in pixels (default: the width of the SVG), the height follows the aspect ratio
//   font:  { buffers: [Buffer, ...], family: 'Name' } for SVGs that contain text (default: no fonts)
async function renderPng(svg, { width, font } = {}) {
  const { Resvg } = await load();
  const options = { font: { fontBuffers: font ? font.buffers : [] } };
  if (font) options.font.defaultFontFamily = options.font.sansSerifFamily = font.family;
  if (width) options.fitTo = { mode: 'width', value: width };
  const renderer = new Resvg(svg, options), image = renderer.render();
  const png = Buffer.from(image.asPng());
  image.free(); renderer.free();
  return png;
}

// Decodes a PNG as written by renderPng (8-bit RGBA, non-interlaced) to { w, h, px } with straight alpha.
function decodePng(buf) {
  let o = 8, w = 0, h = 0; const idat = [];
  while (o < buf.length) {
    const len = buf.readUInt32BE(o), type = buf.toString('ascii', o + 4, o + 8), data = buf.subarray(o + 8, o + 8 + len);
    if (type === 'IHDR') {
      w = data.readUInt32BE(0); h = data.readUInt32BE(4);
      if (data[8] !== 8 || data[9] !== 6 || data[12] !== 0) throw new Error('expected 8-bit RGBA non-interlaced PNG');
    } else if (type === 'IDAT') idat.push(data);
    o += 12 + len;
  }
  const raw = zlib.inflateSync(Buffer.concat(idat)), px = Buffer.alloc(w * h * 4), stride = w * 4;
  for (let y = 0; y < h; y++) {
    const f = raw[y * (stride + 1)], src = raw.subarray(y * (stride + 1) + 1, (y + 1) * (stride + 1)), row = y * stride;
    for (let x = 0; x < stride; x++) {
      const a = x >= 4 ? px[row + x - 4] : 0, b = y ? px[row - stride + x] : 0, c = x >= 4 && y ? px[row - stride + x - 4] : 0;
      let v = src[x];
      if (f === 1) v += a; else if (f === 2) v += b; else if (f === 3) v += (a + b) >> 1;
      else if (f === 4) { const p = a + b - c, pa = Math.abs(p - a), pb = Math.abs(p - b), pc = Math.abs(p - c); v += pa <= pb && pa <= pc ? a : pb <= pc ? b : c; }
      px[row + x] = v & 255;
    }
  }
  return { w, h, px };
}

module.exports = { renderPng, decodePng };
