// Generates SVG icons for plugins/additional/icons (part A: bitmap_toggle_button .. propgridman).
// stroke-width 1 only, outlines on half-pixel coordinates; see ../STYLE.md.
// Usage: node gen_additional_a.js [<svgRoot>]   (default: the data/artwork/icons directory)
'use strict';
const fs = require('fs');
const path = require('path');

const outDir = path.join(process.argv[2] || path.resolve(__dirname, '..'), 'plugins', 'additional', 'icons');
const SW = 'stroke-width="1"';

// ---------- generic helpers ----------
const stopsXml = stops => stops.map(([o, c, a]) =>
  `<stop offset="${o}" stop-color="${c}"${a !== undefined ? ` stop-opacity="${a}"` : ''}/>`).join('');
const lin = (id, stops, x1 = 0, y1 = 0, x2 = 0, y2 = 1, extra = '') =>
  `<linearGradient id="${id}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"${extra}>${stopsXml(stops)}</linearGradient>`;
const rad = (id, stops, cx = 0.5, cy = 0.5, r = 0.5, extra = '') =>
  `<radialGradient id="${id}" cx="${cx}" cy="${cy}" r="${r}"${extra}>${stopsXml(stops)}</radialGradient>`;
const USR = ' gradientUnits="userSpaceOnUse"';

const fillP = (d, c, extra = '') => `<path d="${d}" fill="${c}"${extra}/>`;
const strokeP = (d, c, extra = '') => `<path d="${d}" fill="none" stroke="${c}" ${SW}${extra}/>`;

// Pixel mask -> one filled path of horizontal runs. rows: strings, '#' = set pixel.
function pix(rows, x0 = 0, y0 = 0) {
  let d = '';
  rows.forEach((r, j) => {
    for (let i = 0; i < r.length;) {
      if (r[i] !== '#') { i++; continue; }
      let k = i; while (r[k] === '#') k++;
      d += `M${x0 + i} ${y0 + j}h${k - i}v1h${i - k}z`;
      i = k;
    }
  });
  return d;
}
// Dump rows (dump.ps1 format) -> mask for the given palette chars.
const pixc = (rows, chars, x0 = 0, y0 = 0) =>
  pix(rows.map(r => [...r].map(c => chars.includes(c) ? '#' : '.').join('')), x0, y0);
// Single pixels [[x,y],...] -> filled path.
const px = cells => cells.map(([x, y]) => `M${x} ${y}h1v1h-1z`).join('');

// ---------- shared motifs ----------

// Magnifier of the picker icons (lens centre 13/12, r 4.5; handle to the lower right).
const lensDefs = p =>
  lin(`${p}-lh`, [[0, '#d6bd9a'], [0.5, '#f9e9d0'], [1, '#d6bd9a']], 20, 17, 18, 19, USR) +
  rad(`${p}-lt`, [[0, '#aecce8'], [0.45, '#c3d9ee'], [0.8, '#ebf2f9'], [1, '#9cb7cf']], 13, 12, 4.5, USR) +
  rad(`${p}-lb`, [[0, '#71a5d6'], [0.45, '#96bce1'], [0.75, '#dce9f5'], [1, '#96b3cd']], 13, 12, 4.5, USR);
const lens = p => [
  `<circle cx="13" cy="13" r="4.5" fill="none" stroke="#b7b6ac" ${SW}/>`,
  `<path d="M14 16L17 13L22.5 18.5L19.5 21.5Z" fill="none" stroke="#b7b6ac" ${SW} stroke-linejoin="round"/>`,
  `<path d="M14.5 15.5L16.5 13.5L21.5 18.5L19.5 20.5Z" fill="url(#${p}-lh)" stroke="#ba945c" ${SW} stroke-linejoin="round"/>`,
  `<circle cx="13" cy="12" r="4.5" fill="url(#${p}-lt)"/>`,
  fillP('M8.5 12H17.5A4.5 4.5 0 0 1 8.5 12Z', `url(#${p}-lb)`),
  `<circle cx="13" cy="12" r="4.5" fill="none" stroke="#668eb0" ${SW}/>`,
].join('\n');

// Calendar window (frame rows dy+2..dy+19, day marker box at mx..mx+3 / my..my+3).
const calDefs = p =>
  lin(`${p}-ct`, [[0, '#9ab4ec'], [0.34, '#86a5e4'], [0.5, '#3c71d4'], [1, '#3672c2']], 0, 0, 0, 1) +
  lin(`${p}-cm`, [[0, '#fcf47b'], [0.45, '#ffd02a'], [1, '#c57a00']], 0, 0, 1, 1);
