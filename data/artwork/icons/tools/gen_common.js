// Generates SVG icons for plugins/common/icons (stroke-width 1, outlines on half-pixel coordinates).
// Usage: node gen_common.js [<svgRoot>]   (default: the data/artwork/icons directory)
'use strict';
const fs = require('fs');
const path = require('path');

const outDir = path.join(process.argv[2] || path.resolve(__dirname, '..'), 'plugins', 'common', 'icons');

const SW = 'stroke-width="1"';

// ---------- generic helpers ----------
const stopsXml = stops => stops.map(([o, c, a]) =>
  `<stop offset="${o}" stop-color="${c}"${a !== undefined ? ` stop-opacity="${a}"` : ''}/>`).join('');
const lin = (id, stops, x1 = 0, y1 = 0, x2 = 0, y2 = 1, extra = '') =>
  `<linearGradient id="${id}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"${extra}>${stopsXml(stops)}</linearGradient>`;
const rad = (id, stops, cx = 0.5, cy = 0.5, r = 0.5, extra = '') =>
  `<radialGradient id="${id}" cx="${cx}" cy="${cy}" r="${r}"${extra}>${stopsXml(stops)}</radialGradient>`;

// Pixel rectangles [[x,y,w,h],...] relative to (ox,oy) as one path string.
const rects = (ox, oy, list) => list.map(([x, y, w, h]) => `M${ox + x} ${oy + y}h${w}v${h}h-${w}z`).join('');
const fill = (d, c, extra = '') => `<path d="${d}" fill="${c}"${extra}/>`;
const line = (d, c) => `<path d="${d}" fill="none" stroke="${c}" ${SW}/>`;

// ---------- pixel font (glyphs as filled paths) ----------
const G = {
  a: [[1, 0, 2, 1], [3, 1, 1, 4], [1, 2, 2, 1], [0, 3, 1, 1], [1, 4, 2, 1]],          // 4x5, (x,y)=top of x-height
  b: [[0, 0, 1, 8], [1, 3, 2, 1], [3, 4, 1, 3], [1, 7, 2, 1]],                        // 4x8, (x,y)=top of ascender
  c: [[1, 0, 2, 1], [0, 1, 1, 3], [1, 4, 2, 1]],                                      // 3x5
  O: [[1, 0, 2, 1], [0, 1, 1, 4], [3, 1, 1, 4], [1, 5, 2, 1]],                        // 4x6
  k: [[0, 0, 1, 6], [3, 1, 1, 1], [2, 2, 1, 1], [1, 3, 1, 1], [2, 4, 1, 1], [3, 5, 1, 1]], // 4x6
  O4: [[1, 0, 2, 1], [0, 1, 1, 2], [3, 1, 1, 2], [1, 3, 2, 1]],                       // 4x4 (16 px)
  k4: [[0, 0, 1, 4], [2, 1, 1, 1], [1, 2, 1, 1], [2, 3, 1, 1]],                       // 3x4 (16 px)
  ibeam: [[0, 0, 1, 1], [2, 0, 1, 1], [1, 1, 1, 4], [0, 5, 1, 1], [2, 5, 1, 1]],     // 3x6 text cursor
  down: [[0, 0, 4, 1], [1, 1, 2, 1]],                                                 // dropdown / spin arrow
  up: [[1, 0, 2, 1], [0, 1, 4, 1]],
};
const glyph = (x, y, g) => rects(x, y, G[g]);
const ok = (x, y) => fill(glyph(x, y, 'O') + glyph(x + 5, y, 'k'), '#000000');
const ibeam = (x, y) => fill(glyph(x, y, 'ibeam'), '#808080');

// ---------- mouse cursor ----------
// 22 px arrow, tip pixel at (x,y); 7 px wide, 12 px high.
const cursorPath = (x, y) =>
  `M${x + .5} ${y + .5}L${x + 6.5} ${y + 6.5}V${y + 7.5}H${x + 4.5}L${x + 6.5} ${y + 11.5}H${x + 4.5}L${x + 2.5} ${y + 8.5}L${x + .5} ${y + 10.5}Z`;
