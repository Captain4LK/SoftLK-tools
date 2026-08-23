/*
SLK_img2pixel - a tool for converting images to pixelart

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdlib.h>

//macOS startup defaults for crisper SDL rendering on Retina displays. Users can override via environment variable.
__attribute__((constructor))
static void slk_macos_sdl_env(void)
{
	if(getenv("SDL_RENDER_SCALE_QUALITY")==NULL)
		setenv("SDL_RENDER_SCALE_QUALITY","0",1);

	if(getenv("SDL_RENDER_DRIVER")==NULL)
		setenv("SDL_RENDER_DRIVER","opengl",1);
}
