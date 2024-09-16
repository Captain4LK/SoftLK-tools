/**********************************************************************************************
*
*   rpng v1.5 - A simple and easy-to-use library to manage png chunks
*
*   FEATURES:
*       - Load/Save images from/to raw image data
*       - Load/Save indexed image data (providing palette)
*       - Count/read/write/remove png chunks
*       - Operate on file or memory buffer
*       - Chunks data abstraction
*       - Add custom chunks
*
*   LIMITATIONS:
*       - Bit depths of 1/2/4 bits per pixel not supported, only 8/16 bits
*
*   POSSIBLE IMPROVEMENTS:
*       - Support APNG chunks, added to PNG specs recently (draft)
*
*   CONFIGURATION:
*       #define RPNG_IMPLEMENTATION
*           Generates the implementation of the library into the included file.
*           If not defined, the library is in header only mode and can be included in other headers
*           or source files without problems. But only ONE file should hold the implementation.
*
*       #define RPNG_DEFLATE_IMPLEMENTATION
*           Include sdefl/sinfl deflate implementation with rpng
*
*       #define RPNG_NO_STDIO
*           Do not include FILE I/O API, only read/write from memory buffers
*
*       #define RPNG_NO_STDIO_WARNING
*           Skips issuing a compiler warning when RPNG_NO_STDIO is defined.
*
*   DEPENDENCIES: libc (C standard library)
*       stdlib.h        Required for: malloc(), calloc(), free()
*       string.h        Required for: memcmp(), memcpy()
*       stdio.h         Required for: FILE, fopen(), fread(), fwrite(), fclose() (only if !RPNG_NO_STDIO)
*
*       rpng includes internally a copy of sdefl and sinfl libraries by Micha Mettke (@vurtun)
*       sdelf and sinfl libraries are used for compression and decompression of deflate data streams
*       sdelf and sinfl are double licensed as MIT or Unlicense, check license at the end of this file
*
*   SUMMARY of STANDARD CHUNKS:
*
*   This table summarizes some properties of the standard chunk types.
*
*   Based on official docs:
*       http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html
*       http://www.libpng.org/pub/png/book/chapter11.html
*
*   Critical chunks (must appear in this order, except PLTE is optional):
*
*       Name   Multi?  Ordering constraints
*   -------------------------------------------
*       IHDR    No      Must be first
*       PLTE    No      Before IDAT
*       IDAT    Yes     Multiple IDATs must be consecutive
*       IEND    No      Must be last
*
*   Ancillary chunks (need not appear in this order):
*
*       Name   Multi?  Ordering constraints
*   --------------------------------------------
*       cHRM    No      Before PLTE and IDAT
*       gAMA    No      Before PLTE and IDAT
*       iCCP    No      Before PLTE and IDAT
*       sBIT    No      Before PLTE and IDAT
*       sRGB    No      Before PLTE and IDAT
*       bKGD    No      After PLTE; before IDAT
*       hIST    No      After PLTE; before IDAT
*       tRNS    No      After PLTE; before IDAT
*       pHYs    No      Before IDAT
*       sPLT    Yes     Before IDAT
*       tIME    No      None
*       iTXt    Yes     None
*       tEXt    Yes     None
*       zTXt    Yes     None
*
*   Standard keywords for text chunks (iTXt, tEXt, zTXt):
*
*       Title            Short (one line) title or caption for image
*       Author           Name of image's creator
*       Description      Description of image (possibly long)
*       Copyright        Copyright notice
*       Creation Time    Time of original image creation
*       Software         Software used to create the image
*       Disclaimer       Legal disclaimer
*       Warning          Warning of nature of content
*       Source           Device used to create the image
*       Comment          Miscellaneous comment; conversion from GIF comment
*
*   VERSIONS HISTORY:
*       1.5 (28-Aug-2024) ADDED: Support indexed data loading and saving (PLTE, tRNS)
*                         ADDED: rpng_load_image_indexed() (+ memory version)
*                         ADDED: rpng_save_image_indexed() (+ memory version)
*                         ADDED: Automatic join of IDAT chunks when requested
*                         REVIEWED: Crashes on images loading not found
* 
*       1.1 (29-May-2023) UPDATED: sdefl and sinfl, fixed issue
* 
*       1.0 (24-Dec-2021) ADDED: rpng_load_image()
*                         ADDED: RPNG_LOG() macro
*                         REVIEWED: rpng_save_image() filter process issues
*                         RENAMED: rpng_create_image() to rpng_save_image()
*                         UPDATED: sdefl to latest version 1.0
*                         ADDED: sinfl library (internal copy) for data decompression
* 
*       0.9 (10-Jun-2020) First completely functional version of the library
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2020-2024 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RPNG_H
#define RPNG_H

#define RPNG_VERSION    "1.5"

// Function specifiers in case library is build/used as a shared library (Windows)
// NOTE: Microsoft specifiers to tell compiler that symbols are imported/exported from a .dll
#if defined(_WIN32)
    #if defined(BUILD_LIBTYPE_SHARED)
        #define RPNGAPI __declspec(dllexport)     // We are building the library as a Win32 shared library (.dll)
    #elif defined(USE_LIBTYPE_SHARED)
        #define RPNGAPI __declspec(dllimport)     // We are using the library as a Win32 shared library (.dll)
    #endif
#endif

// Function specifiers definition
#ifndef RPNGAPI
    #define RPNGAPI       // Functions defined as 'extern' by default (implicit specifiers)
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Allow custom memory allocators
#ifndef RPNG_MALLOC
    #define RPNG_MALLOC(sz)         malloc(sz)
#endif
#ifndef RPNG_CALLOC
    #define RPNG_CALLOC(n,sz)       calloc(n,sz)
#endif
#ifndef RPNG_REALLOC
    #define RPNG_REALLOC(ptr,sz)    realloc(ptr,sz)
#endif
#ifndef RPNG_FREE
    #define RPNG_FREE(ptr)          free(ptr)
#endif

// Simple log system to avoid RPNG_LOG() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
//#define RPNG_SHOW_LOG_INFO
#if defined(RPNG_SHOW_LOG_INFO)
  #define RPNG_LOG(...) printf(__VA_ARGS__)
#else
  #define RPNG_LOG(...)
#endif

#ifndef RPNG_MAX_CHUNKS_COUNT
    // Maximum number of chunks to read
    #define RPNG_MAX_CHUNKS_COUNT   64
#endif
#ifndef RPNG_MAX_OUTPUT_SIZE
    // Maximum size for temporal buffer on write/remove chunks,
    // buffer is scaled to required output file size before being returned
    #define RPNG_MAX_OUTPUT_SIZE    (64*1024*1024)
#endif

#ifndef RPNG_COMPRESSION_LEVEL
    // Deflate compression level
    // NOTE: Default to same as stbiw: 8
    #define RPNG_COMPRESSION_LEVEL   8
#endif

// Define some possible error values
// NOTE: Only some are actually used on file saving
#define RPNG_SUCCESS                 0      // Image saved successfully
#define RPNG_ERROR_FILE_OPEN         1      // The requested file can not be opened
#define RPNG_ERROR_PIXEL_FORMAT      2      // Not a supported PNG image format
#define RPNG_ERROR_MEMORY_ALLOC      3      // Memory could not be allocated for operation

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#ifndef __cplusplus
#include <stdbool.h>        // Boolean type
#endif

// PNG chunk type
typedef struct {
    int length;             // Data length, must be converted to big endian when saving!
    char type[4];           // Chunk type FOURCC: IDHR, PLTE, IDAT, IEND / gAMA, sRGB, tEXt, tIME...
    char *data;             // Chunk data pointer
    unsigned int crc;       // 32bit CRC (computed over type and data)
} rpng_chunk;

// Color type
// NOTE: Used for palette loading/saving
typedef struct {
    unsigned char r;        // Color red value
    unsigned char g;        // Color green value
    unsigned char b;        // Color blue value
    unsigned char a;        // Color alpha value
} rpng_color;

// Palette type
typedef struct {
    int color_count;        // Palette color count
    rpng_color *colors;     // Palette colors
} rpng_palette;

// A minimal PNG only requires: png_signature | rpng_chunk(IHDR) | rpng_chunk(IDAT) | rpng_chunk(IEND)

#ifdef __cplusplus
extern "C" {                // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

// Load a PNG file image data
//  - Color channels are returned by reference, supported values: 1 (GRAY), 2 (GRAY+ALPHA), 3 (RGB), 4 (RGBA)
//  - Bit depth is returned by reference, supported values: 8 bit, 16 bit
//  - In case data can not be loaded, returns NULL
RPNGAPI char *rpng_load_image(const char *filename, int *width, int *height, int *color_channels, int *bit_depth);

// Load a PNG file image data indexed (including palette)
//  - Returns indexed data as an index byte array (8bit) along the palette data (PLTE - RGB888 - 24bit)
//  - In case image data is not indexed, returns NULL
RPNGAPI char *rpng_load_image_indexed(const char *filename, int *width, int *height, rpng_palette *palette);

// Save a PNG file from image data (IHDR, IDAT, IEND)
//  - Color channels defines pixel color channels, supported values: 1 (GRAY), 2 (GRAY+ALPHA), 3 (RGB), 4 (RGBA)
//  - Bit depth defines every color channel size, supported values: 8 bit, 16 bit
//  - Returns saving process result: 0-SUCCESS
RPNGAPI int rpng_save_image(const char *filename, const char *data, int width, int height, int color_channels, int bit_depth);

// Save a PNG file from indexed image data (IHDR, PLTE, (tRNS), IDAT, IEND)
//  - Palette colours are saved as RGB888 in PLTE chunk
//  - Palette alpha is saved as R8 in tRNS chunk (if required)
//  - Palette max number of entries is limited to [1..256] colors
//  - Returns saving process result: 0-SUCCESS
RPNGAPI int rpng_save_image_indexed(const char *filename, const char *indexed_data, int width, int height, rpng_palette palette);

// Load and save png data from memory buffer
// WARNING: Provided buffer is expected to be PNG compliant, ending with IEND chunk
RPNGAPI char *rpng_load_image_from_memory(const char *buffer, int *width, int *height, int *color_channels, int *bit_depth); // Load png data from memory buffer
RPNGAPI char *rpng_load_image_indexed_from_memory(const char *buffer, int *width, int *height, rpng_palette *palette); // Load indexed png data from memory buffer (8 bpp)
RPNGAPI char *rpng_save_image_to_memory(const char *data, int width, int height, int color_channels, int bit_depth, int *output_size); // Save png data to memory buffer
RPNGAPI char *rpng_save_image_indexed_to_memory(const char *indexed_data, int width, int height, rpng_palette palette, int *output_size); // Save indexed data to memory buffer

// Convert indexed image data to RGBA data
RPNGAPI char *rpng_unindex_image_data(char *indexed_data, int width, int height, rpng_palette palette);

// Read and write chunks from file
RPNGAPI int rpng_chunk_count(const char *filename);                                  // Count the chunks in a PNG image
RPNGAPI rpng_chunk rpng_chunk_read(const char *filename, const char *chunk_type);    // Read one chunk type
RPNGAPI rpng_chunk *rpng_chunk_read_all(const char *filename, int *count);           // Read all chunks
RPNGAPI void rpng_chunk_remove(const char *filename, const char *chunk_type);        // Remove one chunk type
RPNGAPI void rpng_chunk_remove_ancillary(const char *filename);                      // Remove all chunks except: IHDR-PLTE-IDAT-IEND
RPNGAPI void rpng_chunk_write(const char *filename, rpng_chunk data);                // Write one new chunk after IHDR (any kind)

// Write specific chunks to file
RPNGAPI void rpng_chunk_write_text(const char *filename, char *keyword, char *text);        // Write tEXt chunk
RPNGAPI void rpng_chunk_write_comp_text(const char *filename, char *keyword, char *text);   // Write zTXt chunk, DEFLATE compressed text
RPNGAPI void rpng_chunk_write_gamma(const char *filename, float gamma);                     // Write gAMA chunk (stored as int, gamma*100000)
RPNGAPI void rpng_chunk_write_srgb(const char *filename, char srgb_type);                   // Write sRGB chunk, requires gAMA chunk
RPNGAPI void rpng_chunk_write_time(const char *filename, short year, char month, char day, char hour, char min, char sec);  // Write tIME chunk
RPNGAPI void rpng_chunk_write_physical_size(const char *filename, int pixels_unit_x, int pixels_unit_y, bool meters);       // Write pHYs chunk
RPNGAPI void rpng_chunk_write_chroma(const char *filename, float white_x, float white_y, float red_x, float red_y, float green_x, float green_y, float blue_x, float blue_y); // Write cHRM chunk

// Chunk utilities
RPNGAPI void rpng_chunk_print_info(const char *filename);                            // Output info about the chunks
RPNGAPI bool rpng_chunk_check_all_valid(const char *filename);                       // Check chunks CRC is valid
RPNGAPI void rpng_chunk_combine_image_data(const char *filename);                    // Combine multiple IDAT chunks into a single one
RPNGAPI void rpng_chunk_split_image_data(const char *filename, int split_size);      // Split one IDAT chunk into multiple ones

// Read and write chunks from memory buffer
RPNGAPI int rpng_chunk_count_from_memory(const char *buffer);                                               // Count the chunks in a PNG image from memory
RPNGAPI rpng_chunk rpng_chunk_read_from_memory(const char *buffer, const char *chunk_type);                 // Read one chunk type from memory
RPNGAPI rpng_chunk *rpng_chunk_read_all_from_memory(const char *buffer, int *count);                        // Read all chunks from memory
RPNGAPI char *rpng_chunk_remove_from_memory(const char *buffer, const char *chunk_type, int *output_size);  // Remove one chunk type from memory
RPNGAPI char *rpng_chunk_remove_ancillary_from_memory(const char *buffer, int *output_size);                // Remove all chunks except: IHDR-IDAT-IEND
RPNGAPI char *rpng_chunk_write_from_memory(const char *buffer, rpng_chunk chunk, int *output_size);         // Write one new chunk after IHDR (any kind)
RPNGAPI char *rpng_chunk_combine_image_data_from_memory(char *buffer, int *output_size);                    // Combine multiple IDAT chunks into a single one
RPNGAPI char *rpng_chunk_split_image_data_from_memory(char *buffer, int split_size, int *output_size);      // Split one IDAT chunk into multiple ones

#ifdef __cplusplus
}
#endif

#endif // RPNG_H

/***********************************************************************************
*
*   RPNG IMPLEMENTATION
*
************************************************************************************/

#if defined(RPNG_IMPLEMENTATION)

#if !defined(RPNG_NO_STDIO)
    #include <stdio.h>      // Required for: FILE, fopen(), fread(), fwrite(), fclose()
#endif

#include <stdlib.h>         // Required for: malloc(), calloc(), free()
#include <string.h>         // Required for: memcmp(), memcpy()

#if defined(_WIN32) && defined(_MSC_VER)
    #include <io.h>         // Required for: _access() [file_exists()]
#else
    #include <unistd.h>     // Required for: access() (POSIX, not C standard) [file_exists()]
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// NOTE: Some chunks strutures are defined for convenience,
// but only the ones that can be directly serialized to chunk.dataa

// Critical chunks (IHDR, PLTE, IDAT, IEND)
//------------------------------------------------------------------------
// IHDR: Image header
// Mandatory chunk: image info (13 bytes)
typedef struct {
    unsigned int width;         // Image width
    unsigned int height;        // Image width
    unsigned char bit_depth;    // Bit depth
    unsigned char color_type;   // Pixel format: 0 - Grayscale, 2 - RGB, 3 - Indexed, 4 - GrayAlpha, 6 - RGBA
    unsigned char compression;  // Compression method: 0 (deflate)
    unsigned char filter;       // Filter method: 0 (default)
    unsigned char interlace;    // Interlace scheme (optional): 0 (none)
    // WARNING: 3 bytes of padding required for proper alignment!!!
} rpng_chunk_IHDR;

// PLTE: Palette
// Contains from 1 to 256 palette entries, each a three-byte series (RGB)
// Chunk must appear for color type 3, and can appear for color types 2 and 6; it must not appear for color types 0 and 4.
// If this chunk does appear, it must precede the first IDAT chunk. There must not be more than one PLTE chunk

// IDAT: Image data
// There can be multiple IDAT chunks; if so, they must appear consecutively with no other intervening chunks

// IEND: Image ending trailer
// rpng_chunk_IEND > rpng_chunk (empty), it should be LAST

// Transparency information
//------------------------------------------------------------------------
// tRNS: Transparency
// For color type 3 (indexed color), the tRNS chunk contains a series of one-byte alpha values, corresponding to entries in the PLTE chunk.
// For color type 0 (grayscale), the tRNS chunk contains a single gray level value, stored in the format: Gray (2 bytes), range 0 .. (2^bitdepth)-1
// For color type 2 (truecolor), the tRNS chunk contains a single RGB color value, stored in the format: Red-Green-Blue (2 bytes each), range 0 .. (2^bitdepth)-1
// tRNS is prohibited for color types 4 and 6, since a full alpha channel is already present in those cases.
// When present, the tRNS chunk must precede the first IDAT chunk, and must follow the PLTE chunk, if any.

// Color space information
//------------------------------------------------------------------------
// gAMA: Image gamma
// Specifies the relationship between the image samples and the desired display output intensity as a power function: sample = light_out^gamma
// Sample and light_out are normalized to the range 0.0 (minimum intensity) to 1.0 (maximum intensity). Therefore: sample = integer_sample/(2^bitdepth - 1)

// cHRM: Primary chromaticities
// NOTE: Values multiplied by 100000 (i.e. Value 0.3127 would be stored as the integer 31270)
typedef struct {
    unsigned int white_point_x;
    unsigned int white_point_y;
    unsigned int redx;
    unsigned int redy;
    unsigned int greenx;
    unsigned int greeny;
    unsigned int bluex;
    unsigned int bluey;
} rpng_chunk_cHRM;

