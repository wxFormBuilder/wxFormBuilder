// Generates resources/icons SVGs (16x16 icons, logo16x16, logo32x32, the 512x512 application icon and,
// via resources22.js, the 22x22 icons) plus the platform icon sources in platform/.
// Usage: node gen_resources.js [<svgRoot>]   (default: the data/artwork/icons directory)
'use strict';
const fs = require('fs');
const path = require('path');

const svgRoot = process.argv[2] || path.resolve(__dirname, '..');
const outDir = path.join(svgRoot, 'resources', 'icons');

const SW = 'stroke-width="1"';

// ---------- helpers ----------
const stopsXml = stops => stops.map(([o, c, a]) =>
  `<stop offset="${o}" stop-color="${c}"${a !== undefined ? ` stop-opacity="${a}"` : ''}/>`).join('');
const lin = (id, stops, x1 = 0, y1 = 0, x2 = 0, y2 = 1, extra = '') =>
  `<linearGradient id="${id}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"${extra}>${stopsXml(stops)}</linearGradient>`;
const rad = (id, stops, cx = 0.5, cy = 0.5, r = 0.5, extra = '') =>
  `<radialGradient id="${id}" cx="${cx}" cy="${cy}" r="${r}"${extra}>${stopsXml(stops)}</radialGradient>`;
const n = v => +v.toFixed(2);

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

// Uniformly scales + translates an absolute path (M L H V C A Z only): p' = (p - from) * k + to.
function xform(d, k, [fx, fy], [tx, ty]) {
  const X = v => n((v - fx) * k + tx), Y = v => n((v - fy) * k + ty), R = v => n(v * k);
  const toks = d.match(/[A-Za-z]|-?\d*\.?\d+/g);
  let out = '', i = 0, cmd = '';
  const num = () => parseFloat(toks[i++]);
  while (i < toks.length) {
    if (/[A-Za-z]/.test(toks[i])) { cmd = toks[i++]; out += cmd; if (cmd === 'Z') continue; }
    else out += ' ';
    switch (cmd) {
      case 'M': case 'L': out += `${X(num())} ${Y(num())}`; break;
      case 'H': out += X(num()); break;
      case 'V': out += Y(num()); break;
      case 'C': out += [0, 1, 2].map(() => `${X(num())} ${Y(num())}`).join(' '); break;
      case 'A': { const rx = R(num()), ry = R(num()), rot = num(), la = num(), sw = num();
        out += `${rx} ${ry} ${rot} ${la} ${sw} ${X(num())} ${Y(num())}`; break; }
      default: throw new Error(`unsupported command ${cmd}`);
    }
  }
  return out;
}

// ---------- shared parts ----------
const pageDefs = p => lin(`${p}-pg`, [[0, '#ffffff'], [1, '#e2e2e2']], 0, 0, 1, 1);
const page16 = (p, x0) =>
  `<path d="M${x0}.5 0.5H11.5L14.5 3.5V15.5H${x0}.5Z" fill="url(#${p}-pg)" stroke="#9a9a9a" ${SW} stroke-linejoin="round"/>` +
  `<path d="M11.5 0.5V3.5H14.5" fill="#dcdcdc" stroke="#9a9a9a" ${SW} stroke-linejoin="round"/>`;
const glyphDefs = p => lin(`${p}-c`, [[0, '#8ab8f2'], [1, '#2a5aa8']]);

const squaresDefs = p =>
  lin(`${p}-r`, [[0, '#ffa088'], [0.5, '#f0401c'], [1, '#c41800']]) +
  lin(`${p}-u`, [[0, '#d4ecff'], [0.5, '#62aef2'], [1, '#2a78d0']]) +
  lin(`${p}-w`, [[0, '#fff8b0'], [0.5, '#ffe000'], [1, '#f0aa00']]);
const sq = (id, x, y, w, h, r, stroke, sw = 1) =>
  `<rect x="${x}" y="${y}" width="${w}" height="${h}" rx="${r}" fill="url(#${id})" stroke="${stroke}" stroke-width="${sw}"/>`;

// Python logo halves (designed on the 22 grid around centre 11,12).
const PY_BLUE = 'M9 4H13A2 2 0 0 1 15 6V10A2 2 0 0 1 13 12H9A2 2 0 0 0 7 14V16H5A2 2 0 0 1 3 14V10A2 2 0 0 1 5 8H11V7H7V6A2 2 0 0 1 9 4Z';
const PY_YELLOW = 'M13 20H9A2 2 0 0 1 7 18V14A2 2 0 0 1 9 12H13A2 2 0 0 0 15 10V8H17A2 2 0 0 1 19 10V14A2 2 0 0 1 17 16H11V17H15V18A2 2 0 0 1 13 20Z';

