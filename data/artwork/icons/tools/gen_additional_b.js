// Generates SVG icons for plugins/additional/icons (second half: propgridpage .. treelistctrlcolumn).
// Stroke width 1, outlines on half-pixel coordinates, fills on integer edges.
// Usage: node gen_additional_b.js [<svgRoot>]   (default: the data/artwork/icons directory)
'use strict';
const fs = require('fs');
const path = require('path');

const outDir = path.join(process.argv[2] || path.resolve(__dirname, '..'), 'plugins', 'additional', 'icons');
const SW = 'stroke-width="1"';

// ---------- generic helpers (as in resources22.js) ----------
const stopsXml = stops => stops.map(([o, c, a]) =>
  `<stop offset="${o}" stop-color="${c}"${a !== undefined ? ` stop-opacity="${a}"` : ''}/>`).join('');
const lin = (id, stops, x1 = 0, y1 = 0, x2 = 0, y2 = 1, extra = '') =>
  `<linearGradient id="${id}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"${extra}>${stopsXml(stops)}</linearGradient>`;
const rad = (id, stops, cx = 0.5, cy = 0.5, r = 0.5, extra = '') =>
  `<radialGradient id="${id}" cx="${cx}" cy="${cy}" r="${r}"${extra}>${stopsXml(stops)}</radialGradient>`;
const fillP = (d, fill, extra = '') => `<path d="${d}" fill="${fill}"${extra}/>`;
const strokeP = (d, stroke, extra = '') => `<path d="${d}" fill="none" stroke="${stroke}" ${SW}${extra}/>`;

// Pixel set -> minimal rectangle path (horizontal runs, merged vertically when identical).
function pixPath(pixels) {
  const rows = new Map();
  for (const [x, y] of pixels) {
    if (!rows.has(y)) rows.set(y, new Set());
    rows.get(y).add(x);
  }
  const runs = [];
  for (const [y, xs] of [...rows.entries()].sort((a, b) => a[0] - b[0])) {
    const s = [...xs].sort((a, b) => a - b);
    for (let i = 0; i < s.length;) {
      let j = i;
      while (j + 1 < s.length && s[j + 1] === s[j] + 1) j++;
      runs.push({ x: s[i], w: s[j] - s[i] + 1, y, h: 1 });
      i = j + 1;
    }
  }
  const merged = [];
  for (const r of runs) {
    const m = merged.find(o => o.x === r.x && o.w === r.w && o.y + o.h === r.y);
    if (m) m.h++; else merged.push({ ...r });
  }
  return merged.map(r => `M${r.x} ${r.y}h${r.w}v${r.h}h-${r.w}z`).join('');
}

// ---------- pixel font (6 px high, shared by spin controls, time picker, toggle button) ----------
const GLYPHS = {
  '1': ['.#.', '##.', '.#.', '.#.', '.#.', '###'],
  '0': ['.##.', '#..#', '#..#', '#..#', '#..#', '.##.'],
  'O': ['.##.', '#..#', '#..#', '#..#', '#..#', '.##.'],
  'N': ['#...#', '##..#', '#.#.#', '#.#.#', '#..##', '#...#'],
  '.': ['.', '.', '.', '.', '.', '#'],
  ':': ['.', '.', '#', '.', '.', '#'],
  '|': ['#', '#', '#', '#', '#', '#'],
};
// items: [[char, x], ...] placed at top row y
function pixText(items, y) {
  const px = [];
  for (const [ch, x0] of items)
    GLYPHS[ch].forEach((row, dy) => [...row].forEach((c, dx) => { if (c === '#') px.push([x0 + dx, y + dy]); }));
  return pixPath(px);
}