// sRGB: Standard RGB color space
// When using sRGB chunk, gAMA should also be present (and perhaps a cHRM chunk)
typedef struct {
    unsigned char flag;             // 0: Perceptual, 1: Relative colorimetric, 2: Saturation, 3: Absolute colorimetric
} rpng_chunk_sRGB;

// iCCP: Embedded ICC profile
//   unsigned char *profile;        // Profile name: 1-80 bytes (must end with NULL separator: /0)
//   unsigned char comp;            // Compression method (0 for DEFLATE)
//   unsigned char *comp_profile;   // Compressed profile: n bytes

// Textual information
//------------------------------------------------------------------------
// tEXt: Textual data
//   unsigned char *keyword;        // Keyword: 1-80 bytes (must end with NULL separator: /0)
//   unsigned char *text;           // Text: n bytes (character string, no NULL terminated required)

// zTXt: Compressed text data
//   unsigned char *keyword;        // Keyword: 1-80 bytes (must end with NULL separator: /0)
//   unsigned char comp;            // Compression method (0 for DEFLATE)
//   unsigned char *text;           // UTF-8 text (0 or more bytes)

// iTXt: International textual data
//   unsigned char *keyword;        // Keyword: 1-80 bytes (must end with NULL separator: /0)
//   unsigned char comp_flag;       // Compression flag (0 for uncompressed text, 1 for compressed text)
//   unsigned char comp;            // Compression method (0 for DEFLATE)
//   unsigned char *lang_tag;       // Language tag (0 or more bytes, must end with NULL separator: /0)
//   unsigned char *tr_keyword;     // Translated keyword (0 or more bytes, must end with NULL separator: /0)
//   unsigned char *text;           // UTF-8 text (0 or more bytes)


// Miscellaneous information
//------------------------------------------------------------------------

// bKGD: Background color
// Color type  3 (indexed color) -> Palette index:  1 byte
// Color types 0 and 4 (gray or gray + alpha) -> Gray:  2 bytes, range 0 .. (2^bitdepth)-1
// Color types 2 and 6 (truecolor, with or without alpha)
//      Red:   2 bytes, range 0 .. (2^bitdepth)-1
//      Green: 2 bytes, range 0 .. (2^bitdepth)-1
//      Blue:  2 bytes, range 0 .. (2^bitdepth)-1


// pHYs: Physical pixel dimensions
typedef struct {
    unsigned int pixels_per_unit_x;
    unsigned int pixels_per_unit_y;
    unsigned char unit_specifier;   // 0 - Unit unknown, 1 - Unit is meter
} rpng_chunk_pHYs;

// tIME: Image last-modification time
typedef struct {
    unsigned short year;            // Year complete, i.e. 1995
    unsigned char month;            // 1 to 12
    unsigned char day;              // 1 to 31
    unsigned char hour;             // 0 to 23
    unsigned char minute;           // 0 to 59
    unsigned char second;           // 0 to 60 (yes, 60, for leap seconds; not 61, a common error)
} rpng_chunk_tIME;

// Other chunks (view documentation)
//sBIT: Significant bits
//sPLT: Suggested palette
//hIST: Palette histogram

// TODO: Support APNG chunks
// REF: https://wiki.mozilla.org/APNG_Specification
//acTL: Animation Control
//fcTL: Frame Control
//fdAT: Frame Data

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const unsigned char png_signature[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a }; // PNG Signature

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
// Prefilter and compress image data (image_data -> IDAT chunk.data)
static char *rpng_inflate_image_data(char *image_data, int image_data_size, int width, int height, int pixel_size);
// Decompress and unfilter image data (IDAT chunk.data -> image_data)
static char *rpng_deflate_image_data(const char *image_data, int image_data_size, int width, int height, int pixel_size, int *output_size, int forced_filter_type);

// Swap integer from big<->little endian
static unsigned int swap_endian(unsigned int value);
static unsigned int compute_crc32(unsigned char *buffer, int size);

// Load/save png file data from/to memory buffer
static char *load_file_to_buffer(const char *filename, int *bytes_read);
static int save_file_from_buffer(const char *filename, void *data, int bytesToWrite);
static bool file_exists(const char *filename);

// sdelf and sinfl implementations placed at the end of file
#define SDEFL_IMPLEMENTATION
#define SINFL_IMPLEMENTATION

//===================================================================
//                              SDEFL
// DEFLATE COMPRESSION algorithm: https://github.com/vurtun/sdefl
//===================================================================
#define SDEFL_MAX_OFF   (1 << 15)
#define SDEFL_WIN_SIZ   SDEFL_MAX_OFF
#define SDEFL_WIN_MSK   (SDEFL_WIN_SIZ-1)

#define SDEFL_HASH_BITS 15
#define SDEFL_HASH_SIZ  (1 << SDEFL_HASH_BITS)
#define SDEFL_HASH_MSK  (SDEFL_HASH_SIZ-1)

#define SDEFL_MIN_MATCH 4
#define SDEFL_BLK_MAX   (256*1024)
#define SDEFL_SEQ_SIZ   ((SDEFL_BLK_MAX+2)/3)

#define SDEFL_SYM_MAX   (288)
#define SDEFL_OFF_MAX   (32)
#define SDEFL_PRE_MAX   (19)

#define SDEFL_LVL_MIN   0
#define SDEFL_LVL_DEF   5
#define SDEFL_LVL_MAX   8

struct sdefl_freq {
  unsigned lit[SDEFL_SYM_MAX];
  unsigned off[SDEFL_OFF_MAX];
};
struct sdefl_code_words {
  unsigned lit[SDEFL_SYM_MAX];
  unsigned off[SDEFL_OFF_MAX];
};
struct sdefl_lens {
  unsigned char lit[SDEFL_SYM_MAX];
  unsigned char off[SDEFL_OFF_MAX];
};
struct sdefl_codes {
  struct sdefl_code_words word;
  struct sdefl_lens len;
};
struct sdefl_seqt {
  int off, len;
};
struct sdefl {
  int bits, bitcnt;
  int tbl[SDEFL_HASH_SIZ];
  int prv[SDEFL_WIN_SIZ];

  int seq_cnt;
  struct sdefl_seqt seq[SDEFL_SEQ_SIZ];
  struct sdefl_freq freq;
  struct sdefl_codes cod;
};
extern int sdefl_bound(int in_len);
extern int sdeflate(struct sdefl *s, void *o, const void *i, int n, int lvl);
extern int zsdeflate(struct sdefl *s, void *o, const void *i, int n, int lvl);

//=========================================================================
//                           SINFL
// DEFLATE DECOMPRESSION algorithm: https://github.com/vurtun/lib/sinfl.h
//=========================================================================
#define SINFL_PRE_TBL_SIZE 128
#define SINFL_LIT_TBL_SIZE 1334
#define SINFL_OFF_TBL_SIZE 402

struct sinfl {
  const unsigned char *bitptr;
  unsigned long long bitbuf;
  int bitcnt;

  unsigned lits[SINFL_LIT_TBL_SIZE];
  unsigned dsts[SINFL_OFF_TBL_SIZE];
};

extern int sinflate(void *out, int cap, const void *in, int size);
extern int zsinflate(void *out, int cap, const void *in, int size);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// The Paeth filter function computes a simple linear function of the three neighbouring pixels (left, above, upper left),
// then chooses as predictor the neighbouring pixel closest to the computed value. Ref: https://www.w3.org/TR/PNG/#9Filters
// The algorithm used in this International Standard is an adaptation of the technique due to Alan W. Paeth.
// NOTE: Paeth predictor calculations shall be performed exactly, without overflow.
static unsigned char rpng_paeth_predictor(int a, int b, int c)
{
    unsigned char pr = 0;

    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);

    if ((pa <= pb) && (pa <= pc)) pr = (unsigned char)a;
    else if (pb <= pc) pr = (unsigned char)b;
    else pr = (unsigned char)c;

    return pr;
}

// Load a PNG file image data
//  - Color channels are returned by reference, supported values: 1 (GRAY), 2 (GRAY+ALPHA), 3 (RGB), 4 (RGBA)
//  - Bit depth is returned by reference, supported values: 8 bit, 16 bit
char *rpng_load_image(const char *filename, int *width, int *height, int *color_channels, int *bit_depth)
{
    char *data = NULL;

    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        data = rpng_load_image_from_memory(file_data, width, height, color_channels, bit_depth);
        RPNG_FREE(file_data);
    }

    return data;
}

// Load a PNG file image data indexed (including palette)
//  - Returns indexed data as an index byte array (8bit) along the palette data (PLTE - RGB888 - 24bit)
// WARNING: In case data is not indexed, returns NULL for pointers and sets values to 0
char *rpng_load_image_indexed(const char *filename, int *width, int *height, rpng_palette *palette)
{
    char *data = NULL;

    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        data = rpng_load_image_indexed_from_memory(file_data, width, height, palette);
        RPNG_FREE(file_data);
    }

    return data;
}

// Save a PNG file from image data (IHDR, IDAT, IEND)
//  - Color channels defines pixel color channels, supported values: 1 (GRAY), 2 (GRAY+ALPHA), 3 (RGB), 4 (RGBA)
//  - Bit depth defines every color channel size, supported values: 8 bit, 16 bit
int rpng_save_image(const char *filename, const char *data, int width, int height, int color_channels, int bit_depth)
{
    int result = 0;

    int file_output_size = 0;
    char *file_output = rpng_save_image_to_memory(data, width, height, color_channels, bit_depth, &file_output_size);

    if ((file_output != NULL) && (file_output_size > 0))
    {
        save_file_from_buffer(filename, file_output, file_output_size);
    }
    else RPNG_LOG("WARNING: PNG data saving failed");

    RPNG_FREE(file_output);

    return result;
}

// Save a PNG file from indexed image data (IHDR, PLTE, (tRNS), IDAT, IEND)
//  - Palette colours are saved as RGB888 in PLTE chunk
//  - Palette alpha is saved as R8 in tRNS chunk (if required)
//  - Palette max number of entries is limited to [1..256] colors
int rpng_save_image_indexed(const char *filename, const char *indexed_data, int width, int height, rpng_palette palette)
{
    int result = 0;

    int file_output_size = 0;
    char *file_output = rpng_save_image_indexed_to_memory(indexed_data, width, height, palette, &file_output_size);

    if ((file_output != NULL) && (file_output_size > 0))
    {
        save_file_from_buffer(filename, file_output, file_output_size);
    }
    else RPNG_LOG("WARNING: PNG data saving failed");

    RPNG_FREE(file_output);
    return result;
}

// Count number of PNG chunks
int rpng_chunk_count(const char *filename)
{
    int count = 0;
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        count = rpng_chunk_count_from_memory(file_data);
        RPNG_FREE(file_data);
    }

    return count;
}

// Read one chunk from a PNG file
// NOTE: There could be multiple chunks of same type, only first found is returned
rpng_chunk rpng_chunk_read(const char *filename, const char *chunk_type)
{
    rpng_chunk chunk = { 0 };

    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        chunk = rpng_chunk_read_from_memory(file_data, chunk_type);
        RPNG_FREE(file_data);
    }

    return chunk;
}

// Read all chunks from a PNG file
rpng_chunk *rpng_chunk_read_all(const char *filename, int *count)
{
    int counter = 0;
    rpng_chunk *chunks = NULL;

    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        chunks = rpng_chunk_read_all_from_memory(file_data, &counter);
        RPNG_FREE(file_data);
    }

    *count = counter;
    return chunks;
}

// Remove text chunk by type
void rpng_chunk_remove(const char *filename, const char *chunk_type)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        int file_output_size = 0;
        char *file_output = rpng_chunk_remove_from_memory(file_data, chunk_type, &file_output_size);
        save_file_from_buffer(filename, file_output, file_output_size);

        RPNG_FREE(file_output);
        RPNG_FREE(file_data);
    }
}

// Remove all chunks except: IHDR-IDAT-IEND
void rpng_chunk_remove_ancillary(const char *filename)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        int file_output_size = 0;
        char *file_output = rpng_chunk_remove_ancillary_from_memory(file_data, &file_output_size);
        save_file_from_buffer(filename, file_output, file_output_size);

        RPNG_FREE(file_output);
        RPNG_FREE(file_data);
    }
}

// Add one new chunk (any kind)
// NOTE: Chunk is added by default after IHDR
void rpng_chunk_write(const char *filename, rpng_chunk chunk)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    int file_output_size = 0;
    char *file_output = NULL;
    
    if (file_data != NULL)
    {
        file_output = rpng_chunk_write_from_memory(file_data, chunk, &file_output_size);

        // Verify expected output size before writing to file
        if (file_output_size == (file_size + chunk.length + 12)) save_file_from_buffer(filename, file_output, file_output_size);
        else RPNG_LOG("WARNING: Failed to save file, output size not matching expected size\n");

        RPNG_FREE(file_output);
        RPNG_FREE(file_data);
    }
}

// Write text chunk data into PNG
// NOTE: It will be added just after IHDR chunk
// tEXt chunk data:
//   unsigned char *keyword;     // Keyword: 1-80 bytes (must end with NULL separator: /0)
//   unsigned char *text;        // Text: n bytes (character string, no NULL terminated required)
// Keyword/Text usual values:
//   Title            Short (one line) title or caption for image
//   Author           Name of image's creator
//   Description      Description of image (possibly long)
//   Copyright        Copyright notice
//   Creation Time    Time of original image creation
//   Software         Software used to create the image
//   Disclaimer       Legal disclaimer
//   Warning          Warning of nature of content
//   Source           Device used to create the image
//   Comment          Miscellaneous comment
void rpng_chunk_write_text(const char *filename, char *keyword, char *text)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        rpng_chunk chunk = { 0 };

        int keyword_len = (int)strlen(keyword);
        int text_len = (int)strlen(text);

        // Fill chunk with required data
        // NOTE: CRC can be left to 0, it's calculated internally on writing
        memcpy(chunk.type, "tEXt", 4);
        chunk.length = keyword_len + 1 + text_len;
        chunk.data = (char *)RPNG_CALLOC(chunk.length, 1);
        memcpy(chunk.data, keyword, keyword_len);
        memcpy(chunk.data + keyword_len + 1, text, text_len);
        chunk.crc = 0;  // Computed by rpng_chunk_write_from_memory()

        int file_output_size = 0;
        char *file_output = rpng_chunk_write_from_memory(file_data, chunk, &file_output_size);

        // Verify expected output size before writing to file
        if (file_output_size == (file_size + chunk.length + 12)) save_file_from_buffer(filename, file_output, file_output_size);
        else RPNG_LOG("WARNING: Failed to save file, output size not matching expected size\n");

        RPNG_FREE(chunk.data);
        RPNG_FREE(file_output);
        RPNG_FREE(file_data);
    }
}

// Write zTXt chunk, DEFLATE compressed text
// zTXt chunk information and size:
//    unsigned char *keyword;           // Keyword: 1-80 bytes (must end with NULL separator: /0)
//    unsigned char comp;               // Compression method (0 for DEFLATE)
//    unsigned char *comp_text;         // Compressed text: n bytes
void rpng_chunk_write_comp_text(const char *filename, char *keyword, char *text)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        // Create chunk and fill with data
        rpng_chunk chunk = { 0 };

        int keyword_len = (int)strlen(keyword);
        int text_len = (int)strlen(text);

        // Compress filtered image data and generate a valid zlib stream
        struct sdefl *sde = (struct sdefl *)RPNG_CALLOC(sizeof(struct sdefl), 1);
        int bounds = sdefl_bound(text_len);
        unsigned char *comp_text = (unsigned char *)RPNG_CALLOC(bounds, 1);
        int comp_text_size = zsdeflate(sde, comp_text, (unsigned char *)text, text_len, RPNG_COMPRESSION_LEVEL);
        RPNG_FREE(sde);

        // Fill chunk with required data
        // NOTE: CRC can be left to 0, it's calculated internally on writing
        memcpy(chunk.type, "zTXt", 4);
        chunk.length = keyword_len + 1 + 1 + comp_text_size;
        chunk.data = (char *)RPNG_CALLOC(chunk.length, 1);
        memcpy(chunk.data, keyword, keyword_len);
        memcpy(chunk.data + keyword_len + 2, comp_text, comp_text_size);

        int file_output_size = 0;
        char *file_output = rpng_chunk_write_from_memory(file_data, chunk, &file_output_size);

        // Verify expected output size before writing to file
        if (file_output_size == (file_size + chunk.length + 12)) save_file_from_buffer(filename, file_output, file_output_size);
        else RPNG_LOG("WARNING: Failed to save file, output size not matching expected size\n");

        RPNG_FREE(chunk.data);
        RPNG_FREE(comp_text);
        RPNG_FREE(file_output);
        RPNG_FREE(file_data);
    }
}

// Write gAMA chunk
// NOTE: Gamma is stored as one int: gamma*100000
void rpng_chunk_write_gamma(const char *filename, float gamma)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        rpng_chunk chunk = { 0 };

        int gamma_value = (int)(gamma*100000);

        // Fill chunk with required data
        // NOTE: CRC can be left to 0, it's calculated internally on writing
        memcpy(chunk.type, "gAMA", 4);
        chunk.length = 4;
        chunk.data = (char *)RPNG_CALLOC(chunk.length, 1);
        gamma_value = swap_endian(gamma_value);
        memcpy(chunk.data, &gamma_value, 4);
        chunk.crc = 0;  // Computed by rpng_chunk_write_from_memory()

        int file_output_size = 0;
        char *file_output = rpng_chunk_write_from_memory(file_data, chunk, &file_output_size);

        // Verify expected output size before writing to file
        if (file_output_size == (file_size + chunk.length + 12)) save_file_from_buffer(filename, file_output, file_output_size);
        else RPNG_LOG("WARNING: Failed to save file, output size not matching expected size\n");

        RPNG_FREE(chunk.data);
        RPNG_FREE(file_output);
        RPNG_FREE(file_data);
    }
}

