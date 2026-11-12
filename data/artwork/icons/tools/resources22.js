// 22x22 resource icons (stroke-width 1, outlines on half-pixel coordinates): module used by
// gen_resources.js, which writes the icons whose source PNG really is 22x22.
// Flat style: colours come from palette.js, no gloss highlights or ground shadows.
'use strict';

const { fill, stroke, DARK, LINE } = require('./palette.js');

const SW = 'stroke-width="1"';
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
const alignDefs = p => fill(`${p}-y`, 'yellow') + fill(`${p}-b`, 'blue');
const yBox = (p, d) => `<path d="${d}" fill="url(#${p}-y)" stroke="${stroke('yellow')}" ${SW}/>`;
const bBox = (p, d) => `<path d="${d}" fill="url(#${p}-b)" stroke="${stroke('blue')}" ${SW}/>`;
const bar = d => `<path d="${d}" fill="${DARK}"/>`;
// Paper and folded-corner gradients of document shapes.
const pageDefs = p => fill(`${p}-pg`, 'paper') + fill(`${p}-fd`, 'fold', 1, 1);

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

  stretch: p => [fill(`${p}-a`, 'accent') + fill(`${p}-h`, 'accent', 1, 0),
    `<path d="M6.5 0L10.5 4H9V10H4V4H2.5Z" fill="url(#${p}-a)"/>`,
    `<path d="M22 15.5L18 11.5V13H13V18H18V19.5Z" fill="url(#${p}-h)"/>`,
    `<path d="M1 11H12V21H1ZM3 13V19H10V13Z" fill="${DARK}" fill-rule="evenodd"/>`],

  expand: p => [fill(`${p}-a`, 'accent'),
    `<path fill="url(#${p}-a)" d="M0 0H6.5L4.5 2L8 5.5L5.5 8L2 4.5L0 6.5Z` +
      `M22 0H15.5L17.5 2L14 5.5L16.5 8L20 4.5L22 6.5Z` +
      `M0 22H6.5L4.5 20L8 16.5L5.5 14L2 17.5L0 15.5Z` +
      `M22 22H15.5L17.5 20L14 16.5L16.5 14L20 17.5L22 15.5Z"/>`,
    `<path d="M7 7H15V15H7ZM9 9V13H13V9Z" fill="${DARK}" fill-rule="evenodd"/>`],

  swap: p => [fill(`${p}-bg`, 'accent'),
    `<rect x="1.5" y="1.5" width="19" height="19" rx="2" fill="url(#${p}-bg)" stroke="${stroke('accent')}" ${SW}/>`,
    `<path d="M4 7L8 3.5V6H17V8H8V10.5ZM18 15L14 11.5V14H5V16H14V18.5Z" fill="#ffffff"/>`],

  // --- edit ---
  copy: p => [pageDefs(p),
    `<path d="M0.5 0.5H14.5V15.5H0.5Z" fill="url(#${p}-pg)" stroke="${stroke('paper')}" ${SW}/>`,
    `<path d="M3 4.5H12M3 6.5H12M3 8.5H12M3 10.5H12M3 12.5H12" stroke="${LINE}" ${SW}/>`,
    `<path d="M7.5 4.5H21.5V15.5L15.5 21.5H7.5Z" fill="url(#${p}-pg)" stroke="${stroke('paper')}" ${SW} stroke-linejoin="round"/>`,
    `<path d="M21.5 15.5H15.5V21.5" fill="url(#${p}-fd)" stroke="${stroke('paper')}" ${SW} stroke-linejoin="round"/>`,
    `<path d="M10 8.5H19M10 10.5H19M10 12.5H19M10 14.5H14" stroke="${LINE}" ${SW}/>`],

  // Scissors: each blade has its straight cutting edge on the inside, running into the tip, and a curved
  // back on the outside. The blades are separate paths, so the front one covers the back one.
  cut: p => [fill(`${p}-m`, 'metal', 1, 0) + fill(`${p}-r`, 'red'),
    ...['M5.5 0.5Q4.7 4.5 10.3 11L12.35 14.5L14.5 13.5Z', 'M16.5 0.5Q17.3 4.5 11.7 11L9.65 14.5L7.5 13.5Z'].map(d =>
      `<path d="${d}" fill="url(#${p}-m)" stroke="${stroke('metal')}" ${SW} stroke-linejoin="round"/>`),
    `<circle cx="11" cy="10" r="1" fill="${stroke('metal')}"/>`,
    `<path d="M10 17A4 4 0 1 0 2 17A4 4 0 1 0 10 17ZM8 17A2 2 0 1 0 4 17A2 2 0 1 0 8 17Z` +
      `M20 17A4 4 0 1 0 12 17A4 4 0 1 0 20 17ZM18 17A2 2 0 1 0 14 17A2 2 0 1 0 18 17Z" fill="url(#${p}-r)" fill-rule="evenodd" stroke="${stroke('red')}" ${SW}/>`],

  paste: p => [fill(`${p}-b`, 'wood') + pageDefs(p) + fill(`${p}-c`, 'metal'),
    `<rect x="2.5" y="3.5" width="17" height="17" rx="1.5" fill="url(#${p}-b)" stroke="${stroke('wood')}" ${SW}/>`,
    `<path d="M5.5 6.5H16.5V15.5L12.5 19.5H5.5Z" fill="url(#${p}-pg)" stroke="${stroke('paper')}" ${SW} stroke-linejoin="round"/>`,
    `<path d="M16.5 15.5H12.5V19.5" fill="url(#${p}-fd)" stroke="${stroke('paper')}" ${SW} stroke-linejoin="round"/>`,
    `<path d="M7 9.5H15M7 11.5H15M7 13.5H11" stroke="${LINE}" ${SW}/>`,
    `<path d="M7.5 6.5V3.5H9.5A1.5 1.5 0 0 1 12.5 3.5H14.5V6.5Z" fill="url(#${p}-c)" stroke="${stroke('metal')}" ${SW} stroke-linejoin="round"/>`],

  delete: p => [fill(`${p}-x`, 'red'),
    `<path d="M1 5L5 1L11 7L17 1L21 5L15 11L21 17L17 21L11 15L5 21L1 17L7 11Z" fill="url(#${p}-x)" stroke="${stroke('red')}" ${SW} stroke-linejoin="round"/>`],

  undo: p => [fill(`${p}-a`, 'arrow'),
    `<path d="M7.5 1.5L1 8L7.5 14.5V10.5H9A3.5 3.5 0 0 1 12.5 14V20.5H17.5V14A8.5 8.5 0 0 0 9 5.5H7.5Z" fill="url(#${p}-a)" stroke="${stroke('arrow')}" ${SW} stroke-linejoin="round"/>`],
  redo: p => [fill(`${p}-a`, 'arrow'),
    `<path d="M14.5 1.5L21 8L14.5 14.5V10.5H13A3.5 3.5 0 0 0 9.5 14V20.5H4.5V14A8.5 8.5 0 0 1 13 5.5H14.5Z" fill="url(#${p}-a)" stroke="${stroke('arrow')}" ${SW} stroke-linejoin="round"/>`],

  // --- files ---
  // New: empty window with a four-pointed spark at the top right corner.
  new: p => [fill(`${p}-d`, 'paper') + fill(`${p}-t`, 'title') + fill(`${p}-s`, 'yellow'),
    `<path d="M1.5 3.5H18.5V20.5H1.5Z" fill="url(#${p}-d)"/>`,
    `<path d="M1.5 3.5H18.5V7.5H1.5Z" fill="url(#${p}-t)"/>`,
    `<path d="M1.5 3.5H18.5V20.5H1.5Z" fill="none" stroke="${stroke('title')}" ${SW}/>`,
    `<path d="M17.5 0.5L18.75 3.25L21.5 4.5L18.75 5.75L17.5 8.5L16.25 5.75L13.5 4.5L16.25 3.25Z" fill="url(#${p}-s)" stroke="${stroke('yellow')}" ${SW} stroke-linejoin="round"/>`],

  open: p => [fill(`${p}-k`, 'grey') + fill(`${p}-f`, 'folder'),
    `<path d="M1.5 3.5H7.5L9.5 5.5H17.5V18.5H1.5Z" fill="url(#${p}-k)" stroke="${stroke('grey')}" ${SW} stroke-linejoin="round"/>`,
    `<path d="M4.5 1.5H16.5V12.5H4.5Z" fill="#ffffff" stroke="${stroke('paper')}" ${SW}/>`,
    `<path d="M6 4.5H15M6 6.5H15M6 8.5H15" stroke="${LINE}" ${SW}/>`,
    `<path d="M1.5 19.5L4.5 9.5H20.5L17.5 19.5Z" fill="url(#${p}-f)" stroke="${stroke('folder')}" ${SW} stroke-linejoin="round"/>`],

  save: p => [fill(`${p}-b`, 'disk') + fill(`${p}-r`, 'red') + fill(`${p}-s`, 'metal'),
    `<path d="M1.5 1.5H18.5L20.5 3.5V20.5H1.5Z" fill="url(#${p}-b)"/>`,
    `<path d="M4.5 1.5V9.5H17.5V1.5" fill="#ffffff" stroke="#9fb4d8" ${SW}/>`,
    `<path d="M5 2H17V4H5Z" fill="url(#${p}-r)"/>`,
    `<path d="M7 6.5H15" stroke="${LINE}" ${SW}/>`,
    `<path d="M6.5 13.5H15.5V20.5H6.5Z" fill="url(#${p}-s)" stroke="${stroke('metal')}" ${SW}/>`,
    `<path d="M9 15H11V19H9Z" fill="${stroke('disk')}"/>`,
    `<path d="M1.5 1.5H18.5L20.5 3.5V20.5H1.5Z" fill="none" stroke="${stroke('disk')}" ${SW} stroke-linejoin="round"/>`],

  generate: p => [fill(`${p}-g`, 'violet'),
    `<path fill-rule="evenodd" d="${gear(11, 11, 10, 7, 3, 8, 0.2, 0.3)}" fill="url(#${p}-g)" stroke="${stroke('violet')}" ${SW} stroke-linejoin="round"/>`],
};

// ---------- rendering ----------
const render = name => {
  const [defs, ...body] = icons[name](name.replace(/\+/g, 'x'));
  return `<svg xmlns="http://www.w3.org/2000/svg" width="22" height="22" viewBox="0 0 22 22">\n` +
    (defs ? `<defs>${defs}</defs>\n` : '') + body.join('\n') + '\n</svg>\n';
};
module.exports = { icons, render, gear };