// ---------- spin buttons / small arrows (shared) ----------
const ARROW = '#808080';
const SPIN_LIGHT = '#deffff', SPIN_DARK = '#7abae9';
// up arrow: 2 px tip at (x..x+1, y), 4 px base at (x-1..x+2, y+1)
const arrowUp = (x, y) => `M${x} ${y}h2v1h1v1h-4v-1h1z`;
const arrowDown = (x, y) => `M${x - 1} ${y}h4v1h-1v1h-2v-1h-1z`;
// left/right arrows: 4 px base column, 2 px tip column
const arrowLeft = (x, y) => `M${x} ${y}h1v4h-1v-1h-1v-2h1z`;      // base column x, tip x-1
const arrowRight = (x, y) => `M${x} ${y}h1v1h1v2h-1v1h-1z`;       // base column x, tip x+1

// Spin control: text field + spin button area, frame #333366 (rows 6..15, cols 1..20).
const spinCtrlDefs = p =>
  lin(`${p}-f`, [[0, '#ffffff'], [1, '#dfffff']], 0, 0, 1, 1) +
  lin(`${p}-s`, [[0, '#9bcdff'], [0.5, '#e5ffff'], [1, '#9bcdff']]);
const spinCtrl = (p, textD) => [
  fillP('M1.5 6.5H13.5V15.5H1.5Z', `url(#${p}-f)`),
  fillP('M13.5 6.5H20.5V15.5H13.5Z', `url(#${p}-s)`),
  fillP(arrowUp(16, 8) + arrowDown(16, 12), ARROW),
  fillP(textD, '#000000'),
  strokeP('M1.5 6.5H20.5V15.5H1.5ZM13.5 7V15', '#333366')];

// ---------- ribbon motifs (shared) ----------
const RD = '#333366', RB = '#0066cc';
// ribbon header with three dashes: dark line at row y, dashes at y+2, dark line at y+4
const ribbonHead = y => strokeP(`M2 ${y + 2.5}H7M9 ${y + 2.5}H14M16 ${y + 2.5}H21`, RD);
// frame open to the right: dark rows y0 and y1, left column x0, separator under header (y0+4)
const ribbonFrame = (y0, y1, extra = '') =>
  strokeP(`M22 ${y0 + 0.5}H0.5V${y1 + 0.5}H22M1 ${y0 + 4.5}H22${extra}`, RD);
const ribbonBg = (y0, y1) => fillP(`M0.5 ${y0 + 0.5}H22V${y1 + 0.5}H0.5Z`, '#ffffff');
// ribbon bar with tabs (22 and 16 px variant); top row y0, width w
function ribbonBar(y0, w) {
  const b = [2, 6, 10, 16, 20].filter(x => x < w);
  return [
    fillP(`M0.5 ${y0 + 0.5}H${w}V${y0 + 13.5}H0.5Z`, '#ffffff'),
    strokeP(`M1.5 ${y0 + 1.5}H7.5V${y0 + 4.5}H1.5Z`, RB),
    fillP(`M9 ${y0 + 2}h5v2h-5z` + b.map(x => `M${x} ${y0 + 7}h${Math.min(3, w - x)}v5h-${Math.min(3, w - x)}z`).join(''), RB),
    strokeP(`M${w} ${y0 + 0.5}H0.5V${y0 + 13.5}H${w}M1 ${y0 + 5.5}H${w}M14.5 ${y0 + 6}V${y0 + 13}`, RD)];
}
// standard ribbon button frame (rows 4..18): background, header, button content, frame
const ribbonBtn = (content, y1 = 18, extra = '') =>
  [ribbonBg(4, y1), ...content, ribbonHead(4), ribbonFrame(4, y1, extra)];

// ---------- tree (shared) ----------
const TREE = '#555753';
// node: coloured centre pixel with a dark 4-neighbour cross
const nodeCross = (x, y) => `M${x - 1} ${y + 0.5}H${x + 2}M${x + 0.5} ${y - 1}V${y + 2}`;
const nodeDot = (x, y) => `M${x} ${y}h1v1h-1z`;
// lines: path data of 1 px tree lines; nodes: [[x, y, colour], ...]
function tree(lines, nodes) {
  const out = [strokeP(lines + nodes.map(([x, y]) => nodeCross(x, y)).join(''), TREE)];
  const byColor = {};
  for (const [x, y, c] of nodes) byColor[c] = (byColor[c] || '') + nodeDot(x, y);
  for (const [c, d] of Object.entries(byColor)) out.push(fillP(d, c));
  return out;
}