// Write sRGB chunk, requires gAMA chunk
// NOTE: This chunk only contains 1 byte of data defining rendering intent:
//   0: Perceptual
//   1: Relative colorimetric
//   2: Saturation
//   3: Absolute colorimetric
void rpng_chunk_write_srgb(const char *filename, char srgb_type)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    rpng_chunk chunk = { 0 };

    if ((srgb_type < 0) || (srgb_type > 3)) srgb_type = 0;

    // Fill chunk with required data
    // NOTE: CRC can be left to 0, it's calculated internally on writing
    memcpy(chunk.type, "sRGB", 4);
    chunk.length = 1;
    chunk.data = (char *)RPNG_CALLOC(chunk.length, 1);
    memcpy(chunk.data, &srgb_type, 1);
    chunk.crc = 0;  // Computed by rpng_chunk_write_from_memory()

    int file_output_size = 0;
    char *file_output = rpng_chunk_write_from_memory(file_data, chunk, &file_output_size);

    // Verify expected output size before writing to file
    if (file_output_size == (file_size + chunk.length + 12)) save_file_from_buffer(filename, file_output, file_output_size);
    else RPNG_LOG("WARNING: Failed to save file, output size not matching expected size\n");

    RPNG_FREE(chunk.data);
    RPNG_FREE(file_output);
    RPNG_FREE(file_data);
}

// Write tIME chunk
// tIME chunk information and size:
//   unsigned short year;         // Year complete, i.e. 1995
//   unsigned char month;         // 1 to 12
//   unsigned char day;           // 1 to 31
//   unsigned char hour;          // 0 to 23
//   unsigned char minute;        // 0 to 59
//   unsigned char second;        // 0 to 60 (yes, 60, for leap seconds; not 61, a common error)
void rpng_chunk_write_time(const char *filename, short year, char month, char day, char hour, char min, char sec)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    rpng_chunk chunk = { 0 };

    // Fill chunk with required data
    // NOTE: CRC can be left to 0, it's calculated internally on writing
    memcpy(chunk.type, "tIME", 4);
    chunk.length = 7;
    chunk.data = (char *)RPNG_CALLOC(chunk.length, 1);
    memcpy(chunk.data, &year, 2);
    memcpy(chunk.data + 2, &month, 1);
    memcpy(chunk.data + 3, &day, 1);
    memcpy(chunk.data + 4, &hour, 1);
    memcpy(chunk.data + 5, &min, 1);
    memcpy(chunk.data + 6, &sec, 1);
    chunk.crc = 0;  // Computed by rpng_chunk_write_from_memory()

    int file_output_size = 0;
    char *file_output = rpng_chunk_write_from_memory(file_data, chunk, &file_output_size);

    // Verify expected output size before writing to file
    if (file_output_size == (file_size + chunk.length + 12)) save_file_from_buffer(filename, file_output, file_output_size);
    else RPNG_LOG("WARNING: Failed to save file, output size not matching expected size\n");

    RPNG_FREE(chunk.data);
    RPNG_FREE(file_output);
    RPNG_FREE(file_data);
}

// Write pHYs chunk
// pHYs chunk information and size:
//   unsigned int pixels_per_unit_x;
//   unsigned int pixels_per_unit_y;
//   unsigned char unit_specifier;       // 0 - Unit unknown, 1 - Unit is meter
void rpng_chunk_write_physical_size(const char *filename, int pixels_unit_x, int pixels_unit_y, bool meters)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    rpng_chunk chunk = { 0 };

    // Fill chunk with required data
    // NOTE: CRC can be left to 0, it's calculated internally on writing
    memcpy(chunk.type, "pHYs", 4);
    chunk.length = 9;
    chunk.data = (char *)RPNG_CALLOC(chunk.length, 1);
    pixels_unit_x = swap_endian(pixels_unit_x);
    memcpy(chunk.data, &pixels_unit_x, 4);
    pixels_unit_y = swap_endian(pixels_unit_y);
    memcpy(chunk.data + 4, &pixels_unit_y, 4);
    char meters_value = (meters)? 1 : 0;
    memcpy(chunk.data + 8, &meters_value, 1);
    chunk.crc = 0;  // Computed by rpng_chunk_write_from_memory()

    int file_output_size = 0;
    char *file_output = rpng_chunk_write_from_memory(file_data, chunk, &file_output_size);

    // Verify expected output size before writing to file
    if (file_output_size == (file_size + chunk.length + 12)) save_file_from_buffer(filename, file_output, file_output_size);
    else RPNG_LOG("WARNING: Failed to save file, output size not matching expected size\n");

    RPNG_FREE(chunk.data);
    RPNG_FREE(file_output);
    RPNG_FREE(file_data);
}

// Write cHRM chunk
// cHRM chunk information and size:
//   unsigned int white_point_x;
//   unsigned int white_point_y;
//   unsigned int redx;
//   unsigned int redy;
//   unsigned int greenx;
//   unsigned int greeny;
//   unsigned int bluex;
//   unsigned int bluey;
// NOTE: Each value is stored as one int: value*100000
void rpng_chunk_write_chroma(const char *filename, float white_x, float white_y, float red_x, float red_y, float green_x, float green_y, float blue_x, float blue_y)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    rpng_chunk chunk = { 0 };

    // Fill chunk with required data
    // NOTE: CRC can be left to 0, it's calculated internally on writing
    memcpy(chunk.type, "pHYs", 4);
    chunk.length = 8*4;     // 8 integer values
    chunk.data = (char *)RPNG_CALLOC(chunk.length, 1);
    int white_x_value = swap_endian((int)(white_x*100000));
    memcpy(chunk.data, &white_x_value, 4);
    int white_y_value = swap_endian((int)(white_y*100000));
    memcpy(chunk.data + 4, &white_y_value, 4);
    int red_x_value = swap_endian((int)(red_x*100000));
    memcpy(chunk.data + 8, &red_x_value, 4);
    int red_y_value = swap_endian((int)(red_y*100000));
    memcpy(chunk.data + 12, &red_y_value, 4);
    int green_x_value = swap_endian((int)(green_x*100000));
    memcpy(chunk.data + 16, &green_x_value, 4);
    int green_y_value = swap_endian((int)(green_y*100000));
    memcpy(chunk.data + 20, &green_y_value, 4);
    int blue_x_value = swap_endian((int)(blue_x*100000));
    memcpy(chunk.data + 24, &blue_x_value, 4);
    int blue_y_value = swap_endian((int)(blue_y*100000));
    memcpy(chunk.data + 28, &blue_y_value, 4);
    chunk.crc = 0;  // Computed by rpng_chunk_write_from_memory()

    int file_output_size = 0;
    char *file_output = rpng_chunk_write_from_memory(file_data, chunk, &file_output_size);

    // Verify expected output size before writing to file
    if (file_output_size == (file_size + chunk.length + 12)) save_file_from_buffer(filename, file_output, file_output_size);
    else RPNG_LOG("WARNING: Failed to save file, output size not matching expected size\n");

    RPNG_FREE(chunk.data);
    RPNG_FREE(file_output);
    RPNG_FREE(file_data);
}

// Output info about the chunks
void rpng_chunk_print_info(const char *filename)
{
    int count = 0;
    rpng_chunk *chunks = rpng_chunk_read_all(filename, &count);
    if (chunks == NULL) return;

    RPNG_LOG("\n| Chunk |   Data Length  |   CRC32   |\n");
    RPNG_LOG("|-------|----------------|-----------|\n");
    for (int i = 0; i < count; i++)
    {
        RPNG_LOG("| %c%c%c%c  | %8i bytes |  %08X |\n", chunks[i].type[0], chunks[i].type[1], chunks[i].type[2], chunks[i].type[3], chunks[i].length, chunks[i].crc);
    }
    RPNG_LOG("\n");
    /*
        rpng_chunk_IHDR *IHDRData = (rpng_chunk_IHDR *)chunks[0].data;
        RPNG_LOG("\n| IHDR information    |\n");
        RPNG_LOG("|---------------------|\n");
        RPNG_LOG("| width:         %4i |\n", swap_endian(IHDRData->width));   // Image width
        RPNG_LOG("| weight:        %4i |\n", swap_endian(IHDRData->height));  // Image height
        RPNG_LOG("| bit depth:     %4i |\n", IHDRData->bit_depth);            // Bit depth
        RPNG_LOG("| color type:    %4i |\n", IHDRData->color_type);           // Pixel format: 0-Grayscale, 2-RGB, 3-Indexed, 4-GrayAlpha, 6-RGBA
        RPNG_LOG("| compression:      %i |\n", IHDRData->compression);         // Compression method: 0
        RPNG_LOG("| filter method:    %i |\n", IHDRData->filter);            // Filter method: 0 (default)
        RPNG_LOG("| interlace:        %i |\n", IHDRData->interlace);             // Interlace scheme (optional): 0 (none)
    */
    for (int i = 0; i < count; i++) RPNG_FREE(chunks[i].data);
    RPNG_FREE(chunks);
}

// Check chunks CRC and order
bool rpng_chunk_check_all_valid(const char *filename)
{
    bool result = true;

    int count = 0;
    rpng_chunk *chunks = rpng_chunk_read_all(filename, &count);
    if (chunks == NULL) return false;

    unsigned int crc = 0;
    char *chunk_type_data = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, 1);

    if (chunk_type_data != NULL)
    {
        for (int i = 0; i < count; i++)
        {
            memcpy(chunk_type_data, chunks[i].type, 4);
            memcpy(chunk_type_data + 4, chunks[i].data, chunks[i].length);
            crc = compute_crc32((unsigned char *)chunk_type_data, 4 + chunks[i].length);
            crc = swap_endian(crc);

            // Check computed CRC matches provided CRC
            if (chunks[i].crc != crc)
            {
                result = false;
                break;
            }
        }
    }

    // Free chunks memory
    for (int i = 0; i < count; i++) RPNG_FREE(chunks[i].data);
    RPNG_FREE(chunks);

    return result;
}

// Combine multiple IDAT chunks into a single one
void rpng_chunk_combine_image_data(const char *filename)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        int file_output_size = 0;
        char *file_output = rpng_chunk_combine_image_data_from_memory(file_data, &file_output_size);

        // Verify process worked as expected
        if ((file_output != NULL) && (file_output_size == file_size))
        {
            save_file_from_buffer(filename, file_output, file_output_size);
        }

        RPNG_FREE(file_output);
        RPNG_FREE(file_data);
    }
}

// Split one IDAT chunk into multiple ones
void rpng_chunk_split_image_data(const char *filename, int split_size)
{
    int file_size = 0;
    char *file_data = load_file_to_buffer(filename, &file_size);

    if (file_data != NULL)
    {
        int file_output_size = 0;
        char *file_output = rpng_chunk_split_image_data_from_memory(file_data, split_size, &file_output_size);

        // Verify process worked as expected
        if ((file_output != 0) && (file_output_size > file_size))
        {
            save_file_from_buffer(filename, file_output, file_output_size);
        }

        RPNG_FREE(file_output);
        RPNG_FREE(file_data);
    }
}

// Functions operating on memory buffers data
//----------------------------------------------------------------------------------------------------------
// Load png data from memory buffer
char *rpng_load_image_from_memory(const char *buffer, int *width, int *height, int *color_channels, int *bit_depth)
{
    char *data = NULL;

    rpng_chunk chunk_info = rpng_chunk_read_from_memory(buffer, "IHDR");

    if (chunk_info.data == NULL) return data; // WARNING: Return if no info chunk has been loaded

    // First chunk is always IHDR, we can check image data info
    rpng_chunk_IHDR *IHDRData = (rpng_chunk_IHDR *)chunk_info.data;

    *width = swap_endian(IHDRData->width);      // Image width
    *height = swap_endian(IHDRData->height);    // Image height
    *bit_depth = IHDRData->bit_depth;           // Bit depth

    *color_channels = 0;
    switch (IHDRData->color_type)
    {
        case 0: *color_channels = 1; break;     // Pixel format: 0-Grayscale
        case 4: *color_channels = 2; break;     // Pixel format: 4-GrayAlpha
        case 2: *color_channels = 3; break;     // Pixel format: 2-RGB
        case 6: *color_channels = 4; break;     // Pixel format: 6-RGBA
        case 3: *color_channels = 1; break;     // Pixel format: 3-Indexed (1 channel containing 8-bit indexed data)
        default: break;
    }

    // TODO: Support bit depths of 1/2/4 bits? -> Convert to 8bit grayscale
    if ((*color_channels == 1) && (*bit_depth != 8) && (*bit_depth != 16)) return data;  // Bit depth 1/2/4 not supported

    // Additional info provided by IHDR (in case it was required)
    //IHDRData->compression;        // Compression method: 0 (DEFLATE)
    //IHDRData->filter;             // Filter method: 0 (default)
    //IHDRData->interlace;          // Interlace scheme (optional): 0 (none)

    if (*color_channels != 0)
    {
        // NOTE: All splitted chunks are joined on reading
        rpng_chunk chunk_image = rpng_chunk_read_from_memory(buffer, "IDAT");

        if (chunk_image.data != NULL)
        {
            // Verify data integrity CRC over all chunk data concatenated
            unsigned int crc = 0;
            char *chunk_type_data = (char *)RPNG_CALLOC(4 + chunk_image.length, 1);
            memcpy(chunk_type_data, chunk_image.type, 4);
            memcpy(chunk_type_data + 4, chunk_image.data, chunk_image.length);
            crc = compute_crc32((unsigned char *)chunk_type_data, 4 + chunk_image.length);
            RPNG_FREE(chunk_type_data);

            if (crc == chunk_image.crc) // Validate crc
            {
                int pixel_size = *color_channels*(*bit_depth/8);
                data = rpng_inflate_image_data(chunk_image.data, chunk_image.length, *width, *height, pixel_size);

                if (data == NULL) RPNG_LOG("WARNING: IDAT image data ¡decompression failed\n");
            }
            else RPNG_LOG("WARNING: CRC not valid, IDAT chunk image data could be corrupted\n");
        }

        RPNG_FREE(chunk_image.data);
    }
    else RPNG_LOG("WARNING: Failed to load file, image pixel format not supported\n");

    RPNG_FREE(chunk_info.data);

    return data;
}

// Load indexed png data (including palette) from memory buffer
// NOTE: Returns indexed data as an index byte array (8bit) along the palette data (PLTE - RGB888 - 24bit)
char *rpng_load_image_indexed_from_memory(const char *buffer, int *width, int *height, rpng_palette *palette)
{
    char *data = NULL;
    *width = 0;
    *height = 0;

    // Load indexed palette data, if provided
    // Start verifying it contains palette/indexed data, if not we finish
    rpng_chunk chunk_palette = rpng_chunk_read_from_memory(buffer, "PLTE");

    if (chunk_palette.data != NULL)
    {
        // Palette data is provided as RGB888
        palette->color_count = chunk_palette.length/3;
        palette->colors = (rpng_color *)RPNG_CALLOC(palette->color_count, sizeof(rpng_color));

        for (int i = 0; i < palette->color_count; i++)
        {
            palette->colors[i].r = chunk_palette.data[i*3 + 0];
            palette->colors[i].g = chunk_palette.data[i*3 + 1];
            palette->colors[i].b = chunk_palette.data[i*3 + 2];
            palette->colors[i].a = 255;
        }

        RPNG_FREE(chunk_palette.data);

        // Try loading palette alpha data, if provided 
        rpng_chunk chunk_alpha = rpng_chunk_read_from_memory(buffer, "tRNS");

        if ((chunk_alpha.data != NULL) && (chunk_alpha.length == palette->color_count))
        {
            for (int i = 0; i < palette->color_count; i++) palette->colors[i].a = (unsigned char)chunk_alpha.data[i];

            RPNG_FREE(chunk_alpha.data);
        }

        // Load indexed image data
        rpng_chunk chunk_info = rpng_chunk_read_from_memory(buffer, "IHDR");

        // WARNING: Cast chunk_info data directly to expected IHDR struct layout
        rpng_chunk_IHDR *IHDRData = (rpng_chunk_IHDR *)chunk_info.data;

        *width = swap_endian(IHDRData->width);      // Image width
        *height = swap_endian(IHDRData->height);    // Image height

        // Verify color type is indexed (3) and bit depth is 8
        if ((IHDRData->color_type == 3) && (IHDRData->bit_depth == 8))
        {
            // NOTE: All splitted chunks are joined on reading
            rpng_chunk chunk_image = rpng_chunk_read_from_memory(buffer, "IDAT");

            if (chunk_image.data != NULL)
            {
                // Verify data integrity CRC over all chunk data concatenated
                unsigned int crc = 0;
                char *chunk_type_data = (char *)RPNG_CALLOC(4 + chunk_image.length, 1);
                memcpy(chunk_type_data, chunk_image.type, 4);
                memcpy(chunk_type_data + 4, chunk_image.data, chunk_image.length);
                crc = compute_crc32((unsigned char *)chunk_type_data, 4 + chunk_image.length);
                RPNG_FREE(chunk_type_data);

                if (crc == chunk_image.crc) // Validate crc
                {
                    int pixel_size = (IHDRData->bit_depth/8); // NOTE: Assume 1 channel
                    data = rpng_inflate_image_data(chunk_image.data, chunk_image.length, *width, *height, pixel_size);

                    if (data == NULL) RPNG_LOG("WARNING: IDAT image data ¡decompression failed\n");
                }
                else RPNG_LOG("WARNING: CRC not valid, IDAT chunk image data could be corrupted\n");
            }

            RPNG_FREE(chunk_image.data);
        }

        RPNG_FREE(chunk_info.data);
    }

    return data;
}