// 16 px arrow, tip pixel at (x,y); 6 px wide, 10 px high.
const cursorPath16 = (x, y) =>
  `M${x + .5} ${y + .5}L${x + 5.5} ${y + 5.5}H${x + 3.5}L${x + 5.5} ${y + 9.5}H${x + 4}L${x + 2.5} ${y + 6.5}L${x + .5} ${y + 8.5}Z`;
const cursorDefs = p =>
  lin(`${p}-cf`, [[0, '#ffffff'], [1, '#cfd0f6']], 0, 0, 1, 1) +
  lin(`${p}-cs`, [[0, '#a5a5ff'], [1, '#6f86ca']], 0, 0, 1, 1);
const cursor = (p, x, y, small) =>
  `<path d="${(small ? cursorPath16 : cursorPath)(x, y)}" fill="url(#${p}-cf)" stroke="url(#${p}-cs)" ${SW} stroke-linejoin="round"/>`;

// ---------- toolbar symbols (page, folder, disk) ----------
const tbDefs = p =>
  lin(`${p}-pg`, [[0, '#ffffff'], [1, '#c4e8ff']], 0, 0, 1, 1) +
  lin(`${p}-fo`, [[0, '#fffd00'], [1, '#ffda00']], 0, 0, 1, 1) +
  lin(`${p}-dk`, [[0, '#0083d7'], [1, '#48a5e9']], 0, 0, 1, 0);
// page: 6x7 px, top-left pixel (x,y), dog-ear top right
const tbPage = (p, x, y) =>
  `<path d="M${x + .5} ${y + .5}H${x + 3.5}L${x + 5.5} ${y + 2.5}V${y + 6.5}H${x + .5}Z" fill="url(#${p}-pg)" stroke="#666699" ${SW}/>`;
// folder: 8x7 px, (x,y) = left column / tab top row; double left edge marks the front panel
const tbFolder = (p, x, y) =>
  `<path d="M${x + .5} ${y + 5.5}V${y + 1.5}L${x + 1.5} ${y + .5}H${x + 2.5}L${x + 3.5} ${y + 1.5}V${y + 2.5}H${x + 6.5}L${x + 7.5} ${y + 3.5}V${y + 5.5}L${x + 6.5} ${y + 6.5}H${x + 1.5}Z" fill="url(#${p}-fo)" stroke="#cc9900" ${SW}/>` +
  line(`M${x + 1.5} ${y + 3}V${y + 6}`, '#cc9900');
// disk: 6x7 px, top-left (x,y)
const tbDisk = (p, x, y) => {
  const o = `M${x + 1.5} ${y + .5}H${x + 4.5}A1 1 0 0 1 ${x + 5.5} ${y + 1.5}V${y + 5.5}A1 1 0 0 1 ${x + 4.5} ${y + 6.5}H${x + 1.5}A1 1 0 0 1 ${x + .5} ${y + 5.5}V${y + 1.5}A1 1 0 0 1 ${x + 1.5} ${y + .5}Z`;
  return `<path d="${o}" fill="url(#${p}-dk)"/>` +
    fill(`M${x + 1} ${y + 4}h4v2h-4z`, '#c8c8c8') +
    fill(`M${x + 2} ${y + 1}h2v2h-2z`, '#ffffff') +
    fill(`M${x + 2} ${y + 5}h1v1h-1z`, '#48a5e9') +
    `<path d="${o}" fill="none" stroke="#003399" ${SW}/>`;
};
// toolbar strip: border lines at rows y0 and y1, background gradient in between
const tbBarDefs = p => lin(`${p}-tb`, [[0, '#ece9d8'], [0.7, '#ebe8d7'], [1, '#e1ddce']]);
const tbBar = (p, y0, y1) =>
  fill(`M0 ${y0 + 1}H22V${y1}H0Z`, `url(#${p}-tb)`) + line(`M0 ${y0 + .5}H22M0 ${y1 + .5}H22`, '#aca899');

