// Builds the raster platform icons of the repository from the SVG sources in <svgRoot>
// (default: the data/artwork/icons directory) and writes them into data/platform:
//   macos/icon.icns, macos/docicon.icns                       (PNG elements 16..1024)
//   windows/wxFormBuilder.ico, windows/docicon.ico            (16..64 as 32-bit BMP, 256 as PNG)
//   linux/share/icons/hicolor/<n>x<n>/{apps,mimetypes}/*.png  (16..512) and scalable/{apps,mimetypes}/*.svg
// The installer icon data/packaging/windows/support/wxFormBuilder.ico is updated as well.
// Sizes up to 32 px use the pixel-hinted sources (logo16x16, logo32x32, platform/sources/*.svg).
// Requires Windows: SVGs are rasterised by headless Microsoft Edge with a transparent background and
// cropped with System.Drawing (PowerShell).
// Usage: node build_platform.js [<svgRoot>] [<workDir>]   (workDir default: a temporary directory)
'use strict';
const fs = require('fs');
const path = require('path');
const zlib = require('zlib');
const os = require('os');
const { execFileSync } = require('child_process');

const svgRoot = process.argv[2] || path.resolve(__dirname, '..');
const workDir = process.argv[3] || fs.mkdtempSync(path.join(os.tmpdir(), 'wxfb-icons-'));
const repoRoot = path.resolve(svgRoot, '..', '..', '..');
const EDGE = 'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe';
const fileUrl = p => 'file:///' + path.resolve(p).split(path.sep).join('/');
const S = (...a) => path.join(svgRoot, 'platform', ...a);      // SVG sources
const P = (...a) => path.join(repoRoot, 'data', 'platform', ...a); // build system files
const R = (...a) => path.join(svgRoot, 'resources', 'icons', ...a);

// ---------- rasterising ----------
const SHEET = { 1024: [0, 0], 512: [1040, 0], 256: [1040, 530], 128: [1310, 530], 64: [1450, 530], 48: [1530, 530],
  40: [1450, 610], 32: [1500, 610], 24: [1540, 610], 22: [1570, 610], 20: [1450, 660], 16: [1480, 660] };
const cache = new Map();
// Returns the PNG file of `svg` rendered at `size` px (renders all sheet sizes of an SVG once).
function render(svg, size) {
  if (!cache.has(svg)) {
    const key = path.basename(svg, '.svg') + '-' + cache.size, dir = path.join(workDir, key);
    fs.mkdirSync(dir, { recursive: true });
    const html = path.join(dir, 'sheet.html'), sheet = path.join(dir, 'sheet.png');
    fs.writeFileSync(html, '<html><body style="margin:0;background:transparent">' + Object.entries(SHEET).map(([s, [x, y]]) =>
      `<img src="${fileUrl(svg)}" width="${s}" height="${s}" style="position:absolute;left:${x}px;top:${y}px">`).join('') + '</body></html>');
    execFileSync(EDGE, ['--headless', '--disable-gpu', '--allow-file-access-from-files', '--force-device-scale-factor=1',
      '--default-background-color=00000000', '--window-size=1620,1040', `--screenshot=${sheet}`, fileUrl(html)], { stdio: 'ignore', timeout: 60000 });
    const ps = `Add-Type -AssemblyName System.Drawing; $b=[System.Drawing.Bitmap]::new('${sheet}'); ` + Object.entries(SHEET).map(([s, [x, y]]) =>
      `$b.Clone([System.Drawing.Rectangle]::new(${x},${y},${s},${s}),[System.Drawing.Imaging.PixelFormat]::Format32bppArgb).Save('${path.join(dir, s + '.png')}',[System.Drawing.Imaging.ImageFormat]::Png)`).join('; ') + '; $b.Dispose()';
    execFileSync('powershell', ['-NoProfile', '-Command', ps], { stdio: 'inherit' });
    cache.set(svg, dir);
  }
  return path.join(cache.get(svg), `${size}.png`);
}

// ---------- PNG decoding (8-bit RGBA, non-interlaced, as written by System.Drawing) ----------
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

// ---------- ICNS ----------
const ICNS_TYPES = [['icp4', 16], ['ic11', 32], ['icp5', 32], ['ic12', 64], ['ic07', 128], ['ic13', 256],
  ['ic08', 256], ['ic14', 512], ['ic09', 512], ['ic10', 1024]];
function writeIcns(file, svgFor) {
  const els = ICNS_TYPES.map(([type, s]) => {
    const data = fs.readFileSync(render(svgFor(s), s)), head = Buffer.alloc(8);
    head.write(type, 0, 'ascii'); head.writeUInt32BE(data.length + 8, 4); return Buffer.concat([head, data]);
  });
  const body = Buffer.concat(els), head = Buffer.alloc(8);
  head.write('icns', 0, 'ascii'); head.writeUInt32BE(body.length + 8, 4);
  fs.mkdirSync(path.dirname(file), { recursive: true }); fs.writeFileSync(file, Buffer.concat([head, body]));
}

// ---------- ICO ----------
// 32-bit BGRA DIB (height doubled, bottom-up) followed by an empty AND mask.
function dib(pngFile) {
  const { w, h, px } = decodePng(fs.readFileSync(pngFile)), maskStride = Math.ceil(w / 32) * 4;
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
function writeIco(file, frames) { // frames: [[size, svg], ...]
  const images = frames.map(([s, svg]) => [s, s >= 256 ? fs.readFileSync(render(svg, s)) : dib(render(svg, s))]);
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

writeIcns(P('macos', 'icon.icns'), macFor);
writeIcns(P('macos', 'docicon.icns'), docFor);

const WIN_SIZES = [256, 64, 48, 40, 32, 24, 20, 16];
writeIco(P('windows', 'wxFormBuilder.ico'), WIN_SIZES.map(s => [s, appFor(s)]));
writeIco(P('windows', 'docicon.ico'), WIN_SIZES.map(s => [s, docFor(s)]));

const LINUX_SIZES = [16, 22, 24, 32, 48, 64, 128, 256, 512];
for (const s of LINUX_SIZES) {
  const dir = P('linux', 'share', 'icons', 'hicolor', `${s}x${s}`);
  fs.mkdirSync(path.join(dir, 'apps'), { recursive: true }); fs.mkdirSync(path.join(dir, 'mimetypes'), { recursive: true });
  fs.copyFileSync(render(appFor(s), s), path.join(dir, 'apps', 'org.wxformbuilder.wxFormBuilder.png'));
  fs.copyFileSync(render(docFor(s), s), path.join(dir, 'mimetypes', 'org.wxformbuilder.wxFormBuilder.application-x-wxformbuilder.png'));
}
for (const kind of ['apps', 'mimetypes']) {
  const from = S('linux', 'share', 'icons', 'hicolor', 'scalable', kind), to = P('linux', 'share', 'icons', 'hicolor', 'scalable', kind);
  fs.mkdirSync(to, { recursive: true });
  for (const f of fs.readdirSync(from)) fs.copyFileSync(path.join(from, f), path.join(to, f));
}
fs.copyFileSync(P('windows', 'wxFormBuilder.ico'), path.join(repoRoot, 'data', 'packaging', 'windows', 'support', 'wxFormBuilder.ico'));
console.log('platform icons written to', P());
