# SLK_img2pixel Manual

This is a complete usage guide for SLK_img2pixel, covering every feature:
sampling, dithering, palettes, color adjustments, the UI theme, GIF
import/export, and Lua scripting. For the full scripting reference, see
[LUA_SCRIPTING.md](LUA_SCRIPTING.md).

There are two programs:

- **SLK_img2pix** - the windowed app, with a live preview.
- **SLK_img2pix_cmd** - the command-line version, for automation/scripting.
  Same engine, no preview window.

---

## 1. The basics

Open **SLK_img2pix** and either drag an image onto the window, or use
**Load > Image**. The right-hand tab strip (Sample / Dither / Palette /
Colors / Theme) controls how the image gets turned into pixel art, and
changes apply to the live preview immediately. When you're happy with it,
use **Save > Image** and pick a filename - the file extension you type
decides the output format (`.png`, `.bmp`, `.tga`, `.pcx`, or `.gif`).

Supported image formats for input: PNG, GIF (all frames, see §4), BMP,
JPEG, TGA.

### Sample tab

Controls the pixel-art resolution and how the source image is downscaled
into it.

| Control | What it does |
|---|---|
| Absolute / Relative | Absolute: set an exact output size in pixels. Relative: output size is the input size divided by a factor (e.g. "÷4" turns a 512px image into 128px). |
| Width / Height (absolute) | Exact output dimensions. |
| Scale X / Scale Y (relative) | Divisor applied to the input size. |
| Sample mode | The filter used when downscaling: Nearest, Bilinear, Bicubic, Lanczos, or Cluster (a dithering-aware downsampler, generally the best choice for pixel art). |
| Sample x/y offset | Shifts the sampling grid, for fine-tuning which pixels get sampled. |
| Blur amount | Gaussian blur applied *before* downsampling, to soften the source so downsampling doesn't alias as badly. |
| Sharpen amount | Unsharp-mask sharpening applied *before* downsampling. |

### Dither tab

Controls how the (large) set of source colors gets reduced down to your
target palette.

| Control | What it does |
|---|---|
| Alpha threshold | Pixels with alpha below this become fully transparent in the output. |
| Distance metric | How "distance" between two colors is measured when picking the closest palette entry: RGB Euclidian, RGB Weighted, RGB Redmean, CIE76, CIE94, or CIEDE2000 (the LAB-space metrics generally match human color perception better, at some CPU cost). |
| Dither/Assignment mode | See the table below. |
| Dither amount | (Ordered-dithering modes only) how strong the dither pattern is. |

Available dither modes:

| Mode | Kind | Notes |
|---|---|---|
| None | closest-color | No dithering, just flat color banding. |
| Bayer 8x8 / 4x4 / 2x2 | ordered | Classic ordered dithering. Larger matrix means smoother gradients, but a more visible repeating pattern. |
| Bayer 5x5 / 3x3 *(new)* | ordered | Non-power-of-two ordered dither, generated as an evenly-dispersed dot pattern rather than the classic recursive Bayer construction (which only exists for power-of-two sizes). Useful when 4x4 looks too coarse and 8x8 too fine. |
| Cluster 8x8 / 4x4 | ordered | Clustered-dot ordered dithering. Clumps dots together rather than spreading them out, giving a halftone-print look. |
| Floyd-Steinberg / Floyd-Steinberg 2 | error diffusion | The classic error-diffusion dither. "2" is a simplified/faster variant. |
| Stucki *(new)* | error diffusion | Wider error-diffusion kernel than Floyd-Steinberg, giving smoother gradients with less visible directional artifacting. |
| Burkes *(new)* | error diffusion | Similar to Stucki but with a cheaper 2-row kernel, a middle ground between Floyd-Steinberg and Stucki. |
| Sierra *(new)* | error diffusion | Another wide-kernel diffusion dither, roughly the same quality tier as Stucki. |
| Sierra Two-Row *(new)* | error diffusion | Cheaper 2-row version of Sierra. |
| Sierra Lite *(new)* | error diffusion | Very cheap 3-tap diffusion kernel, the fastest of the diffusion dithers, at a bit lower quality. |
| Median-Cut | palette assignment | Not really a "dither", it assigns colors via median-cut bucketing. |
| PicoCAD *(new)* | special | Mimics the picoCAD 3D tool's shading trick: instead of a full dither pattern, it only ever uses a single fixed checkerboard to blend between the two closest palette colors. Gives a distinctive "retro low-color 3D render" look rather than a photographic dither. |