// ---------- widget frames ----------
const DARK = '#333366', BLUE = '#0066cc', SHADOW = '#c0c0c0';
const fieldDefs = p =>
  lin(`${p}-f`, [[0, '#ffffff'], [0.35, '#ffffff'], [1, '#dfffff']], 0, 0, 1, 1) +
  lin(`${p}-d`, [[0, '#9bcdff'], [1, '#cef3ff']], 0, 0, 1, 1);
// single-line field with dropdown button (choice / combo box)
const dropField = (p, content) => [fieldDefs(p),
  fill('M0.5 6.5H21.5V15.5H0.5Z', `url(#${p}-f)`),
  fill('M14.5 6.5H21.5V15.5H14.5Z', `url(#${p}-d)`),
  fill(glyph(2, 9, 'a'), '#000000'),
  content,
  fill(glyph(16, 10, 'down'), '#808080'),
  line('M0.5 6.5H21.5V15.5H0.5ZM14.5 7V15', DARK)];

const btnDefs = (p, top) => lin(`${p}-b`, [[0, top], [1, '#c7eaff']]);
const btnFrame = (x, y, w, h) => `<rect x="${x}" y="${y}" width="${w}" height="${h}" rx="1" fill="none" stroke="#003399" ${SW}/>`;

// menu text bar (1 px high) helper
const bars = (list, c) => fill(list.map(([x, y, w]) => `M${x} ${y}h${w}v1h-${w}z`).join(''), c);

// "H"-shaped menu fragment used by menuitem / separator: rows y..y+10
const menuFrag = (y, sideColor, middle) => [
  fill(`M14 ${y}h1v11h-1z`, SHADOW),
  fill(`M4 ${y + 2}H13V${y + 9}H4Z`, '#ffffff'),
  fill(`M4 ${y + 1}H13V${y + 2}H4ZM4 ${y + 9}H13V${y + 10}H4Z`, '#ffffff', ' fill-opacity=".5"'),
  middle,
  line(`M3.5 ${y}V${y + 11}M13.5 ${y}V${y + 11}`, sideColor)];

