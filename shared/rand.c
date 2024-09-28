/*
Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include "HLH.h"
//-------------------------------------

//Internal includes
#include "shared/rand.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static uint64_t rand_murmur3_avalanche64(uint64_t h);
//-------------------------------------

//Function implementations

void rand_pcg_seed(rand_pcg *pcg, uint32_t seed)
{
   uint64_t value = (((uint64_t)seed) << 1) | 1;
   value = rand_murmur3_avalanche64(value);
   (*pcg)[0] = 0;
   (*pcg)[1] = (value << 1) | 1;
   rand_pcg_next(pcg);
   (*pcg)[0] += rand_murmur3_avalanche64(value);
   rand_pcg_next(pcg);
}

uint32_t rand_pcg_next(rand_pcg *pcg)
{
   uint64_t oldstate = (*pcg)[0];
   (*pcg)[0] = oldstate * UINT64_C(0x5851f42d4c957f2d) + (*pcg)[1];
   uint32_t xorshifted = (uint32_t)(((oldstate >> 18) ^ oldstate) >> 27);
   uint32_t rot = (uint32_t)(oldstate >> 59);

   return (xorshifted >> rot) | (xorshifted << ((-(int32_t)rot) & 31));
}

int32_t rand_pcg_next_range(rand_pcg *pcg, int32_t min, int32_t max)
{
   uint32_t range = HLH_abs((max - min)) + 1;
   return min + (rand_pcg_next(pcg) % range);
}

void rand_xor_seed(rand_xor * xor, uint64_t seed)
{
   uint64_t value = rand_murmur3_avalanche64((seed << 1) | 1);
   (*xor)[0] = value;
   value = rand_murmur3_avalanche64(value);
   (*xor)[1] = value;
}

uint64_t rand_xor_next(rand_xor * xor)
{
   uint64_t x = (*xor)[0];
   uint64_t const y = (*xor)[1];
   (*xor)[0] = y;
   x ^= x << 23;
   x ^= x >> 17;
   x ^= y ^ (y >> 26);
   (*xor)[1] = x;

   return x + y;
}

int64_t rand_xor_next_range(rand_xor * xor, int64_t min, int64_t max)
{
   uint64_t range = HLH_abs((max - min)) + 1;
   return min + (rand_xor_next(xor) % range);
}

static uint64_t rand_murmur3_avalanche64(uint64_t h)
{
   h ^= h >> 33;
   h *= 0xff51afd7ed558ccd;
   h ^= h >> 33;
   h *= 0xc4ceb9fe1a85ec53;
   h ^= h >> 33;

   return h;
}
//-------------------------------------