### Palette tab

- **Red / Green / Blue** sliders edit the currently-selected palette color
  (click a color swatch to select it).
- **Color count** sets how many palette entries are in use.
- **Generate palette** runs k-means clustering on the current (processed)
  image to auto-pick a palette. **k-means++** toggles a smarter, slower
  seeding method for that clustering.
- **Load > Palette** / **Save > Palette** read and write palette files
  (`.gpl`, `.png`, `.hex`, `.pal`, and more).

### Colors tab

Post-processing color adjustments applied before dithering: Brightness,
Contrast, Saturation, Hue, Gamma, and a Tint (red/green/blue) that gets
multiplied over the whole image.

### Theme tab *(new)*

Customizes the app's own UI colors. This has nothing to do with your
image, it's purely cosmetic for the app window itself.

- **Presets**: Default (the original look), Dark, Light, Pink, one click
  each.
- **Custom colors**: five fields, each a 6-digit hex code (`RRGGBB`),
  edited directly:
  - **Background** - the flat fill behind most widgets
  - **Border/shadow** - widget outlines and pressed-state shading
  - **Bevel (dark)** / **Bevel (light)** - the two halves of the 3D bevel
    edge every button/slider has
  - **Text** - label and button text color

  Type a new value and press Enter to apply it live. Whatever you set is
  remembered and reloaded automatically the next time you open the app.

### Presets (JSON)

**Load > Preset** / **Save > Preset** save all of the above (sample,
dither, palette, colors settings, not the theme, which is separate) to a
JSON file, so you can reuse a full configuration later, share it, or use
it with the command-line tool via `--preset`.

### Batch processing

**Tools > Batch** processes every image in a folder with the current
settings, writing results to another folder. Good for converting a whole
folder of source images at once without scripting.

---

## 2. GIF import & export *(new)*

Both **SLK_img2pix** and **SLK_img2pix_cmd** can read and write animated
GIFs, decoding and encoding every frame, not just the first one.

- **Import**: drag a `.gif` onto the window, or use **Load > Image** and
  pick one. Every frame is decoded, and while the live preview only shows
  frame 1, all frames are kept.
- **Export**: use **Save > Image** and type (or pick) a filename ending
  in `.gif`. If the loaded input was itself an animated GIF, every frame
  gets processed with your current settings and re-encoded as a new
  animated GIF, using one shared palette so the whole animation stays
  consistent. If the input wasn't animated, you still get a (single-frame)
  GIF, which also works as a plain "export as GIF" for any image.

Frame timing (delay per frame) is preserved from the source GIF.

On the command line, this all happens automatically based on file
extensions, so `--in foo.gif --out bar.gif` just works.

---

## 3. Lua scripting *(new)*

Both programs can run Lua scripts for automation: looping over many
files, rendering the same image in every dither mode, picking settings
based on a filename, and so on. See §4 below for the full API and worked
examples.

- **Command line**: `SLK_img2pix_cmd --script myscript.lua`
- **GUI**: use **Tools > Run script...**, pick a `.lua` file, and click
  **Run**. Anything the script `print()`s appears in the window, since
  there's no console to print to otherwise.

---

## 4. Lua API reference

See [LUA_SCRIPTING.md](LUA_SCRIPTING.md) for the full API reference,
worked examples, and a rundown of what scripting is (and isn't) good for.

---

## 5. Command-line reference (SLK_img2pix_cmd)

```
SLK_img2pix_cmd --in filename --out filename [--preset preset]
   --in                 image file to process (image or .gif)
   --out                output file (image or .gif)
   --outp                output palette
   --dump-defaults       write default preset to stdout
   --gen COLORS          generate a COLORS-entry color palette via k-means
   --preset              JSON preset file to use for processing
   --script              run a Lua script instead of a single --in/--out job
   --help                print this text
```

A typical single-shot conversion:

```sh
SLK_img2pix_cmd --in photo.png --out pixelart.png --preset my_settings.json
```

Dump the current defaults to a preset file you can then edit by hand:

```sh
SLK_img2pix_cmd --dump-defaults > default.json
```

---

## 6. Where things are stored

- **settings.json** (next to the executable) - your window's remembered
  file-dialog paths, GUI scale, and theme colors. Delete it to reset the
  app to first-run state.
- **default.json** (next to the executable) - loaded automatically on
  startup if present. This is what "load default preset on launch" means
  in practice.