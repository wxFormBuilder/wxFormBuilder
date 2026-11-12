// Checks the SVG icon set in <svgRoot> (default: the data/artwork/icons directory) against the
// bitmap icons of the repository (resources/icons, plugins/*/icons, sdk/plugin_interface):
// every bitmap has an SVG at the same relative path, its viewBox equals the bitmap size (logo: 512),
// only stroke-width 1 is used (except the logos), no <text>/<image>/comments, gradient ids are
// unique across the whole set and every url(#id) reference resolves.
// Usage: node verify.js [<svgRoot>]
'use strict';
const fs = require('fs');
const path = require('path');

const svgRoot = process.argv[2] || path.resolve(__dirname, '..');
const repoRoot = path.resolve(svgRoot, '..', '..', '..');
const walk = d => fs.readdirSync(d, { withFileTypes: true })
  .flatMap(e => (e.isDirectory() ? walk(path.join(d, e.name)) : [path.join(d, e.name)]));

// Bitmap size from a PNG header or the values line of an XPM file.
function bitmapSize(file) {
  if (file.endsWith('.png')) { const b = fs.readFileSync(file); return [b.readUInt32BE(16), b.readUInt32BE(20)]; }
  const m = fs.readFileSync(file, 'utf8').match(/"\s*(\d+)\s+(\d+)\s+\d+\s+\d+/);
  return m ? [Number(m[1]), Number(m[2])] : null;
}

const iconDirs = [path.join(repoRoot, 'resources', 'icons'), path.join(repoRoot, 'sdk', 'plugin_interface'),
  ...fs.readdirSync(path.join(repoRoot, 'plugins'), { withFileTypes: true })
    .filter(e => e.isDirectory()).map(e => path.join(repoRoot, 'plugins', e.name, 'icons'))]
  .filter(d => fs.existsSync(d));
const bitmaps = iconDirs.flatMap(d => fs.readdirSync(d).filter(f => /\.(png|xpm)$/.test(f)).map(f => path.join(d, f)));

const problems = [], ids = new Map(), sizes = {};
const covered = new Set();
for (const bmp of bitmaps) {
  const rel = path.relative(repoRoot, bmp), svgFile = path.join(svgRoot, rel.replace(/\.(png|xpm)$/, '.svg'));
  covered.add(path.resolve(svgFile));
  if (!fs.existsSync(svgFile)) { problems.push(`missing SVG: ${rel}`); continue; }
  const svg = fs.readFileSync(svgFile, 'utf8'), [w] = bitmapSize(bmp) || [0];
  const isLogo = /[\\/]resources[\\/]icons[\\/]logo(16x16|32x32)?\.png$/.test(bmp);
  const expect = /[\\/]logo\.png$/.test(bmp) ? 512 : w;
  const vb = (svg.match(/viewBox="0 0 (\d+) (\d+)"/) || []).slice(1).map(Number);
  if (vb[0] !== expect || vb[1] !== expect) problems.push(`viewBox ${vb.join('x')} != ${expect}: ${rel}`);
  sizes[expect] = (sizes[expect] || 0) + 1;
  const widths = [...svg.matchAll(/stroke-width="([^"]*)"/g)].map(m => m[1]).filter(v => v !== '1');
  if (widths.length && !isLogo) problems.push(`stroke-width ${[...new Set(widths)]}: ${rel}`);
  if (/<text|<image|<!--/.test(svg)) problems.push(`text, image or comment: ${rel}`);
}
// Id uniqueness and references over all SVGs of the set (icons and platform sources).
for (const file of walk(svgRoot).filter(f => f.endsWith('.svg') && !f.includes(`${path.sep}tools${path.sep}`))) {
  const rel = path.relative(svgRoot, file), svg = fs.readFileSync(file, 'utf8');
  const own = [...svg.matchAll(/\bid="([^"]+)"/g)].map(m => m[1]);
  if (new Set(own).size !== own.length) problems.push(`duplicate id within file: ${rel}`);
  for (const id of own) { if (ids.has(id)) problems.push(`id ${id} in ${rel} and ${ids.get(id)}`); ids.set(id, rel); }
  for (const m of svg.matchAll(/url\(#([^)]+)\)/g)) if (!own.includes(m[1])) problems.push(`unresolved #${m[1]}: ${rel}`);
}
const orphans = ['resources', 'plugins', 'sdk'].map(d => path.join(svgRoot, d)).filter(d => fs.existsSync(d))
  .flatMap(walk).filter(f => f.endsWith('.svg') && !covered.has(path.resolve(f))).map(f => path.relative(svgRoot, f));
for (const o of orphans) problems.push(`SVG without bitmap: ${o}`);

console.log(`bitmaps: ${bitmaps.length}, SVG sizes:`, sizes);
console.log(problems.length ? problems.join('\n') : 'no problems');
process.exitCode = problems.length ? 1 : 0;
