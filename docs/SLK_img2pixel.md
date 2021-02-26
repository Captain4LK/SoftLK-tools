The user interface is divided into two windows. 

# Preview

The window titled "Preview" serves as a preview window for both the input and the output image. You can switch between the input and output image by clicking the specific tab at the top of the window.

# Settings

The other window is titled "Settings" and is were you actually interact with the program. The window is divided into five tabs, aligned vertically on the left of the window.

## Save/Load

### Load image

Load an image from disc, supported formats (same as stb_image): 
   JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
   PNG 1/2/4/8/16-bit-per-channel
   TGA (not sure what subset, if a subset)
   BMP non-1bpp, non-RLE
   PSD (composited view only, no extra channels, 8/16 bit-per-channel)
   GIF (*comp always reports as 4-channel)
   HDR (radiance rgbE format)
   PIC (Softimage PIC)
   PNM (PPM and PGM binary only)

### Save image

Save the created image to disk. Only png files are supported for now.

### Load preset

Loads a preset in from a json file. Presets store the current settings and palette for later use.

### Save preset

Saves the current settings and palette to a json file. Presets can be loaded again later.

### Select input dir

Used for selecting the input directory for batch processing an entire directory (non-recursive). After selecting an input directory the batch processing will start once you select an output directory.

### Select output dir

Used to select the output directory for batch processing. After selecting an output directory, all files in the directory will be processed and placed into the output directory.

## Palette

### Load palette

### Save palette