function calendar(p, dy, mx, my) {
  const y = v => v + dy;
  const vb = [4, 8, 12, 16].map(x => `M${x} ${y(7)}h2V${y(18)}h-2z`).join('');
  return [
    fillP(`M1 ${y(6)}H21V${y(19)}H1Z`, '#ffffff'),
    fillP(vb + `M2 ${y(9)}H20v2H2zM2 ${y(14)}H20v1H2z`, '#e7e7e7'),
    fillP(`M2 ${y(6)}H20v1H2z`, '#e7eff5'),
    fillP([4, 8, 12, 16].map(x => `M${x} ${y(6)}h2v1h-2z`).join(''), '#cdd6dc'),
    fillP(`M${mx} ${my + 4}h4v1h-4z`, '#b8a47c'),
    `<path d="M${mx + 0.5} ${my + 0.5}h3v3h-3z" fill="url(#${p}-cm)" stroke="#b4882a" ${SW}/>`,
    strokeP(`M1.5 ${y(6)}V${y(18.5)}H20.5V${y(6)}`, '#9cbce0'),
    fillP(`M1 ${y(3)}H21V${y(6)}H1Z`, `url(#${p}-ct)`),
    fillP(`M16 ${y(3)}h2v2h-2z`, '#86a5e4') + fillP(`M19 ${y(3)}h2v2h-2z`, '#b1c4f0'),
    strokeP(`M0.5 ${y(2.5)}H21.5V${y(19.5)}H0.5Z`, '#2a6db2'),
  ].join('\n');
}

// Scroll bar of the list boxes (x 15..20, rows 4..17) inside the #333366 frame.
const scrollDefs = p =>
  lin(`${p}-sb`, [[0, '#deffff'], [0.5, '#b8e5f6'], [1, '#7abae9']], 0, 0, 1, 1) +
  lin(`${p}-st`, [[0, '#cccbcc'], [1, '#ebeaeb']], 0, 0, 1, 0);
const scrollBar = p => [
  fillP('M15 4H21V9H15ZM15 13H21V18H15Z', `url(#${p}-sb)`),
  fillP('M15 10H21V12H15Z', `url(#${p}-st)`),
  fillP('M17 5h2v1h1v1h-4v-1h1zM16 15h4v1h-1v1h-2v-1h-1z', '#808080'),
].join('\n');
const listFrame = '<path d="M0.5 3.5H21.5V18.5H0.5ZM14.5 4V18M15 9.5H21M15 12.5H21" fill="none" stroke="#333366" stroke-width="1"/>';
const listBg = p => fillP('M1 4H14V18H1Z', `url(#${p}-lb)`);
const listBgDef = p => lin(`${p}-lb`, [[0, '#ffffff'], [0.45, '#ffffff'], [0.6, '#f5ffff'], [1, '#e3ffff']]);

// Tree node: dark cross arms around a coloured centre pixel.
const treeCentre = (x, y, c) => fillP(px([[x, y]]), c);

// Property grid header glyph (expander) at row y.
const hdrGlyph = (x, y) => px([[x + 1, y], [x + 4, y], [x, y + 1], [x + 2, y + 1], [x + 3, y + 1]]);

// Property grid body: grey margin/headers, white value rows, lines and divider.
function propGrid({ frame, x1 = 20, headers, white, lines, bottom }) {
  return [
    fillP(`M1 ${headers[0]}H${x1 + 1}V${bottom}H1Z`, '#858585'),
    fillP(white.map(([a, b]) => `M3 ${a}H${x1 + 1}V${b + 1}H3Z`).join(''), '#ffffff'),
    strokeP(lines.map(r => `M3 ${r + 0.5}H${x1 + 1}`).join('') +
      white.map(([a, b]) => `M11.5 ${a}V${b + 1}`).join(''), '#b8b8b8'),
    fillP(headers.map(h => hdrGlyph(2, h)).join(''), '#2c2c2c'),
    strokeP(frame, '#000000'),
  ].join('\n');
}