// ---------- logo master geometry (32-unit grid; the 512 master is this x16) ----------
// Squares: outer left/top edge and outer size in grid units.
const LOGO_SQUARES = [['r', '#a01000', 1, 9], ['u', '#1858b0', 8, 1], ['w', '#c08800', 17, 7]];
const LOGO_SQ = 14;
// "wxFB": angular letters. F and B are stroke centre lines (miter joins, square caps); at 32 px the
// letter weight is 2 px, so caps span y 23..31 and the x-height y 25..31. F has a wide top bar.
const LOGO_STROKES = [
  [[18, 30], [18, 24], [22, 24]],
  [[18, 27], [21, 27]],
];
// B: two equal bowls whose right corners are rounded with radius 1.
const LOGO_B = 'M26 27H29A1 1 0 0 0 30 26V25A1 1 0 0 0 29 24H25V30H29A1 1 0 0 0 30 29V28A1 1 0 0 0 29 27H26';
// Polygon/sector cover of the B stroke (half width h, scale k) for the outline layer.
const bCover = (h, k) => {
  const lines = [[[[26, 27], [29, 27]], true, false], [[[30, 26], [30, 25]], false, false],
    [[[29, 24], [25, 24], [25, 30], [29, 30]], false, false], [[[30, 29], [30, 28]], false, false],
    [[[29, 27], [26, 27]], false, true]];
  const sector = ([cx, cy], a0, a1) => {
    const p = (r, a) => `${n((cx + r * Math.cos(a * Math.PI / 180)) * k)} ${n((cy + r * Math.sin(a * Math.PI / 180)) * k)}`;
    const ro = n((1 + h) * k), ri = n((1 - h) * k);
    return `M${p(1 + h, a0)}A${ro} ${ro} 0 0 0 ${p(1 + h, a1)}L${p(1 - h, a1)}A${ri} ${ri} 0 0 1 ${p(1 - h, a0)}Z`;
  };
  return lines.flatMap(([pts, cs, ce]) => strokeCover(pts, h, cs, ce)).map(q => shapeD(q, k)).join('') +
    sector([29, 26], 90, 0) + sector([29, 25], 0, -90) + sector([29, 29], 90, 0) + sector([29, 28], 0, -90);
};
// Diagonal bars with horizontal cuts: centre line from (x0, y0) to (x1, y1), horizontal width w.
const bar = (x0, y0, x1, y1, w) => [[x0 - w / 2, y0], [x0 + w / 2, y0], [x1 + w / 2, y1], [x1 - w / 2, y1]];
// Fills the acute tip of the counter between edge a (left) and edge b (right) up to the point where
// the counter is `gap` wide, so an outline of that width does not leave dark slivers in the tip.
function counterTip([[ax0, ay0], [ax1, ay1]], [[bx0, by0], [bx1, by1]], gap) {
  const ka = (ax1 - ax0) / (ay1 - ay0), kb = (bx1 - bx0) / (by1 - by0);
  const xa = y => ax0 + ka * (y - ay0), xb = y => bx0 + kb * (y - by0);
  const yp = (ax0 - ka * ay0 - bx0 + kb * by0) / (kb - ka), yg = yp + gap / (kb - ka);
  return [[xa(yp), yp], [xa(yg), yg], [xb(yg), yg]];
}
// w and x as diagonal bars for half letter weight h (in grid units); `gap` > 0 adds counter tips.
const logoX = (h, gap = 0) => {
  const t = 26 - h, b = 30 + h, apex = t, ww = 1.9 * h, xw = 2.4 * h;
  const l = 11.5 - h + xw / 2, r = 15 + h - xw / 2;
  const w = [bar(1.5, t, 3.6, b, ww), bar(3.6, b, 5.3, apex, ww), bar(5.3, apex, 7, b, ww), bar(7, b, 9.1, t, ww)];
  const right = s => [s[1], s[2]], left = s => [s[0], s[3]];
  const tips = gap ? [counterTip(right(w[0]), left(w[1]), gap), counterTip(right(w[1]), left(w[2]), gap),
    counterTip(right(w[2]), left(w[3]), gap)] : [];
  return [...w, ...tips, bar(l, t, r, b, xw), bar(r, t, l, b, xw)];
};
const polyD = (pts, k = 1) => 'M' + pts.map(([x, y]) => `${n(x * k)} ${n(y * k)}`).join('L');
// Closed polygon with clockwise orientation, so overlapping polygons never cancel under nonzero fill.
const area = pts => pts.reduce((s, [x, y], i) => { const [x2, y2] = pts[(i + 1) % pts.length]; return s + x * y2 - x2 * y; }, 0);
const shapeD = (pts, k = 1) => polyD(area(pts) < 0 ? [...pts].reverse() : pts, k) + 'Z';
const strokesD = k => LOGO_STROKES.map(pts => polyD(pts, k)).join('') + xform(LOGO_B, k, [0, 0], [0, 0]);
const xD = (h, k, gap = 0) => logoX(h, gap).map(pts => shapeD(pts, k)).join('');

