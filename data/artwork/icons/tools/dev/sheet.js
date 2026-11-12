// Building blocks of the developer sheets (module, no output): preview.js composes one SVG from
// bitmaps, icon SVGs and labels and renders it with ../raster.js.
'use strict';
const fs = require('fs');
const path = require('path');
const { renderPng } = require('../raster.js');

const esc = s => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');

// Pixels of an XPM file as { w, h, px } (RGBA, straight alpha).
function readXpm(file) {
  const lines = [...fs.readFileSync(file, 'utf8').matchAll(/"([^"]*)"/g)].map(m => m[1]);
  const [w, h, nc, cpp] = lines[0].trim().split(/\s+/).map(Number);
  const colors = {};
  for (const l of lines.slice(1, 1 + nc)) {
    const m = l.slice(cpp).match(/c\s+(#[0-9a-fA-F]{6}|None)/);
    colors[l.slice(0, cpp)] = m && m[1] !== 'None'
      ? [1, 3, 5].map(i => parseInt(m[1].slice(i, i + 2), 16)).concat(255) : [0, 0, 0, 0];
  }
  const px = Buffer.alloc(w * h * 4);
  lines.slice(1 + nc, 1 + nc + h).forEach((row, y) => {
    for (let x = 0; x < w; x++) px.set(colors[row.slice(x * cpp, x * cpp + cpp)], (y * w + x) * 4);
  });
  return { w, h, px };
}

// Width in pixels of a PNG or XPM bitmap file.
const bitmapSize = file => (file.endsWith('.xpm') ? readXpm(file).w : fs.readFileSync(file).readUInt32BE(16));

// Width of an SVG file as given by its width attribute.
const svgSize = file => Number(fs.readFileSync(file, 'utf8').match(/<svg[^>]*\swidth="(\d+)"/)[1]);

// SVG document that shows a bitmap file pixel by pixel at its native size.
function bitmapSvg(file) {
  if (file.endsWith('.xpm')) {
    const { w, h, px } = readXpm(file); let rects = '';
    for (let i = 0; i < w * h; i++) {
      const [r, g, b, a] = px.subarray(i * 4, i * 4 + 4);
      if (a) rects += `<rect x="${i % w}" y="${Math.floor(i / w)}" width="1" height="1" fill="rgb(${r},${g},${b})"/>`;
    }
    return `<svg xmlns="http://www.w3.org/2000/svg" width="${w}" height="${h}" shape-rendering="crispEdges">${rects}</svg>`;
  }
  const png = fs.readFileSync(file), w = png.readUInt32BE(16), h = png.readUInt32BE(20);
  return `<svg xmlns="http://www.w3.org/2000/svg" width="${w}" height="${h}"><image width="${w}" height="${h}" ` +
    `image-rendering="optimizeSpeed" href="data:image/png;base64,${png.toString('base64')}"/></svg>`;
}

// <image> element that places an SVG document at (x, y) with the given width; `pixelated` keeps the
// pixels of an enlarged bitmap sharp.
const image = (svg, x, y, size, pixelated = false) =>
  `<image x="${x}" y="${y}" width="${size}" height="${size}"${pixelated ? ' image-rendering="optimizeSpeed"' : ''} ` +
  `href="data:image/svg+xml;base64,${Buffer.from(svg).toString('base64')}"/>`;
// A bitmap file at (x, y), enlarged pixel-exact to `size`.
const bitmapImage = (file, x, y, size) => {
  if (file.endsWith('.xpm')) return image(bitmapSvg(file), x, y, size);
  const png = fs.readFileSync(file);
  return `<image x="${x}" y="${y}" width="${size}" height="${size}" image-rendering="optimizeSpeed" ` +
    `href="data:image/png;base64,${png.toString('base64')}"/>`;
};
// An SVG file at (x, y), rendered as vector at `size`. It is rasterised on its own first: a sheet of
// plain raster images renders much faster than one with hundreds of nested SVG documents.
const svgImage = async (file, x, y, size) =>
  `<image x="${x}" y="${y}" width="${size}" height="${size}" ` +
  `href="data:image/png;base64,${(await renderPng(fs.readFileSync(file, 'utf8'), { width: size })).toString('base64')}"/>`;

const rect = (x, y, w, h, fill, rx = 0) => `<rect x="${x}" y="${y}" width="${w}" height="${h}" rx="${rx}" fill="${fill}"/>`;
const text = (x, y, str, size = 12, fill = '#000000') =>
  `<text x="${x}" y="${y}" font-size="${size}" fill="${fill}">${esc(str)}</text>`;

// Font for the labels: the file named by WXFB_FONT (family: WXFB_FONT_FAMILY), else the first common
// system font found. Returns null if there is none; the sheet is then rendered without labels.
function findFont() {
  const candidates = [
    [process.env.WXFB_FONT, process.env.WXFB_FONT_FAMILY || 'sans-serif'],
    ['C:/Windows/Fonts/segoeui.ttf', 'Segoe UI'],
    ['C:/Windows/Fonts/arial.ttf', 'Arial'],
    ['/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf', 'DejaVu Sans'],
    ['/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf', 'Liberation Sans'],
    ['/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf', 'Liberation Sans'],
    ['/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf', 'Noto Sans'],
    ['/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf', 'DejaVu Sans'],
    ['/System/Library/Fonts/Supplemental/Arial.ttf', 'Arial'],
    ['/mnt/c/Windows/Fonts/segoeui.ttf', 'Segoe UI'],
  ];
  for (const [file, family] of candidates) {
    if (file && fs.existsSync(file)) return { buffers: [fs.readFileSync(file)], family };
  }
  return null;
}

// Renders the sheet (SVG body on a white page of width x height) to the PNG file `out`.
async function writeSheet(out, width, height, body) {
  const font = findFont();
  if (!font) console.warn('No label font found: the sheet has no labels. Set WXFB_FONT to a .ttf file.');
  const svg = `<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${height}" font-family="sans-serif">` +
    rect(0, 0, width, height, '#ffffff') + body + '</svg>';
  fs.mkdirSync(path.dirname(path.resolve(out)), { recursive: true });
  fs.writeFileSync(out, await renderPng(svg, { font }));
}

module.exports = { esc, readXpm, bitmapSize, svgSize, bitmapSvg, bitmapImage, svgImage, rect, text, writeSheet };
