/*
SLK_img2pixel - a tool for converting images to pixelart

Minimal animated GIF writer.

Written in 2026 by June / SerbianKnifeFight, released to the public domain (CC0), same terms as the
rest of this project - see COPYING.
*/

#ifndef _SLK_GIF_H_

#define _SLK_GIF_H_

#include <stdint.h>

//palette: same color32() layout as the rest of SLK_img2pixel, alpha ignored
//frames: 'frame_count' pointers to width*height uint8_t palette indices (Image8::data)
//delays_cs: per-frame delay in 1/100s, may be NULL if frame_count==1
//transparent_index: palette index to mark transparent, or -1 for none
//returns 1 on success, 0 on failure
int SLK_gif_write(const char *path, int width, int height,
                   const uint32_t *palette, int palette_size,
                   uint8_t *const *frames, int frame_count,
                   const int *delays_cs, int loop, int transparent_index);

#endif