// Polygon cover of a miter-joined, square-capped stroke (half width h): one quad per segment plus
// the miter wedges. Used for the outline layer, which is stroked with round joins behind the letters.
function strokeCover(pts, h, capStart = true, capEnd = true) {
  const norm = ([x0, y0], [x1, y1]) => { const l = Math.hypot(x1 - x0, y1 - y0); return [(x1 - x0) / l, (y1 - y0) / l]; };
  const out = [];
  for (let i = 0; i + 1 < pts.length; i++) {
    const [dx, dy] = norm(pts[i], pts[i + 1]), [nx, ny] = [-dy * h, dx * h];
    const s = i === 0 && capStart ? h : 0, e = i + 2 === pts.length && capEnd ? h : 0;
    const a = [pts[i][0] - dx * s, pts[i][1] - dy * s], b = [pts[i + 1][0] + dx * e, pts[i + 1][1] + dy * e];
    out.push([[a[0] + nx, a[1] + ny], [b[0] + nx, b[1] + ny], [b[0] - nx, b[1] - ny], [a[0] - nx, a[1] - ny]]);
  }
  for (let i = 1; i + 1 < pts.length; i++) {
    const d1 = norm(pts[i - 1], pts[i]), d2 = norm(pts[i], pts[i + 1]);
    const n1 = [-d1[1], d1[0]], n2 = [-d2[1], d2[0]], dot = n1[0] * n2[0] + n1[1] * n2[1];
    if (dot < -0.99) continue;
    const v = pts[i], m = h / (1 + dot);
    for (const sgn of [1, -1]) {
      out.push([v, [v[0] + sgn * h * n1[0], v[1] + sgn * h * n1[1]],
        [v[0] + sgn * m * (n1[0] + n2[0]), v[1] + sgn * m * (n1[1] + n2[1])],
        [v[0] + sgn * h * n2[0], v[1] + sgn * h * n2[1]]]);
    }
  }
  return out;
}
// 16 px lettering: the same letters hinted to whole pixels (5 px caps, 4 px x-height).
const LOGO_TEXT16 =
  'M0 12h1v2h-1zM2 12h1v2h-1zM4 12h1v2h-1zM1 14h1v2h-1zM3 14h1v2h-1z' +
  'M6 12h1v1h-1zM8 12h1v1h-1zM7 13h1v2h-1zM6 15h1v1h-1zM8 15h1v1h-1z' +
  'M10 11h2v1h-2zM10 12h1v4h-1zM11 13h1v1h-1z' +
  'M13 11h2v1h-2zM13 12h1v3h-1zM14 13h1v1h-1zM13 15h2v1h-2zM15 12h1v1h-1zM15 14h1v1h-1z';
const logoTextDefs = (p, y1, y2) =>
  lin(`${p}-t`, [[0, '#b4dcff'], [0.5, '#4a94f0'], [1, '#1a56b8']], 0, y1, 0, y2, ' gradientUnits="userSpaceOnUse"');

// 1 px pixel lettering needs a darker gradient than the outlined lettering of the large logo.
const pixTextDefs = (p, y1, y2) =>
  lin(`${p}-t`, [[0, '#4a8ff0'], [1, '#1848a0']], 0, y1, 0, y2, ' gradientUnits="userSpaceOnUse"');

// 16 px logo (squares halved from the 32 grid, lettering hinted), placeable at an offset.
const logo16Parts = (p, ox = 0, oy = 0) => [squaresDefs(p) + pixTextDefs(p, 11 + oy, 16 + oy),
  ...LOGO_SQUARES.map(([c, stroke, x, y]) =>
    sq(`${p}-${c}`, Math.floor(x / 2) + ox + 0.5, Math.floor(y / 2) + oy + 0.5, 6, 6, 1, stroke)),
  `<path d="${ox || oy ? xformRel(LOGO_TEXT16, ox, oy) : LOGO_TEXT16}" fill="url(#${p}-t)"/>`];

// Moves a path built from M/h/v/z commands by (dx, dy).
function xformRel(d, dx, dy) {
  return d.replace(/M(-?[\d.]+) (-?[\d.]+)/g, (_, x, y) => `M${n(+x + dx)} ${n(+y + dy)}`);
}

