// Usage: node preview.js <out.png> <png1> <svg1> [<png2> <svg2> ...]
// Renders original PNG (8x, pixelated) | SVG at 8x | SVG at native size, via headless Edge.
const fs = require('fs'), path = require('path'), { execFileSync } = require('child_process');
const [out, ...rest] = process.argv.slice(2);
const url = p => 'file:///' + path.resolve(p).split(String.fromCharCode(92)).join('/');
let h = '<html><body style="margin:0;background:#c8c8c8;font:11px Arial;display:grid;grid-template-columns:repeat(3,1fr);gap:4px;width:1200px">';
for (let i = 0; i < rest.length; i += 2) {
  const png = rest[i], svg = rest[i + 1];
  h += `<div style="background:#fff;padding:3px"><div>${path.basename(path.dirname(path.dirname(png)))}/${path.basename(png)}</div>` +
    `<img id="p${i}" src="${url(png)}" style="image-rendering:pixelated" onload="this.width=this.naturalWidth*7">` +
    ` <img id="s${i}" src="${url(svg)}" onload="this.width=document.getElementById('p${i}').naturalWidth*7||154"> ` +
    `<img src="${url(svg)}" onload="this.width=document.getElementById('p${i}').naturalWidth"></div>`;
}
const html = out.replace(/\.png$/, '.html');
fs.writeFileSync(html, h + '</body></html>');
const rows = Math.ceil(rest.length / 6);
execFileSync('C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',
  ['--headless', '--disable-gpu', '--allow-file-access-from-files', '--force-device-scale-factor=1',
   `--window-size=1200,${rows * 270 + 20}`, `--screenshot=${path.resolve(out)}`, url(html)],
  { stdio: 'ignore', timeout: 60000 });
console.log('written', out);
