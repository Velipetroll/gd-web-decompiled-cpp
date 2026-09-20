# Geometry Dash clone — decompiled, renamed & split

## What was done

The original `index-game.js` was a single 56,000-line file: the entire
Phaser 3.90.0 engine bundled via webpack (~51,000 lines) plus your actual
game code (~5,000 lines), all obfuscated with `javascript-obfuscator` -
every string replaced with calls into a lookup table, e.g. `_0x4e0e(0x1a2)`.

This rewrite:

1. **Replaces the vendor bundle** with the real, official `phaser.min.js`
   3.90.0 build (`assets/vendor/phaser.min.js`) - identical to what
   `npm install phaser@3.90.0` gives you.
2. **Fully deobfuscated your game code**: resolved ~40,000 obfuscated
   string-array calls (including multi-hop alias chains) back to their real
   values - asset keys, Phaser method names, object property names, etc.
3. **Eliminated dead code**: the deobfuscation left behind hundreds of
   now-pointless leftover variable declarations (artifacts of the
   string-decoding machinery). These were removed with a real dead-code
   elimination pass (not just hidden/stubbed).
4. **Renamed every remaining auto-generated identifier** - every local
   variable, function parameter, function name, and loop label that started
   out as `_0x...` hex or a bare 1-2 letter minified name now has a real
   word-based name. This was done with a scope-aware script (using Babel's
   parser/traverser, not find-and-replace) so renames can never collide with
   an unrelated variable of the same short name elsewhere, and can never
   break cross-file references.
5. **Split the code into 16 focused files** under `src/` (see below),
   instead of one continuous blob, and ran Prettier over all of them.

## How the automatic renaming worked, and what to expect

There is no way to recover the *original* names your code once had - only
the string literals survived obfuscation, not the identifier names. So
every renamed variable's new name is a **best-effort guess** built from:

- What it's assigned (`new Sprite(...)` → `sprite`, `this.add.rectangle(...)`
  → `rectangle`, a string literal `"hazard"` → `hazard`, etc.)
- How it's used afterward (assigned to `.scaleX`, `.setOrigin(...)` etc. →
  named as a game object; used as a loop bound → `i`)
- Its position in a callback (first argument to `.on("pointerdown", ...)`
  → `pointer`, arguments to `.forEach(...)` → `item`/`index`, etc.)

Where none of these heuristics found a strong signal, variables got a
generic but still real name (`value`, `value2`, `options`, `table`, etc.) -
you'll see plenty of these, especially in the bundled pako compression
library (see below) where a purely numeric algorithm doesn't lend itself to
descriptive names. A handful of the most-used top-level helpers were
additionally renamed by hand once their purpose was clear from reading the
code (`findAtlasFrame`, `addImageFromAtlas`, `createLayeredSprite`,
`drawExpandingRing`, `spawnFinishParticles`, `defineFontFromFnt`,
`zeroArray`, plus the shared constants in `constants.js` like
`screenWidth`/`screenHeight`/`flipY`).

**If a name still reads generically once you're in the code, that's an
honest signal it needs a human's judgment** - the heuristics got the vast
majority of cases to something meaningful, but a fully accurate name
sometimes requires understanding what the surrounding game logic is *for*,
which no automated pass can know for certain.

## File structure

```
index.html                        - wires everything together via <script> tags
assets/vendor/phaser.min.js       - official Phaser 3.90.0 (unmodified)
assets/                           - put your game assets here (see below)
src/
  constants.js                    - shared screen size, physics tuning constants, blend modes
  boot-scene.js         (BootScene)   - preload screen, loads all assets
  font-helpers.js                 - bitmap font (.fnt) parsing, used by BootScene
  player-physics-state.js (PlayerPhysicsState) - player's physics state (velocity, ground state, etc.)
  level-data-helpers.js           - texture-atlas frame lookup + the LevelObject class
  pako-compression.js             - a bundled copy of the pako/zlib deflate-inflate
                                     library (see note below) - likely used to
                                     decompress the level string
  level-renderer.js     (LevelRenderer) - ground/ceiling tiles, level object containers
  trail-renderer.js     (TrailRenderer) - the player's motion trail effect
  sprite-layer-helper.js          - creates a sprite from the atlas with depth/visibility set
  player.js              (Player) - the player entity: sprites, rotation, particles, explosion
  tween-value.js         (TweenValue) - tiny from/to/duration interpolation helper
  color-manager.js       (ColorManager) - level background/ground color state
  audio-manager.js       (AudioManager) - music playback and volume/metering
  game-scene.js           (GameScene) - the main gameplay scene, ties everything together
  win-effects.js                  - the expanding-ring / particle burst effect on level complete
  main.js                         - Phaser game config + `new Phaser.Game(...)`
```

## A note on `pako-compression.js`

While tracing through the code, this ~1,900-line chunk turned out to be a
bundled copy of **pako**, the well-known open-source JavaScript port of
zlib (deflate/inflate). It's genuinely entangled with `level-data-helpers.js`
(a couple of its static Huffman-tree setup tables ended up split across that
boundary) rather than being one clean self-contained block, so it wasn't
practical to simply swap in the official pako package the way Phaser was
swapped in. Its internals are renamed the same way as everything else
(no more hex names), but many of its variable names are still generic
(`table`, `value`, `byteTable`) since zlib's own algorithm doesn't map
cleanly onto descriptive names without deep expertise in the compression
format itself.

## Assets

`boot-scene.js` loads assets from a flat `assets/` folder using the exact
same filenames as your original build (`assets/GJ_WebSheet.png`,
`assets/1.txt`, `assets/StereoMadness.mp3`, etc.) - so you can drop your
existing `assets/` folder in as-is; nothing needs to change there.