// tree list control body (used by treelistctrl and, clipped, by treelistctrlcolumn)
const treeListDefs = p =>
  lin(`${p}-b`, [[0.55, '#ffffff'], [1, '#e0ffff']], 0, 0, 1, 0) +
  lin(`${p}-h`, [[0, '#ebeadb'], [0.5, '#e6e4d5'], [1, '#d4d1c2']]);
const treeListBody = p => [
  fillP('M1.5 1.5H21.5V21.5H1.5Z', `url(#${p}-b)`),
  fillP('M1 2H21V6H1Z', `url(#${p}-h)`),
  fillP('M14 3h1v2h-1z', '#cbc7b8'),
  fillP('M15 3h1v2h-1z', '#ffffff'),
  fillP('M7 10H21V13H7Z', RB),
  fillP('M8 11h2v1h-2zM15 11h4v1h-4z', '#ffffff'),
  strokeP('M7 7.5H11M15 7.5H20M9 15.5H11M15 15.5H20M9 18.5H11M7 20.5H11M15 20.5H20', RD),
  ...tree('M4.5 6V21M5 11.5H6M5 15.5H7M7.5 16V19M8 18.5H9M5 20.5H6', [[4, 7, '#00aaf0'], [7, 15, '#00aaf0']])];

// ---------- icons ----------
const icons = {
  propgridpage: () => ['',
    fillP('M1 5H21V22H1Z', '#858585'),
    fillP('M3 7H21V15H3ZM3 17H21V22H3Z', '#ffffff'),
    strokeP('M3 9.5H21M3 12.5H21M3 19.5H21M11.5 7V15M11.5 17V22', '#b8b8b8'),
    fillP(pixPath([[3, 5], [6, 5], [2, 6], [4, 6], [5, 6], [3, 15], [6, 15], [2, 16], [4, 16], [5, 16]]), '#2c2c2c'),
    fillP('M1 1h3v2h-3zM5 0h3v3h-3zM1 4h3v1h-3z', '#000000'),
    fillP('M2 1h1v1h-1zM6 1h1v1h-1z', '#dedede'),
    strokeP('M0.5 22V0.5H4.5V4.5H21.5V22', '#ff0018')],

  ribbon_bar: () => ['', ...ribbonBar(4, 22)],
  ribbon_bar16x16: () => ['', ...ribbonBar(1, 16)],

  ribbon_button: () => ['', ...ribbonBtn([fillP('M3 10H14V17H3Z', RB)])],
  ribbon_button_dropdown: () => ['', ...ribbonBtn([
    fillP('M3 10H14V11H3ZM3 12H14V17H3Z', RB),
    fillP('M3.5 12H13.5L8.5 17Z', '#ffffff')])],
  ribbon_button_hybrid: () => ['', ...ribbonBtn([
    fillP('M3 10H14V17H3Z', RB),
    fillP('M5.5 14H11.5L8.5 17Z', '#ffffff'),
    strokeP('M3 13.5H14', RD)])],
  ribbon_button_toggle: () => ['', ...ribbonBtn([
    fillP('M3 10H14V17H3Z', RB),
    strokeP('M4.5 16V11.5H13', '#ffffff')])],
  ribbon_buttonbar: () => ['', ...ribbonBtn([
    fillP('M3 10H19V16H3Z', RB),
    strokeP('M6.5 10V12M10.5 10V12M14.5 10V12M6.5 14V16M10.5 14V16M14.5 14V16', '#ffffff')], 17, 'M20.5 9V17')],
  ribbon_gallery: () => ['', ...ribbonBtn([
    fillP('M3 10h3v3h-3zM7 10h3v3h-3zM11 10h3v3h-3zM15 10h3v3h-3z' +
          'M3 14h3v3h-3zM7 14h3v3h-3zM11 14h3v3h-3zM15 14h3v3h-3z', RB)], 18, 'M20.5 9V18')],

  ribbon_page: () => ['',
    fillP('M0.5 3.5H22V16H0.5Z', '#ffffff'),
    fillP('M1 4H15V11H1ZM3 6V9H13V6Z', RB, ' fill-rule="evenodd"'),
    strokeP('M22 3.5H0.5V16M1 11.5H22', RD)],

  ribbon_panel: () => ['',
    fillP('M0.5 3.5H22V7.5H0.5Z', '#ffffff'),
    fillP('M0.5 7.5H19.5V16.5H0.5ZM3 10V14H17V10Z', RB, ' fill-rule="evenodd"'),
    fillP('M3 10H17V14H3Z', '#ffffff'),
    ribbonHead(3),
    strokeP('M22 3.5H0.5V16.5H19.5V8M1 7.5H22', RD)],

  richtextctrl: p => [
    lin(`${p}-bg`, [[0, '#fdfdfd'], [1, '#f2f2f2']]) +
    lin(`${p}-y`, [[0, '#ffcc00'], [1, '#ffed64']], 0, 0, 1, 1) +
    lin(`${p}-g`, [[0, '#0e7407'], [1, '#5bc62e']], 0, 0, 0.6, 1) +
    lin(`${p}-t`, [[0, '#e6e6e6'], [0.45, '#acacac'], [1, '#dedede']]),
    fillP('M2.5 0.5H19.5V21.5H2.5Z', `url(#${p}-bg)`),
    fillP('M13 2H18V5H13ZM13 6H18V9H13ZM4 10H18V15H4ZM4 16H18V20H4Z', `url(#${p}-t)`),
    `<path d="M4.5 2.5H11.5V9.5H4.5Z" fill="url(#${p}-y)" stroke="#cc9900" ${SW}/>`,
    fillP('M12 4.5V10H7.5Z', `url(#${p}-g)`),
    strokeP('M2.5 0.5H19.5V21.5H2.5Z', '#3e9ade')],

  scintilla: p => [
    lin(`${p}-pg`, [[0, '#fdfdfd'], [1, '#e6e6e6']], 0, 0, 1, 1) +
    lin(`${p}-k`, [[0, '#b4b4b4'], [1, '#f2f2f2']], 0, 1, 1, 0),
    `<path d="M2.5 0.5H14.5L19.5 5.5V21.5H2.5Z" fill="url(#${p}-pg)" stroke="#d8d8d8" ${SW} stroke-linejoin="round"/>`,
    `<path d="M14.5 0.5V5.5H19.5Z" fill="url(#${p}-k)" stroke="#bcbcbc" ${SW} stroke-linejoin="round"/>`,
    strokeP('M9 9.5H5.5L4.5 10.5V12.5L5.5 13.5H9M10 11.5H14M11.5 10V14M15 11.5H19M16.5 10V14', '#4a4a4a')],

  scrollbar: p => [
    lin(`${p}-a`, [[0, SPIN_LIGHT], [1, SPIN_DARK]], 0, 1, 1, 0) +
    lin(`${p}-t`, [[0, '#ebeaeb'], [1, '#cccbcb']]),
    fillP('M1.5 7.5H7.5V14.5H1.5ZM14.5 7.5H20.5V14.5H14.5Z', `url(#${p}-a)`),
    fillP('M7.5 7.5H14.5V14.5H7.5Z', `url(#${p}-t)`),
    fillP(arrowLeft(4, 9) + arrowRight(17, 9), ARROW),
    strokeP('M1.5 7.5H20.5V14.5H1.5ZM7.5 8V14M14.5 8V14', RD)],

  spinbtn: p => [
    lin(`${p}-a`, [[0, SPIN_LIGHT], [1, SPIN_DARK]], 0, 0, 1, 1),
    fillP('M7.5 4.5H14.5V10.5H7.5ZM7.5 10.5H14.5V16.5H7.5Z', `url(#${p}-a)`),
    fillP(arrowUp(10, 6) + arrowDown(10, 13), ARROW),
    strokeP('M7.5 4.5H14.5V16.5H7.5ZM8 10.5H14', RD)],

  spin_ctrl: p => [spinCtrlDefs(p), ...spinCtrl(p, pixText([['1', 3], ['0', 7]], 8))],
  spin_ctrl_double: p => [spinCtrlDefs(p), ...spinCtrl(p, pixText([['1', 3], ['.', 7], ['1', 9]], 8))],
  timepicker: p => [spinCtrlDefs(p),
    ...spinCtrl(p, pixText([['|', 3], [':', 5], ['|', 7], [':', 9], ['|', 11]], 8))],

  timer: p => [
    lin(`${p}-r`, [[0, '#fdfcfb'], [0.5, '#c4c4d1'], [1, '#6e6e85']], 0.2, 0, 0.8, 1) +
    lin(`${p}-o`, [[0, '#b4b4c4'], [1, '#30303e']], 0.2, 0, 0.8, 1) +
    rad(`${p}-f`, [[0, '#f0faff'], [0.45, '#c9edff'], [1, '#5d9bf0']], 0.45, 0.4, 0.62),
    `<circle cx="10.5" cy="10.5" r="10" fill="url(#${p}-r)" stroke="url(#${p}-o)" ${SW}/>`,
    `<circle cx="10.5" cy="10.5" r="7.5" fill="url(#${p}-f)" stroke="#5b79ba" ${SW}/>`,
    fillP('M10 3h1v2h-1zM10 16h1v2h-1zM3 10h2v1h-2zM16 10h2v1h-2z', '#4766db'),
    fillP('M9 10H14V12H9Z', '#3c4d5a', ' fill-opacity=".75"'),
    fillP('M9 11L13.3 6.7L14.3 7.7L10 12Z', '#12181f')],

  toggle_button: p => [
    lin(`${p}-b`, [[0, '#cbebff'], [1, '#fafdff']]),
    `<rect x="0.5" y="6.5" width="21" height="9" rx="1" fill="url(#${p}-b)" stroke="#003399" ${SW}/>`,
    fillP(pixText([['O', 6], ['N', 11]], 8), '#000000')],

  search: p => [
    rad(`${p}-s`, [[0, '#000000', 0.08], [1, '#000000', 0]]) +
    rad(`${p}-g`, [[0, '#ffffff'], [0.55, '#e1ebf5'], [1, '#9cb0c3']], 0.4, 0.35, 0.65) +
    lin(`${p}-r`, [[0, '#eeefed'], [1, '#a6a8a5']], 0, 0, 1, 1) +
    lin(`${p}-h`, [[0, '#f3e1c5'], [0.35, '#deb26b'], [0.7, '#c2821e'], [1, '#986410']], 16.5, 14.5, 14.5, 16.5, ' gradientUnits="userSpaceOnUse"'),
    fillP('M0.5 0.5H21.5V21.5H0.5Z', '#ffffff'),
    `<ellipse cx="10" cy="16.5" rx="7.5" ry="2.5" fill="url(#${p}-s)"/>`,
    fillP('M13.6 11.4L19.1 16.9A1.5 1.5 0 0 1 16.9 19.1L11.4 13.6Z', `url(#${p}-h)`),
    `<circle cx="8.5" cy="8.5" r="4" fill="url(#${p}-g)"/>`,
    fillP('M14 8.5A5.5 5.5 0 1 0 3 8.5A5.5 5.5 0 1 0 14 8.5ZM12.25 8.5A3.75 3.75 0 1 0 4.75 8.5A3.75 3.75 0 1 0 12.25 8.5Z',
      `url(#${p}-r)`, ' fill-rule="evenodd"'),
    strokeP('M0.5 0.5H21.5V21.5H0.5Z', '#a6b7cb')],

  tree_ctrl: () => ['',
    fillP('M1.5 1.5H20.5V20.5H1.5Z', '#ffffff'),
    strokeP('M6 3.5H12M10 7.5H16M14 11.5H20M14 15.5H20M10 18.5H16', '#386093'),
    ...tree('M3.5 4V18M4 7.5H6M7.5 8V15M8 11.5H10M8 15.5H10M4 18.5H6',
      [[3, 3, '#00f0f0'], [7, 7, '#ffff00'], [11, 11, '#00ff00'], [11, 15, '#00ff00'], [7, 18, '#ffff00']]),
    strokeP('M1.5 1.5H20.5V20.5H1.5Z', '#3e9ade')],

  tree_ctrl16x16: () => ['',
    fillP('M0.5 0.5H15.5V15.5H0.5Z', '#ffffff'),
    strokeP('M5 3.5H11M9 6.5H13M13 9.5H15M9 12.5H13', '#386093'),
    ...tree('M2.5 4V13M3 6.5H5M6.5 7V12M7 9.5H9M3 12.5H5',
      [[2, 3, '#00f0f0'], [6, 6, '#ffff00'], [10, 9, '#00ff00'], [6, 12, '#ffff00']]),
    strokeP('M0.5 0.5H15.5V15.5H0.5Z', '#3e9ade')],

  treelistctrl: p => [treeListDefs(p), ...treeListBody(p), strokeP('M1.5 1.5H21.5V21.5H1.5Z', RD)],

  treelistctrlcolumn: p => {
    const zig = (x0, x1) => { // edge alternating between columns x0 (rows 2,6,..) and x1 (rows 4,8,..)
      let d = ''; for (let y = 4; y <= 20; y += 4) d += `L${x1 + 0.5} ${y + 0.5}L${x0 + 0.5} ${y + 2.5}`;
      return d.replace(/L[^L]*$/, '');
    };
    const frame = 'M1.5 1.5H21.5V21.5H1.5Z';
    return [treeListDefs(p) +
      `<clipPath id="${p}-l"><path d="M0 0H8V2L7.5 2.5${zig(7, 5)}L5 22H0Z"/></clipPath>` +
      `<clipPath id="${p}-r"><path d="M22 0H14V2L13.5 2.5${zig(13, 11)}L11 22H22Z"/></clipPath>`,
      `<g clip-path="url(#${p}-l)"><g transform="translate(-3 0)">${treeListBody(p).join('')}${strokeP(frame, RD)}</g></g>`,
      `<g clip-path="url(#${p}-r)">${treeListBody(p).join('')}${strokeP(frame, RD)}</g>`,
      strokeP(`M7.5 1V2.5${zig(7, 5)}L4.5 21.5M14.5 1.5L13.5 2.5${zig(13, 11)}V22`, RD),
      fillP('M9 6h1v1h1v1h-1v1h-1v-1h-1v-1h1zM9 14h1v1h1v1h-1v1h-1v-1h-1v-1h1z', '#d90000')];
  },
};

// ---------- write ----------
fs.mkdirSync(outDir, { recursive: true });
for (const [name, fn] of Object.entries(icons)) {
  const size = /16x16$/.test(name) ? 16 : 22;
  const [defs, ...body] = fn(`additional-${name}`);
  const svg =
    `<svg xmlns="http://www.w3.org/2000/svg" width="${size}" height="${size}" viewBox="0 0 ${size} ${size}">\n` +
    (defs ? `<defs>${defs}</defs>\n` : '') +
    body.join('\n') + '\n</svg>\n';
  fs.writeFileSync(path.join(outDir, `${name}.svg`), svg);
}
console.log(`${Object.keys(icons).length} icons written to ${outDir}`);