// ---------- 16x16 icons ----------
const icons16 = {
  'c++': p => [lin(`${p}-pg`, [[0, '#ffffff'], [1, '#b8d6f4']], 0, 0, 1, 1) +
               rad(`${p}-g`, [[0, '#f8f8f8'], [1, '#9a9a9a']], 0.4, 0.35, 0.7),
    `<path d="M1.5 0.5H11.5L14.5 3.5V15.5H1.5Z" fill="url(#${p}-pg)" stroke="#5a8fcb" ${SW} stroke-linejoin="round"/>`,
    `<path d="M11.5 0.5V3.5H14.5" fill="#c0daf4" stroke="#5a8fcb" ${SW} stroke-linejoin="round"/>`,
    `<path d="M4 3.5H9M4 5.5H7" stroke="#a6cbe9" ${SW}/>`,
    `<path fill-rule="evenodd" d="${gear(10.5, 10.5, 5, 3.6, 1.3, 8, 0.22, 0.32)}" fill="url(#${p}-g)" stroke="#5d5d5d" ${SW} stroke-linejoin="round"/>`],

  cpp: p => [pageDefs(p) + glyphDefs(p),
    page16(p, 3),
    `<path d="M7.18 5.25A3.5 3.5 0 1 0 7.18 9.75L5.73 8.53A1.6 1.6 0 1 1 5.73 6.47Z" fill="url(#${p}-c)"/>`,
    `<path d="M8 7.5H11M9.5 6V9M12 7.5H15M13.5 6V9" stroke="#3c74d0" ${SW}/>`],

  h: p => [pageDefs(p) + glyphDefs(p),
    page16(p, 2),
    `<path d="M0.5 3.5H2.5V7H5A2 2 0 0 1 7 9V12.5H5V9.5H2.5V12.5H0.5Z" fill="url(#${p}-c)"/>`],

  xrc: p => [pageDefs(p) + glyphDefs(p),
    page16(p, 3),
    `<path d="M3 4.5H5L2.5 7.5L5 10.5H3L0.5 7.5ZM6.5 4.5H8.5L11 7.5L8.5 10.5H6.5L9 7.5Z" fill="url(#${p}-c)"/>`],

  php: p => [pageDefs(p) + lin(`${p}-e`, [[0, '#a4a8e6'], [1, '#585ea6']]),
    page16(p, 3),
    `<ellipse cx="7" cy="7" rx="6.5" ry="3.75" fill="url(#${p}-e)" stroke="#3c4078" ${SW}/>`,
    `<path d="M2.5 5V10M3 5.5H4.5V7.5H3M6.5 4V8.5M7 5.5H8.5V8.5M10.5 5V9.5M11 5.5H12.5V7.5H11" fill="none" stroke="#ffffff" ${SW}/>`],

  python: p => [pageDefs(p) +
      lin(`${p}-b`, [[0, '#6aaee4'], [1, '#2f6698']], 0, 0, 1, 1) +
      lin(`${p}-y`, [[0, '#ffd43b'], [1, '#ffef9a']], 0, 0, 1, 1),
    page16(p, 1),
    `<path d="${xform(PY_BLUE, 0.625, [11, 12], [7.5, 8.5])}" fill="url(#${p}-b)"/>`,
    `<path d="${xform(PY_YELLOW, 0.625, [11, 12], [7.5, 8.5])}" fill="url(#${p}-y)"/>`],

  designer: p => [lin(`${p}-t`, [[0, '#92b9e8'], [1, '#6a8fc6']]) +
                  lin(`${p}-w`, [[0, '#f6f6f6'], [1, '#e2e2e2']]),
    `<path d="M0.5 1.5H14.5V13.5H0.5Z" fill="url(#${p}-w)"/>`,
    `<path d="M0.5 1.5H14.5V3.5H0.5Z" fill="url(#${p}-t)" stroke="#5a7cb2" ${SW}/>`,
    `<path d="M2 6H5V7H2ZM2 10H5V11H2Z" fill="#5883d2"/>`,
    `<path d="M6.5 5.5H13.5V7.5H6.5ZM6.5 9.5H13.5V11.5H6.5Z" fill="#ffffff" stroke="#a8a8a8" ${SW}/>`,
    `<path d="M0.5 1.5H14.5V13.5H0.5Z" fill="none" stroke="#5a7cb2" ${SW}/>`],

  properties: p => [lin(`${p}-w`, [[0, '#ffffff'], [1, '#e2ebf6']]) +
                    lin(`${p}-t`, [[0, '#8fb4ea'], [1, '#4f84d6']]) +
                    lin(`${p}-h`, [[0, '#fbe4bc'], [1, '#d49a5e']], 0, 0, 1, 1),
    `<path d="M0.5 4.5H13.5V14.5H0.5Z" fill="url(#${p}-w)"/>`,
    `<path d="M0.5 4.5H13.5V6.5H0.5Z" fill="url(#${p}-t)"/>`,
    `<path d="M2 8H4V9H2Z" fill="#2a9a2a"/>`,
    `<path d="M2 10H4V11H2Z" fill="#cc3c3c"/>`,
    `<path d="M2 12H4V13H2Z" fill="#4a86d8"/>`,
    `<path d="M5 8H12V9H5ZM5 10H12V11H5ZM5 12H12V13H5Z" fill="#c8c8c8"/>`,
    `<path d="M0.5 4.5H13.5V14.5H0.5Z" fill="none" stroke="#3f73c4" ${SW}/>`,
    `<path d="M6.5 0.5H13.5A1.5 1.5 0 0 1 15 2V3H13V4.5A1 1 0 0 1 11 4.5V5.5A1 1 0 0 1 9 5.5V6A1 1 0 0 1 7 6V5.5A1 1 0 0 1 5 5.5V2A1.5 1.5 0 0 1 6.5 0.5Z" fill="url(#${p}-h)" stroke="#8a5a2a" ${SW} stroke-linejoin="round"/>`],

  events: p => [lin(`${p}-z`, [[0, '#fff8c8'], [0.4, '#ffe400'], [1, '#ff7a00']]),
    `<path d="M6 0.5H11L9 4.5H11.5L5.5 15.5L7 8.5H4.5Z" fill="url(#${p}-z)" stroke="#b05400" ${SW} stroke-linejoin="round"/>`],

  lua: p => [squaresDefs(p) + rad(`${p}-m`, [[0, '#8a8af0'], [1, '#000080']], 0.35, 0.3, 0.75),
    `<circle cx="8" cy="8.5" r="6.5" fill="none" stroke="#d8d8d8" ${SW}/>`,
    sq(`${p}-r`, 2.5, 9.5, 3, 3, 1, '#a01000'),
    sq(`${p}-u`, 5.5, 5.5, 3, 3, 1, '#1858b0'),
    sq(`${p}-w`, 9.5, 8.5, 3, 3, 1, '#c08800'),
    `<circle cx="12.5" cy="3" r="2.5" fill="url(#${p}-m)" stroke="#000060" ${SW}/>`],

  logo16x16: p => logo16Parts(p),
};

