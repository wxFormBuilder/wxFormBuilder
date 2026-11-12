// Generates SVG icons for plugins/containers, plugins/forms, plugins/layout and sdk/plugin_interface.
// Usage: node gen_containers_forms_layout_sdk.js [<svgRoot>]   (default: the data/artwork/icons directory)
'use strict';
const fs = require('fs');
const path = require('path');

const root = process.argv[2] || path.resolve(__dirname, '..');
const SW = 'stroke-width="1"';

// ---------- generic helpers ----------
const stopsXml = stops => stops.map(([o, c, a]) =>
  `<stop offset="${o}" stop-color="${c}"${a !== undefined ? ` stop-opacity="${a}"` : ''}/>`).join('');
const lin = (id, stops, x1 = 0, y1 = 0, x2 = 0, y2 = 1, extra = '') =>
  `<linearGradient id="${id}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"${extra}>${stopsXml(stops)}</linearGradient>`;
const linU = (id, stops, x1, y1, x2, y2) => lin(id, stops, x1, y1, x2, y2, ' gradientUnits="userSpaceOnUse"');
const rad = (id, stops, cx = 0.5, cy = 0.5, r = 0.5, extra = '') =>
  `<radialGradient id="${id}" cx="${cx}" cy="${cy}" r="${r}"${extra}>${stopsXml(stops)}</radialGradient>`;
const n = v => +v.toFixed(2);

const fill = (d, c, extra = '') => `<path d="${d}" fill="${c}"${extra}/>`;
const line = (d, c) => `<path d="${d}" fill="none" stroke="${c}" ${SW}/>`;
const shape = (d, f, s, extra = '') => `<path d="${d}" fill="${f}" stroke="${s}" ${SW}${extra}/>`;

// Pixel glyph: rows of '#'/'.' at (x0,y0) -> one filled path of horizontal runs.
function bits(x0, y0, rows, color) {
  let d = '';
  rows.forEach((r, y) => {
    for (let x = 0; x < r.length; x++) {
      if (r[x] !== '#') continue;
      let e = x; while (e + 1 < r.length && r[e + 1] === '#') e++;
      d += `M${x0 + x} ${y0 + y}h${e - x + 1}v1h${-(e - x + 1)}z`;
      x = e;
    }
  });
  return fill(d, color);
}
// Rectangle outline around pixel columns x0..x1 / rows y0..y1 (stroke on pixel centres).
const box = (x0, y0, x1, y1) => `M${x0 + 0.5} ${y0 + 0.5}H${x1 + 0.5}V${y1 + 0.5}H${x0 + 0.5}Z`;
// Same, with radius-1 corners (corner pixel left out).
const rbox = (x0, y0, x1, y1, r = 1) =>
  `M${x0 + 0.5 + r} ${y0 + 0.5}H${x1 + 0.5 - r}A${r} ${r} 0 0 1 ${x1 + 0.5} ${y0 + 0.5 + r}` +
  `V${y1 + 0.5 - r}A${r} ${r} 0 0 1 ${x1 + 0.5 - r} ${y1 + 0.5}H${x0 + 0.5 + r}` +
  `A${r} ${r} 0 0 1 ${x0 + 0.5} ${y1 + 0.5 - r}V${y0 + 0.5 + r}A${r} ${r} 0 0 1 ${x0 + 0.5 + r} ${y0 + 0.5}Z`;
// Filled rectangle covering pixel columns x0..x1 / rows y0..y1.
const area = (x0, y0, x1, y1) => `M${x0} ${y0}H${x1 + 1}V${y1 + 1}H${x0}Z`;

// ---------- shared motifs ----------

