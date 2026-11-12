// 22x22 resource icons (stroke-width 1, outlines on half-pixel coordinates): module used by
// gen_resources.js, which writes the icons whose source PNG really is 22x22.
'use strict';

const SW = 'stroke-width="1"';

// ---------- helpers ----------
const stopsXml = stops => stops.map(([o, c, a]) =>
  `<stop offset="${o}" stop-color="${c}"${a !== undefined ? ` stop-opacity="${a}"` : ''}/>`).join('');
const lin = (id, stops, x1 = 0, y1 = 0, x2 = 0, y2 = 1, extra = '') =>
  `<linearGradient id="${id}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"${extra}>${stopsXml(stops)}</linearGradient>`;
const rad = (id, stops, cx = 0.5, cy = 0.5, r = 0.5, extra = '') =>
  `<radialGradient id="${id}" cx="${cx}" cy="${cy}" r="${r}"${extra}>${stopsXml(stops)}</radialGradient>`;
const n = v => +v.toFixed(2);

// Gear outline (teeth + centre hole as evenodd sub-path).
function gear(cx, cy, rOut, rIn, rHole, teeth, tipHalf, baseHalf) {
  const pt = (r, a) => `${n(cx + r * Math.cos(a))} ${n(cy + r * Math.sin(a))}`;
  const step = 2 * Math.PI / teeth;
  let d = '';
  for (let i = 0; i < teeth; i++) {
    const a = i * step - Math.PI / 2;
    d += (i === 0 ? `M${pt(rIn, a - baseHalf)}` : `A${rIn} ${rIn} 0 0 1 ${pt(rIn, a - baseHalf)}`) +
      `L${pt(rOut, a - tipHalf)}A${rOut} ${rOut} 0 0 1 ${pt(rOut, a + tipHalf)}L${pt(rIn, a + baseHalf)}`;
  }
  d += `A${rIn} ${rIn} 0 0 1 ${pt(rIn, -Math.PI / 2 - baseHalf)}Z`;
  d += `M${n(cx + rHole)} ${cy}A${rHole} ${rHole} 0 1 0 ${n(cx - rHole)} ${cy}A${rHole} ${rHole} 0 1 0 ${n(cx + rHole)} ${cy}Z`;
  return d;
}

// 2x2 squares as one filled path; cells = [[col,row],...]
const dots = cells => `<path d="${cells.map(([x, y]) => `M${x} ${y}h2v2h-2z`).join('')}" fill="#0066cc"/>`;
const range = (a, b, s = 3) => { const r = []; for (let i = a; i <= b; i += s) r.push(i); return r; };

// ---------- shared parts ----------
const alignDefs = p =>
  lin(`${p}-y`, [[0, '#ffffd0'], [0.35, '#fff200'], [1, '#ffbc00']], 0, 0, 1, 1) +
  lin(`${p}-b`, [[0, '#ecf7ff'], [0.2, '#a9d7ff'], [1, '#59a6ff']]);
const yBox = (p, d) => `<path d="${d}" fill="url(#${p}-y)" stroke="#b86920" ${SW}/>`;
const bBox = (p, d) => `<path d="${d}" fill="url(#${p}-b)" stroke="#1b6cd0" ${SW}/>`;
const bar = d => `<path d="${d}" fill="#4c4465"/>`;

const shadow = p => rad(`${p}-s`, [[0, '#000000', 0.35], [1, '#000000', 0]]);
const arrowDefs = p => shadow(p) + lin(`${p}-a`, [[0, '#d2e6fa'], [1, '#6a9fdc']]);

