# Lua scripting

Both **SLK_img2pix_cmd** (`--script file.lua`) and **SLK_img2pix**
(**Tools > Run script...**) can run Lua 5.4 scripts. A script sees one
global table, `img2pixel`, through which it can change every setting the
GUI's tabs expose, and process images.

This isn't a plugin system, scripts can't add new dither algorithms or
change the UI. What they can do is drive the existing pipeline
programmatically: loops, conditionals, file discovery, batching, and
picking settings based on logic, none of which the GUI or a single preset
file can do on its own.

It doesn't touch the live GUI preview or your currently-loaded image.
`img2pixel.process()` always reads from and writes to files on disk,
independent of whatever you have open. This is deliberate, so running a
script never changes what's on screen out from under you (though once
the script finishes, the GUI's sliders and preview do refresh to reflect
any settings the script changed).

## Quick start

```lua
-- Convert one image with custom settings
img2pixel.size_absolute_x = 96
img2pixel.size_absolute_y = 96
img2pixel.dither_mode = img2pixel.DITHER.STUCKI
img2pixel.palette_colors = 16

local ok = img2pixel.process("input.png", "output.png")
print("done:", ok)
```

Run it with `SLK_img2pix_cmd --script convert.lua`, or load it via
**Tools > Run script...** in the GUI.

## Settings

These are plain fields on `img2pixel`, read or write them directly. They
map straight onto the same values the JSON preset format and the GUI's
sliders use.

| Field | Type | Matches GUI control |
|---|---|---|
| `blur_amount` | number | Sample tab: Blur amount |
| `sharp_amount` | number | Sample tab: Sharpen amount |
| `sample_mode` | integer | Sample tab: Sample mode (0=Nearest, 1=Bilinear, 2=Bicubic, 3=Lanczos, 4=Cluster) |
| `x_offset`, `y_offset` | number | Sample tab: Sample x/y offset |
| `scale_relative` | boolean | Sample tab: Absolute/Relative toggle |
| `size_absolute_x`, `size_absolute_y` | integer | Sample tab: Width/Height (absolute mode) |
| `size_relative_x`, `size_relative_y` | integer | Sample tab: Scale X/Y (relative mode) |
| `dither_mode` | integer | Dither tab: Dither/Assignment mode, use `img2pixel.DITHER.*` (see below) rather than a raw number |
| `color_dist` | integer | Dither tab: Distance metric, use `img2pixel.COLORDIST.*` |
| `dither_amount` | number | Dither tab: Dither amount |
| `dither_alpha_threshold` | integer (0-255) | Dither tab: Alpha threshold |
| `target_colors` | integer | (Median-cut) target color count |
| `palette_colors` | integer (1-256) | Palette tab: Color count |
| `kmeanspp` | boolean | Palette tab: k-means++ checkbox |
| `brightness`, `contrast`, `saturation`, `hue`, `gamma` | number | Colors tab |
| `tint_red`, `tint_green`, `tint_blue` | integer (0-255) | Colors tab: Tint |

Reading an unset/unknown field returns `nil`. Writing an unknown field
name raises a Lua error, so a typo fails loudly instead of silently doing
nothing.

## Named constants

Instead of memorizing dither-mode numbers, use:

```lua
img2pixel.dither_mode = img2pixel.DITHER.PICOCAD
```

`img2pixel.DITHER` has one entry per mode: `NONE`, `BAYER8X8`, `BAYER4X4`,
`BAYER2X2`, `CLUSTER8X8`, `CLUSTER4X4`, `FLOYD`, `FLOYD2`, `MEDIAN_CUT`,
`BAYER5X5`, `BAYER3X3`, `STUCKI`, `BURKES`, `SIERRA`, `SIERRA_TWOROW`,
`SIERRA_LITE`, `PICOCAD`.

`img2pixel.COLORDIST` has: `RGB_EUCLIDIAN`, `RGB_WEIGHTED`, `RGB_REDMEAN`,
`LAB_CIE76`, `LAB_CIE94`, `LAB_CIEDE2000`.

## Functions

### `img2pixel.process(in_path, out_path)` -> boolean

Runs the full pipeline (sample, dither, save) on `in_path` using the
current settings, and writes to `out_path`. Output format is decided by
`out_path`'s extension, same as the GUI's Save dialog: `.gif` writes an
animated GIF (all frames, if the input was an animated GIF), anything
else writes a single still image. Returns `true` on success, `false` on
failure (bad path, unreadable image, etc.). It does not raise an error,
so check the return value if you need to know.

### `img2pixel.load_preset(path)` -> boolean

Loads a JSON preset file (the same format **Save > Preset** writes),
overwriting the current settings fields. Returns `true`/`false`.

### `img2pixel.get_palette_color(index)` -> integer

Returns palette entry `index` (0-255) as a `0xRRGGBB` integer.

### `img2pixel.set_palette_color(index, rgb)`

Sets palette entry `index` (0-255) to the color `rgb` (a `0xRRGGBB`
integer, e.g. `0xff8000` for orange).

### `print(...)`

Standard Lua `print`. On the command line this goes to stdout as normal.
In the GUI, output is captured and shown in the Run Script window instead,
since there's no console to print to there.

Everything else in the standard Lua library is available too (`string`,
`table`, `math`, `os`, `io`, loops, `pcall`, etc.). Scripts run with the
same access to your filesystem as the app itself, same as any local
script you'd run yourself.

## Examples

### Render every dither mode for comparison

```lua
img2pixel.size_absolute_x = 64
img2pixel.size_absolute_y = 64
img2pixel.palette_colors = 16

for name, mode in pairs(img2pixel.DITHER) do
   img2pixel.dither_mode = mode
   img2pixel.process("source.png", "compare_" .. name .. ".png")
end
print("done")
```

### Batch-convert a folder, with per-file logic

```lua
-- requires an external file-listing helper since Lua's stdlib alone
-- can't list directory contents; os.execute + a shell command works:
local p = io.popen('ls *.png')
for filename in p:lines() do
   if filename:find("icon") then
      img2pixel.palette_colors = 8
      img2pixel.size_absolute_x = 32
      img2pixel.size_absolute_y = 32
   else
      img2pixel.palette_colors = 32
      img2pixel.size_absolute_x = 128
      img2pixel.size_absolute_y = 128
   end
   img2pixel.process(filename, "out_" .. filename)
end
p:close()
```

### Sweep a parameter to find the best value

```lua
img2pixel.dither_mode = img2pixel.DITHER.STUCKI
for colors = 4, 32, 4 do
   img2pixel.palette_colors = colors
   img2pixel.process("source.png", string.format("sweep_%02d.png", colors))
end
```

### Build a custom palette in code

```lua
img2pixel.palette_colors = 8
for i = 0, 7 do
   local v = math.floor(255 * i / 7)
   img2pixel.set_palette_color(i, v * 0x010101) -- grayscale ramp
end
img2pixel.dither_mode = img2pixel.DITHER.FLOYD
img2pixel.process("source.png", "grayscale8.png")
```