// Plain wx panel: light face, white top/left, blue bottom/right.
// full=true: bottom edge starts at column x0 (panel.png); else at x0+1 (collpane/wizpage).
const panel = (content, full = true, x1 = 20, y1 = 20) => [
  fill(`M0.5 0.5H${x1 + 0.5}V${y1 + 0.5}H0.5Z`, '#dde2ec'),
  ...content,
  line(`M0.5 ${y1}V0.5H${full ? x1 + 1 : x1}`, '#ffffff'),
  line(`M${full ? 0 : 1} ${y1 + 0.5}H${x1 + 0.5}V1`, '#708cbd')];

// Splitter window: two panels separated at column m, sash handle at (hx,hy).
const splitter = (L, T, m, R, B, hx, hy) => [
  fill(`M${L + 0.5} ${T + 0.5}H${R + 0.5}V${B + 0.5}H${L + 0.5}Z`, '#dde2ec'),
  line(`M${L + 0.5} ${B + 1}V${T + 0.5}H${m}M${m + 1.5} ${B}V${T + 0.5}H${R}`, '#ffffff'),
  line(`M${L + 1} ${B + 0.5}H${m}M${m + 0.5} ${B}V${T}M${m + 1} ${B + 0.5}H${R + 0.5}V${T + 1}`, '#708cbd'),
  shape(box(hx, hy, hx + 4, hy + 4), '#ffffff', '#708cbd'),
  fill(`M${hx + 2} ${hy + 2}h1v1h-1z`, '#333366')];

// Layout sizer: kräftig blaue Außenkontur + Trennlinien, hellblaue Zellränder.
const sizer = (outer, dividers, cells) => [
  line(cells.map(c => box(...c)).join(''), '#9b9bff'),
  line(outer + dividers, '#0000ff')];

// Tiny "ok" (3 rows) used in choicebook/listbook buttons.
const okSmall = (x, y) => bits(x, y, ['.#..#.#', '#.#.##.', '.#..#.#'], '#000000');

// Large window (frame.png, doc_*_frame.png): 22 px wide, rows 2..19, 2-row title bar.
const bigWinDefs = p =>
  linU(`${p}-tl`, [[0, '#7ba7dc'], [1, '#678dc3']], 1, 0, 21, 0) +
  linU(`${p}-r`, [[0, '#6e90c0'], [1, '#5171a9']], 0, 3, 0, 19) +
  linU(`${p}-b`, [[0, '#5e81b7'], [0.5, '#5e81b7'], [1, '#5171a9']], 1, 0, 21, 0);
const bigWin = (p, client) => [
  fill('M0 2H22V20H0Z', '#a7c4e8'),
  fill('M1 3H21V5H1Z', '#92b9e8'),
  line('M1 5.5H21', `url(#${p}-tl)`),
  fill('M2 3H4V5H2Z', '#cedcf1'),
  fill('M4 3h1v1h-1z', '#a0c1ea'),
  fill('M13 3H15V5H13ZM16 3H17V5H16Z', '#c5c8cd'),
  fill('M19 3H20V5H19Z', '#ff6444'),
  ...client,
  line('M1 2.5H21M0.5 3V19', '#5e81b7'),
  line('M21.5 3V19', `url(#${p}-r)`),
  line('M1 19.5H21', `url(#${p}-b)`)];

// Small window (frame16x16, document, MDI child): 16 px wide at (x,y).
// rows: 'T' top border, 't' title, 'g' title bottom (frame colour), 'm' menu,
//       'w' white client row, 'c' client row with inner gradient, 's' status bar, 'B' bottom border.
const smallWinDefs = p =>
  lin(`${p}-f`, [[0, '#86acdc'], [0.5, '#6e95cb'], [1, '#5171a9']], 0, 0, 1, 1) +
  lin(`${p}-c`, [[0, '#fdfdfd'], [1, '#efefef']], 0, 0, 1, 1) +
  '';
