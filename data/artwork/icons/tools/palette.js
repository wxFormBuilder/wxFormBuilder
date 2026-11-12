// Shared flat colour palette of the application icons (resources/icons, sdk): every material has a
// subtle top-to-bottom gradient (light from above, no gloss) and a contour colour.
'use strict';

const stopsXml = stops => stops.map(([o, c, a]) =>
  `<stop offset="${o}" stop-color="${c}"${a !== undefined ? ` stop-opacity="${a}"` : ''}/>`).join('');
const lin = (id, stops, x1 = 0, y1 = 0, x2 = 0, y2 = 1, extra = '') =>
  `<linearGradient id="${id}" x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}"${extra}>${stopsXml(stops)}</linearGradient>`;
const rad = (id, stops, cx = 0.5, cy = 0.5, r = 0.5, extra = '') =>
  `<radialGradient id="${id}" cx="${cx}" cy="${cy}" r="${r}"${extra}>${stopsXml(stops)}</radialGradient>`;

const MATERIALS = {
  accent: { top: '#4aa3f5', bottom: '#0a63cc', stroke: '#0a4fa8' }, // arrows, markers
  blue: { top: '#8cc4f7', bottom: '#4b93e2', stroke: '#1f63c0' },   // boxes, panels
  yellow: { top: '#ffe066', bottom: '#f5b400', stroke: '#b5770a' },
  red: { top: '#f2654e', bottom: '#cf2a14', stroke: '#9b1508' },
  paper: { top: '#ffffff', bottom: '#edf0f4', stroke: '#8f99a6' },
  fold: { top: '#f2f4f7', bottom: '#cfd6df', stroke: '#8f99a6' },
  metal: { top: '#eef1f5', bottom: '#a3acb9', stroke: '#5f6878' },
  violet: { top: '#c7cbf6', bottom: '#8c92dc', stroke: '#3f4fa8' },
  wood: { top: '#dca253', bottom: '#a8661c', stroke: '#6e3f0c' },
  disk: { top: '#6f97da', bottom: '#3d61ab', stroke: '#2a4885' },
  folder: { top: '#a3c8f3', bottom: '#5b8fd8', stroke: '#3a64a8' },
  grey: { top: '#e5e8ec', bottom: '#b3bac4', stroke: '#6b7380' },
  title: { top: '#79a3e2', bottom: '#4a78c4', stroke: '#3f69ad' },
  arrow: { top: '#a9cdf5', bottom: '#5b93dc', stroke: '#2f62b0' },
  skin: { top: '#f8dcb0', bottom: '#d9a066', stroke: '#8a5a2a' },
  glyph: { top: '#4a8ff0', bottom: '#1f5fc0', stroke: '#173f8a' },  // lettering on documents
};
const DARK = '#4c4465';     // bars and frames of the layout icons
const LINE = '#b4bcc8';     // text lines on paper

// Gradient of a material (vertical by default), referenced as url(#id).
const fill = (id, material, x2 = 0, y2 = 1) =>
  lin(id, [[0, MATERIALS[material].top], [1, MATERIALS[material].bottom]], 0, 0, x2, y2);
const stroke = material => MATERIALS[material].stroke;

module.exports = { lin, rad, MATERIALS, DARK, LINE, fill, stroke };
