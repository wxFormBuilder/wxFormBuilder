// Canvas of the application and plugin icons (module, no output).
// Icons drawn on the 22 px grid are placed with a 1 px margin on a 24 px canvas: 24 px is a standard
// toolbar icon size and stays integral at every display scale factor (30 px at 125 %, 42 px at 175 %),
// which 22 px does not. The drawing keeps its coordinates, the viewBox origin moves to (-1, -1).
'use strict';

const CANVAS = { 22: 24 };

// Canvas size of an icon drawn on a size x size pixel grid.
const canvasOf = size => CANVAS[size] || size;

// viewBox of an icon drawn on a size x size pixel grid.
const viewBoxOf = size => { const c = canvasOf(size), o = (size - c) / 2; return `${o} ${o} ${c} ${c}`; };

// SVG document of an icon drawn on a size x size pixel grid.
const iconDoc = (size, defs, body) =>
  `<svg xmlns="http://www.w3.org/2000/svg" width="${canvasOf(size)}" height="${canvasOf(size)}" viewBox="${viewBoxOf(size)}">\n` +
  (defs ? `<defs>${defs}</defs>\n` : '') + body.join('\n') + '\n</svg>\n';

module.exports = { canvasOf, viewBoxOf, iconDoc };
