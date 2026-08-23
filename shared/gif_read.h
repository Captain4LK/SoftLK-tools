/*
SLK_img2pixel - a tool for converting images to pixelart

Multi-frame GIF reader.

Written in 2026 by June / SerbianKnifeFight, released to the public domain (CC0), same terms as the
rest of this project - see COPYING.
*/

#ifndef _SLK_GIF_READ_H_

#define _SLK_GIF_READ_H_

#include "image.h"

//out_frames: malloc'd array of *out_frame_count Image32*, caller frees each + the array
//out_delays_cs: malloc'd, 1/100s per frame, may be NULL if not needed
//returns 1 on success, 0 on failure
int SLK_gif_read_all_frames(const char *path, Image32 ***out_frames, int *out_frame_count, int **out_delays_cs);

#endif