// Save png data to memory buffer
char *rpng_save_image_to_memory(const char *data, int width, int height, int color_channels, int bit_depth, int *output_size)
{
    char *output_buffer = NULL;
    int output_buffer_size = 0;

    if ((bit_depth != 8) && (bit_depth != 16))
    {
        RPNG_LOG("WARNING: Requested bit depth (%i bit per channel) not supported\n", bit_depth);
        return output_buffer;  // WARNING: Bit depth 1/2/4 not supported
    }

    int color_type = -1;
    if (color_channels == 1) color_type = 0;        // Grayscale
    else if (color_channels == 2) color_type = 4;   // Gray + Alpha
    else if (color_channels == 3) color_type = 2;   // RGB
    else if (color_channels == 4) color_type = 6;   // RGBA

    if (color_type == -1) return output_buffer;   // WARNING: Number of channels not supported

    rpng_chunk_IHDR image_info = { 0 };
    image_info.width = swap_endian(width);
    image_info.height = swap_endian(height);
    image_info.bit_depth = (unsigned char)bit_depth;
    image_info.color_type = (unsigned char)color_type;

    // Image data pre-processing to append filter type byte to every scanline
    int pixel_size = color_channels*(bit_depth/8);
    int comp_data_size = 0;
    char *comp_data = rpng_deflate_image_data(data, width*height*pixel_size, width, height, pixel_size, &comp_data_size, -1);

    // Security check to verify compression worked
    if ((comp_data != NULL) && (comp_data_size > 0))
    {
        output_buffer = (char *)RPNG_CALLOC(8 + 13 + 12 + (comp_data_size + 12) + 12, 1); // Signature + IHDR + IDAT + IEND

        // Write PNG signature
        memcpy(output_buffer, png_signature, 8);

        // Write PNG chunk IHDR
        unsigned int length_IHDR = 13;
        length_IHDR = swap_endian(length_IHDR);
        memcpy(output_buffer + 8, &length_IHDR, 4);
        memcpy(output_buffer + 8 + 4, "IHDR", 4);
        memcpy(output_buffer + 8 + 4 + 4, &image_info, 13);
        unsigned int crc = compute_crc32((unsigned char *)output_buffer + 8 + 4, 4 + 13);
        crc = swap_endian(crc);
        memcpy(output_buffer + 8 + 8 + 13, &crc, 4);
        output_buffer_size += (8 + 12 + 13);

        // Write PNG chunk IDAT
        unsigned int length_IDAT = comp_data_size;
        length_IDAT = swap_endian(length_IDAT);
        memcpy(output_buffer + output_buffer_size, &length_IDAT, 4);
        memcpy(output_buffer + output_buffer_size + 4, "IDAT", 4);
        memcpy(output_buffer + output_buffer_size + 8, comp_data, comp_data_size);
        crc = compute_crc32((unsigned char *)output_buffer + output_buffer_size + 4, 4 + comp_data_size);
        crc = swap_endian(crc);
        memcpy(output_buffer + output_buffer_size + 8 + comp_data_size, &crc, 4);
        output_buffer_size += (comp_data_size + 12);

        // Write PNG chunk IEND
        unsigned char chunk_IEND[12] = { 0, 0, 0, 0, 'I', 'E', 'N', 'D', 0xAE, 0x42, 0x60, 0x82 };
        memcpy(output_buffer + output_buffer_size, chunk_IEND, 12);
        output_buffer_size += 12;
    }

    RPNG_FREE(comp_data);

    *output_size = output_buffer_size;
    return output_buffer;
}

// Save indexed png data to memory buffer
char *rpng_save_image_indexed_to_memory(const char *indexed_data, int width, int height, rpng_palette palette, int *output_size)
{
    char *output_buffer = NULL;
    int output_buffer_size = 0;

    rpng_chunk_IHDR image_info = { 0 };
    image_info.width = swap_endian(width);
    image_info.height = swap_endian(height);
    image_info.bit_depth = 8;  // WARNING: Indexed data assumes 8-bit indexes
    image_info.color_type = 3; // NOTE: Indexed data requires 3

    // Image data pre-processing to append filter type byte to every scanline
    int pixel_size = 1; // 1 byte per pixel (indexed data)
    int comp_data_size = 0;
    char *comp_data = rpng_deflate_image_data(indexed_data, width*height*pixel_size, width, height, pixel_size, &comp_data_size, 0);

    // Security check to verify compression worked
    if ((comp_data != NULL) && (comp_data_size > 0))
    {
        // Verify if tRNS chunk with palette alpha values is required (if there is any alpha != 255)
        bool trns_required = false;
        for (int i = 0; i < palette.color_count; i++)
        {
            if (palette.colors[i].a != 255) { trns_required = true; break; }
        }

        // Allocate output buffer at required size
        output_buffer = (char *)RPNG_CALLOC(8 + ( 4 + 4 + 13 + 4) +  // Signature + IHDR
            (4 + 4 + palette.color_count*3 + 4) +   // PLTE
            (trns_required? (4 + 4 + palette.color_count + 4) : 0) + // tRNS
            (4 + 4 + comp_data_size + 4) + 12, 1);  // IDAT + IEND

        // Write PNG signature
        memcpy(output_buffer, png_signature, 8);

        // Write PNG chunk IHDR
        unsigned int length_IHDR = 13;
        length_IHDR = swap_endian(length_IHDR);
        memcpy(output_buffer + 8, &length_IHDR, 4);
        memcpy(output_buffer + 8 + 4, "IHDR", 4);
        memcpy(output_buffer + 8 + 8, &image_info, 13);
        unsigned int crc = compute_crc32((unsigned char *)output_buffer + 8 + 4, 4 + 13);
        crc = swap_endian(crc);
        memcpy(output_buffer + 8 + 8 + 13, &crc, 4);
        output_buffer_size += (8 + 12 + 13);

        // Write PNG chunk PLTE (palette)
        unsigned int length_PLTE = palette.color_count*3;
        length_PLTE = swap_endian(length_PLTE);
        memcpy(output_buffer + output_buffer_size, &length_PLTE, 4);
        memcpy(output_buffer + output_buffer_size + 4, "PLTE", 4);
        char *plte_data = (char *)RPNG_CALLOC(palette.color_count*3, sizeof(char));
        for (int i = 0; i < palette.color_count; i++)
        {
            plte_data[i*3 + 0] = palette.colors[i].r;
            plte_data[i*3 + 1] = palette.colors[i].g;
            plte_data[i*3 + 2] = palette.colors[i].b;
        }
        memcpy(output_buffer + output_buffer_size + 8, plte_data, palette.color_count*3);
        RPNG_FREE(plte_data);
        crc = compute_crc32((unsigned char *)output_buffer + output_buffer_size + 4, 4 + palette.color_count*3);
        crc = swap_endian(crc);
        memcpy(output_buffer + output_buffer_size + 8 + palette.color_count*3, &crc, 4);
        output_buffer_size += (palette.color_count*3 + 12);

        // Write PNG chunk tRNS (palette alpha values, if required)
        if (trns_required)
        {
            unsigned int length_tRNS = palette.color_count;
            length_tRNS = swap_endian(length_tRNS);
            memcpy(output_buffer + output_buffer_size, &length_tRNS, 4);
            memcpy(output_buffer + output_buffer_size + 4, "tRNS", 4);
            char *trns_data = (char *)RPNG_CALLOC(palette.color_count, sizeof(char));
            for (int i = 0; i < palette.color_count; i++) trns_data[i] = palette.colors[i].a;
            memcpy(output_buffer + output_buffer_size + 8, trns_data, palette.color_count);
            RPNG_FREE(trns_data);
            crc = compute_crc32((unsigned char *)output_buffer + output_buffer_size + 4, 4 + palette.color_count);
            crc = swap_endian(crc);
            memcpy(output_buffer + output_buffer_size + 8 + palette.color_count, &crc, 4);
            output_buffer_size += (palette.color_count + 12);
        }

        // Write PNG chunk IDAT
        unsigned int length_IDAT = comp_data_size;
        length_IDAT = swap_endian(length_IDAT);
        memcpy(output_buffer + output_buffer_size, &length_IDAT, 4);
        memcpy(output_buffer + output_buffer_size + 4, "IDAT", 4);
        memcpy(output_buffer + output_buffer_size + 8, comp_data, comp_data_size);
        crc = compute_crc32((unsigned char *)output_buffer + output_buffer_size + 4, 4 + comp_data_size);
        crc = swap_endian(crc);
        memcpy(output_buffer + output_buffer_size + 8 + comp_data_size, &crc, 4);
        output_buffer_size += (comp_data_size + 12);

        // Write PNG chunk IEND
        unsigned char chunk_IEND[12] = { 0, 0, 0, 0, 'I', 'E', 'N', 'D', 0xAE, 0x42, 0x60, 0x82 };
        memcpy(output_buffer + output_buffer_size, chunk_IEND, 12);
        output_buffer_size += 12;
    }

    RPNG_FREE(comp_data);

    *output_size = output_buffer_size;
    return output_buffer;
}


// Convert indexed image data to RGBA data
char *rpng_unindex_image_data(char *indexed_data, int width, int height, rpng_palette palette)
{
    char *data = NULL;

    if ((indexed_data != NULL) && (palette.color_count > 0) && (palette.colors != NULL))
    {
        data = (char *)RPNG_CALLOC(width*height*4, sizeof(char));

        for (int i = 0; i < width*height; i++)
        {
            data[i*4 + 0] = palette.colors[(int)indexed_data[i]].r;
            data[i*4 + 1] = palette.colors[(int)indexed_data[i]].g;
            data[i*4 + 2] = palette.colors[(int)indexed_data[i]].b;
            data[i*4 + 3] = palette.colors[(int)indexed_data[i]].a;
        }

    }
    else RPNG_LOG("Provided indexed data or palette not valid, data can not be un-indexed\n");

    return data;
}

//-------------------------------------------------------------------------------------------------
// PNG chunks managemeng functionality
//-------------------------------------------------------------------------------------------------

// Count the chunks in a PNG image from memory buffer
int rpng_chunk_count_from_memory(const char *buffer)
{
    char *buffer_ptr = (char *)buffer;
    int count = 0;

    // NOTE: We check minimum file_size for a PNG (Signature + chunk IHDR + chunk IDAT + chunk IEND)
    if ((buffer_ptr != NULL) && (memcmp(buffer_ptr, png_signature, 8) == 0))  // Check valid PNG file
    {
        buffer_ptr += 8;       // Move pointer after signature

        unsigned int chunk_size = swap_endian(((int *)buffer_ptr)[0]);

        while (memcmp(buffer_ptr + 4, "IEND", 4) != 0) // While IEND chunk not reached
        {
            buffer_ptr += (4 + 4 + chunk_size + 4);    // Skip chunk Length + FOURCC + chunk data + CRC32

            chunk_size = swap_endian(((int *)buffer_ptr)[0]);
            count++;
        }

        count++; // IEND chunk!
    }

    return count;
}

// Read one chunk type from memory buffer
rpng_chunk rpng_chunk_read_from_memory(const char *buffer, const char *chunk_type)
{
    char *buffer_ptr = (char *)buffer;
    rpng_chunk chunk = { 0 };

    // NOTE: We check minimum file_size for a PNG (Signature + chunk IHDR + chunk IDAT + chunk IEND)
    if ((buffer_ptr != NULL) && (memcmp(buffer_ptr, png_signature, 8) == 0))  // Check valid PNG file
    {
        buffer_ptr += 8;   // Move pointer after signature
        unsigned int chunk_size = swap_endian(((int *)buffer_ptr)[0]);

        // In case chunk(s) requested is IDAT, all IDAT chunks are concatenated
        if (memcmp(chunk_type, "IDAT", 4) == 0)
        {
            char *idat_data_concat = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, sizeof(char));
            int idat_data_concat_size = 0;

            while (memcmp(buffer_ptr + 4, "IEND", 4) != 0) // While IEND chunk not reached
            {
                if (memcmp(buffer_ptr + 4, chunk_type, 4) == 0) // Check next IDAT chunk
                {
                    memcpy(idat_data_concat + idat_data_concat_size, (char *)(buffer_ptr + 8), chunk_size);
                    idat_data_concat_size += chunk_size;

                    // TODO: Validate every IDAT chunk CRC32
                }

                buffer_ptr += (4 + 4 + chunk_size + 4); // Move pointer to next chunk of input data
                chunk_size = swap_endian(((int *)buffer_ptr)[0]); // Compute next chunk file_size
            }

            // Fill chunk data with all accumulated IDAT
            chunk.length = idat_data_concat_size;
            memcpy(chunk.type, "IDAT", 4);
            chunk.data = (char *)RPNG_CALLOC(idat_data_concat_size, sizeof(char));
            memcpy(chunk.data, idat_data_concat, idat_data_concat_size);
            RPNG_FREE(idat_data_concat);

            // Compute CRC32 for security
            unsigned char *chunk_type_data = (unsigned char *)RPNG_CALLOC(4 + chunk.length, 1);
            memcpy(chunk_type_data, chunk.type, 4);
            memcpy(chunk_type_data + 4, chunk.data, chunk.length);
            chunk.crc = compute_crc32(chunk_type_data, 4 + chunk.length);
            RPNG_FREE(chunk_type_data);
        }
        else // Only one chunk required, not IDAT type
        {
            while (memcmp(buffer_ptr + 4, "IEND", 4) != 0) // While IEND chunk not reached
            {
                if (memcmp(buffer_ptr + 4, chunk_type, 4) == 0)
                {
                    chunk.length = chunk_size;
                    memcpy(chunk.type, (char *)(buffer_ptr + 4), 4);
                    chunk.data = (char *)RPNG_MALLOC(chunk_size);
                    memcpy(chunk.data, buffer_ptr + 8, chunk_size);
                    chunk.crc = swap_endian(((unsigned int *)(buffer_ptr + 8 + chunk_size))[0]);

                    break;
                }

                buffer_ptr += (4 + 4 + chunk_size + 4); // Move pointer to next chunk of input data
                chunk_size = swap_endian(((int *)buffer_ptr)[0]); // Compute next chunk file_size
            }
        }
    }

    return chunk;
}

// Read all chunks from memory buffer
rpng_chunk *rpng_chunk_read_all_from_memory(const char *buffer, int *count)
{
    char *buffer_ptr = (char *)buffer;
    rpng_chunk *chunks = NULL;
    int counter = 0;

    // NOTE: We check minimum file_size for a PNG (Signature + chunk IHDR + chunk IDAT + chunk IEND)
    if ((buffer_ptr != NULL) && (memcmp(buffer_ptr, png_signature, 8) == 0))  // Check valid PNG file signature
    {
        // We allocate enough space for 64 chunks
        chunks = (rpng_chunk *)RPNG_CALLOC(RPNG_MAX_CHUNKS_COUNT, sizeof(rpng_chunk));
        buffer_ptr += 8; // Move pointer after signature

        unsigned int chunk_size = swap_endian(((int *)buffer_ptr)[0]);

        while (memcmp(buffer_ptr + 4, "IEND", 4) != 0) // While IEND chunk not reached
        {
            chunks[counter].length = chunk_size;
            memcpy(chunks[counter].type, (char *)(buffer_ptr + 4), 4);
            chunks[counter].data = (char *)RPNG_MALLOC(chunk_size);
            memcpy(chunks[counter].data, buffer_ptr + 8, chunk_size);
            chunks[counter].crc = swap_endian(((unsigned int *)(buffer_ptr + 8 + chunk_size))[0]);

            buffer_ptr += (4 + 4 + chunk_size + 4);   // Move pointer to next chunk
            chunk_size = swap_endian(((int *)buffer_ptr)[0]);

            counter++;
            if (counter >= (RPNG_MAX_CHUNKS_COUNT - 2)) break;   // WARNING: Too many chunks!
        }

        // Read final IEND chunk
        chunks[counter].length = chunk_size;
        memcpy(chunks[counter].type, (char *)(buffer_ptr + 4), 4);
        chunks[counter].data = (char *)RPNG_MALLOC(chunk_size);
        memcpy(chunks[counter].data, buffer_ptr + 8, chunk_size);
        chunks[counter].crc = swap_endian(((unsigned int *)(buffer_ptr + 8 + chunk_size))[0]);
        counter++;

        // Reallocate chunks file_size
        rpng_chunk *chunks_resized = (rpng_chunk*)RPNG_REALLOC(chunks, counter*sizeof(rpng_chunk));
        if (chunks_resized != NULL) chunks = chunks_resized;
    }

    *count = counter;
    return chunks;
}

// Remove one chunk type from memory buffer
// NOTE: returns output_data and output_size through parameter
char *rpng_chunk_remove_from_memory(const char *buffer, const char *chunk_type, int *output_size)
{
    char *buffer_ptr = (char *)buffer;
    char *output_buffer = NULL;
    int output_buffer_size = 0;

    if ((buffer_ptr != NULL) && (memcmp(buffer_ptr, png_signature, 8) == 0))  // Check valid PNG file
    {
        output_buffer = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, 1);  // Output buffer allocation

        memcpy(output_buffer, png_signature, 8);        // Copy PNG signature
        output_buffer_size += 8;

        buffer_ptr += 8;       // Move pointer after signature

        unsigned int chunk_size = swap_endian(((int *)buffer_ptr)[0]);

        while (memcmp(buffer_ptr + 4, "IEND", 4) != 0) // While IEND chunk not reached
        {
            // If chunk type is not the requested, just copy input data into output buffer
            if (memcmp(buffer_ptr + 4, chunk_type, 4) != 0)
            {
                memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + chunk_size + 4);  // Length + FOURCC + chunk_size + CRC32
                output_buffer_size += (4 + 4 + chunk_size + 4);
            }

            buffer_ptr += (4 + 4 + chunk_size + 4);   // Move pointer to next chunk
            chunk_size = swap_endian(((int *)buffer_ptr)[0]);
        }

        // Write IEND chunk
        memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + 4);
        output_buffer_size += 12;

        // Resize output buffer
        char *output_buffer_sized = (char *)RPNG_REALLOC(output_buffer, output_buffer_size);
        if (output_buffer_sized != NULL) output_buffer = output_buffer_sized;
    }

    *output_size = output_buffer_size;
    return output_buffer;
}