// ---------- 32x32 logo ----------
const glossDefs = p => lin(`${p}-gl`, [[0, '#ffffff', 0.75], [1, '#ffffff', 0.1]]);
// Glossy highlight in the upper part of a rounded square (x, y, size, inset, corner radius).
const gloss = (p, x, y, w, h, i, r) => {
  const L = x + i, T = y + i, Rr = x + w - i;
  return `<path d="M${n(L)} ${n(T + r)}A${r} ${r} 0 0 1 ${n(L + r)} ${n(T)}H${n(Rr - r)}A${r} ${r} 0 0 1 ${n(Rr)} ${n(T + r)}` +
    `V${n(y + h * 0.42)}C${n(x + w * 0.7)} ${n(y + h * 0.3)} ${n(x + w * 0.35)} ${n(y + h * 0.62)} ${n(L)} ${n(y + h * 0.5)}Z" fill="url(#${p}-gl)"/>`;
};

// 32 px: squares on the pixel grid (1 px contour), lettering 2 px strong without outline.
const logo32 = p => [squaresDefs(p) + glossDefs(p) + logoTextDefs(p, 23, 31),
  ...LOGO_SQUARES.map(([c, stroke, x, y]) =>
    sq(`${p}-${c}`, x + 0.5, y + 0.5, LOGO_SQ - 1, LOGO_SQ - 1, 2, stroke) + gloss(p, x + 0.5, y + 0.5, LOGO_SQ - 1, LOGO_SQ - 1, 1.5, 1)),
  `<path d="${strokesD(1)}" fill="none" stroke="url(#${p}-t)" stroke-width="2" stroke-linecap="square"/>`,
  `<path d="${xD(1, 1)}" fill="url(#${p}-t)"/>`];

// ---------- 512x512 application icon (master: the 32 grid scaled by 16) ----------
// Letter weight 24 plus a 6 px outline whose round joins give the corners a slight rounding.
const LOGO_K = 16, LOGO_SW = 12, LOGO_TEXT_W = 24, LOGO_TEXT_OUTLINE = 6;
const logo512 = p => {
  const S = LOGO_SQ * LOGO_K - LOGO_SW, h = LOGO_TEXT_W / 2 / LOGO_K, gap = 2 * LOGO_TEXT_OUTLINE / LOGO_K;
  const cover = LOGO_STROKES.flatMap(pts => strokeCover(pts, h)).concat(logoX(h, gap))
    .map(pts => shapeD(pts, LOGO_K)).join('') + bCover(h, LOGO_K);
  return [squaresDefs(p) + glossDefs(p) + logoTextDefs(p, 23 * LOGO_K, 31 * LOGO_K),
    ...LOGO_SQUARES.map(([c, stroke, x, y]) => {
      const rx = x * LOGO_K + LOGO_SW / 2, ry = y * LOGO_K + LOGO_SW / 2;
      return sq(`${p}-${c}`, rx, ry, S, S, 32, stroke, LOGO_SW) + gloss(p, rx, ry, S, S, 14, 22);
    }),
    `<path d="${cover}" fill="#123c8c" stroke="#123c8c" stroke-width="${2 * LOGO_TEXT_OUTLINE}" stroke-linejoin="round"/>`,
    `<path d="${strokesD(LOGO_K)}" fill="none" stroke="url(#${p}-t)" stroke-width="${LOGO_TEXT_W}" stroke-linecap="square"/>`,
    `<path d="${xD(h, LOGO_K, gap)}" fill="url(#${p}-t)"/>`];
};

