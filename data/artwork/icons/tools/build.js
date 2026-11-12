// Regenerates the complete SVG icon set in data/artwork/icons and verifies it against the bitmap icons
// of the repository. With --platform the raster platform icons in data/platform are rebuilt as well
// (Windows only, see build_platform.js).
// Usage: node build.js [--platform]
'use strict';
const path = require('path');
const { execFileSync } = require('child_process');

const run = script => execFileSync(process.execPath, [path.join(__dirname, script)], { stdio: 'inherit' });

for (const script of ['gen_resources.js', 'gen_additional_a.js', 'gen_additional_b.js', 'gen_common.js',
  'gen_containers_forms_layout_sdk.js']) run(script);
if (process.argv.includes('--platform')) run('build_platform.js');
run('verify.js');