// Remove all chunks from memory buffer except: IHDR-IDAT-IEND
// NOTE: returns output_data and output_size through parameter
char *rpng_chunk_remove_ancillary_from_memory(const char *buffer, int *output_size)
{
    char *buffer_ptr = (char *)buffer;
    char *output_buffer = NULL;
    int output_buffer_size = 0;

    if ((buffer_ptr != NULL) && (memcmp(buffer_ptr, png_signature, 8) == 0))  // Check valid PNG file
    {
        bool preserve_palette_transparency = false;

        output_buffer = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, 1);  // Output buffer allocation

        memcpy(output_buffer, png_signature, 8);        // Copy PNG signature
        output_buffer_size += 8;
        buffer_ptr += 8;       // Move pointer after signature

        unsigned int chunk_size = swap_endian(((int *)buffer_ptr)[0]);

        while (memcmp(buffer_ptr + 4, "IEND", 4) != 0) // While IEND chunk not reached
        {
            if (memcmp(buffer_ptr + 4, "PLTE", 4) == 0) preserve_palette_transparency = true;

            // If chunk type is mandatory, just copy input data into output buffer
            if ((memcmp(buffer_ptr + 4, "IHDR", 4) == 0) ||
                (memcmp(buffer_ptr + 4, "PLTE", 4) == 0) ||
                (memcmp(buffer_ptr + 4, "IDAT", 4) == 0) ||
                (preserve_palette_transparency && (memcmp(buffer_ptr + 4, "tRNS", 4) == 0)))
            {
                memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + chunk_size + 4);  // Length + FOURCC + chunk_size + CRC32
                output_buffer_size += (4 + 4 + chunk_size + 4);
            }

            buffer_ptr += (4 + 4 + chunk_size + 4);   // Move pointer to next chunk
            chunk_size = swap_endian(((int *)buffer_ptr)[0]);
        }

        // Write IEND chunk
        memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + 4);
        output_buffer_size += 12;

        // Resize output buffer
        char *output_buffer_sized = (char *)RPNG_REALLOC(output_buffer, output_buffer_size);
        if (output_buffer_sized != NULL) output_buffer = output_buffer_sized;
    }

    *output_size = output_buffer_size;
    return output_buffer;
}

// Write one new chunk after IHDR (any kind) to memory buffer
// NOTE: returns output data file_size
char *rpng_chunk_write_from_memory(const char *buffer, rpng_chunk chunk, int *output_size)
{
    char *buffer_ptr = (char *)buffer;
    char *output_buffer = NULL;
    int output_buffer_size = 0;

    if ((buffer_ptr != NULL) && (memcmp(buffer_ptr, png_signature, 8) == 0))  // Check valid PNG file
    {
        output_buffer = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, 1);

        memcpy(output_buffer, png_signature, 8);        // Copy PNG signature
        output_buffer_size += 8;
        buffer_ptr += 8;       // Move pointer after signature

        unsigned int chunk_size = swap_endian(((int *)buffer_ptr)[0]);

        while (memcmp(buffer_ptr + 4, "IEND", 4) != 0) // While IEND chunk not reached
        {
            memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + chunk_size + 4);  // Length + FOURCC + chunk_size + CRC32
            output_buffer_size += (4 + 4 + chunk_size + 4);

            // Check if we just copied the IHDR chunk to append our chunk after it
            if (memcmp(buffer_ptr + 4, "IHDR", 4) == 0)
            {
                int chunk_length_be = swap_endian(chunk.length);
                memcpy(output_buffer + output_buffer_size, &chunk_length_be, sizeof(int));           // Write chunk length
                memcpy(output_buffer + output_buffer_size + 4, chunk.type, 4);                // Write chunk type
                memcpy(output_buffer + output_buffer_size + 4 + 4, chunk.data, chunk.length); // Write chunk data

                unsigned char *type_data = (unsigned char *)RPNG_MALLOC(4 + chunk.length);
                memcpy(type_data, chunk.type, 4);
                memcpy(type_data + 4, chunk.data, chunk.length);
                unsigned int crc = compute_crc32(type_data, 4 + chunk.length);
                crc = swap_endian(crc);
                memcpy(output_buffer + output_buffer_size + 4 + 4 + chunk.length, &crc, 4);   // Write CRC32 (computed over type + data)

                RPNG_FREE(type_data);

                output_buffer_size += (4 + 4 + chunk.length + 4);  // Update output file file_size with new chunk
            }

            buffer_ptr += (4 + 4 + chunk_size + 4);           // Move pointer to next chunk of input data
            chunk_size = swap_endian(((int *)buffer_ptr)[0]);  // Compute next chunk file_size
        }

        // Write IEND chunk
        memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + 4);
        output_buffer_size += 12;

        // Resize output buffer
        char *output_buffer_sized = (char *)RPNG_REALLOC(output_buffer, output_buffer_size);
        if (output_buffer_sized != NULL) output_buffer = output_buffer_sized;
    }

    *output_size = output_buffer_size;
    return output_buffer;
}

// Combine multiple IDAT chunks into a single one
// NOTE: Returns buffer with all concatenated IDAT chunks
char *rpng_chunk_combine_image_data_from_memory(char *buffer, int *output_size)
{
    char *buffer_ptr = (char *)buffer;
    char *output_buffer = NULL;
    int output_buffer_size = 0;

    if ((buffer_ptr != NULL) && (memcmp(buffer_ptr, png_signature, 8) == 0)) // Check valid PNG file
    {
        char *idata_buffer = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, 1); // Output buffer allocation
        memcpy(idata_buffer, "IDAT", 4);
        int idata_buffer_size = 0;
        output_buffer = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, 1); // Output buffer allocation

        memcpy(output_buffer, png_signature, 8); // Copy PNG signature
        output_buffer_size += 8;
        buffer_ptr += 8; // Move pointer after signature

        unsigned int chunk_size = swap_endian(((int *)buffer_ptr)[0]);

        while (memcmp(buffer_ptr + 4, "IEND", 4) != 0)         // While IEND chunk not reached
        {
            // If IDAT chunk just copy data into idata_buffer
            if ((memcmp(buffer_ptr + 4, "IDAT", 4) == 0))
            {
                memcpy(idata_buffer + 4 + idata_buffer_size, buffer_ptr + 8, chunk_size);
                idata_buffer_size += chunk_size;
            }
            else
            {
                memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + chunk_size + 4);  // Length + FOURCC + chunk_size + CRC32
                output_buffer_size += (4 + 4 + chunk_size + 4);
            }

            buffer_ptr += (4 + 4 + chunk_size + 4);   // Move pointer to next chunk
            chunk_size = swap_endian(((int *)buffer_ptr)[0]);
        }

        // Write IDAT combined chunk
        unsigned int idata_buffer_size_be = swap_endian(idata_buffer_size);
        memcpy(output_buffer + output_buffer_size, &idata_buffer_size_be, 4);
        memcpy(output_buffer + output_buffer_size + 4, idata_buffer, 4 + idata_buffer_size);
        unsigned int crc = compute_crc32((unsigned char *)idata_buffer, 4 + idata_buffer_size);
        crc = swap_endian(crc);
        memcpy(output_buffer + output_buffer_size + 4 + 4 + idata_buffer_size, &crc, 4);
        RPNG_FREE(idata_buffer);

        output_buffer_size += (idata_buffer_size + 12);

        // Write IEND chunk
        memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + 4);
        output_buffer_size += 12;

        // Resize output buffer
        char *output_buffer_sized = (char *)RPNG_REALLOC(output_buffer, output_buffer_size);
        if (output_buffer_sized != NULL) output_buffer = output_buffer_sized;
    }

    *output_size = output_buffer_size;
    return output_buffer;
}

// Split one IDAT chunk into multiple ones
char *rpng_chunk_split_image_data_from_memory(char *buffer, int split_size, int *output_size)
{
    char *buffer_ptr = (char *)buffer;
    char *output_buffer = NULL;
    int output_buffer_size = 0;

    if ((buffer_ptr != NULL) && (memcmp(buffer_ptr, png_signature, 8) == 0))  // Check valid PNG file
    {
        char *idata_split_buffer = (char *)RPNG_CALLOC(split_size + 12, 1);    // Output buffer allocation

        output_buffer = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, 1);  // Output buffer allocation

        memcpy(output_buffer, png_signature, 8);    // Copy PNG signature
        output_buffer_size += 8;
        buffer_ptr += 8;       // Move pointer after signature

        unsigned int chunk_size = swap_endian(((int *)buffer_ptr)[0]);

        while (memcmp(buffer_ptr + 4, "IEND", 4) != 0)         // While IEND chunk not reached
        {
            // If IDAT chunk, split into multiple sized chunks
            if ((memcmp(buffer_ptr + 4, "IDAT", 4) == 0) && (chunk_size > (unsigned int)split_size))
            {
                // Split chunk into pieces!
                unsigned int chunk_remain_size = chunk_size;
                char *buffer_ptr_offset = buffer_ptr + 4 + 4;

                while (chunk_remain_size > (unsigned int)split_size)
                {
                    unsigned int split_size_be = swap_endian(split_size);
                    memcpy(idata_split_buffer, &split_size_be, 4);
                    memcpy(idata_split_buffer + 4, "IDAT", 4);
                    memcpy(idata_split_buffer + 4 + 4, buffer_ptr_offset, split_size);
                    unsigned int crc = compute_crc32((unsigned char *)(idata_split_buffer + 4), 4 + split_size);
                    crc = swap_endian(crc);
                    memcpy(idata_split_buffer + 4 + 4 + split_size, &crc, 4);

                    chunk_remain_size -= split_size;
                    buffer_ptr_offset += split_size;

                    memcpy(output_buffer + output_buffer_size, idata_split_buffer, split_size + 12);
                    output_buffer_size += (split_size + 12);
                }

                // Save last IDAT chunk piece
                unsigned int chunk_remain_size_be = swap_endian(chunk_remain_size);
                memcpy(idata_split_buffer, &chunk_remain_size_be, 4);
                memcpy(idata_split_buffer + 4, "IDAT", 4);
                memcpy(idata_split_buffer + 4 + 4, buffer_ptr_offset, chunk_remain_size);
                unsigned int crc = compute_crc32((unsigned char *)(idata_split_buffer + 4), 4 + chunk_remain_size);
                crc = swap_endian(crc);
                memcpy(idata_split_buffer + 4 + 4 + chunk_remain_size, &crc, 4);

                memcpy(output_buffer + output_buffer_size, idata_split_buffer, chunk_remain_size + 12);
                output_buffer_size += (chunk_remain_size + 12);
            }
            else
            {
                memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + chunk_size + 4);  // Length + FOURCC + chunk_size + CRC32
                output_buffer_size += (4 + 4 + chunk_size + 4);
            }

            buffer_ptr += (4 + 4 + chunk_size + 4);   // Move pointer to next chunk
            chunk_size = swap_endian(((int *)buffer_ptr)[0]);
        }

        RPNG_FREE(idata_split_buffer);

        // Write IEND chunk
        memcpy(output_buffer + output_buffer_size, buffer_ptr, 4 + 4 + 4);
        output_buffer_size += 12;

        // Resize output buffer
        char *output_buffer_sized = (char *)RPNG_REALLOC(output_buffer, output_buffer_size);
        if (output_buffer_sized != NULL) output_buffer = output_buffer_sized;
    }

    *output_size = output_buffer_size;
    return output_buffer;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Prefilter and compress image data
static char *rpng_deflate_image_data(const char *image_data, int image_data_size, int width, int height, int pixel_size, int *output_size, int forced_filter_type)
{
    char *idat_data = NULL;

    // Image data pre-processing to append filter type byte to every scanline
    //int pixel_size = color_channels*(bit_depth/8);
    int scanline_size = width*pixel_size;
    unsigned int data_filtered_size = (scanline_size + 1)*height;   // Adding 1 byte per scanline filter
    unsigned char *data_filtered = (unsigned char *)RPNG_CALLOC(data_filtered_size, 1);

    int out = 0, x = 0, a = 0, b = 0, c = 0;
    int sum_value[5] = { 0 };
    int best_filter = 0;

    for (int y = 0; y < height; y++)
    {
        if (forced_filter_type == -1)
        {
            // Choose the best filter type for every scanline
            // REF: https://www.w3.org/TR/PNG-Encoders.html#E.Filter-selection
            for (int p = 0; p < scanline_size; p++)
            {
                // x = current byte
                // a = left pixel byte (from current)
                // b = above pixel byte (from current)
                // c = left pixel byte (from b)
                x = (int)((unsigned char *)image_data)[scanline_size*y + p];
                a = (p >= pixel_size) ? (int)((unsigned char *)image_data)[scanline_size*y + p - pixel_size] : 0;
                b = (y > 0) ? (int)((unsigned char *)image_data)[scanline_size*(y - 1) + p] : 0;
                c = (y > 0) ? ((p >= pixel_size) ? (int)((unsigned char *)image_data)[scanline_size*(y - 1) + p - pixel_size] : 0) : 0;

                // Heuristic: Compute the output scanline using all five filters
                // REF: https://www.w3.org/TR/PNG/#9Filters
                for (int filter = 0; filter < 5; filter++)
                {
                    switch (filter)
                    {
                        case 0: out = x; break;
                        case 1: out = x - a; break;
                        case 2: out = x - b; break;
                        case 3: out = x - ((a + b)>>1); break;
                        case 4: out = x - rpng_paeth_predictor(a, b, c); break;
                        default: break;
                    }

                    sum_value[filter] += abs((signed char)out);
                }
            }

            // Select the filter that gives the smallest sum of absolute values of outputs.
            // NOTE: Considering the output bytes as signed differences for the test.
            best_filter = 0;
            int best_value = sum_value[0];

            for (int filter = 1; filter < 5; filter++)
            {
                if (sum_value[filter] < best_value)
                {
                    best_value = sum_value[filter];
                    best_filter = filter;
                }
            }
        }
        else if ((forced_filter_type >= 0) && (forced_filter_type <= 4)) best_filter = forced_filter_type;

        // Register scanline filter byte
        data_filtered[(scanline_size + 1)*y] = best_filter;

        // Apply the best_filter to scanline
        for (int p = 0; p < scanline_size; p++)
        {
            x = (int)((unsigned char *)image_data)[scanline_size*y + p];
            a = (p >= pixel_size)? (int)((unsigned char *)image_data)[scanline_size*y + p - pixel_size] : 0;
            b = (y > 0)? (int)((unsigned char *)image_data)[scanline_size*(y - 1) + p] : 0;
            c = (y > 0)? ((p >= pixel_size) ? (int)((unsigned char *)image_data)[scanline_size*(y - 1) + p - pixel_size] : 0) : 0;

            switch (best_filter)
            {
                case 0: out = x; break;
                case 1: out = x - a; break;
                case 2: out = x - b; break;
                case 3: out = x - ((a + b)>>1); break;
                case 4: out = x - rpng_paeth_predictor(a, b, c); break;
                default: break;
            }

            // Register scanline filtered values, byte by byte
            data_filtered[(scanline_size + 1)*y + 1 + p] = (unsigned char)out;
        }
    }

    // Compress filtered image data and generate a valid zlib stream
    struct sdefl *sde = (struct sdefl*)RPNG_CALLOC(sizeof(struct sdefl), 1);
    int bounds = sdefl_bound(data_filtered_size);
    char *comp_data = (char *)RPNG_CALLOC(bounds, 1);
    int comp_data_size = zsdeflate(sde, comp_data, data_filtered, data_filtered_size, RPNG_COMPRESSION_LEVEL);
    RPNG_FREE(data_filtered);
    RPNG_FREE(sde);

    if ((comp_data != NULL) && (comp_data_size > 0))
    {
        idat_data = comp_data;
        *output_size = comp_data_size;
        RPNG_LOG("INFO: Image data deflated successfully: %i bytes -> %i bytes\n", data_filtered_size, comp_data_size);
    }
    else RPNG_LOG("INFO: Image data deflating failed\n");

    return idat_data;
}

// Decompress and unfilter image data (IDAT)
static char *rpng_inflate_image_data(char *image_data, int image_data_size, int width, int height, int pixel_size)
{
    char *image_data_unfiltered = NULL;
    char *image_data_filtered = (char *)RPNG_CALLOC(RPNG_MAX_OUTPUT_SIZE, 1); // WARNING: Allocate enough memory to load full image decompressed

    // Decompress IDAT chunk data
    int image_data_decomp_size = zsinflate(image_data_filtered, RPNG_MAX_OUTPUT_SIZE, image_data, image_data_size);

    RPNG_LOG("INFO: IDAT data decompressed: %i -> %i\n", image_data_size, image_data_decomp_size);

    if ((image_data_filtered != NULL) && (image_data_decomp_size > 0))
    {
        // Now we have the data decompressed but every scanline of the image was originally filtered for
        // maximum compression and one extra byte with the filter type was added to every scanline
        // We must undo that image prefiltering for every scanline

        // Image data reverse pre-processing for filter type
        //int pixel_size = *color_channels*(*bit_depth/8);
        int scanline_size = width*pixel_size;
        image_data_unfiltered = (char *)RPNG_CALLOC(image_data_decomp_size, 1);  // Actually data unfiltered size should be smaller

        int current_filter = 0;
        unsigned char out = 0, x = 0, a = 0, b = 0, c = 0;

        // Reverse scanlines filters
        for (int y = 0; y < height; y++)   // Move scanline by scanline, we must discard first byte = current_filter
        {
            current_filter = (int)image_data_filtered[(1 + scanline_size)*y];

            for (int p = 0; p < scanline_size; p++)
            {
                // x = current byte
                // a = left pixel byte (from current)
                // b = above pixel byte (from current)
                // c = left pixel byte (from b)
                x = (unsigned char)(image_data_filtered[(1 + scanline_size)*y + 1 + p]);
                a = (p >= pixel_size) ? (unsigned char)(image_data_unfiltered[scanline_size*y + p - pixel_size]) : 0;
                b = (y > 0) ? (unsigned char)(image_data_unfiltered[scanline_size*(y - 1) + p]) : 0;
                c = (y > 0) ? ((p >= pixel_size) ? (unsigned char)(image_data_unfiltered[scanline_size*(y - 1) + p - pixel_size]) : 0) : 0;

                switch (current_filter)
                {
                    case 0: out = x; break;         // Filter type 0: None (Usually used for indexed images)
                    case 1: out = x + a; break;     // Filter type 1: Sub
                    case 2: out = x + b; break;     // Filter type 2: Up
                    case 3: out = x + ((a + b)>>1); break;    // Filter type 3: Average
                    case 4: out = x + rpng_paeth_predictor(a, b, c); break;  // Filter type 4: Paeth
                    default: break;
                }

                // Register scanline unfiltered values, byte by byte
                image_data_unfiltered[y*scanline_size + p] = (char)out;
            }
        }

        RPNG_FREE(image_data_filtered);
    }

    return image_data_unfiltered;
}