// ---------- icons ----------
const icons = {
  // --- alignment ---
  balign: p => [alignDefs(p),
    yBox(p, 'M2.5 6.5H8.5V14.5H2.5Z'), bBox(p, 'M11.5 4.5H19.5V14.5H11.5Z'), bar('M1 16H21V18H1Z')],
  talign: p => [alignDefs(p),
    bar('M1 6H21V8H1Z'), yBox(p, 'M2.5 9.5H8.5V17.5H2.5Z'), bBox(p, 'M11.5 9.5H19.5V19.5H11.5Z')],
  lalign: p => [alignDefs(p),
    bar('M4 1H6V21H4Z'), yBox(p, 'M7.5 2.5H15.5V8.5H7.5Z'), bBox(p, 'M7.5 11.5H17.5V19.5H7.5Z')],
  ralign: p => [alignDefs(p),
    bar('M16 1H18V21H16Z'), yBox(p, 'M6.5 2.5H14.5V8.5H6.5Z'), bBox(p, 'M4.5 11.5H14.5V19.5H4.5Z')],
  chalign: p => [alignDefs(p),
    bar('M10 1H12V20H10Z'), yBox(p, 'M6.5 2.5H15.5V7.5H6.5Z'), bBox(p, 'M5.5 10.5H16.5V18.5H5.5Z')],
  cvalign: p => [alignDefs(p),
    bar('M1 10H21V12H1Z'), yBox(p, 'M2.5 6.5H8.5V15.5H2.5Z'), bBox(p, 'M11.5 5.5H19.5V16.5H11.5Z')],

  // --- border flags ---
  top: () => ['', bar('M1 1H21V3H1Z'),
    dots([...range(4, 16).flatMap(y => [[1, y], [19, y]]), ...range(1, 19).map(x => [x, 19])])],
  bottom: () => ['', bar('M1 19H21V21H1Z'),
    dots([...range(1, 19).map(x => [x, 1]), ...range(4, 16).flatMap(y => [[1, y], [19, y]])])],
  left: () => ['', bar('M1 1H3V21H1Z'),
    dots([...range(4, 19).flatMap(x => [[x, 1], [x, 19]]), ...range(4, 16).map(y => [19, y])])],
  right: () => ['', bar('M19 1H21V21H19Z'),
    dots([...range(1, 16).flatMap(x => [[x, 1], [x, 19]]), ...range(4, 16).map(y => [1, y])])],

  stretch: p => [lin(`${p}-a`, [[0, '#4aa8ff'], [1, '#0060c8']], 0, 0, 0, 1) +
                 lin(`${p}-h`, [[0, '#0060c8'], [1, '#4aa8ff']], 0, 0, 1, 0),
    `<path d="M6.5 0L10.5 4H9V10H4V4H2.5Z" fill="url(#${p}-a)"/>`,
    `<path d="M22 15.5L18 11.5V13H13V18H18V19.5Z" fill="url(#${p}-h)"/>`,
    `<path d="M1 11H12V21H1ZM3 13V19H10V13Z" fill="#4c4465" fill-rule="evenodd"/>`],

  expand: p => [rad(`${p}-a`, [[0, '#6ab8ff'], [1, '#0a5cc6']], 11, 11, 15.6, ' gradientUnits="userSpaceOnUse"'),
    `<path fill="url(#${p}-a)" d="M0 0H6.5L4.5 2L8 5.5L5.5 8L2 4.5L0 6.5Z` +
      `M22 0H15.5L17.5 2L14 5.5L16.5 8L20 4.5L22 6.5Z` +
      `M0 22H6.5L4.5 20L8 16.5L5.5 14L2 17.5L0 15.5Z` +
      `M22 22H15.5L17.5 20L14 16.5L16.5 14L20 17.5L22 15.5Z"/>`,
    `<path d="M7 7H15V15H7ZM9 9V13H13V9Z" fill="#4c4465" fill-rule="evenodd"/>`],

  swap: p => [lin(`${p}-bg`, [[0, '#8ab4ff'], [1, '#3a6ee8']], 0, 0, 1, 1),
    `<rect x="1.5" y="1.5" width="19" height="19" rx="2" fill="url(#${p}-bg)" stroke="#3460d0" ${SW}/>`,
    `<path d="M4 7L8 3.5V6H17V8H8V10.5ZM18 15L14 11.5V14H5V16H14V18.5Z" fill="#ffffff"/>`],

  // --- edit ---
  copy: p => [lin(`${p}-d`, [[0, '#ffffff'], [1, '#e6e6e6']], 0, 0, 1, 1),
    `<path d="M0.5 0.5H14.5V15.5H0.5Z" fill="url(#${p}-d)" stroke="#878c8a" ${SW}/>`,
    `<path d="M3 4.5H12M3 6.5H12M3 8.5H12M3 10.5H12M3 12.5H12" stroke="#ababab" ${SW}/>`,
    `<path d="M7.5 4.5H21.5V15.5L15.5 21.5H7.5Z" fill="url(#${p}-d)" stroke="#878c8a" ${SW} stroke-linejoin="round"/>`,
    `<path d="M21.5 15.5H15.5V21.5" fill="#cfcfcf" stroke="#878c8a" ${SW} stroke-linejoin="round"/>`,
    `<path d="M10 8.5H19M10 10.5H19M10 12.5H19M10 14.5H14" stroke="#ababab" ${SW}/>`],

  cut: p => [lin(`${p}-m`, [[0, '#f4f5f7'], [1, '#a4aab6']], 0, 0, 1, 0) +
             lin(`${p}-r`, [[0, '#f08080'], [1, '#c00000']]),
    `<path d="M17 0.5L15.5 1.5L7.5 13.5L9.5 14.5ZM5 0.5L6.5 1.5L14.5 13.5L12.5 14.5Z" fill="url(#${p}-m)" stroke="#7d8699" ${SW} stroke-linejoin="round"/>`,
    `<circle cx="11" cy="10.5" r="1" fill="#5a6070"/>`,
    `<path d="M10 17A4 4 0 1 0 2 17A4 4 0 1 0 10 17ZM8 17A2 2 0 1 0 4 17A2 2 0 1 0 8 17Z` +
      `M20 17A4 4 0 1 0 12 17A4 4 0 1 0 20 17ZM18 17A2 2 0 1 0 14 17A2 2 0 1 0 18 17Z" fill="url(#${p}-r)" fill-rule="evenodd" stroke="#900000" ${SW}/>`],

  paste: p => [lin(`${p}-b`, [[0, '#dca04a'], [1, '#9a5a10']], 0, 0, 1, 1) +
               lin(`${p}-d`, [[0, '#ffffff'], [1, '#e4e4e4']], 0, 0, 1, 1) +
               lin(`${p}-c`, [[0, '#f4f4f4'], [1, '#9a9a9a']]),
    `<rect x="2.5" y="3.5" width="17" height="17" rx="1.5" fill="url(#${p}-b)" stroke="#6e3f0c" ${SW}/>`,
    `<path d="M5.5 6.5H16.5V15.5L12.5 19.5H5.5Z" fill="url(#${p}-d)" stroke="#8a8a8a" ${SW} stroke-linejoin="round"/>`,
    `<path d="M16.5 15.5H12.5V19.5" fill="#d0d0d0" stroke="#8a8a8a" ${SW} stroke-linejoin="round"/>`,
    `<path d="M7 9.5H15M7 11.5H15M7 13.5H11" stroke="#a8a8a8" ${SW}/>`,
    `<path d="M7.5 6.5V3.5H9.5A1.5 1.5 0 0 1 12.5 3.5H14.5V6.5Z" fill="url(#${p}-c)" stroke="#505050" ${SW} stroke-linejoin="round"/>`],

  delete: p => [rad(`${p}-x`, [[0, '#ffb8b8'], [0.45, '#f03838'], [1, '#c80000']], 0.35, 0.3, 0.8),
    `<path d="M1 5L5 1L11 7L17 1L21 5L15 11L21 17L17 21L11 15L5 21L1 17L7 11Z" fill="url(#${p}-x)" stroke="#a01818" ${SW} stroke-linejoin="round"/>`],

  undo: p => [arrowDefs(p),
    `<ellipse cx="11" cy="19.5" rx="10" ry="2.5" fill="url(#${p}-s)"/>`,
    `<path d="M7.5 1.5L1 8L7.5 14.5V10.5H9A3.5 3.5 0 0 1 12.5 14V18.5H17.5V14A8.5 8.5 0 0 0 9 5.5H7.5Z" fill="url(#${p}-a)" stroke="#3d6fb5" ${SW} stroke-linejoin="round"/>`],
  redo: p => [arrowDefs(p),
    `<ellipse cx="11" cy="19.5" rx="10" ry="2.5" fill="url(#${p}-s)"/>`,
    `<path d="M14.5 1.5L21 8L14.5 14.5V10.5H13A3.5 3.5 0 0 0 9.5 14V18.5H4.5V14A8.5 8.5 0 0 1 13 5.5H14.5Z" fill="url(#${p}-a)" stroke="#3d6fb5" ${SW} stroke-linejoin="round"/>`],

  // --- files ---
  new: p => [lin(`${p}-d`, [[0, '#ffffff'], [1, '#e0e0e0']]) +
             lin(`${p}-t`, [[0, '#3d6fc0'], [1, '#a8c4ec']], 0, 0, 1, 0) +
             rad(`${p}-g`, [[0, '#ffffd8'], [0.35, '#ffea30'], [1, '#ffea30', 0]]),
    `<path d="M1.5 2.5H20.5V19.5H1.5Z" fill="url(#${p}-d)"/>`,
    `<path d="M1.5 2.5H20.5V6.5H1.5Z" fill="url(#${p}-t)"/>`,
    `<path d="M1.5 2.5H20.5V19.5H1.5Z" fill="none" stroke="#7d8ca6" ${SW}/>`,
    `<circle cx="18" cy="4" r="4" fill="url(#${p}-g)"/>`],

  open: p => [lin(`${p}-k`, [[0, '#ececec'], [1, '#a4a4a4']]) +
              lin(`${p}-f`, [[0, '#bcd6f6'], [1, '#4f82cc']]),
    `<path d="M1.5 3.5H7.5L9.5 5.5H17.5V18.5H1.5Z" fill="url(#${p}-k)" stroke="#666666" ${SW} stroke-linejoin="round"/>`,
    `<path d="M4.5 1.5H16.5V12.5H4.5Z" fill="#ffffff" stroke="#9a9a9a" ${SW}/>`,
    `<path d="M6 4.5H15M6 6.5H15M6 8.5H15" stroke="#c8c8c8" ${SW}/>`,
    `<path d="M1.5 19.5L4.5 9.5H20.5L17.5 19.5Z" fill="url(#${p}-f)" stroke="#3a64a8" ${SW} stroke-linejoin="round"/>`],

  save: p => [lin(`${p}-b`, [[0, '#8eb0e6'], [1, '#3a5ca6']], 0, 0, 1, 1) +
              lin(`${p}-r`, [[0, '#f46060'], [1, '#c82020']]) +
              lin(`${p}-s`, [[0, '#f0f0f0'], [1, '#8a8a8a']], 0, 0, 1, 0),
    `<path d="M1.5 1.5H18.5L20.5 3.5V20.5H1.5Z" fill="url(#${p}-b)"/>`,
    `<path d="M4.5 1.5V9.5H17.5V1.5" fill="#ffffff" stroke="#9fb4d8" ${SW}/>`,
    `<path d="M5 2H17V4H5Z" fill="url(#${p}-r)"/>`,
    `<path d="M7 6.5H15" stroke="#c0c0c0" ${SW}/>`,
    `<path d="M6.5 13.5H15.5V20.5H6.5Z" fill="url(#${p}-s)" stroke="#444444" ${SW}/>`,
    `<path d="M9 15H11V19H9Z" fill="#333333"/>`,
    `<path d="M1.5 1.5H18.5L20.5 3.5V20.5H1.5Z" fill="none" stroke="#2c4a88" ${SW} stroke-linejoin="round"/>`],

  generate: p => [rad(`${p}-g`, [[0, '#f4f4ff'], [0.6, '#c4c6f0'], [1, '#8a8ed8']], 0.4, 0.35, 0.7),
    `<path fill-rule="evenodd" d="${gear(11, 11, 10, 7, 3, 8, 0.2, 0.3)}" fill="url(#${p}-g)" stroke="#3a5aa8" ${SW} stroke-linejoin="round"/>`],
};

// ---------- rendering ----------
const render = name => {
  const [defs, ...body] = icons[name](name.replace(/\+/g, 'x'));
  return `<svg xmlns="http://www.w3.org/2000/svg" width="22" height="22" viewBox="0 0 22 22">\n` +
    (defs ? `<defs>${defs}</defs>\n` : '') + body.join('\n') + '\n</svg>\n';
};
module.exports = { icons, render };