// ---------- icons ----------
const icons = {
  animation: p => [
    lin(`${p}-r`, [[0, '#8a8a8a'], [1, '#2c2c2c']]) +
    rad(`${p}-t`, [[0, '#f8f8f8'], [0.6, '#e2e2e2'], [1, '#a8a8a8']], 0.5, 0.55, 0.6) +
    lin(`${p}-h`, [[0, '#8e8e8e'], [1, '#1e1e1e']]),
    `<ellipse cx="11" cy="11.5" rx="10.5" ry="7" fill="url(#${p}-r)"/>`,
    `<ellipse cx="11" cy="9" rx="10.5" ry="6.5" fill="url(#${p}-t)" stroke="#8a8a8a" ${SW}/>`,
    `<path d="M9 3.5A1.6 0.9 0 0 0 12.2 3.5A1.6 0.9 0 0 0 9 3.5Z` +
      `M3.2 7.4A1.8 1 0 0 0 6.8 7.4A1.8 1 0 0 0 3.2 7.4Z` +
      `M15.2 7.4A1.8 1 0 0 0 18.8 7.4A1.8 1 0 0 0 15.2 7.4Z` +
      `M5.6 12A2.1 1.4 0 0 0 9.8 12A2.1 1.4 0 0 0 5.6 12Z` +
      `M12.2 12A2.1 1.4 0 0 0 16.4 12A2.1 1.4 0 0 0 12.2 12Z" fill="url(#${p}-h)"/>`],

  auitoolbar: p => [tbDefs(p),
    fill('M0 4H22V7H0Z', '#aca899'),
    fill('M0 4h1v1h-1zM2 4h1v1h-1zM1 5h1v1h-1zM0 6h1v1h-1zM2 6h1v1h-1z', '#000000'),
    fill('M0 7H22V15H0Z', '#ffffff'),
    line('M0 15.5H22', '#aca99d'), line('M0 16.5H22', '#dadad5'),
    tbPage(p, 0, 8), tbFolder(p, 7, 8), tbDisk(p, 16, 8)],

  bitmap_button: p => [btnDefs(p, '#fafdff') +
    rad(`${p}-r`, [[0, '#ffa060'], [0.6, '#ff3300'], [1, '#d92000']], 0.6, 0.65, 0.6) +
    lin(`${p}-y`, [[0, '#ffcc00'], [1, '#ffed64']], 0, 0, 1, 1) +
    lin(`${p}-g`, [[0, '#2b9116'], [1, '#5bc62e']], 1, 0, 0, 1),
    fill('M0.5 6.5H21.5V17.5H0.5Z', `url(#${p}-b)`),
    `<circle cx="5.5" cy="12.5" r="3" fill="url(#${p}-r)" stroke="#9c0e0f" ${SW}/>`,
    `<path d="M7.5 7.5H14.5V14.5H7.5Z" fill="url(#${p}-y)" stroke="#cc9900" ${SW}/>`,
    `<path d="M14 10.5L18.5 16.5H11.5Z" fill="url(#${p}-g)" stroke="#0a7005" ${SW} stroke-linejoin="round"/>`,
    btnFrame(0.5, 6.5, 21, 11)],

  button: p => [btnDefs(p, '#fafdff'),
    fill('M0.5 6.5H21.5V15.5H0.5Z', `url(#${p}-b)`),
    ok(6, 8),
    btnFrame(0.5, 6.5, 21, 9)],

  button16x16: p => [btnDefs(p, '#f4fbff'),
    fill('M0.5 4.5H15.5V11.5H0.5Z', `url(#${p}-b)`),
    fill(glyph(4, 6, 'O4') + glyph(9, 6, 'k4'), '#000000'),
    btnFrame(0.5, 4.5, 15, 7)],

  checkbox: p => [lin(`${p}-b`, [[0, '#d6d6d6'], [1, '#fcfcfc']], 0, 0, 1, 1),
    fill('M4.5 5.5H15.5V16.5H4.5Z', `url(#${p}-b)`),
    fill('M13 7H14V10L9 15H8L6 13V10H7L8.5 12Z', '#009900'),
    line('M4.5 5.5H15.5V16.5H4.5Z', DARK)],

  choice: p => dropField(p, ''),

  combo_box: p => dropField(p, ibeam(8, 8)),

  gauge: p => [lin(`${p}-f`, [[0, '#ffffff'], [1, '#deffff']], 0, 0, 1, 1) +
    lin(`${p}-g`, [[0, '#009900'], [1, '#6ce148']], 0, 8, 0, 13, ' gradientUnits="userSpaceOnUse"'),
    fill('M0.5 6.5H21.5V14.5H0.5Z', `url(#${p}-f)`),
    fill('M2 8h3v5h-3zM6 8h3v5h-3zM10 8h3v5h-3zM14 8h3v5h-3z', `url(#${p}-g)`),
    `<rect x="0.5" y="6.5" width="21" height="8" rx="1" fill="none" stroke="${DARK}" ${SW}/>`],

  infobar: () => ['',
    fill('M0 6.5H20.5L21.5 7.5V16.5L20.5 17.5H0Z', '#ece9d8'),
    line('M0 16.5H20M20.5 8V16', '#dfddcd'),
    `<ellipse cx="15.5" cy="12" rx="4.5" ry="4" fill="#4d32c3"/>`,
    fill('M15 9h1v1h-1zM14 11h2v3h-1v-2h-1zM14 14h3v1h-3z', '#ffffff'),
    `<path d="M0 6.5H20.5L21.5 7.5V16.5L20.5 17.5H0" fill="none" stroke="#716f64" ${SW}/>`],

  list_box: p => [
    lin(`${p}-l`, [[0, '#ffffff'], [0.35, '#ffffff'], [1, '#e3ffff']]) +
    lin(`${p}-s`, [[0, '#deffff'], [1, '#8dc7ec']], 0, 0, 1, 1) +
    lin(`${p}-t`, [[0, '#cccbcc'], [1, '#ebeaeb']], 0, 0, 1, 0),
    fill('M0.5 3.5H14.5V18.5H0.5Z', `url(#${p}-l)`),
    fill('M14.5 3.5H21.5V9.5H14.5ZM14.5 12.5H21.5V18.5H14.5Z', `url(#${p}-s)`),
    fill('M15 10H21V12H15Z', `url(#${p}-t)`),
    fill('M1 9H14V12H1Z', BLUE),
    fill('M2 10h11v1h-11z', '#ffffff'),
    bars([[2, 6, 11], [2, 14, 11]], DARK),
    fill(glyph(16, 5, 'up') + glyph(16, 15, 'down'), '#808080'),
    line('M0.5 3.5H21.5V18.5H0.5ZM14.5 4V18M15 9.5H21M15 12.5H21', DARK)],

  list_ctrl: p => [
    lin(`${p}-h`, [[0, '#ebeadb'], [0.6, '#ebeadb'], [1, '#e2e0d1']]) +
    lin(`${p}-l`, [[0, '#ffffff'], [0.45, '#ffffff'], [1, '#deffff']], 0, 0, 1, 1),
    fill('M0.5 2.5H21.5V19.5H0.5Z', `url(#${p}-l)`),
    fill('M1 3H21V6H1Z', `url(#${p}-h)`),
    line('M1 6.5H21', '#d4d2c2'), line('M11.5 4V6', '#cbc7b8'), line('M12.5 4V6', '#ffffff'),
    fill('M1 12H21V15H1Z', BLUE),
    fill('M2 13h4v1h-4zM14 13h4v1h-4z', '#ffffff'),
    bars([[2, 9, 7], [14, 9, 3], [2, 17, 6], [14, 17, 5]], DARK),
    line('M0.5 2.5H21.5V19.5H0.5Z', DARK)],

  menu: p => [cursorDefs(p) + lin(`${p}-m`, [[0, '#ffffff'], [1, '#dfffff']]),
    fill('M15 4h1v15h-11v-1h10z', SHADOW),
    fill('M4.5 2.5H14.5V17.5H4.5Z', `url(#${p}-m)`),
    fill('M5 6H14V9H5Z', BLUE),
    fill('M6 7h5v1h-5z', '#ffffff'),
    bars([[6, 4, 6], [6, 12, 5], [6, 15, 6]], DARK),
    line('M4.5 2.5H14.5V17.5H4.5ZM5 10.5H14', DARK),
    cursor(p, 12, 8)],

  menubar: p => [cursorDefs(p),
    fill('M0 1H21V6H0Z', '#ffffff'),
    fill('M1 2H8V5H1Z', BLUE), fill('M2 3h5v1h-5z', '#ffffff'),
    fill('M11 7h1v14h-10v-1h9z', SHADOW),
    fill('M1.5 5.5H10.5V19.5H1.5Z', '#ffffff'),
    bars([[9, 3, 4], [15, 3, 5], [3, 7, 6], [3, 9, 4], [3, 11, 6], [3, 15, 4], [3, 17, 6]], BLUE),
    line('M21 1.5H0.5V5.5H21M1.5 6V19.5H10.5V6M2 13.5H10', DARK),
    cursor(p, 13, 8)],

  menubar16x16: p => [cursorDefs(p),
    fill('M0 1H16V6H0Z', '#ffffff'),
    fill('M1 2H6V5H1Z', BLUE), fill('M2 3h3v1h-3z', '#ffffff'),
    fill('M9 7h1v8h-8v-1h7z', SHADOW),
    fill('M1.5 5.5H8.5V13.5H1.5Z', '#ffffff'),
    bars([[7, 3, 4], [13, 3, 3], [3, 7, 4], [3, 9, 2], [3, 11, 4]], BLUE),
    line('M16 1.5H0.5V5.5H16M1.5 6V13.5H8.5V6', DARK),
    cursor(p, 10, 6, true)],

  menuitem: p => [cursorDefs(p),
    ...menuFrag(2, DARK, fill('M4 6H13V9H4Z', BLUE) + fill('M5 7h6v1h-6z', '#ffffff')),
    cursor(p, 11, 8)],

  radio_box: p => [...icons.radio_button(p),
    `<path d="M5 2.5H3.5L1.5 4.5V17.5L3.5 19.5H18.5L20.5 17.5V4.5L18.5 2.5H12" fill="none" stroke="#d0d0bf" ${SW} stroke-linejoin="round"/>`,
    fill('M6 1H11V3H6Z', '#003399')],

  radio_button: p => [
    lin(`${p}-o`, [[0, '#343466'], [1, '#9898b0']], 0, 0, 1, 1) +
    lin(`${p}-i`, [[0, '#c8c8c8'], [1, '#f8f8f8']], 0, 0, 1, 1) +
    lin(`${p}-g`, [[0, '#009900'], [0.5, '#41c52b'], [1, '#c7efb7']], 0, 0, 1, 1),
    `<circle cx="10" cy="11" r="5.5" fill="url(#${p}-i)" stroke="url(#${p}-o)" ${SW}/>`,
    `<circle cx="9.75" cy="10.75" r="3.25" fill="url(#${p}-g)"/>`],

  separator: p => [cursorDefs(p),
    ...menuFrag(3, '#aca899', line('M4 8.5H13', '#aca899')),
    cursor(p, 11, 8)],

  slider: p => [lin(`${p}-t`, [[0, '#ece9d8'], [1, '#d4d1c3']], 0, 0, 1, 0),
    fill('M1 10H21V11H1Z', '#ece9d8'),
    line('M1 9.5H21M1 11.5H21M0.5 10V11M21.5 10V11', '#bbb9ad'),
    `<path d="M5.5 5.5H7.5L8.5 6.5V14.5L7.5 15.5H5.5L4.5 14.5V6.5Z" fill="url(#${p}-t)" stroke="#716f64" ${SW}/>`,
    fill('M5 6h3v1h-3zM5 14h3v1h-3z', '#66cc33')],

  static_bitmap: p => [
    lin(`${p}-r`, [[0, '#ff3300'], [1, '#ffa167']], 0, 0, 1, 1) +
    lin(`${p}-b`, [[0, '#1871ce'], [1, '#a4d7fa']]) +
    lin(`${p}-y`, [[0, '#ffcc00'], [0.5, '#ffe13f'], [1, '#fff780']], 0, 0, 1, 1),
    `<rect x="1.5" y="2.5" width="9" height="9" rx="1" fill="url(#${p}-r)" stroke="#990000" ${SW}/>`,
    `<path d="M6 11.5L15.5 18.5H2.5Z" fill="url(#${p}-b)" stroke="#1c51ab" ${SW} stroke-linejoin="round"/>`,
    `<circle cx="14" cy="11.5" r="6.3" fill="url(#${p}-y)" fill-opacity=".92" stroke="#e0ac00" ${SW}/>`],

  static_line: () => ['', line('M1 9.5H20', '#aca899'), line('M2 10.5H21', '#ffffff')],

  static_text: () => ['', fill(glyph(4, 9, 'a') + glyph(9, 6, 'b') + glyph(14, 9, 'c'), '#000000')],

  statusbar: p => [lin(`${p}-s`, [[0, '#bbb9ad'], [0.25, '#dfdccd'], [0.33, '#ece9d8'], [1, '#ece9d8']]),
    fill('M0 7H21V17H0Z', `url(#${p}-s)`),
    fill('M18 11h1v1h-1zM16 13h1v1h-1zM18 13h1v1h-1zM14 15h1v1h-1zM16 15h1v1h-1zM18 15h1v1h-1z', '#aca899'),
    line('M0 17.5H21.5V7', '#716f64')],

  submenu: p => [cursorDefs(p) +
    lin(`${p}-m`, [[0, '#ffffff'], [1, '#eeffff']]) + lin(`${p}-n`, [[0, '#ffffff'], [0.2, '#ffffff'], [1, '#dfffff']]),
    fill('M1 8h9v1h-9zM10 1h1v2h-1z', SHADOW),
    fill('M0 0H9.5V7.5H0Z', `url(#${p}-m)`),
    bars([[1, 1, 6]], DARK),
    fill('M0 3H9V6H0Z', BLUE),
    fill('M1 4h5v1h-5zM7 3h1v3h-1zM8 4h1v1h-1z', '#ffffff'),
    line('M9.5 0V7.5H0', DARK),
    fill('M21 5h1v15h-11v-1h10z', SHADOW),
    fill('M10.5 3.5H20.5V18.5H10.5Z', `url(#${p}-n)`),
    fill('M11 7H20V10H11Z', BLUE),
    fill('M12 8h5v1h-5z', '#ffffff'),
    bars([[12, 5, 6], [12, 13, 6], [12, 16, 6]], DARK),
    line('M10.5 3.5H20.5V18.5H10.5ZM11 11.5H20', DARK),
    cursor(p, 15, 9)],

  text_ctrl: p => [lin(`${p}-f`, [[0, '#ffffff'], [0.35, '#ffffff'], [1, '#eaffff']], 0, 0, 1, 1),
    fill('M0.5 5.5H21.5V14.5H0.5Z', `url(#${p}-f)`),
    fill(glyph(2, 8, 'a'), '#000000'),
    ibeam(7, 7),
    line('M0.5 5.5H21.5V14.5H0.5Z', '#3e9ade')],

  tool: p => [tbDefs(p) + tbBarDefs(p) + cursorDefs(p),
    tbBar(p, 5, 15),
    line('M5.5 6.5H16.5V13.5H5.5Z', '#e1ddce'),
    tbFolder(p, 7, 7),
    cursor(p, 12, 10)],

  toolSeparator: p => [tbDefs(p) + tbBarDefs(p),
    tbBar(p, 6, 16),
    line('M11.5 7V15', '#aca899'), line('M10.5 8V16', '#ffffff'),
    tbPage(p, 2, 8), tbDisk(p, 14, 8)],

  toolbar: p => [tbDefs(p) + tbBarDefs(p),
    tbBar(p, 6, 16),
    tbPage(p, 0, 8), tbFolder(p, 7, 8), tbDisk(p, 16, 8)],
};

// ---------- write ----------
const size16 = new Set(['button16x16', 'menubar16x16']);
fs.mkdirSync(outDir, { recursive: true });
for (const [name, fn] of Object.entries(icons)) {
  const s = size16.has(name) ? 16 : 22;
  const [defs, ...body] = fn(`common-${name}`);
  const svg =
    `<svg xmlns="http://www.w3.org/2000/svg" width="${s}" height="${s}" viewBox="0 0 ${s} ${s}">\n` +
    (defs ? `<defs>${defs}</defs>\n` : '') +
    body.filter(Boolean).join('\n') + '\n</svg>\n';
  fs.writeFileSync(path.join(outDir, `${name}.svg`), svg);
}
console.log(`${Object.keys(icons).length} icons written to ${outDir}`);