// Swap integer from big<->little endian
static unsigned int swap_endian(unsigned int value)
{
    // Swap endian (big to little) or (little to big)
    unsigned int b0, b1, b2, b3;
    unsigned int res;

    b0 = (value & 0x000000ff) << 24u;
    b1 = (value & 0x0000ff00) << 8u;
    b2 = (value & 0x00ff0000) >> 8u;
    b3 = (value & 0xff000000) >> 24u;

    res = b0 | b1 | b2 | b3;

    return res;
}

// Compute CRC32
static unsigned int compute_crc32(unsigned char *buffer, int size)
{
    static unsigned int crc_table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0eDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
        0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
        0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
        0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
        0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
        0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
        0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
        0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
        0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
        0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
        0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
        0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
        0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
        0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
        0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
        0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
        0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
        0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
        0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };

    unsigned int crc = ~0u;

    for (int i = 0; i < size; i++) crc = (crc >> 8) ^ crc_table[buffer[i] ^ (crc & 0xff)];

    return ~crc;
}

// Load data from file into a buffer
static char *load_file_to_buffer(const char *filename, int *bytes_read)
{
    char *data = NULL;
    *bytes_read = 0;

#if !defined(RPNG_NO_STDIO)
    // Check if the file exists before reading it
    if ((filename != NULL) && file_exists(filename))
    {
        FILE *file = fopen(filename, "rb");

        if (file != NULL)
        {
            // WARNING: On binary streams SEEK_END could not be found,
            // using fseek() and ftell() could not work in some (rare) cases
            fseek(file, 0, SEEK_END);
            int file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (file_size > 0)
            {
                data = (char *)RPNG_MALLOC(sizeof(unsigned char)*file_size);

                // NOTE: fread() returns number of read elements instead of bytes, so we read [1 byte, file_size elements]
                int count = (int)fread(data, sizeof(char), file_size, file);
                *bytes_read = count;

                if (count != file_size) RPNG_LOG("FILEIO: [%s] File partially loaded\n", filename);
                else RPNG_LOG("FILEIO: [%s] File loaded successfully\n", filename);
            }
            else RPNG_LOG("FILEIO: [%s] Failed to read file\n", filename);

            fclose(file);
        }
        else RPNG_LOG("FILEIO: [%s] Failed to open file\n", filename);
    }
    else RPNG_LOG("FILEIO: File path provided is not valid\n");
#else
    (void)filename;
    #ifndef RPNG_NO_STDIO_WARNING
        #warning No FILE I/O API, RPNG_NO_STDIO defined
    #endif
#endif
    return data;
}

// Write data to file from buffer
static int save_file_from_buffer(const char *filename, void *data, int bytesToWrite)
{
    int result = RPNG_SUCCESS;
#if !defined(RPNG_NO_STDIO)
    if ((filename != NULL) && (data != NULL) && (bytesToWrite > 0))
    {
        FILE *file = fopen(filename, "wb");

        if (file != NULL)
        {
            int count = (int)fwrite(data, sizeof(char), bytesToWrite, file);

            if (count == 0) RPNG_LOG("FILEIO: [%s] Failed to write file\n", filename);
            else if (count != bytesToWrite) RPNG_LOG("FILEIO: [%s] File partially written\n", filename);
            else RPNG_LOG("FILEIO: [%s] File saved successfully\n", filename);

            fclose(file);
        }
        else
        {
            result = RPNG_ERROR_FILE_OPEN;
            RPNG_LOG("FILEIO: [%s] Failed to open file\n", filename);
        }
    }
    else RPNG_LOG("FILEIO: File path or data provided are not valid\n");
#else
    (void)filename;
    (void)data;
    (void)bytesToWrite;
    #ifndef RPNG_NO_STDIO_WARNING
        #warning No FILE I/O API, RPNG_NO_STDIO defined
    #endif
#endif
    return result;
}

// Check if the file exists
static bool file_exists(const char *filename)
{
    bool result = false;

#if defined(_WIN32)
    if (_access(filename, 0) != -1) result = true;
#else
    if (access(filename, F_OK) != -1) result = true;
#endif

    return result;
}

#if defined(RPNG_DEFLATE_IMPLEMENTATION)

//=========================================================================
//                              SDEFL
// DEFLATE COMPRESSION algorithm: https://github.com/vurtun/lib/sdefl.h
//=========================================================================
#ifdef SDEFL_IMPLEMENTATION

#include <assert.h> /* assert */
#include <string.h> /* memcpy */
#include <limits.h> /* CHAR_BIT */

#define SDEFL_NIL               (-1)
#define SDEFL_MAX_MATCH         258
#define SDEFL_MAX_CODE_LEN      (15)
#define SDEFL_SYM_BITS          (10u)
#define SDEFL_SYM_MSK           ((1u << SDEFL_SYM_BITS)-1u)
#define SDEFL_RAW_BLK_SIZE      (65535)
#define SDEFL_LIT_LEN_CODES     (14)
#define SDEFL_OFF_CODES         (15)
#define SDEFL_PRE_CODES         (7)
#define SDEFL_CNT_NUM(n)        ((((n)+3u/4u)+3u)&~3u)
#define SDEFL_EOB               (256)

#define sdefl_npow2(n) (1 << (sdefl_ilog2((n)-1) + 1))
#define sdefl_div_round_up(n,d) (((n)+((d)-1))/(d))

