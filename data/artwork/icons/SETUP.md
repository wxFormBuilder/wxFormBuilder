# Setting up the icon tools

How to prepare a machine for working on the vector icons in this directory. The tools are Node.js
scripts and run the same way on Windows, Linux (including WSL 2) and macOS. What the tools do is
described in [`README.md`](README.md), how icons are drawn in [`STYLE.md`](STYLE.md).

## Requirements

| Tool | Version | Used for |
|---|---|---|
| Git | any current version | the repository |
| Node.js with npm | 18 or newer | all scripts in `tools` |
| `@resvg/resvg-wasm` | pinned in `tools/package-lock.json` | rendering SVG to PNG, installed by `npm ci` |
| A TrueType font | optional | labels of the developer sheets (`tools/dev/preview.js`) |

No browser and no native build tools are needed. The icons contain no text, so
fonts do not affect any icon.

## Install

### Windows

```powershell
winget install OpenJS.NodeJS.LTS
cd data\artwork\icons\tools
npm ci
```

### Ubuntu, also under WSL 2

```sh
sudo apt update
sudo apt install -y git nodejs npm fonts-dejavu-core
cd data/artwork/icons/tools
npm ci
```

`fonts-dejavu-core` only provides the label font of the developer sheets and can be left out.

### macOS (not tested yet)

```sh
brew install node
cd data/artwork/icons/tools
npm ci
```

`npm ci` installs exactly the package version recorded in `tools/package-lock.json` into
`tools/node_modules`, which Git ignores. The lock file contains no registry addresses
(`tools/.npmrc` sets `omit-lockfile-registry-resolved`), so the package is fetched from whatever
registry or mirror npm is configured to use and verified against the recorded checksum.

## Check the installation

Run from the repository root:

```sh
node data/artwork/icons/tools/build.js
```

It regenerates all SVGs and must end with `no problems`. On a branch that contains the generated
platform icons in `data/platform` (`feature/platform-icons`), additionally run:

```sh
node data/artwork/icons/tools/build.js --platform
git diff --quiet && echo "nothing changed"
```

The rebuilt `.ico`, `.icns` and `.png` files must be identical to the committed ones. They are
rendered by the WebAssembly build of resvg, which runs the same code on every operating system and
CPU, so the output does not depend on the machine.

## Working on icons

1. Edit the generator script of the icon in `tools` (never the generated SVG files).
2. Regenerate and verify: `node data/artwork/icons/tools/build.js`.
3. Look at the result, for example next to its bitmap:
   ```sh
   node data/artwork/icons/tools/dev/preview.js preview.png resources/icons/cut.png data/artwork/icons/resources/icons/cut.svg
   ```
   Write such images to a place outside the repository.
4. If a logo or a platform source changed, rebuild the platform icons with `build.js --platform`.

## Notes

- **Line endings.** The generators write LF. In a Windows checkout with `core.autocrlf=true`
  `git status` can therefore list regenerated files as modified although their content is unchanged;
  `git diff --quiet` tells whether anything really changed, and `git checkout -- .` resets such files.
- **WSL on a Windows checkout.** The tools also run from WSL on a repository under `/mnt/c`, with the
  `node_modules` installed from either side, because the renderer is platform independent. A clone
  inside the Linux file system is noticeably faster.
- **Label font.** The developer sheets look for a common system font (Segoe UI or Arial on Windows,
  DejaVu Sans, Liberation Sans or Noto Sans on Linux, Arial on macOS). Set `WXFB_FONT` to a `.ttf`
  file and `WXFB_FONT_FAMILY` to its family name to use another one. Without a font the sheets are
  rendered without labels.
- **Updating resvg.** Change the version in `tools/package.json`, run `npm install` in `tools` and
  rebuild the platform icons; a new renderer version may change pixels, which then shows up as changed
  files in `data/platform`. Commit `package.json`, `package-lock.json` and the rebuilt icons together.

## Tested environments

| System | Node.js | Result |
|---|---|---|
| Windows 11 | 26.13 | all scripts run; platform icons identical to the committed files |
| Ubuntu 26.04 under WSL 2 | 22.22 | all scripts run; platform icons byte-identical to the Windows build |