// Table grid with blue header row/column (cells 4 px, lines every 5th pixel).
function tableGrid(edge) {            // edge = pixel column/row of the right/bottom frame
  const last = edge - 1;               // last interior pixel
  const lines = []; for (let v = 5; v < last; v += 5) lines.push(v);
  const hl = []; for (let v = 3; v < last; v += 3) hl.push(v);
  return [
    fillP(`M1 1H${last + 1}V${last + 1}H1Z`, '#ffffff'),
    fillP(`M1 1H${last + 1}V3H5V${last + 1}H1Z`, '#6c77b5'),
    strokeP(lines.map(x => `M${x + 0.5} 1V${last + 1}`).join('') +
      hl.map(y => `M1 ${y + 0.5}H${last + 1}`).join(''), '#c9c7c7'),
    strokeP(`M0.5 0.5H${last + 1.5}V${last + 1.5}H0.5Z`, '#000000'),
  ].join('\n');
}

// Light document page with folded corner (x 2..19, rows 0..21).
const pageDefs = p => lin(`${p}-pg`, [[0, '#fbfbfb'], [1, '#e9e9e9']], 0, 0, 0.6, 1);
const page = p => [
  fillP('M2 0H15.5L19.5 4.5V21.5H2Z', `url(#${p}-pg)`),
  strokeP('M19.5 4.5V21.5H2', '#cdcdcd'),
  `<path d="M15.5 0.5V4.5H19.5" fill="#f2f2f2" stroke="#b6b6b6" ${SW} stroke-linejoin="round"/>`,
].join('\n');

// Blue rounded button frame (bitmap toggle / colour picker).
const btnFrame = (y0, y1) => `<rect x="0.5" y="${y0 + 0.5}" width="21" height="${y1 - y0}" rx="1" fill="none" stroke="#003399" ${SW}/>`;

// genericdir_ctrl rows 2..19 (dump): 4 = text, 2 = folder, 6 = tree line.
const GD = [
  '.12223443343333333331.', '.12523433333333333331.', '.12223443343333333331.',
  '.13633333333333333331.', '.13633322333433343431.', '.13633322234434344331.',
  '.13633322234334343441.', '.13633363333333333331.', '.13633322333433333431.',
  '.13633322234334344331.', '.13633322234334343441.', '.13633363333333333331.',
  '.13633322333333433331.', '.13633322234434434441.', '.13633322234343434431.',
  '.12223443343333333331.', '.12523434333333333331.', '.12223443343333333331.'];