static int
sdefl_ilog2(int n) {
  if (!n) return 0;
#ifdef _MSC_VER
  unsigned long msbp = 0;
  _BitScanReverse(&msbp, (unsigned long)n);
  return (int)msbp;
#elif defined(__GNUC__) || defined(__clang__)
  return (int)sizeof(unsigned long) * CHAR_BIT - 1 - __builtin_clzl((unsigned long)n);
#else
  #define lt(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
  static const char tbl[256] = {
    0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,lt(4), lt(5), lt(5), lt(6), lt(6), lt(6), lt(6),
    lt(7), lt(7), lt(7), lt(7), lt(7), lt(7), lt(7), lt(7)};
  int tt, t;
  if ((tt = (n >> 16))) {
    return (t = (tt >> 8)) ? 24 + tbl[t] : 16 + tbl[tt];
  } else {
    return (t = (n >> 8)) ? 8 + tbl[t] : tbl[n];
  }
  #undef lt
#endif
}
static unsigned
sdefl_uload32(const void *p) {
  /* hopefully will be optimized to an unaligned read */
  unsigned n = 0;
  memcpy(&n, p, sizeof(n));
  return n;
}
static unsigned
sdefl_hash32(const void *p) {
  unsigned n = sdefl_uload32(p);
  return (n * 0x9E377989) >> (32 - SDEFL_HASH_BITS);
}
static void
sdefl_put(unsigned char **dst, struct sdefl *s, int code, int bitcnt) {
  s->bits |= (code << s->bitcnt);
  s->bitcnt += bitcnt;
  while (s->bitcnt >= 8) {
    unsigned char *tar = *dst;
    *tar = (unsigned char)(s->bits & 0xFF);
    s->bits >>= 8;
    s->bitcnt -= 8;
    *dst = *dst + 1;
  }
}
static void
sdefl_heap_sub(unsigned A[], unsigned len, unsigned sub) {
  unsigned c, p = sub;
  unsigned v = A[sub];
  while ((c = p << 1) <= len) {
    if (c < len && A[c + 1] > A[c]) c++;
    if (v >= A[c]) break;
    A[p] = A[c], p = c;
  }
  A[p] = v;
}
static void
sdefl_heap_array(unsigned *A, unsigned len) {
  unsigned sub;
  for (sub = len >> 1; sub >= 1; sub--)
    sdefl_heap_sub(A, len, sub);
}
static void
sdefl_heap_sort(unsigned *A, unsigned n) {
  A--;
  sdefl_heap_array(A, n);
  while (n >= 2) {
    unsigned tmp = A[n];
    A[n--] = A[1];
    A[1] = tmp;
    sdefl_heap_sub(A, n, 1);
  }
}
static unsigned
sdefl_sort_sym(unsigned sym_cnt, unsigned *freqs,
               unsigned char *lens, unsigned *sym_out) {
  unsigned cnts[SDEFL_CNT_NUM(SDEFL_SYM_MAX)] = {0};
  unsigned cnt_num = SDEFL_CNT_NUM(sym_cnt);
  unsigned used_sym = 0;
  unsigned sym, i;
  for (sym = 0; sym < sym_cnt; sym++)
    cnts[freqs[sym] < cnt_num-1 ? freqs[sym]: cnt_num-1]++;
  for (i = 1; i < cnt_num; i++) {
    unsigned cnt = cnts[i];
    cnts[i] = used_sym;
    used_sym += cnt;
  }
  for (sym = 0; sym < sym_cnt; sym++) {
    unsigned freq = freqs[sym];
    if (freq) {
        unsigned idx = freq < cnt_num-1 ? freq : cnt_num-1;
        sym_out[cnts[idx]++] = sym | (freq << SDEFL_SYM_BITS);
    } else lens[sym] = 0;
  }
  sdefl_heap_sort(sym_out + cnts[cnt_num-2], cnts[cnt_num-1] - cnts[cnt_num-2]);
  return used_sym;
}
static void
sdefl_build_tree(unsigned *A, unsigned sym_cnt) {
  unsigned i = 0, b = 0, e = 0;
  do {
    unsigned m, n, freq_shift;
    if (i != sym_cnt && (b == e || (A[i] >> SDEFL_SYM_BITS) <= (A[b] >> SDEFL_SYM_BITS)))
      m = i++;
    else m = b++;
    if (i != sym_cnt && (b == e || (A[i] >> SDEFL_SYM_BITS) <= (A[b] >> SDEFL_SYM_BITS)))
      n = i++;
    else n = b++;

    freq_shift = (A[m] & ~SDEFL_SYM_MSK) + (A[n] & ~SDEFL_SYM_MSK);
    A[m] = (A[m] & SDEFL_SYM_MSK) | (e << SDEFL_SYM_BITS);
    A[n] = (A[n] & SDEFL_SYM_MSK) | (e << SDEFL_SYM_BITS);
    A[e] = (A[e] & SDEFL_SYM_MSK) | freq_shift;
  } while (sym_cnt - ++e > 1);
}
static void
sdefl_gen_len_cnt(unsigned *A, unsigned root, unsigned *len_cnt,
                  unsigned max_code_len) {
  int n;
  unsigned i;
  for (i = 0; i <= max_code_len; i++)
    len_cnt[i] = 0;
  len_cnt[1] = 2;

  A[root] &= SDEFL_SYM_MSK;
  for (n = (int)root - 1; n >= 0; n--) {
    unsigned p = A[n] >> SDEFL_SYM_BITS;
    unsigned pdepth = A[p] >> SDEFL_SYM_BITS;
    unsigned depth = pdepth + 1;
    unsigned len = depth;

    A[n] = (A[n] & SDEFL_SYM_MSK) | (depth << SDEFL_SYM_BITS);
    if (len >= max_code_len) {
      len = max_code_len;
      do len--; while (!len_cnt[len]);
    }
    len_cnt[len]--;
    len_cnt[len+1] += 2;
  }
}
static void
sdefl_gen_codes(unsigned *A, unsigned char *lens, const unsigned *len_cnt,
                unsigned max_code_word_len, unsigned sym_cnt) {
  unsigned i, sym, len, nxt[SDEFL_MAX_CODE_LEN + 1];
  for (i = 0, len = max_code_word_len; len >= 1; len--) {
    unsigned cnt = len_cnt[len];
    while (cnt--) lens[A[i++] & SDEFL_SYM_MSK] = (unsigned char)len;
  }
  nxt[0] = nxt[1] = 0;
  for (len = 2; len <= max_code_word_len; len++)
    nxt[len] = (nxt[len-1] + len_cnt[len-1]) << 1;
  for (sym = 0; sym < sym_cnt; sym++)
    A[sym] = nxt[lens[sym]]++;
}
static unsigned
sdefl_rev(unsigned c, unsigned char n) {
  c = ((c & 0x5555) << 1) | ((c & 0xAAAA) >> 1);
  c = ((c & 0x3333) << 2) | ((c & 0xCCCC) >> 2);
  c = ((c & 0x0F0F) << 4) | ((c & 0xF0F0) >> 4);
  c = ((c & 0x00FF) << 8) | ((c & 0xFF00) >> 8);
  return c >> (16-n);
}
static void
sdefl_huff(unsigned char *lens, unsigned *codes, unsigned *freqs,
           unsigned num_syms, unsigned max_code_len) {
  unsigned c, *A = codes;
  unsigned len_cnt[SDEFL_MAX_CODE_LEN + 1];
  unsigned used_syms = sdefl_sort_sym(num_syms, freqs, lens, A);
  if (!used_syms) return;
  if (used_syms == 1) {
    unsigned s = A[0] & SDEFL_SYM_MSK;
    unsigned i = s ? s : 1;
    codes[0] = 0, lens[0] = 1;
    codes[i] = 1, lens[i] = 1;
    return;
  }
  sdefl_build_tree(A, used_syms);
  sdefl_gen_len_cnt(A, used_syms-2, len_cnt, max_code_len);
  sdefl_gen_codes(A, lens, len_cnt, max_code_len, num_syms);
  for (c = 0; c < num_syms; c++) {
    codes[c] = sdefl_rev(codes[c], lens[c]);
  }
}
struct sdefl_symcnt {
  int items;
  int lit;
  int off;
};
static void
sdefl_precode(struct sdefl_symcnt *cnt, unsigned *freqs, unsigned *items,
              const unsigned char *litlen, const unsigned char *offlen) {
  unsigned *at = items;
  unsigned run_start = 0;

  unsigned total = 0;
  unsigned char lens[SDEFL_SYM_MAX + SDEFL_OFF_MAX];
  for (cnt->lit = SDEFL_SYM_MAX; cnt->lit > 257; cnt->lit--)
    if (litlen[cnt->lit - 1]) break;
  for (cnt->off = SDEFL_OFF_MAX; cnt->off > 1; cnt->off--)
    if (offlen[cnt->off - 1]) break;

  total = (unsigned)(cnt->lit + cnt->off);
  memcpy(lens, litlen, sizeof(unsigned char) * (size_t)cnt->lit);
  memcpy(lens + cnt->lit, offlen, sizeof(unsigned char) * (size_t)cnt->off);
  do {
    unsigned len = lens[run_start];
    unsigned run_end = run_start;
    do run_end++; while (run_end != total && len == lens[run_end]);
    if (!len) {
      while ((run_end - run_start) >= 11) {
        unsigned n = (run_end - run_start) - 11;
        unsigned xbits =  n < 0x7f ? n : 0x7f;
        freqs[18]++;
        *at++ = 18u | (xbits << 5u);
        run_start += 11 + xbits;
      }
      if ((run_end - run_start) >= 3) {
        unsigned n = (run_end - run_start) - 3;
        unsigned xbits =  n < 0x7 ? n : 0x7;
        freqs[17]++;
        *at++ = 17u | (xbits << 5u);
        run_start += 3 + xbits;
      }
    } else if ((run_end - run_start) >= 4) {
      freqs[len]++;
      *at++ = len;
      run_start++;
      do {
        unsigned xbits = (run_end - run_start) - 3;
        xbits = xbits < 0x03 ? xbits : 0x03;
        *at++ = 16 | (xbits << 5);
        run_start += 3 + xbits;
        freqs[16]++;
      } while ((run_end - run_start) >= 3);
    }
    while (run_start != run_end) {
      freqs[len]++;
      *at++ = len;
      run_start++;
    }
  } while (run_start != total);
  cnt->items = (int)(at - items);
}
struct sdefl_match_codest {
  int ls, lc;
  int dc, dx;
};
static void
sdefl_match_codes(struct sdefl_match_codest *cod, int dist, int len) {
  static const short dxmax[] = {0,6,12,24,48,96,192,384,768,1536,3072,6144,12288,24576};
  static const unsigned char lslot[258+1] = {
    0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 12,
    12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16,
    16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18,
    18, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 28
  };
  assert(len <= 258);
  assert(dist <= 32768);
  cod->ls = lslot[len];
  cod->lc = 257 + cod->ls;
  assert(cod->lc <= 285);

  cod->dx = sdefl_ilog2(sdefl_npow2(dist) >> 2);
  cod->dc = cod->dx ? ((cod->dx + 1) << 1) + (dist > dxmax[cod->dx]) : dist-1;
}
enum sdefl_blk_type {
  SDEFL_BLK_UCOMPR,
  SDEFL_BLK_DYN
};
static enum sdefl_blk_type
sdefl_blk_type(const struct sdefl *s, int blk_len, int pre_item_len,
               const unsigned *pre_freq, const unsigned char *pre_len) {
  static const unsigned char x_pre_bits[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};
  static const unsigned char x_len_bits[] = {0,0,0,0,0,0,0,0, 1,1,1,1,2,2,2,2,
    3,3,3,3,4,4,4,4, 5,5,5,5,0};
  static const unsigned char x_off_bits[] = {0,0,0,0,1,1,2,2, 3,3,4,4,5,5,6,6,
    7,7,8,8,9,9,10,10, 11,11,12,12,13,13};

  int dyn_cost = 0;
  int fix_cost = 0;
  int sym = 0;

  dyn_cost += 5 + 5 + 4 + (3 * pre_item_len);
  for (sym = 0; sym < SDEFL_PRE_MAX; sym++)
    dyn_cost += pre_freq[sym] * (x_pre_bits[sym] + pre_len[sym]);
  for (sym = 0; sym < 256; sym++)
    dyn_cost += s->freq.lit[sym] * s->cod.len.lit[sym];
  dyn_cost += s->cod.len.lit[SDEFL_EOB];
  for (sym = 257; sym < 286; sym++)
    dyn_cost += s->freq.lit[sym] * (x_len_bits[sym - 257] + s->cod.len.lit[sym]);
  for (sym = 0; sym < 30; sym++)
    dyn_cost += s->freq.off[sym] * (x_off_bits[sym] + s->cod.len.off[sym]);

  fix_cost += 8*(5 * sdefl_div_round_up(blk_len, SDEFL_RAW_BLK_SIZE) + blk_len + 1 + 2);
  return (dyn_cost < fix_cost) ? SDEFL_BLK_DYN : SDEFL_BLK_UCOMPR;
}
static void
sdefl_put16(unsigned char **dst, unsigned short x) {
  unsigned char *val = *dst;
  val[0] = (unsigned char)(x & 0xff);
  val[1] = (unsigned char)(x >> 8);
  *dst = val + 2;
}
static void
sdefl_match(unsigned char **dst, struct sdefl *s, int dist, int len) {
  static const char lxn[] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
  static const short lmin[] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,
      51,59,67,83,99,115,131,163,195,227,258};
  static const short dmin[] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,
      385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};

  struct sdefl_match_codest cod;
  sdefl_match_codes(&cod, dist, len);
  sdefl_put(dst, s, (int)s->cod.word.lit[cod.lc], s->cod.len.lit[cod.lc]);
  sdefl_put(dst, s, len - lmin[cod.ls], lxn[cod.ls]);
  sdefl_put(dst, s, (int)s->cod.word.off[cod.dc], s->cod.len.off[cod.dc]);
  sdefl_put(dst, s, dist - dmin[cod.dc], cod.dx);
}
static void
sdefl_flush(unsigned char **dst, struct sdefl *s, int is_last,
            const unsigned char *in, int blk_begin, int blk_end) {
  int blk_len = blk_end - blk_begin;
  int j, i = 0, item_cnt = 0;
  struct sdefl_symcnt symcnt = {0};
  unsigned codes[SDEFL_PRE_MAX];
  unsigned char lens[SDEFL_PRE_MAX];
  unsigned freqs[SDEFL_PRE_MAX] = {0};
  unsigned items[SDEFL_SYM_MAX + SDEFL_OFF_MAX];
  static const unsigned char perm[SDEFL_PRE_MAX] = {16,17,18,0,8,7,9,6,10,5,11,
      4,12,3,13,2,14,1,15};

  /* calculate huffman codes */
  s->freq.lit[SDEFL_EOB]++;
  sdefl_huff(s->cod.len.lit, s->cod.word.lit, s->freq.lit, SDEFL_SYM_MAX, SDEFL_LIT_LEN_CODES);
  sdefl_huff(s->cod.len.off, s->cod.word.off, s->freq.off, SDEFL_OFF_MAX, SDEFL_OFF_CODES);
  sdefl_precode(&symcnt, freqs, items, s->cod.len.lit, s->cod.len.off);
  sdefl_huff(lens, codes, freqs, SDEFL_PRE_MAX, SDEFL_PRE_CODES);
  for (item_cnt = SDEFL_PRE_MAX; item_cnt > 4; item_cnt--) {
    if (lens[perm[item_cnt - 1]]){
      break;
    }
  }
  /* write block */
  switch (sdefl_blk_type(s, blk_len, item_cnt, freqs, lens)) {
  case SDEFL_BLK_UCOMPR: {
    /* uncompressed blocks */
    int n = sdefl_div_round_up(blk_len, SDEFL_RAW_BLK_SIZE);
    for (i = 0; i < n; ++i) {
      int fin = is_last && (i + 1 == n);
      int amount = blk_len < SDEFL_RAW_BLK_SIZE ? blk_len : SDEFL_RAW_BLK_SIZE;
      sdefl_put(dst, s, !!fin, 1); /* block */
      sdefl_put(dst, s, 0x00, 2); /* stored block */
      if (s->bitcnt) {
        sdefl_put(dst, s, 0x00, 8 - s->bitcnt);
      }
      assert(s->bitcnt == 0);
      sdefl_put16(dst, (unsigned short)amount);
      sdefl_put16(dst, ~(unsigned short)amount);
      memcpy(*dst, in + blk_begin + i * SDEFL_RAW_BLK_SIZE, amount);
      *dst = *dst + amount;
      blk_len -= amount;
    }
  } break;
  case SDEFL_BLK_DYN: {
    /* dynamic huffman block */
    sdefl_put(dst, s, !!is_last, 1); /* block */
    sdefl_put(dst, s, 0x02, 2); /* dynamic huffman */
    sdefl_put(dst, s, symcnt.lit - 257, 5);
    sdefl_put(dst, s, symcnt.off - 1, 5);
    sdefl_put(dst, s, item_cnt - 4, 4);
    for (i = 0; i < item_cnt; ++i) {
      sdefl_put(dst, s, lens[perm[i]], 3);
    }
    for (i = 0; i < symcnt.items; ++i) {
      unsigned sym = items[i] & 0x1F;
      sdefl_put(dst, s, (int)codes[sym], lens[sym]);
      if (sym < 16) continue;
      if (sym == 16) sdefl_put(dst, s, items[i] >> 5, 2);
      else if(sym == 17) sdefl_put(dst, s, items[i] >> 5, 3);
      else sdefl_put(dst, s, items[i] >> 5, 7);
    }
    /* block sequences */
    for (i = 0; i < s->seq_cnt; ++i) {
      if (s->seq[i].off >= 0) {
        for (j = 0; j < s->seq[i].len; ++j) {
          int c = in[s->seq[i].off + j];
          sdefl_put(dst, s, (int)s->cod.word.lit[c], s->cod.len.lit[c]);
        }
      } else {
        sdefl_match(dst, s, -s->seq[i].off, s->seq[i].len);
      }
    }
    sdefl_put(dst, s, (int)(s)->cod.word.lit[SDEFL_EOB], (s)->cod.len.lit[SDEFL_EOB]);
  } break;}
  memset(&s->freq, 0, sizeof(s->freq));
  s->seq_cnt = 0;
}
static void
sdefl_seq(struct sdefl *s, int off, int len) {
  assert(s->seq_cnt + 2 < SDEFL_SEQ_SIZ);
  s->seq[s->seq_cnt].off = off;
  s->seq[s->seq_cnt].len = len;
  s->seq_cnt++;
}
static void
sdefl_reg_match(struct sdefl *s, int off, int len) {
  struct sdefl_match_codest cod;
  sdefl_match_codes(&cod, off, len);

  assert(cod.lc < SDEFL_SYM_MAX);
  assert(cod.dc < SDEFL_OFF_MAX);

  s->freq.lit[cod.lc]++;
  s->freq.off[cod.dc]++;
}
struct sdefl_match {
  int off;
  int len;
};
static void
sdefl_fnd(struct sdefl_match *m, const struct sdefl *s, int chain_len,
          int max_match, const unsigned char *in, int p, int e) {
  int i = s->tbl[sdefl_hash32(in + p)];
  int limit = ((p - SDEFL_WIN_SIZ) < SDEFL_NIL) ? SDEFL_NIL : (p-SDEFL_WIN_SIZ);

  assert(p < e);
  assert(p + max_match <= e);
  while (i > limit) {
    assert(i + m->len < e);
    assert(p + m->len < e);
    assert(i + SDEFL_MIN_MATCH < e);
    assert(p + SDEFL_MIN_MATCH < e);

    if (in[i + m->len] == in[p + m->len] &&
      (sdefl_uload32(&in[i]) == sdefl_uload32(&in[p]))) {
      int n = SDEFL_MIN_MATCH;
      while (n < max_match && in[i + n] == in[p + n]) {
        assert(i + n < e);
        assert(p + n < e);
        n++;
      }
      if (n > m->len) {
        m->len = n, m->off = p - i;
        if (n == max_match)
          break;
      }
    }
    if (!(--chain_len)) break;
    i = s->prv[i & SDEFL_WIN_MSK];
  }
}
static int
sdefl_compr(struct sdefl *s, unsigned char *out, const unsigned char *in,
            int in_len, int lvl) {
  unsigned char *q = out;
  static const unsigned char pref[] = {8,10,14,24,30,48,65,96,130};
  int max_chain = (lvl < 8) ? (1 << (lvl + 1)): (1 << 13);
  int n, i = 0, litlen = 0;
  for (n = 0; n < SDEFL_HASH_SIZ; ++n) {
    s->tbl[n] = SDEFL_NIL;
  }
  do {int blk_begin = i;
    int blk_end = ((i + SDEFL_BLK_MAX) < in_len) ? (i + SDEFL_BLK_MAX) : in_len;
    while (i < blk_end) {
      struct sdefl_match m = {0};
      int left = blk_end - i;
      int max_match = (left > SDEFL_MAX_MATCH) ? SDEFL_MAX_MATCH : left;
      int nice_match = pref[lvl] < max_match ? pref[lvl] : max_match;
      int run = 1, inc = 1, run_inc = 0;
      if (max_match > SDEFL_MIN_MATCH) {
        sdefl_fnd(&m, s, max_chain, max_match, in, i, in_len);
      }
      if (lvl >= 5 && m.len >= SDEFL_MIN_MATCH && m.len + 1 < nice_match){
        struct sdefl_match m2 = {0};
        sdefl_fnd(&m2, s, max_chain, m.len + 1, in, i + 1, in_len);
        m.len = (m2.len > m.len) ? 0 : m.len;
      }
      if (m.len >= SDEFL_MIN_MATCH) {
        if (litlen) {
          sdefl_seq(s, i - litlen, litlen);
          litlen = 0;
        }
        sdefl_seq(s, -m.off, m.len);
        sdefl_reg_match(s, m.off, m.len);
        if (lvl < 2 && m.len >= nice_match) {
          inc = m.len;
        } else {
          run = m.len;
        }
      } else {
        s->freq.lit[in[i]]++;
        litlen++;
      }
      run_inc = run * inc;
      if (in_len - (i + run_inc) > SDEFL_MIN_MATCH) {
        while (run-- > 0) {
          unsigned h = sdefl_hash32(&in[i]);
          s->prv[i&SDEFL_WIN_MSK] = s->tbl[h];
          s->tbl[h] = i, i += inc;
          assert(i <= blk_end);
        }
      } else {
        i += run_inc;
        assert(i <= blk_end);
      }
    }
    if (litlen) {
      sdefl_seq(s, i - litlen, litlen);
      litlen = 0;
    }
    sdefl_flush(&q, s, blk_end == in_len, in, blk_begin, blk_end);
  } while (i < in_len);
  if (s->bitcnt) {
    sdefl_put(&q, s, 0x00, 8 - s->bitcnt);
  }
  assert(s->bitcnt == 0);
  return (int)(q - out);
}
extern int
sdeflate(struct sdefl *s, void *out, const void *in, int n, int lvl) {
  s->bits = s->bitcnt = 0;
  return sdefl_compr(s, (unsigned char*)out, (const unsigned char*)in, n, lvl);
}
static unsigned
sdefl_adler32(unsigned adler32, const unsigned char *in, int in_len) {
  #define SDEFL_ADLER_INIT (1)
  const unsigned ADLER_MOD = 65521;
  unsigned s1 = adler32 & 0xffff;
  unsigned s2 = adler32 >> 16;
  unsigned blk_len, i;

  blk_len = in_len % 5552;
  while (in_len) {
    for (i = 0; i + 7 < blk_len; i += 8) {
      s1 += in[0]; s2 += s1;
      s1 += in[1]; s2 += s1;
      s1 += in[2]; s2 += s1;
      s1 += in[3]; s2 += s1;
      s1 += in[4]; s2 += s1;
      s1 += in[5]; s2 += s1;
      s1 += in[6]; s2 += s1;
      s1 += in[7]; s2 += s1;
      in += 8;
    }
    for (; i < blk_len; ++i) {
      s1 += *in++, s2 += s1;
    }
    s1 %= ADLER_MOD;
    s2 %= ADLER_MOD;
    in_len -= blk_len;
    blk_len = 5552;
  }
  return (unsigned)(s2 << 16) + (unsigned)s1;
}
extern int
zsdeflate(struct sdefl *s, void *out, const void *in, int n, int lvl) {
  int p = 0;
  unsigned a = 0;
  unsigned char *q = (unsigned char*)out;

  s->bits = s->bitcnt = 0;
  sdefl_put(&q, s, 0x78, 8); /* deflate, 32k window */
  sdefl_put(&q, s, 0x01, 8); /* fast compression */
  q += sdefl_compr(s, q, (const unsigned char*)in, n, lvl);

  /* append adler checksum */
  a = sdefl_adler32(SDEFL_ADLER_INIT, (const unsigned char*)in, n);
  for (p = 0; p < 4; ++p) {
    sdefl_put(&q, s, (a >> 24) & 0xFF, 8);
    a <<= 8;
  }
  return (int)(q - (unsigned char*)out);
}
extern int
sdefl_bound(int len) {
  int max_blocks = 1 + sdefl_div_round_up(len, SDEFL_RAW_BLK_SIZE);
  int bound = 5 * max_blocks + len + 1 + 4 + 8;
  return bound;
}
#endif /* SDEFL_IMPLEMENTATION */


//=========================================================================
//                           SINFL
// DEFLATE DECOMPRESSION algorithm: https://github.com/vurtun/lib/sinfl.h
//=========================================================================
#ifdef SINFL_IMPLEMENTATION

#include <string.h> /* memcpy, memset */
#include <assert.h> /* assert */

#if defined(__GNUC__) || defined(__clang__)
#define sinfl_likely(x)       __builtin_expect((x),1)
#define sinfl_unlikely(x)     __builtin_expect((x),0)
#else
#define sinfl_likely(x)       (x)
#define sinfl_unlikely(x)     (x)
#endif

#ifndef SINFL_NO_SIMD
#if defined(__x86_64__) || defined(_WIN32) || defined(_WIN64)
  #include <emmintrin.h>
  #define sinfl_char16 __m128i
  #define sinfl_char16_ld(p) _mm_loadu_si128((const __m128i *)(void*)(p))
  #define sinfl_char16_str(d,v)  _mm_storeu_si128((__m128i*)(void*)(d), v)
  #define sinfl_char16_char(c) _mm_set1_epi8(c)
#elif defined(__arm__) || defined(__aarch64__)
  #include <arm_neon.h>
  #define sinfl_char16 uint8x16_t
  #define sinfl_char16_ld(p) vld1q_u8((const unsigned char*)(p))
  #define sinfl_char16_str(d,v) vst1q_u8((unsigned char*)(d), v)
  #define sinfl_char16_char(c) vdupq_n_u8(c)
#else
  #define SINFL_NO_SIMD
#endif
#endif