// ---------- macOS application icon (Apple 1024 grid) ----------
// Rounded-square plate 824x824 at (100, 100) with corner radius 185.4 and a soft drop shadow, as in
// Apple's macOS app icon template; the 512 master artwork is centred on it at 125 %.
const logoMac = p => {
  const [defs, ...body] = logo512(p);
  return [defs +
      lin(`${p}-plate`, [[0, '#ffffff'], [1, '#dfe7f1']]) +
      `<filter id="${p}-shadow" x="-10%" y="-10%" width="120%" height="125%">` +
      `<feDropShadow dx="0" dy="12" stdDeviation="14" flood-color="#000000" flood-opacity="0.3"/></filter>`,
    `<rect x="100" y="100" width="824" height="824" rx="185.4" fill="url(#${p}-plate)" filter="url(#${p}-shadow)"/>`,
    `<rect x="101" y="101" width="822" height="822" rx="184.4" fill="none" stroke="#000000" stroke-opacity="0.08" stroke-width="2"/>`,
    `<g transform="translate(188 188) scale(1.25)">`, ...body, `</g>`];
};

// ---------- macOS document icon for .fbp project files (Apple 1024 grid) ----------
// Page with folded top-right corner, the three logo squares and the "FBP" extension label. The label
// uses the logo letter forms (F, B with rounded right corners, P) as 2-unit strokes on a local grid
// with cap centre lines at y 0..6.
const FBP_LABEL =
  'M1 6V0H5M1 3H4' +
  'M9 3H12A1 1 0 0 0 13 2V1A1 1 0 0 0 12 0H8V6H12A1 1 0 0 0 13 5V4A1 1 0 0 0 12 3H9' +
  'M16 6V0H20A1 1 0 0 1 21 1V2A1 1 0 0 1 20 3H16';
const logoDoc = p => {
  const [defs, ...body] = logo512(p);
  const LK = 14; // label scale: 22 x 8 units -> 308 x 112 px
  return [defs +
      lin(`${p}-page`, [[0, '#ffffff'], [1, '#eef1f5']]) +
      lin(`${p}-fold`, [[0, '#f4f6f9'], [1, '#c9d0da']], 0, 0, 1, 1) +
      `<filter id="${p}-shadow" x="-15%" y="-10%" width="130%" height="125%">` +
      `<feDropShadow dx="0" dy="12" stdDeviation="14" flood-color="#000000" flood-opacity="0.3"/></filter>`,
    `<path d="M232 80H672L832 240V904A40 40 0 0 1 792 944H232A40 40 0 0 1 192 904V120A40 40 0 0 1 232 80Z" fill="url(#${p}-page)" filter="url(#${p}-shadow)"/>`,
    `<path d="M232 81H672L831 240V904A39 39 0 0 1 792 943H232A39 39 0 0 1 193 904V120A39 39 0 0 1 232 81Z" fill="none" stroke="#000000" stroke-opacity="0.12" stroke-width="2"/>`,
    `<path d="M672 80V200A40 40 0 0 0 712 240H832Z" fill="url(#${p}-fold)" stroke="#000000" stroke-opacity="0.12" stroke-width="2" stroke-linejoin="round"/>`,
    `<g transform="translate(307 287) scale(0.8)">`, ...body.slice(0, LOGO_SQUARES.length), `</g>`,
    `<path d="${xform(FBP_LABEL, LK, [0, -1], [358, 768])}" fill="none" stroke="#7d8796" stroke-width="${2 * LK}" stroke-linecap="square"/>`];
};

