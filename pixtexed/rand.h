/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _RAND_H_

#define _RAND_H_

#include <stdint.h>

typedef uint64_t rand_pcg[2];
typedef uint64_t rand_xor[2];

void     rand_pcg_seed(rand_pcg *pcg, uint32_t seed);
uint32_t rand_pcg_next(rand_pcg *pcg);
int32_t  rand_pcg_next_range(rand_pcg *pcg, int32_t min, int32_t max);

void rand_xor_seed(rand_xor * xor, uint64_t seed);
uint64_t rand_xor_next(rand_xor * xor);
int64_t rand_xor_next_range(rand_xor * xor, int64_t min, int64_t max);

#endif