static int
sinfl_bsr(unsigned n) {
#ifdef _MSC_VER
  _BitScanReverse(&n, n);
  return n;
#elif defined(__GNUC__) || defined(__clang__)
  return 31 - __builtin_clz(n);
#endif
}
static unsigned long long
sinfl_read64(const void *p) {
  unsigned long long n;
  memcpy(&n, p, 8);
  return n;
}
static void
sinfl_copy64(unsigned char **dst, unsigned char **src) {
  unsigned long long n;
  memcpy(&n, *src, 8);
  memcpy(*dst, &n, 8);
  *dst += 8, *src += 8;
}
static unsigned char*
sinfl_write64(unsigned char *dst, unsigned long long w) {
  memcpy(dst, &w, 8);
  return dst + 8;
}
#ifndef SINFL_NO_SIMD
static unsigned char*
sinfl_write128(unsigned char *dst, sinfl_char16 w) {
  sinfl_char16_str(dst, w);
  return dst + 8;
}
static void
sinfl_copy128(unsigned char **dst, unsigned char **src) {
  sinfl_char16 n = sinfl_char16_ld(*src);
  sinfl_char16_str(*dst, n);
  *dst += 16, *src += 16;
}
#endif
static void
sinfl_refill(struct sinfl *s) {
  s->bitbuf |= sinfl_read64(s->bitptr) << s->bitcnt;
  s->bitptr += (63 - s->bitcnt) >> 3;
  s->bitcnt |= 56; /* bitcount in range [56,63] */
}
static int
sinfl_peek(struct sinfl *s, int cnt) {
  assert(cnt >= 0 && cnt <= 56);
  assert(cnt <= s->bitcnt);
  return s->bitbuf & ((1ull << cnt) - 1);
}
static void
sinfl_eat(struct sinfl *s, int cnt) {
  assert(cnt <= s->bitcnt);
  s->bitbuf >>= cnt;
  s->bitcnt -= cnt;
}
static int
sinfl__get(struct sinfl *s, int cnt) {
  int res = sinfl_peek(s, cnt);
  sinfl_eat(s, cnt);
  return res;
}
static int
sinfl_get(struct sinfl *s, int cnt) {
  sinfl_refill(s);
  return sinfl__get(s, cnt);
}
struct sinfl_gen {
  int len;
  int cnt;
  int word;
  short* sorted;
};
static int
sinfl_build_tbl(struct sinfl_gen *gen, unsigned *tbl, int tbl_bits,
                const int *cnt) {
  int tbl_end = 0;
  while (!(gen->cnt = cnt[gen->len])) {
    ++gen->len;
  }
  tbl_end = 1 << gen->len;
  while (gen->len <= tbl_bits) {
    do {unsigned bit = 0;
      tbl[gen->word] = (*gen->sorted++ << 16) | gen->len;
      if (gen->word == tbl_end - 1) {
        for (; gen->len < tbl_bits; gen->len++) {
          memcpy(&tbl[tbl_end], tbl, (size_t)tbl_end * sizeof(tbl[0]));
          tbl_end <<= 1;
        }
        return 1;
      }
      bit = 1 << sinfl_bsr((unsigned)(gen->word ^ (tbl_end - 1)));
      gen->word &= bit - 1;
      gen->word |= bit;
    } while (--gen->cnt);
    do {
      if (++gen->len <= tbl_bits) {
        memcpy(&tbl[tbl_end], tbl, (size_t)tbl_end * sizeof(tbl[0]));
        tbl_end <<= 1;
      }
    } while (!(gen->cnt = cnt[gen->len]));
  }
  return 0;
}
static void
sinfl_build_subtbl(struct sinfl_gen *gen, unsigned *tbl, int tbl_bits,
                   const int *cnt) {
  int sub_bits = 0;
  int sub_start = 0;
  int sub_prefix = -1;
  int tbl_end = 1 << tbl_bits;
  while (1) {
    unsigned entry;
    int bit, stride, i;
    /* start new sub-table */
    if ((gen->word & ((1 << tbl_bits)-1)) != sub_prefix) {
      int used = 0;
      sub_prefix = gen->word & ((1 << tbl_bits)-1);
      sub_start = tbl_end;
      sub_bits = gen->len - tbl_bits;
      used = gen->cnt;
      while (used < (1 << sub_bits)) {
        sub_bits++;
        used = (used << 1) + cnt[tbl_bits + sub_bits];
      }
      tbl_end = sub_start + (1 << sub_bits);
      tbl[sub_prefix] = (sub_start << 16) | 0x10 | (sub_bits & 0xf);
    }
    /* fill sub-table */
    entry = (*gen->sorted << 16) | ((gen->len - tbl_bits) & 0xf);
    gen->sorted++;
    i = sub_start + (gen->word >> tbl_bits);
    stride = 1 << (gen->len - tbl_bits);
    do {
      tbl[i] = entry;
      i += stride;
    } while (i < tbl_end);
    if (gen->word == (1 << gen->len)-1) {
      return;
    }
    bit = 1 << sinfl_bsr(gen->word ^ ((1 << gen->len) - 1));
    gen->word &= bit - 1;
    gen->word |= bit;
    gen->cnt--;
    while (!gen->cnt) {
      gen->cnt = cnt[++gen->len];
    }
  }
}
static void
sinfl_build(unsigned *tbl, unsigned char *lens, int tbl_bits, int maxlen,
            int symcnt) {
  int i, used = 0;
  short sort[288];
  int cnt[16] = {0}, off[16]= {0};
  struct sinfl_gen gen = {0};
  gen.sorted = sort;
  gen.len = 1;

  for (i = 0; i < symcnt; ++i)
    cnt[lens[i]]++;
  off[1] = cnt[0];
  for (i = 1; i < maxlen; ++i) {
    off[i + 1] = off[i] + cnt[i];
    used = (used << 1) + cnt[i];
  }
  used = (used << 1) + cnt[i];
  for (i = 0; i < symcnt; ++i)
    gen.sorted[off[lens[i]]++] = (short)i;
  gen.sorted += off[0];

  if (used < (1 << maxlen)){
    for (i = 0; i < 1 << tbl_bits; ++i)
      tbl[i] = (0 << 16u) | 1;
    return;
  }
  if (!sinfl_build_tbl(&gen, tbl, tbl_bits, cnt)){
    sinfl_build_subtbl(&gen, tbl, tbl_bits, cnt);
  }
}
static int
sinfl_decode(struct sinfl *s, const unsigned *tbl, int bit_len) {
  int idx = sinfl_peek(s, bit_len);
  unsigned key = tbl[idx];
  if (key & 0x10) {
    /* sub-table lookup */
    int len = key & 0x0f;
    sinfl_eat(s, bit_len);
    idx = sinfl_peek(s, len);
    key = tbl[((key >> 16) & 0xffff) + (unsigned)idx];
  }
  sinfl_eat(s, key & 0x0f);
  return (key >> 16) & 0x0fff;
}
static int
sinfl_decompress(unsigned char *out, int cap, const unsigned char *in, int size) {
  static const unsigned char order[] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
  static const short dbase[30+2] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
      257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
  static const unsigned char dbits[30+2] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,
      10,10,11,11,12,12,13,13,0,0};
  static const short lbase[29+2] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,
      43,51,59,67,83,99,115,131,163,195,227,258,0,0};
  static const unsigned char lbits[29+2] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,
      4,4,4,5,5,5,5,0,0,0};

  const unsigned char *oe = out + cap;
  const unsigned char *e = in + size, *o = out;
  enum sinfl_states {hdr,stored,fixed,dyn,blk};
  enum sinfl_states state = hdr;
  struct sinfl s = {0};
  int last = 0;

  s.bitptr = in;
  while (1) {
    switch (state) {
    case hdr: {
      /* block header */
      int type = 0;
      sinfl_refill(&s);
      last = sinfl__get(&s,1);
      type = sinfl__get(&s,2);

      switch (type) {default: return (int)(out-o);
      case 0x00: state = stored; break;
      case 0x01: state = fixed; break;
      case 0x02: state = dyn; break;}
    } break;
    case stored: {
      /* uncompressed block */
      unsigned len, nlen;
      sinfl__get(&s,s.bitcnt & 7);
      len = (unsigned short)sinfl__get(&s,16);
      nlen = (unsigned short)sinfl__get(&s,16);
      s.bitptr -= s.bitcnt / 8;
      s.bitbuf = s.bitcnt = 0;

      if ((unsigned short)len != (unsigned short)~nlen)
        return (int)(out-o);
      if (len > (e - s.bitptr) || !len)
        return (int)(out-o);

      memcpy(out, s.bitptr, (size_t)len);
      s.bitptr += len, out += len;
      if (last) return (int)(out-o);
      state = hdr;
    } break;
    case fixed: {
      /* fixed huffman codes */
      int n; unsigned char lens[288+32];
      for (n = 0; n <= 143; n++) lens[n] = 8;
      for (n = 144; n <= 255; n++) lens[n] = 9;
      for (n = 256; n <= 279; n++) lens[n] = 7;
      for (n = 280; n <= 287; n++) lens[n] = 8;
      for (n = 0; n < 32; n++) lens[288+n] = 5;

      /* build lit/dist tables */
      sinfl_build(s.lits, lens, 10, 15, 288);
      sinfl_build(s.dsts, lens + 288, 8, 15, 32);
      state = blk;
    } break;
    case dyn: {
      /* dynamic huffman codes */
      int n, i;
      unsigned hlens[SINFL_PRE_TBL_SIZE];
      unsigned char nlens[19] = {0}, lens[288+32];

      sinfl_refill(&s);
      {int nlit = 257 + sinfl__get(&s,5);
      int ndist = 1 + sinfl__get(&s,5);
      int nlen = 4 + sinfl__get(&s,4);
      for (n = 0; n < nlen; n++)
        nlens[order[n]] = (unsigned char)sinfl_get(&s,3);
      sinfl_build(hlens, nlens, 7, 7, 19);

      /* decode code lengths */
      for (n = 0; n < nlit + ndist;) {
        int sym = 0;
        sinfl_refill(&s);
        sym = sinfl_decode(&s, hlens, 7);
        switch (sym) {default: lens[n++] = (unsigned char)sym; break;
        case 16: for (i=3+sinfl_get(&s,2);i;i--,n++) lens[n]=lens[n-1]; break;
        case 17: for (i=3+sinfl_get(&s,3);i;i--,n++) lens[n]=0; break;
        case 18: for (i=11+sinfl_get(&s,7);i;i--,n++) lens[n]=0; break;}
      }
      /* build lit/dist tables */
      sinfl_build(s.lits, lens, 10, 15, nlit);
      sinfl_build(s.dsts, lens + nlit, 8, 15, ndist);
      state = blk;}
    } break;
    case blk: {
      /* decompress block */
      while (1) {
        int sym;
        sinfl_refill(&s);
        sym = sinfl_decode(&s, s.lits, 10);
        if (sym < 256) {
          /* literal */
          if (sinfl_unlikely(out >= oe)) {
            return (int)(out-o);
          }
          *out++ = (unsigned char)sym;
          sym = sinfl_decode(&s, s.lits, 10);
          if (sym < 256) {
            *out++ = (unsigned char)sym;
            continue;
          }
        }
        if (sinfl_unlikely(sym == 256)) {
          /* end of block */
          if (last) return (int)(out-o);
          state = hdr;
          break;
        }
        /* match */
        if (sym >= 286) {
          /* length codes 286 and 287 must not appear in compressed data */
          return (int)(out-o);
        }
        sym -= 257;
        {int len = sinfl__get(&s, lbits[sym]) + lbase[sym];
        int dsym = sinfl_decode(&s, s.dsts, 8);
        int offs = sinfl__get(&s, dbits[dsym]) + dbase[dsym];
        unsigned char *dst = out, *src = out - offs;
        if (sinfl_unlikely(offs > (int)(out-o))) {
          return (int)(out-o);
        }
        out = out + len;

#ifndef SINFL_NO_SIMD
        if (sinfl_likely(oe - out >= 16 * 3)) {
          if (offs >= 16) {
            /* simd copy match */
            sinfl_copy128(&dst, &src);
            sinfl_copy128(&dst, &src);
            do sinfl_copy128(&dst, &src);
            while (dst < out);
          } else if (offs >= 8) {
            /* word copy match */
            sinfl_copy64(&dst, &src);
            sinfl_copy64(&dst, &src);
            do sinfl_copy64(&dst, &src);
            while (dst < out);
          } else if (offs == 1) {
            /* rle match copying */
            sinfl_char16 w = sinfl_char16_char(src[0]);
            dst = sinfl_write128(dst, w);
            dst = sinfl_write128(dst, w);
            do dst = sinfl_write128(dst, w);
            while (dst < out);
          } else {
            /* byte copy match */
            *dst++ = *src++;
            *dst++ = *src++;
            do *dst++ = *src++;
            while (dst < out);
          }
        }
#else
        if (sinfl_likely(oe - out >= 3 * 8 - 3)) {
          if (offs >= 8) {
            /* word copy match */
            sinfl_copy64(&dst, &src);
            sinfl_copy64(&dst, &src);
            do sinfl_copy64(&dst, &src);
            while (dst < out);
          } else if (offs == 1) {
            /* rle match copying */
            unsigned int c = src[0];
            unsigned int hw = (c << 24u) | (c << 16u) | (c << 8u) | (unsigned)c;
            unsigned long long w = (unsigned long long)hw << 32llu | hw;
            dst = sinfl_write64(dst, w);
            dst = sinfl_write64(dst, w);
            do dst = sinfl_write64(dst, w);
            while (dst < out);
          } else {
            /* byte copy match */
            *dst++ = *src++;
            *dst++ = *src++;
            do *dst++ = *src++;
            while (dst < out);
          }
        }
#endif
        else {
          *dst++ = *src++;
          *dst++ = *src++;
          do *dst++ = *src++;
          while (dst < out);
        }}
      }
    } break;}
  }
  return (int)(out-o);
}
extern int
sinflate(void *out, int cap, const void *in, int size) {
  return sinfl_decompress((unsigned char*)out, cap, (const unsigned char*)in, size);
}
static unsigned
sinfl_adler32(unsigned adler32, const unsigned char *in, int in_len) {
  const unsigned ADLER_MOD = 65521;
  unsigned s1 = adler32 & 0xffff;
  unsigned s2 = adler32 >> 16;
  unsigned blk_len, i;

  blk_len = in_len % 5552;
  while (in_len) {
    for (i=0; i + 7 < blk_len; i += 8) {
      s1 += in[0]; s2 += s1;
      s1 += in[1]; s2 += s1;
      s1 += in[2]; s2 += s1;
      s1 += in[3]; s2 += s1;
      s1 += in[4]; s2 += s1;
      s1 += in[5]; s2 += s1;
      s1 += in[6]; s2 += s1;
      s1 += in[7]; s2 += s1;
      in += 8;
    }
    for (; i < blk_len; ++i)
      s1 += *in++, s2 += s1;
    s1 %= ADLER_MOD; s2 %= ADLER_MOD;
    in_len -= blk_len;
    blk_len = 5552;
  } return (unsigned)(s2 << 16) + (unsigned)s1;
}
extern int
zsinflate(void *out, int cap, const void *mem, int size) {
  const unsigned char *in = (const unsigned char*)mem;
  if (size >= 6) {
    const unsigned char *eob = in + size - 4;
    int n = sinfl_decompress((unsigned char*)out, cap, in + 2u, size);
    unsigned a = sinfl_adler32(1u, (unsigned char*)out, n);
    unsigned h = eob[0] << 24 | eob[1] << 16 | eob[2] << 8 | eob[3] << 0;
    return a == h ? n : -1;
  } else {
    return -1;
  }
}

#endif  /* SINFL_IMPLEMENTATION */

/*
# Small Deflate
`sdefl` is a small bare bone lossless compression library in ANSI C (ISO C90)
which implements the Deflate (RFC 1951) compressed data format specification standard.
It is mainly tuned to get as much speed and compression ratio from as little code
as needed to keep the implementation as concise as possible.

## Features
- Portable single header and source file duo written in ANSI C (ISO C90)
- Dual license with either MIT or public domain
- Small implementation
    - Deflate: 525 LoC
    - Inflate: 500 LoC
- Webassembly:
    - Deflate ~3.7 KB (~2.2KB compressed)
    - Inflate ~3.6 KB (~2.2KB compressed)

## Usage:
This file behaves differently depending on what symbols you define
before including it.

Header-File mode:
If you do not define `SINFL_IMPLEMENTATION` before including this file, it
will operate in header only mode. In this mode it declares all used structs
and the API of the library without including the implementation of the library.

Implementation mode:
If you define `SINFL_IMPLEMENTATION` before including this file, it will
compile the implementation. Make sure that you only include
this file implementation in *one* C or C++ file to prevent collisions.

### Benchmark

| Compressor name         | Compression| Decompress.| Compr. size | Ratio |
| ------------------------| -----------| -----------| ----------- | ----- |
| miniz 1.0 -1            |   122 MB/s |   208 MB/s |    48510028 | 48.51 |
| miniz 1.0 -6            |    27 MB/s |   260 MB/s |    36513697 | 36.51 |
| miniz 1.0 -9            |    23 MB/s |   261 MB/s |    36460101 | 36.46 |
| zlib 1.2.11 -1          |    72 MB/s |   307 MB/s |    42298774 | 42.30 |
| zlib 1.2.11 -6          |    24 MB/s |   313 MB/s |    36548921 | 36.55 |
| zlib 1.2.11 -9          |    20 MB/s |   314 MB/s |    36475792 | 36.48 |
| sdefl 1.0 -0            |   127 MB/s |   355 MB/s |    40004116 | 39.88 |
| sdefl 1.0 -1            |   111 MB/s |   413 MB/s |    38940674 | 38.82 |
| sdefl 1.0 -5            |    45 MB/s |   436 MB/s |    36577183 | 36.46 |
| sdefl 1.0 -7            |    38 MB/s |   432 MB/s |    36523781 | 36.41 |
| libdeflate 1.3 -1       |   147 MB/s |   667 MB/s |    39597378 | 39.60 |
| libdeflate 1.3 -6       |    69 MB/s |   689 MB/s |    36648318 | 36.65 |
| libdeflate 1.3 -9       |    13 MB/s |   672 MB/s |    35197141 | 35.20 |
| libdeflate 1.3 -12      |  8.13 MB/s |   670 MB/s |    35100568 | 35.10 |

### Compression
Results on the [Silesia compression corpus](http://sun.aei.polsl.pl/~sdeor/index.php?page=silesia):

| File    |   Original | `sdefl 0`    | `sdefl 5`  | `sdefl 7`   |
| --------| -----------| -------------| ---------- | ------------|
| dickens | 10.192.446 | 4,260,187    |  3,845,261 |   3,833,657 |
| mozilla | 51.220.480 | 20,774,706   | 19,607,009 |  19,565,867 |
| mr      |  9.970.564 | 3,860,531    |  3,673,460 |   3,665,627 |
| nci     | 33.553.445 | 4,030,283    |  3,094,526 |   3,006,075 |
| ooffice |  6.152.192 | 3,320,063    |  3,186,373 |   3,183,815 |
| osdb    | 10.085.684 | 3,919,646    |  3,649,510 |   3,649,477 |
| reymont |  6.627.202 | 2,263,378    |  1,857,588 |   1,827,237 |
| samba   | 21.606.400 | 6,121,797    |  5,462,670 |   5,450,762 |
| sao     |  7.251.944 | 5,612,421    |  5,485,380 |   5,481,765 |
| webster | 41.458.703 | 13,972,648   | 12,059,432 |  11,991,421 |
| xml     |  5.345.280 | 886,620      |    674,009 |     662,141 |
| x-ray   |  8.474.240 | 6,304,655    |  6,244,779 |   6,244,779 |

## License
```
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2020 Micha Mettke
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
```
*/
#endif  // RPNG_DEFLATE_IMPLEMENTATION

#endif  // RPNG_IMPLEMENTATION
