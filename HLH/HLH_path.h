/*
 * HLH - base layer - path manipulation
 *
 * Written in 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com
 *
 * To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef _HLH_PATH0_H_
#define _HLH_PATH0_H_

// Return value is view of input string, or view of string literal
HLH_string HLH_path_directory(HLH_string path);
void       HLH_path_split(HLH_string path, HLH_string dir[static 1], HLH_string filename[static 1]);
HLH_string HLH_path_ext(HLH_string path);
HLH_string HLH_path_stem(HLH_string path);
HLH_string HLH_path_base(HLH_string path);

#endif
