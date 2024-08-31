/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _COLOR_H_

#define _COLOR_H_

uint64_t color32_to_64(uint32_t c);
uint32_t color64_to_32(uint64_t c);

uint64_t color64_r(uint64_t c);
uint64_t color64_g(uint64_t c);
uint64_t color64_b(uint64_t c);
uint64_t color64_a(uint64_t c);
uint32_t color32_r(uint32_t c);
uint32_t color32_g(uint32_t c);
uint32_t color32_b(uint32_t c);
uint32_t color32_a(uint32_t c);

inline uint32_t color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return (uint32_t)(r)|(uint32_t)(g<<8)|(uint32_t)(b<<16)|(uint32_t)(a<<24); }

#endif
