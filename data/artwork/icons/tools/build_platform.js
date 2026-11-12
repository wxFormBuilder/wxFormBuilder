// Builds the raster platform icons of the repository from the SVG sources in <svgRoot>
// (default: the data/artwork/icons directory) and writes them into data/platform:
//   macos/icon.icns, macos/docicon.icns                       (PNG elements 16..1024)
//   windows/wxFormBuilder.ico, windows/docicon.ico            (16..64 as 32-bit BMP, 256 as PNG)
//   linux/share/icons/hicolor/<n>x<n>/{apps,mimetypes}/*.png  (16..512) and scalable/{apps,mimetypes}/*.svg
// The installer icon data/packaging/windows/support/wxFormBuilder.ico is updated as well.
// Sizes up to 32 px use the pixel-hinted sources (logo16x16, logo32x32, platform/sources/*.svg).
// The SVGs are rasterised by resvg (see raster.js), so the output is identical on every system.
// Usage: node build_platform.js [<svgRoot>]
'use strict';
const fs = require('fs');
const path = require('path');
const { renderPng, decodePng } = require('./raster.js');

const svgRoot = process.argv[2] || path.resolve(__dirname, '..');
const repoRoot = path.resolve(svgRoot, '..', '..', '..');
const S = (...a) => path.join(svgRoot, 'platform', ...a);      // SVG sources
const P = (...a) => path.join(repoRoot, 'data', 'platform', ...a); // build system files
const R = (...a) => path.join(svgRoot, 'resources', 'icons', ...a);

// ---------- rasterising ----------
const cache = new Map();
// Returns the PNG content of the SVG file `svg` rendered at `size` px.
async function render(svg, size) {
  const key = `${svg}@${size}`;
  if (!cache.has(key)) cache.set(key, await renderPng(fs.readFileSync(svg, 'utf8'), { width: size }));
  return cache.get(key);
}

// ---------- ICNS ----------
const ICNS_TYPES = [['icp4', 16], ['ic11', 32], ['icp5', 32], ['ic12', 64], ['ic07', 128], ['ic13', 256],
  ['ic08', 256], ['ic14', 512], ['ic09', 512], ['ic10', 1024]];
async function writeIcns(file, svgFor) {
  const els = [];
  for (const [type, s] of ICNS_TYPES) {
    const data = await render(svgFor(s), s), head = Buffer.alloc(8);
    head.write(type, 0, 'ascii'); head.writeUInt32BE(data.length + 8, 4); els.push(Buffer.concat([head, data]));
  }
  const body = Buffer.concat(els), head = Buffer.alloc(8);
  head.write('icns', 0, 'ascii'); head.writeUInt32BE(body.length + 8, 4);
  fs.mkdirSync(path.dirname(file), { recursive: true }); fs.writeFileSync(file, Buffer.concat([head, body]));
}

// ---------- ICO ----------
// 32-bit BGRA DIB (height doubled, bottom-up) followed by an empty AND mask.
function dib(png) {
  const { w, h, px } = decodePng(png), maskStride = Math.ceil(w / 32) * 4;
  const head = Buffer.alloc(40);
  head.writeUInt32LE(40, 0); head.writeInt32LE(w, 4); head.writeInt32LE(h * 2, 8); head.writeUInt16LE(1, 12);
  head.writeUInt16LE(32, 14); head.writeUInt32LE(w * h * 4 + maskStride * h, 20);
  const xor = Buffer.alloc(w * h * 4);
  for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
    const s = (y * w + x) * 4, d = ((h - 1 - y) * w + x) * 4;
    xor[d] = px[s + 2]; xor[d + 1] = px[s + 1]; xor[d + 2] = px[s]; xor[d + 3] = px[s + 3];
  }
  return Buffer.concat([head, xor, Buffer.alloc(maskStride * h)]);
}
async function writeIco(file, frames) { // frames: [[size, svg], ...]
  const images = [];
  for (const [s, svg] of frames) { const png = await render(svg, s); images.push([s, s >= 256 ? png : dib(png)]); }
  const head = Buffer.alloc(6 + 16 * images.length);
  head.writeUInt16LE(1, 2); head.writeUInt16LE(images.length, 4);
  let off = head.length;
  images.forEach(([s, data], i) => {
    const e = 6 + 16 * i;
    head[e] = s >= 256 ? 0 : s; head[e + 1] = s >= 256 ? 0 : s; head.writeUInt16LE(1, e + 4); head.writeUInt16LE(32, e + 6);
    head.writeUInt32LE(data.length, e + 8); head.writeUInt32LE(off, e + 12); off += data.length;
  });
  fs.mkdirSync(path.dirname(file), { recursive: true });
  fs.writeFileSync(file, Buffer.concat([head, ...images.map(([, d]) => d)]));
}

// ---------- sources ----------
const LOGO16 = R('logo16x16.svg'), LOGO32 = R('logo32x32.svg'), LOGO = R('logo.svg');
const MAC_ICON = S('macos', 'icon.svg'), DOC = S('macos', 'docicon.svg');
const SRC = name => S('sources', `${name}.svg`);
// Pixel-hinted sources for small sizes, scalable masters above.
const appFor = s => ({ 16: LOGO16, 20: SRC('icon20'), 22: SRC('icon22'), 24: SRC('icon24'), 32: LOGO32 })[s] || LOGO;
const docFor = s => ([16, 20, 22, 24, 32].includes(s) ? SRC(`docicon${s}`) : DOC);
const macFor = s => ([16, 32].includes(s) ? SRC(`macicon${s}`) : MAC_ICON);

async function main() {
  await writeIcns(P('macos', 'icon.icns'), macFor);
  await writeIcns(P('macos', 'docicon.icns'), docFor);

  const WIN_SIZES = [256, 64, 48, 40, 32, 24, 20, 16];
  await writeIco(P('windows', 'wxFormBuilder.ico'), WIN_SIZES.map(s => [s, appFor(s)]));
  await writeIco(P('windows', 'docicon.ico'), WIN_SIZES.map(s => [s, docFor(s)]));

  const LINUX_SIZES = [16, 22, 24, 32, 48, 64, 128, 256, 512];
  for (const s of LINUX_SIZES) {
    const dir = P('linux', 'share', 'icons', 'hicolor', `${s}x${s}`);
    fs.mkdirSync(path.join(dir, 'apps'), { recursive: true }); fs.mkdirSync(path.join(dir, 'mimetypes'), { recursive: true });
    fs.writeFileSync(path.join(dir, 'apps', 'org.wxformbuilder.wxFormBuilder.png'), await render(appFor(s), s));
    fs.writeFileSync(path.join(dir, 'mimetypes', 'org.wxformbuilder.wxFormBuilder.application-x-wxformbuilder.png'), await render(docFor(s), s));
  }
  for (const kind of ['apps', 'mimetypes']) {
    const from = S('linux', 'share', 'icons', 'hicolor', 'scalable', kind), to = P('linux', 'share', 'icons', 'hicolor', 'scalable', kind);
    fs.mkdirSync(to, { recursive: true });
    for (const f of fs.readdirSync(from)) fs.copyFileSync(path.join(from, f), path.join(to, f));
  }
  fs.copyFileSync(P('windows', 'wxFormBuilder.ico'), path.join(repoRoot, 'data', 'packaging', 'windows', 'support', 'wxFormBuilder.ico'));
  console.log('platform icons written to', P());
}

main().catch(e => { console.error(e.message || e); process.exitCode = 1; });