function smallWin(p, x, y, rows) {
  const h = rows.length, out = [fill(`M${x} ${y}h16v${h}h-16z`, `url(#${p}-f)`)];
  const at = ch => y + rows.indexOf(ch);
  const t = at('t');
  out.push(fill(`M${x + 1} ${t}h14v1h-14z`, '#92b9e8'),
    fill(`M${x + 1} ${t}h1v1h-1z`, '#cedcf1'), fill(`M${x + 2} ${t}h1v1h-1z`, '#a0c1ea'),
    fill(`M${x + 9} ${t}h1v1h-1zM${x + 11} ${t}h1v1h-1z`, '#c5c8cd'),
    fill(`M${x + 13} ${t}h1v1h-1z`, '#ff6444'), fill(`M${x + 14} ${t}h1v1h-1z`, '#a7c4e8'));
  out.push(line(`M${x + 1} ${at('m') + 0.5}h14`, '#d4d4d4'));
  const w0 = at('w'), w1 = y + rows.lastIndexOf('w') + 1, c0 = at('c'), c1 = y + rows.lastIndexOf('c') + 1;
  out.push(fill(`M${x + 1} ${w0}h14v${w1 - w0}h-14z`, '#ffffff'),
    fill(`M${x + 2} ${c0}h12v${c1 - c0}h-12z`, `url(#${p}-c)`));
  const s = at('s') + 0.5;
  out.push(line(`M${x + 1} ${s}h13`, '#c8c8c8'), line(`M${x + 14} ${s}h1`, '#afafaf'));
  return out;
}
const FRAME16 = 'Ttgmwccccccwsb'.replace('b', 'B');
const MDICHILD = 'TtmwcccccsB';

// Dialog window (dialog.png, wizard.png): rows 2..20, soft XP look with text field and buttons.
const dialogDefs = p =>
  lin(`${p}-t`, [[0, '#86b0e4'], [0.3, '#8fb6e6'], [0.6, '#80abe0'], [1, '#9abbe3']]) +
  linU(`${p}-l`, [[0, '#85aada'], [1, '#6c8fc2']], 0, 3, 0, 20) +
  linU(`${p}-r`, [[0, '#8da8d2'], [1, '#708cbd']], 0, 3, 0, 20) +
  linU(`${p}-bt`, [[0, '#5a7bab'], [1, '#4f6b9c']], 1, 0, 21, 0) +
  lin(`${p}-k`, [[0, '#7094d6'], [1, '#a0b6e0']]);
const dialog = (p, extra = []) => [
  fill('M0 2H22V21H0Z', '#c9d8eb'),
  fill('M1 2H21V6H1Z', `url(#${p}-t)`),
  fill('M1 6H21V20H1Z', '#f4f2f0'),
  line('M2 6.5H20', '#e8edf3'),
  line('M1.5 6V19', '#c4d3e6'),
  line('M20.5 6V19', '#e2e5ea'),
  line('M1 19.5H21', '#b4c3d9'),
  line('M4 7.5H18', '#dad9d7'),
  line('M4.5 12V8.5H18', '#cbcbcb'),
  fill('M5 9H19V12H5Z', '#ffffff'),
  fill('M3 14H9V17H3ZM13 14H19V17H13Z', '#d4dce8'),
  fill('M4 15H8V17H4ZM14 15H18V17H14Z', `url(#${p}-k)`),
  ...extra,
  line('M0.5 3V20', `url(#${p}-l)`),
  line('M21.5 3V20', `url(#${p}-r)`),
  line('M1 20.5H21', `url(#${p}-bt)`)];