// ---------- pixel-hinted small platform icons ----------
// App logo at 20/22/24 px: squares (outer size s at the given outer top-left corners) and the 16 px
// lettering centred horizontally on the bottom rows.
const SMALL_LOGO = {
  20: { s: 8, r: [1, 6], u: [5, 0], w: [11, 3] },
  22: { s: 9, r: [1, 7], u: [5, 0], w: [12, 4] },
  24: { s: 10, r: [1, 8], u: [6, 0], w: [13, 5] },
};
const stroked = Object.fromEntries(LOGO_SQUARES.map(([c, stroke]) => [c, stroke]));
const pxSquares = (p, s, pos, rx = 1) => ['r', 'u', 'w'].map(c =>
  sq(`${p}-${c}`, pos[c][0] + 0.5, pos[c][1] + 0.5, s - 1, s - 1, rx, stroked[c]));
const logoSmall = (p, size) => {
  const L = SMALL_LOGO[size], ox = (size - 16) / 2, oy = size - 16;
  return [squaresDefs(p) + pixTextDefs(p, 11 + oy, 16 + oy), ...pxSquares(p, L.s, L),
    `<path d="${xformRel(LOGO_TEXT16, ox, oy)}" fill="url(#${p}-t)"/>`];
};

// macOS app icon at 16/32 px: plate (Apple proportions) with the hinted logo, without drop shadow
// blur (a faint 1 px shade under the plate instead).
const macSmall = (p, size) => {
  const plateDefs = lin(`${p}-plate`, [[0, '#ffffff'], [1, '#dfe7f1']]);
  if (size === 16) {
    return [squaresDefs(p) + plateDefs,
      `<path d="M3 15h10v1h-10z" fill="#000000" fill-opacity="0.08"/>`,
      `<rect x="1.5" y="1.5" width="13" height="13" rx="3" fill="url(#${p}-plate)" stroke="#000000" stroke-opacity="0.15" ${SW}/>`,
      ...pxSquares(p, 5, { r: [3, 7], u: [5, 4], w: [8, 6] })];
  }
  return [squaresDefs(p) + plateDefs + pixTextDefs(p, 21, 26),
    `<path d="M5 29h22v1h-22z" fill="#000000" fill-opacity="0.08"/>`,
    `<rect x="3.5" y="3.5" width="25" height="25" rx="6" fill="url(#${p}-plate)" stroke="#000000" stroke-opacity="0.12" ${SW}/>`,
    ...pxSquares(p, 8, { r: [6, 12], u: [10, 6], w: [17, 10] }),
    `<path d="${xformRel(LOGO_TEXT16, 8, 10)}" fill="url(#${p}-t)"/>`];
};

// Document icon at 16..32 px: page with folded corner and the three squares; "FBP" label at 32 px.
const SMALL_DOC = {
  16: { page: [2, 0, 13, 15], fold: 4, s: 4, r: [4, 8], u: [6, 5], w: [8, 7] },
  20: { page: [3, 0, 16, 19], fold: 4, s: 5, r: [5, 10], u: [7, 6], w: [10, 9] },
  22: { page: [3, 0, 18, 21], fold: 5, s: 6, r: [5, 11], u: [8, 7], w: [11, 10] },
  24: { page: [3, 0, 20, 23], fold: 6, s: 6, r: [6, 12], u: [9, 8], w: [12, 11] },
  32: { page: [5, 1, 26, 30], fold: 6, s: 7, r: [8, 13], u: [12, 9], w: [16, 12], label: [10, 23] },
};
// 5-row pixel lettering "FBP" (F 3 wide, B and P with rounded right corners), top-left at (x, y).
const fbpPixels = (x, y) => [
  [0, 0, 3, 1], [0, 1, 1, 4], [1, 2, 1, 1],
  [4, 0, 2, 1], [4, 1, 1, 4], [5, 2, 1, 1], [5, 4, 1, 1], [6, 1, 1, 1], [6, 3, 1, 1],
  [8, 0, 2, 1], [8, 1, 1, 4], [9, 2, 1, 1], [10, 1, 1, 1],
].map(([dx, dy, w, h]) => `M${x + dx} ${y + dy}h${w}v${h}h-${w}z`).join('');
const docSmall = (p, size) => {
  const D = SMALL_DOC[size], [l, t, r, b] = D.page, f = D.fold;
  const page = `M${l}.5 ${t}.5H${r - f}.5L${r}.5 ${t + f}.5V${b}.5H${l}.5Z`;
  return [squaresDefs(p) + lin(`${p}-page`, [[0, '#ffffff'], [1, '#e9edf2']]) + lin(`${p}-fold`, [[0, '#f4f6f9'], [1, '#c9d0da']], 0, 0, 1, 1),
    `<path d="${page}" fill="url(#${p}-page)" stroke="#8f99a6" ${SW}/>`,
    `<path d="M${r - f}.5 ${t}.5V${t + f}.5H${r}.5Z" fill="url(#${p}-fold)" stroke="#8f99a6" ${SW} stroke-linejoin="round"/>`,
    ...pxSquares(p, D.s, D),
    ...(D.label ? [`<path d="${fbpPixels(...D.label)}" fill="#7d8796"/>`] : [])];
};