// ---------- icons ----------
const icons = {
  bitmap_toggle_button: p => [
    lin(`${p}-bg`, [[0, '#fafdff'], [1, '#c3e7ff']]) +
    rad(`${p}-r`, [[0, '#ff8a40'], [0.45, '#ff3300'], [1, '#a00c04']], 0.65, 0.7, 0.65) +
    lin(`${p}-y`, [[0, '#ffcc00'], [1, '#ffee66']], 0, 0, 1, 1) +
    lin(`${p}-g`, [[0, '#7ad448'], [1, '#13840a']], 0, 0, 1, 1),
    fillP('M1 7H21V17H1Z', `url(#${p}-bg)`),
    `<circle cx="6.5" cy="12.5" r="3.5" fill="url(#${p}-r)"/>`,
    `<path d="M7.5 7.5H14.5V14.5H7.5Z" fill="url(#${p}-y)" stroke="#cc9900" ${SW}/>`,
    `<path d="M13.5 10L18.5 16.5H11.5Z" fill="url(#${p}-g)" stroke="#0a7005" ${SW} stroke-linejoin="round"/>`,
    btnFrame(6, 17)],

  calendar: p => [calDefs(p), calendar(p, 0, 13, 10)],

  checklistbox: p => [scrollDefs(p) + listBgDef(p),
    listBg(p),
    fillP('M1 9H14V12H1Z', '#0066cc'),
    strokeP('M6 6.5H13M5 10.5H13', '#ffffff'),
    strokeP('M6 6.5H13M6 14.5H13M2.5 5.5h2v2h-2zM2.5 9.5h2v2h-2zM2.5 13.5h2v2h-2z', '#333366'),
    fillP('M5 10h8v1h-8z', '#ffffff'),
    fillP(px([[3, 10]]), '#3cba00'),
    scrollBar(p), listFrame],

  colourPickerIcon: p => [lin(`${p}-bg`, [[0, '#fafdff'], [1, '#cbebff']]) + lensDefs(p),
    fillP('M1 2H21V10H1Z', `url(#${p}-bg)`),
    fillP('M1 3H21V9H1Z', '#c5c5c5'),
    fillP('M2 3H20V9H2Z', '#9b0032'),
    fillP(pixc([
      '2456566565556565565542',
      '2465565656566556566542',
      '2456566566556565565542',
      '2465565556577776565542'], '6', 0, 4), '#000000'),
    btnFrame(1, 10),
    lens(p)],

  custom_control: p => [
    lin(`${p}-bg`, [[0, '#dadada'], [1, '#9c9c9c']], 0, 0, 1, 1) +
    rad(`${p}-q`, [[0, '#e2eafa'], [0.35, '#8fb3fb'], [0.75, '#4782fb'], [1, '#1c5ee6']], 0.35, 0.3, 0.75),
    `<rect x="2.5" y="1.5" width="17" height="18" rx="1.5" fill="url(#${p}-bg)"/>`,
    strokeP('M3.5 18V2.5H19', '#ececec'),
    `<circle cx="11.5" cy="10.5" r="5" fill="url(#${p}-q)" stroke="#4a7ee0" ${SW}/>`,
    fillP('M9 8.6C9 6.2 14 6.2 14 8.6C14 10 12.3 10.4 12.3 11.6V12.2H10.7V11.4C10.7 9.8 12.4 9.5 12.4 8.6' +
      'C12.4 7.6 10.6 7.6 10.6 8.6ZM10.7 13H12.3V14.6H10.7Z', '#ffffff'),
    `<rect x="2.5" y="1.5" width="17" height="18" rx="1.5" fill="none" stroke="#646464" ${SW}/>`],

  customcode: p => [pageDefs(p),
    page(p),
    `<path d="M3.5 5.5H17.5V8.5H3.5ZM3.5 10.5H17.5V13.5H3.5ZM3.5 15.5H17.5V18.5H3.5Z" fill="#606060" stroke="#d9d9d9" ${SW}/>`],

  dataview_ctrl: () => ['',
    fillP('M2 2H22V22H2Z', '#ffffff'),
    strokeP('M1.5 22V1.5H22', '#858585'),
    strokeP('M0.5 22V0.5H22', '#3e9ade'),
    fillP(px([[4, 3], [3, 4], [6, 6], [5, 7], [8, 9], [7, 10], [8, 12], [7, 13], [10, 15], [9, 16], [6, 18], [5, 19]]), '#555753'),
    fillP(px([[4, 4]]), '#ff3300'),
    fillP(px([[6, 7], [6, 19]]), '#ffcc00'),
    fillP(px([[8, 10], [8, 13]]), '#1871ce'),
    fillP(px([[10, 16]]), '#5cad25'),
    strokeP('M6 4.5H12M8 7.5H14M10 10.5H16M10 13.5H16M12 16.5H18M8 19.5H14', '#386093')],

  dataviewlist_column: () => ['',
    fillP('M1 1H7V3H2V22H1Z', '#858585'),
    fillP('M2 3H13V22H2Z', '#ffffff'),
    strokeP('M2 6.5H7M2 10.5H7M2 14.5H7M2 18.5H7', '#b8b8b8'),
    fillP(hdrGlyph(2, 1), '#2c2c2c'),
    fillP('M7 1H14V3H7Z', '#ff3300'),
    strokeP('M7.5 3V22M13.5 3V22M8 6.5H13M8 10.5H13M8 14.5H13M8 18.5H13', '#ff3300'),
    strokeP('M0.5 22V0.5H14', '#3e9ade')],

  dataviewlist_ctrl: () => ['',
    fillP('M1 1H21V3H2V21H1Z', '#858585'),
    fillP('M2 3H21V21H2Z', '#ffffff'),
    strokeP('M2 6.5H21M2 10.5H21M2 14.5H21M2 18.5H21M7.5 3V21M13.5 3V21', '#b8b8b8'),
    fillP(hdrGlyph(2, 1) + hdrGlyph(8, 1) + hdrGlyph(14, 1), '#2c2c2c'),
    strokeP('M0.5 0.5H21.5V21.5H0.5Z', '#3e9ade')],

  dataviewtree_ctrl: () => ['',
    fillP('M1 1H22V22H1Z', '#ffffff'),
    strokeP('M3.5 2V18M2 3.5H5M3 7.5H9M7.5 6V15M7 11.5H13M11.5 10V13M8 15.5H13M11.5 14V17M4 18.5H9M7.5 17V20', '#555753'),
    treeCentre(3, 3, '#ff3300'), treeCentre(7, 7, '#ffcc00'), treeCentre(7, 18, '#ffcc00'),
    treeCentre(11, 11, '#1871ce'), treeCentre(11, 15, '#1871ce'),
    strokeP('M6 3.5H12M10 7.5H16M14 11.5H20M14 15.5H20M10 18.5H16', '#386093'),
    strokeP('M0.5 22V0.5H22', '#3e9ade')],

  datepicker: p => [calDefs(p) + lensDefs(p), calendar(p, -2, 5, 4), lens(p)],

  dirPicker: p => [
    lin(`${p}-k`, [[0, '#d6d6d6'], [1, '#a2a2a2']]) +
    lin(`${p}-f`, [[0, '#9ebcdd'], [0.45, '#89aed6'], [1, '#6b9cce']]) + lensDefs(p),
    `<path d="M0.5 16.5V0.5H7.5L8.5 1.5V2.5H15.5V5" fill="url(#${p}-k)" stroke="#626460" ${SW} stroke-linejoin="round"/>`,
    strokeP('M1 2.5H8', '#8e8e8d'),
    `<path d="M2.5 4.5H19.5V16.5H2.5Z" fill="url(#${p}-f)" stroke="#3465a4" ${SW}/>`,
    strokeP('M3.5 16V5.5H19', '#aac4e2'),
    lens(p)],

  filePicker: p => [lin(`${p}-bg`, [[0, '#dfdfdf'], [1, '#eeeeee']]) + lensDefs(p),
    fillP('M1 1H17V19H1Z', `url(#${p}-bg)`),
    strokeP('M1.5 1.5H16.5V18.5H1.5Z', '#ffffff'),
    strokeP('M5 3.5H15M5 13.5H8M5 15.5H9', '#a8a9a6'),
    fillP(px([[2, 7], [2, 12]]), '#adafac') + fillP(px([[3, 8], [3, 13]]), '#ffffff'),
    `<path d="M5.5 5.5H14.5V11.5H5.5Z" fill="#9ab8d8" stroke="#729fcf" ${SW}/>`,
    fillP('M6 6h3v2h-3z', '#dbe5d2') + fillP('M6 9h3v2h-3z', '#7b88a0'),
    strokeP('M0.5 0.5H17.5V19.5H0.5Z', '#888a85'),
    lens(p)],

  fontPicker: p => [
    lin(`${p}-bg`, [[0, '#fefefe'], [1, '#efefed']], 0, 0, 1, 1) +
    lin(`${p}-a`, [[0, '#9ab9da'], [1, '#4f7eb6']]) + lensDefs(p),
    fillP('M0 0H16.5V17.5H0Z', `url(#${p}-bg)`),
    strokeP('M0 16.5H15.5V0', '#ffffff'),
    strokeP('M0 17.5H16.5V0', '#8c8e89'),
    fillP('M3.6 8H9.6V10H3Z', '#b2c6dd'),
    fillP('M7.8 0.3H9.9L11 11H8.4ZM7.8 0.3H8.9L2.1 13.5H0.9Z', `url(#${p}-a)`),
    strokeP('M0 13.5H4', '#7a9ecb'),
    lens(p)],

  genericdir_ctrl: () => ['',
    fillP('M2 2H20V20H2Z', '#ffffff'),
        `<path d="M2.5 2.5h2v2h-2zM2.5 17.5h2v2h-2z" fill="#c5c5c5" stroke="#386093" ${SW}/>`,
    fillP(pixc(GD.slice(4, 15), '2', 0, 6), '#386093'),
    fillP(pixc(GD.slice(3, 15), '6', 0, 5), '#555753'),
    fillP(pixc(GD, '4', 0, 2), '#000000'),
    strokeP('M1.5 1.5H20.5V20.5H1.5Z', '#3e9ade')],

  grid: () => ['', tableGrid(20)],
  grid16x16: () => ['', tableGrid(15)],

  htmlwin: p => [pageDefs(p) +
    rad(`${p}-g`, [[0, '#e4e7f6'], [0.45, '#8f9cd2'], [0.8, '#2f47a3'], [1, '#0c2792']], 0.62, 0.22, 0.85),
    page(p),
    `<circle cx="10.5" cy="12" r="6" fill="url(#${p}-g)" stroke="#3a4fa6" ${SW}/>`,
    fillP('M6 8C7.6 6.4 13 6.2 15 8C13 8.8 8.5 8.9 6 8Z', '#e0e4f5'),
    fillP('M8 11C9 10.6 10 11 10 12L9.8 15.6C9.3 16.3 8.6 16 8.4 15.2L8 12.5Z' +
      'M11.2 9.2C12.5 8.6 14.6 9 15 10.3C14.6 11.3 14 11.6 13.6 12.8C13 13.4 12.4 12.8 12.4 12C12.2 11 11.3 10.6 11.2 9.2Z',
    '#c4cbea')],

  hyperlink_ctrl: () => ['',
    fillP(pixc([
      '.........1............',
      '.........1............',
      '.........1............',
      '.....11..111...11.....',
      '.......1.1..1.1.......',
      '.....111.1..2.1.......',
      '....1..1.1.2321.......',
      '.....111.11232.11.....'], '1', 0, 2) + 'M3 11H19V12H3Z', '#0066cc'),
    `<path d="M12.5 7.5L13.5 8.5V10.5H15.5L17.5 12.5V15.5L16.5 16.5V18.5H12.5V17.5L11.5 16.5V15.5L10.5 14.5V13.5L9.5 12.5L10.5 11.5H11.5V8.5Z" fill="#ffffff" stroke="#000000" ${SW} stroke-linejoin="round"/>`,
    strokeP('M13 11.5H16M11 13V15', '#a6a6a6')],

  list_box: p => [scrollDefs(p) + listBgDef(p),
    listBg(p),
    fillP('M1 9H14V12H1Z', '#0066cc'),
    strokeP('M2 6.5H13M2 14.5H13', '#333366'),
    fillP('M2 10h11v1h-11z', '#ffffff'),
    scrollBar(p), listFrame],

  media_ctrl: p => [
    lin(`${p}-s`, [[0, '#8a8a8a'], [1, '#3c3c3c']]) +
    rad(`${p}-f`, [[0, '#f6f6f6'], [0.55, '#d6d6d6'], [1, '#a4a4a4']], 0.5, 0.6, 0.6) +
    lin(`${p}-h`, [[0, '#7a7a7a'], [1, '#262626']]),
    `<ellipse cx="11" cy="11.5" rx="10.5" ry="7" fill="url(#${p}-s)" stroke="#444444" ${SW}/>`,
    `<ellipse cx="11" cy="9" rx="10.5" ry="6.5" fill="url(#${p}-f)" stroke="#7a7a7a" ${SW}/>`,
    `<path fill="url(#${p}-h)" d="M9 3.8A2 1 0 0 0 13 3.8A2 1 0 0 0 9 3.8Z` +
      `M3.6 7.3A1.9 1.2 0 0 0 7.4 7.3A1.9 1.2 0 0 0 3.6 7.3Z` +
      `M14.2 7.3A1.9 1.2 0 0 0 18 7.3A1.9 1.2 0 0 0 14.2 7.3Z` +
      `M5.3 12.2A2.1 1.4 0 0 0 9.5 12.2A2.1 1.4 0 0 0 5.3 12.2Z` +
      `M12.5 12.2A2.1 1.4 0 0 0 16.7 12.2A2.1 1.4 0 0 0 12.5 12.2Z"/>`],

  propgrid: () => ['', propGrid({
    frame: 'M0.5 0.5H21.5V21.5H0.5Z', headers: [1, 11], bottom: 21,
    white: [[3, 10], [13, 20]], lines: [5, 8, 15, 18] })],

  propgriditem: () => ['', propGrid({
    frame: 'M0.5 0.5H21.5V21.5H0.5Z', headers: [1, 11], bottom: 21,
    white: [[3, 10], [13, 20]], lines: [15, 18] }),
    strokeP('M1.5 5.5H20.5V8.5H1.5Z', '#ff0018')],

  propgridman: () => ['',
    `<path d="M1.5 0.5h2v2h-2zM5.5 0.5h2v2h-2z" fill="#dedede" stroke="#000000" ${SW}/>`,
    propGrid({
      frame: 'M0.5 22V4.5H21.5V22', headers: [5, 15], bottom: 22,
      white: [[7, 14], [17, 21]], lines: [9, 12, 19] })],
};

// ---------- write ----------
fs.mkdirSync(outDir, { recursive: true });
for (const [name, fn] of Object.entries(icons)) {
  const size = name === 'grid16x16' ? 16 : 22;
  const [defs, ...body] = fn(`additional-${name}`);
  const svg =
    `<svg xmlns="http://www.w3.org/2000/svg" width="${size}" height="${size}" viewBox="0 0 ${size} ${size}">\n` +
    (defs ? `<defs>${defs}</defs>\n` : '') +
    body.join('\n') + '\n</svg>\n';
  fs.writeFileSync(path.join(outDir, `${name}.svg`), svg);
}
console.log(`${Object.keys(icons).length} icons written to ${outDir}`);