// Magic wand (wizard, wizpage): sparkle star + black stick with silver tip, offset (dx,dy).
const wandDefs = (p, dx, dy) => {
  const cx = 7 + dx, cy = 7 + dy, mx = 12.5 + dx, my = 13 + dy;
  return rad(`${p}-st`, [[0, '#fffbe0'], [0.15, '#fff78c'], [0.3, '#f7e742'], [0.5, '#ffa518'],
    [0.7, '#f77329'], [1, '#d81c08']], cx, cy, 5.5, ' gradientUnits="userSpaceOnUse"') +
    linU(`${p}-wk`, [[0, '#101010'], [0.5, '#101010'], [0.78, '#4a4a4a'], [1, '#8c8c8c']], mx - 1, my + 1, mx + 1, my - 1) +
    linU(`${p}-wt`, [[0, '#6b6b6b'], [0.45, '#ffffff'], [1, '#a5a5a5']], mx + 1.5, my + 2.5, mx + 3.5, my + 0.5);
};
function wand(p, dx, dy) {
  const cx = 7 + dx, cy = 7 + dy;
  const axis = [6, 5.5, 5, 5]; // right, down, left, up
  let d = '';
  for (let i = 0; i < 16; i++) {
    const a = i * Math.PI / 8;
    const r = i % 4 === 0 ? axis[i / 4] : (i % 2 === 0 ? 3.6 : 2.3);
    d += `${i ? 'L' : 'M'}${n(cx + r * Math.cos(a))} ${n(cy + r * Math.sin(a))}`;
  }
  const P = (x, y) => `${x + dx} ${y + dy}`;
  return [
    fill(`M${P(10, 8.5)}L${P(8, 10.5)}L${P(13, 15.5)}L${P(15, 13.5)}Z`, `url(#${p}-wk)`),
    fill(`M${P(15, 13.5)}L${P(13, 15.5)}L${P(15, 17.5)}L${P(17, 15.5)}Z`, `url(#${p}-wt)`),
    fill(d + 'Z', `url(#${p}-st)`)];
}

// Mauszeiger (wie plugins/common), Spitze in Pixelzelle (x,y).
const cursor = (p, x, y) => shape(`M${x + .5} ${y + .5}L${x + 6.5} ${y + 6.5}V${y + 7.5}H${x + 4.5}L${x + 6.5} ${y + 11.5}H${x + 4.5}L${x + 2.5} ${y + 8.5}L${x + .5} ${y + 10.5}Z`,
  `url(#${p}-cf)`, `url(#${p}-cs)`, ' stroke-linejoin="round"');
// Toolbar-Symbole (wie plugins/common): Seite, Ordner, Diskette.
const tbPage = (p, x, y) => shape(`M${x + .5} ${y + .5}H${x + 3.5}L${x + 5.5} ${y + 2.5}V${y + 6.5}H${x + .5}Z`, `url(#${p}-pg)`, '#666699');
const tbFolder = (p, x, y) =>
  shape(`M${x + .5} ${y + 5.5}V${y + 1.5}L${x + 1.5} ${y + .5}H${x + 2.5}L${x + 3.5} ${y + 1.5}V${y + 2.5}H${x + 6.5}L${x + 7.5} ${y + 3.5}V${y + 5.5}L${x + 6.5} ${y + 6.5}H${x + 1.5}Z`, `url(#${p}-fo)`, '#cc9900') +
  line(`M${x + 1.5} ${y + 3}V${y + 6}`, '#cc9900');
function tbDisk(p, x, y) {
  const d = `M${x + 1.5} ${y + .5}H${x + 4.5}A1 1 0 0 1 ${x + 5.5} ${y + 1.5}V${y + 5.5}A1 1 0 0 1 ${x + 4.5} ${y + 6.5}H${x + 1.5}A1 1 0 0 1 ${x + .5} ${y + 5.5}V${y + 1.5}A1 1 0 0 1 ${x + 1.5} ${y + .5}Z`;
  return fill(d, `url(#${p}-dk)`) + fill(`M${x + 1} ${y + 4}h4v2h-4z`, '#c8c8c8') + fill(`M${x + 2} ${y + 1}h2v2h-2z`, '#ffffff') +
    fill(`M${x + 2} ${y + 5}h1v1h-1z`, '#48a5e9') + line(d, '#003399');
}