// ---------- project (22 px): page + the 16 px logo placed at (6, 6) ----------
const project = p => {
  const [logoDefs, ...logoBody] = logo16Parts(p, 6, 6);
  return [logoDefs + lin(`${p}-d`, [[0, '#ffffff'], [1, '#e2e2e2']], 0, 0, 1, 1),
    `<path d="M1.5 1.5H14.5V15.5H1.5Z" fill="url(#${p}-d)" stroke="#8a8a8a" ${SW}/>`,
    `<path d="M3 4.5H12M3 6.5H8M3 8.5H5M3 10.5H5M3 12.5H5" stroke="#b4b4b4" ${SW}/>`,
    ...logoBody];
};

// ---------- write ----------
const svgDoc = (size, defs, body) =>
  `<svg xmlns="http://www.w3.org/2000/svg" width="${size}" height="${size}" viewBox="0 0 ${size} ${size}">\n` +
  (defs ? `<defs>${defs}</defs>\n` : '') + body.join('\n') + '\n</svg>\n';
const prefixOf = name => 'res-' + name.replace(/\+/g, 'x');

fs.mkdirSync(outDir, { recursive: true });
const written = [];
for (const [name, fn] of Object.entries(icons16)) {
  const [defs, ...body] = fn(prefixOf(name));
  fs.writeFileSync(path.join(outDir, `${name}.svg`), svgDoc(16, defs, body)); written.push(name);
}
{ const [defs, ...body] = logo32(prefixOf('logo32x32'));
  fs.writeFileSync(path.join(outDir, 'logo32x32.svg'), svgDoc(32, defs, body)); written.push('logo32x32'); }
{ const [defs, ...body] = logo512(prefixOf('logo'));
  fs.writeFileSync(path.join(outDir, 'logo.svg'), svgDoc(512, defs, body)); written.push('logo'); }

// Platform icon sources (svgRoot/platform/...), mirroring the build system's data/platform tree.
const titled = (svg, title) => svg.replace('>\n', `>\n<title>${title}</title>\n`);
const writeTo = (rel, content) => {
  const file = path.join(svgRoot, 'platform', ...rel.split('/'));
  fs.mkdirSync(path.dirname(file), { recursive: true }); fs.writeFileSync(file, content); written.push(rel);
};
{ const [defs, ...body] = logoMac('mac-icon');
  writeTo('macos/icon.svg', titled(svgDoc(1024, defs, body), 'wxFormBuilder')); }
const docSvg = p => { const [defs, ...body] = logoDoc(p); return titled(svgDoc(1024, defs, body), 'wxFormBuilder Project'); };
writeTo('macos/docicon.svg', docSvg('mac-docicon'));
const HICOLOR = 'linux/share/icons/hicolor/scalable';
{ const [defs, ...body] = logo512('linux-app');
  writeTo(`${HICOLOR}/apps/org.wxformbuilder.wxFormBuilder.svg`, titled(svgDoc(512, defs, body), 'wxFormBuilder')); }
writeTo(`${HICOLOR}/mimetypes/org.wxformbuilder.wxFormBuilder.application-x-wxformbuilder.svg`, docSvg('linux-mime'));
// Pixel-hinted sources for the small raster sizes of all platforms.
for (const size of Object.keys(SMALL_LOGO).map(Number)) {
  const [defs, ...body] = logoSmall(`small-icon${size}`, size);
  writeTo(`sources/icon${size}.svg`, svgDoc(size, defs, body));
}
for (const size of [16, 32]) {
  const [defs, ...body] = macSmall(`small-macicon${size}`, size);
  writeTo(`sources/macicon${size}.svg`, svgDoc(size, defs, body));
}
for (const size of Object.keys(SMALL_DOC).map(Number)) {
  const [defs, ...body] = docSmall(`small-docicon${size}`, size);
  writeTo(`sources/docicon${size}.svg`, svgDoc(size, defs, body));
}
{ const [defs, ...body] = project(prefixOf('project'));
  fs.writeFileSync(path.join(outDir, 'project.svg'), svgDoc(22, defs, body)); written.push('project'); }

// Remaining 22x22 resource icons come from resources22.js.
const res22 = require('./resources22.js');
const keep22 = ['balign', 'bottom', 'chalign', 'copy', 'cut', 'cvalign', 'delete', 'expand', 'generate', 'lalign',
  'left', 'new', 'open', 'paste', 'ralign', 'redo', 'right', 'save', 'stretch', 'swap', 'talign', 'top', 'undo'];
for (const name of keep22) {
  fs.writeFileSync(path.join(outDir, `${name}.svg`), res22.render(name)); written.push(name);
}
console.log(`${written.length} files written to ${svgRoot}`);