// ---------- icons ----------
// key: output path relative to root (without .svg) -> [size, fn(prefix) => [defs, ...body]]
const icons = {
  // ===== containers =====
  'plugins/containers/icons/auinotebook': [22, p => [
    lin(`${p}-s`, [[0, '#b8b8b8'], [1, '#d2d2d2']]) + lin(`${p}-t`, [[0, '#fffbf0'], [1, '#ebebeb']]),
    fill('M1.5 1.5H20.5V20.5H1.5Z', '#dde2ec'),
    line('M2 2.5H20M2 7.5H20', '#9f9f9f'),
    fill('M2 3H20V6H2Z', `url(#${p}-s)`),
    fill('M4 4H12V7H4Z', `url(#${p}-t)`),
    line('M2 6.5H3.5V4.5A1 1 0 0 1 4.5 3.5H11.5A1 1 0 0 1 12.5 4.5V6.5H20M2 8.5H20', '#525252'),
    bits(8, 4, ['#.#', '.#.', '#.#'], '#393939'),
    line('M1.5 1.5H20.5V20.5H1.5Z', '#708cbd')]],

  'plugins/containers/icons/choicebook': [22, p => [
    lin(`${p}-e`, [[0, '#ffffff'], [1, '#e2ffff']], 0, 0, 1, 1) +
    lin(`${p}-d`, [[0, '#9ecfff'], [1, '#c9efff']], 0, 0, 1, 1),
    fill('M1.5 1.5H20.5V20.5H1.5Z', '#dde2ec'),
    fill('M2 2H13V8H2Z', `url(#${p}-e)`),
    fill('M14 2H20V8H14Z', `url(#${p}-d)`),
    fill('M15 4H19V5H18V6H16V5H15Z', '#808080'),
    bits(3, 3, ['.##.', '...#', '.###', '#..#', '.###'], '#000000'),
    shape(box(3, 10, 18, 13), '#ffffff', '#848284'),
    fill('M10 14H19V19H10Z', '#d2d2d2'),
    line('M10.5 18V14.5H18', '#ebebeb'),
    line('M10 18.5H18.5V14', '#848284'),
    okSmall(11, 15),
    line('M1.5 1.5H20.5V20.5H1.5ZM2 8.5H20M13.5 2V8', '#333366')]],

  'plugins/containers/icons/collpane': [22, () => ['',
    ...panel([
      fill('M2 3H7L4.5 6Z', '#3e4043'),
      bits(8, 2, ['###......#.', '.#.#####.##', '.#.##.#..#.', '.#.#####.##'], '#545454'),
      line('M5 9.5H18', '#ced2db'),
      fill('M5 11H18V13H5Z', '#d5dae3'),
      line('M5 14.5H19', '#d8dde7'),
      line('M5 15.5H19', '#c6cad2'),
      line('M5 16.5H10', '#d9dee8')], false)]],

  'plugins/containers/icons/listbook': [22, p => [
    lin(`${p}-l`, [[0, '#ffffff'], [0.45, '#ffffff'], [0.55, '#f9ffff'], [1, '#e3ffff']]),
    fill('M1.5 1.5H20.5V20.5H1.5Z', '#dde2ec'),
    fill('M2 2H9V20H2Z', `url(#${p}-l)`),
    line('M3 5.5H8M3 15.5H8', '#333366'),
    fill('M2 9H9V12H2Z', '#0066cc'),
    line('M3 10.5H8', '#ffffff'),
    shape('M20 3.5H11.5V6.5H20M20 8.5H11.5V11.5H20', '#ffffff', '#808080'),
    fill('M12 15H19V18H12Z', '#ecffff'),
    line('M12 14.5H19M11.5 15V18.5H19.5V15', '#ebebeb'),
    line('M12 19.5H19', '#848284'),
    okSmall(12, 15),
    line('M1.5 1.5H20.5V20.5H1.5ZM9.5 2V20', '#333366')]],

  'plugins/containers/icons/notebook': [22, () => ['',
    fill('M1 7H21V19H1Z', '#dde2ec'),
    fill('M1 3H7V7H1Z', '#d8d8d8'),
    fill('M7 3H18V7H7Z', '#989898'),
    line('M1.5 18V3.5H7M8 3.5H12M13 3.5H17M7 7.5H20', '#ffffff'),
    line('M7.5 3V7M12.5 3V7M17.5 3V7M1 18.5H20.5V7', '#708cbd')]],

  'plugins/containers/icons/panel': [22, () => ['', ...panel([])]],

  'plugins/containers/icons/scrollwin': [22, p => [
    lin(`${p}-u`, [[0, '#deffff'], [1, '#8dc7ec']], 0, 0, 1, 1) +
    lin(`${p}-m`, [[0, '#cccbcc'], [1, '#ebeaeb']], 0, 0, 1, 0) +
    lin(`${p}-d`, [[0, '#d3f7fd'], [1, '#7abae9']], 0, 0, 1, 1),
    fill('M1.5 1.5H20.5V20.5H1.5Z', '#dde2ec'),
    fill('M14 3H20V8H14Z', `url(#${p}-u)`),
    fill('M14 9H20V14H14Z', `url(#${p}-m)`),
    fill('M14 15H20V20H14Z', `url(#${p}-d)`),
    fill('M16 4H18V5H19V6H15V5H16ZM15 17H19V18H18V19H16V18H15Z', '#808080'),
    line('M1.5 20V1.5H21', '#fbfbfb'),
    line('M1 20.5H20.5V2.5H13.5V20M14 8.5H20M14 14.5H20', '#708cbd')]],

  'plugins/containers/icons/splitter_window': [22, () => ['', ...splitter(1, 1, 11, 20, 20, 8, 13)]],
  'plugins/containers/icons/splitter_window16x16': [16, () => ['', ...splitter(0, 0, 8, 15, 15, 5, 8)]],

  // ===== forms =====
  'plugins/forms/icons/dialog': [22, p => [dialogDefs(p), ...dialog(p)]],

  'plugins/forms/icons/wizard': [22, p => [dialogDefs(p) + wandDefs(p, 1, 1), ...dialog(p, wand(p, 1, 1))]],

  'plugins/forms/icons/wizpage': [22, p => [wandDefs(p, 0, 0), ...panel(wand(p, 0, 0), false)]],

  'plugins/forms/icons/frame': [22, p => [
    bigWinDefs(p) + lin(`${p}-c`, [[0, '#fdfdfd'], [1, '#eeeeee']], 0, 0, 1, 1) +
    linU(`${p}-s`, [[0, '#d6d6d6'], [1, '#c2c2c2']], 2, 0, 20, 0),
    ...bigWin(p, [
      line('M1 6.5H21', '#d4d4d4'),
      fill('M1 7H21V18H1Z', '#ffffff'),
      fill('M2 8H20V16H2Z', `url(#${p}-c)`),
      line('M2 18.5H20', `url(#${p}-s)`),
      line('M1 18.5h1M20 18.5h1', '#afafaf')])]],

  'plugins/forms/icons/doc_parent_frame': [22, p => [bigWinDefs(p),
    ...bigWin(p, [fill('M1 6H21V19H1Z', '#787878')])]],

  'plugins/forms/icons/doc_mdi_parent_frame': [22, p => [bigWinDefs(p) + smallWinDefs(p),
    ...bigWin(p, [fill('M1 6H21V19H1Z', '#787878'), ...smallWin(p, 2, 7, MDICHILD)])]],

  'plugins/forms/icons/frame16x16': [16, p => [smallWinDefs(p), ...smallWin(p, 0, 1, FRAME16)]],

  'plugins/forms/icons/document': [22, p => [smallWinDefs(p),
    ...smallWin(p, 0, 0, FRAME16), ...smallWin(p, 4, 6, FRAME16)]],

  'plugins/forms/icons/panel': [22, () => ['', ...panel([])]],

  // menubar/toolbar: identische PNGs wie plugins/common -> gleiche Geometrie wie gen_common.js
  'plugins/forms/icons/menubar': [22, p => [
    lin(`${p}-cf`, [[0, '#ffffff'], [1, '#cfd0f6']], 0, 0, 1, 1) + lin(`${p}-cs`, [[0, '#a5a5ff'], [1, '#6f86ca']], 0, 0, 1, 1),
    fill('M0 1H21V6H0Z', '#ffffff'),
    fill('M1 2H8V5H1Z', '#0066cc'),
    fill('M2 3h5v1h-5z', '#ffffff'),
    fill('M11 7h1v14h-10v-1h9z', '#c0c0c0'),
    fill('M1.5 5.5H10.5V19.5H1.5Z', '#ffffff'),
    fill('M9 3h4v1h-4zM15 3h5v1h-5zM3 7h6v1h-6zM3 9h4v1h-4zM3 11h6v1h-6zM3 15h4v1h-4zM3 17h6v1h-6z', '#0066cc'),
    line('M21 1.5H0.5V5.5H21M1.5 6V19.5H10.5V6M2 13.5H10', '#333366'),
    cursor(p, 13, 8)]],

  'plugins/forms/icons/toolbar': [22, p => [
    lin(`${p}-pg`, [[0, '#ffffff'], [1, '#c4e8ff']], 0, 0, 1, 1) +
    lin(`${p}-fo`, [[0, '#fffd00'], [1, '#ffda00']], 0, 0, 1, 1) +
    lin(`${p}-dk`, [[0, '#0083d7'], [1, '#48a5e9']], 0, 0, 1, 0) +
    lin(`${p}-tb`, [[0, '#ece9d8'], [0.7, '#ebe8d7'], [1, '#e1ddce']]),
    fill('M0 7H22V16H0Z', `url(#${p}-tb)`),
    line('M0 6.5H22M0 16.5H22', '#aca899'),
    tbPage(p, 0, 8), tbFolder(p, 7, 8), tbDisk(p, 16, 8)]],

  // ===== layout =====
  'plugins/layout/icons/flex_grid_sizer': [22, () => ['',
    ...sizer(rbox(1, 1, 19, 19), 'M2 6.5H19M10.5 2V19',
      [[2, 2, 9, 5], [11, 2, 18, 5], [2, 7, 9, 18], [11, 7, 18, 18]])]],
  'plugins/layout/icons/grid_bag_sizer': [22, () => ['',
    ...sizer(rbox(1, 1, 19, 19), 'M10.5 2V19M2 10.5H10M11 6.5H19M11 14.5H19',
      [[2, 2, 9, 9], [2, 11, 9, 18], [11, 2, 18, 5], [11, 7, 18, 13], [11, 15, 18, 18]])]],
  'plugins/layout/icons/grid_sizer': [22, () => ['',
    ...sizer(rbox(1, 1, 19, 19), 'M10.5 2V19M2 10.5H19',
      [[2, 2, 9, 9], [11, 2, 18, 9], [2, 11, 9, 18], [11, 11, 18, 18]])]],
  'plugins/layout/icons/sizer': [22, () => ['',
    ...sizer(rbox(1, 1, 19, 19), 'M2 7.5H19M2 13.5H19',
      [[2, 2, 18, 6], [2, 8, 18, 12], [2, 14, 18, 18]])]],
  'plugins/layout/icons/sizer16x16': [16, () => ['',
    ...sizer(rbox(1, 0, 14, 15), 'M2 5.5H14M2 10.5H14',
      [[2, 1, 13, 4], [2, 6, 13, 9], [2, 11, 13, 14]])]],
  'plugins/layout/icons/wrap_sizer': [22, () => ['',
    ...sizer('M2.5 1.5H18.5A1 1 0 0 1 19.5 2.5V12.5A1 1 0 0 1 18.5 13.5H10.5V18.5A1 1 0 0 1 9.5 19.5H2.5A1 1 0 0 1 1.5 18.5V2.5A1 1 0 0 1 2.5 1.5Z',
      'M2 7.5H19M2 13.5H10',
      [[2, 2, 18, 6], [2, 8, 18, 12], [2, 14, 9, 18]])]],
  'plugins/layout/icons/staticbox_sizer': [22, () => ['',
    line('M5 3.5H3.5A2 2 0 0 0 1.5 5.5V17.5A2 2 0 0 0 3.5 19.5H18.5A2 2 0 0 0 20.5 17.5V5.5A2 2 0 0 0 18.5 3.5H12', '#b7b79b'),
    fill('M6 2H11V4H6Z', '#003399'),
    ...sizer(rbox(3, 5, 18, 17), 'M4 11.5H18', [[4, 6, 17, 10], [4, 12, 17, 16]])]],
  'plugins/layout/icons/stddialogbuttonsizer': [22, p => [
    lin(`${p}-k`, [[0, '#f4fbff'], [1, '#c7eaff']]),
    ...sizer(rbox(0, 3, 21, 18), '', [[1, 4, 20, 17]]),
    shape(rbox(3, 7, 18, 14), `url(#${p}-k)`, '#003399'),
    bits(7, 9, ['.##..#..', '#..#.#.#', '#..#.##.', '.##..#.#'], '#000000')]],
  'plugins/layout/icons/spacer': [22, () => ['',
    line('M2.5 7V16M19.5 7V16', '#929292'),
    fill('M4 11.5L7 9V14ZM18 11.5L15 9V14ZM7 11H15V12H7Z', '#929292'),
    line('M1.5 6V15M18.5 6V15', '#000000'),
    fill('M3 10.5L6 8V13ZM17 10.5L14 8V13ZM6 10H14V11H6Z', '#000000')]],

  // ===== sdk =====
  // Question mark as a lettering glyph: 3 px red stroke with a 1 px dark outline (a 5 px stroke below).
  'sdk/plugin_interface/default': [22, p => {
    const red = require('./palette.js').MATERIALS.red, dark = '#2a1010';
    const hook = 'M7.5 6.5A3.5 3.5 0 1 1 13.47 8.97L11 11.4V13.75';
    return [linU(`${p}-q`, [[0, red.top], [1, red.bottom]], 0, 3, 0, 20),
      `<path d="${hook}" fill="none" stroke="${dark}" stroke-width="5" stroke-linecap="round" stroke-linejoin="round"/>`,
      `<path d="${hook}" fill="none" stroke="url(#${p}-q)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/>`,
      `<circle cx="11" cy="19.25" r="2.5" fill="${dark}"/><circle cx="11" cy="19.25" r="1.5" fill="url(#${p}-q)"/>`];
  }],
};

// ---------- write ----------
let count = 0;
for (const [rel, [size, fn]] of Object.entries(icons)) {
  const folder = path.basename(path.dirname(rel)) === 'icons'
    ? path.basename(path.dirname(path.dirname(rel))) : path.basename(path.dirname(rel));
  const prefix = `${folder}-${path.basename(rel)}`;
  const [defs, ...body] = fn(prefix);
  const svg =
    `<svg xmlns="http://www.w3.org/2000/svg" width="${size}" height="${size}" viewBox="0 0 ${size} ${size}">\n` +
    (defs ? `<defs>${defs}</defs>\n` : '') +
    body.join('\n') + '\n</svg>\n';
  const file = path.join(root, rel + '.svg');
  fs.mkdirSync(path.dirname(file), { recursive: true });
  fs.writeFileSync(file, svg);
  count++;
}
console.log(`${count} icons written to ${root}`);
