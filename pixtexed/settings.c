/*
pixtexed - pixel art editor

Written in 2024 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

//External includes
#include <stdlib.h>
//-------------------------------------

//Internal includes
#include "settings.h"
#include "shared/color.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
static uint8_t color_closest(const Settings *settings, uint8_t r, uint8_t g, uint8_t b);
//-------------------------------------

//Function implementations

Settings *settings_init()
{
   Settings *s = calloc(1,sizeof(*s));

   //Palette
   //-------------------------------------
   s->palette_colors = 256;
   s->palette[0] = 0xff000000;
   s->palette[1] = 0xff111111;
   s->palette[2] = 0xff222222;
   s->palette[3] = 0xff333333;
   s->palette[4] = 0xff444444;
   s->palette[5] = 0xff555555;
   s->palette[6] = 0xff666666;
   s->palette[7] = 0xff777777;
   s->palette[8] = 0xff888888;
   s->palette[9] = 0xff999999;
   s->palette[10] = 0xffaaaaaa;
   s->palette[11] = 0xffbbbbbb;
   s->palette[12] = 0xffcccccc;
   s->palette[13] = 0xffdddddd;
   s->palette[14] = 0xffeeeeee;
   s->palette[15] = 0xffffffff;
   s->palette[16] = 0xff7f7f00;
   s->palette[17] = 0xffbfbf3f;
   s->palette[18] = 0xffffff00;
   s->palette[19] = 0xffffffbf;
   s->palette[20] = 0xffff8181;
   s->palette[21] = 0xffff0000;
   s->palette[22] = 0xffbf3f3f;
   s->palette[23] = 0xff7f0000;
   s->palette[24] = 0xff500f0f;
   s->palette[25] = 0xff7f007f;
   s->palette[26] = 0xffbf3fbf;
   s->palette[27] = 0xfff500f5;
   s->palette[28] = 0xffff81fd;
   s->palette[29] = 0xffcbc0ff;
   s->palette[30] = 0xff8181ff;
   s->palette[31] = 0xff0000ff;
   s->palette[32] = 0xff3f3fbf;
   s->palette[33] = 0xff00007f;
   s->palette[34] = 0xff141455;
   s->palette[35] = 0xff003f7f;
   s->palette[36] = 0xff3f7fbf;
   s->palette[37] = 0xff007fff;
   s->palette[38] = 0xff81bfff;
   s->palette[39] = 0xffbfffff;
   s->palette[40] = 0xff00ffff;
   s->palette[41] = 0xff3fbfbf;
   s->palette[42] = 0xff007f7f;
   s->palette[43] = 0xff007f00;
   s->palette[44] = 0xff3fbf3f;
   s->palette[45] = 0xff00ff00;
   s->palette[46] = 0xffafffaf;
   s->palette[47] = 0xffffbf00;
   s->palette[48] = 0xffff7f00;
   s->palette[49] = 0xffc87d4b;
   s->palette[50] = 0xffc0afbc;
   s->palette[51] = 0xff89aacb;
   s->palette[52] = 0xff90a0a6;
   s->palette[53] = 0xff94947e;
   s->palette[54] = 0xff87826e;
   s->palette[55] = 0xff606e7e;
   s->palette[56] = 0xff5f69a0;
   s->palette[57] = 0xff7278c0;
   s->palette[58] = 0xff748ad0;
   s->palette[59] = 0xff7d9be1;
   s->palette[60] = 0xff8caaeb;
   s->palette[61] = 0xff9bb9f5;
   s->palette[62] = 0xffafc8f6;
   s->palette[63] = 0xffd2e1f5;
   s->palette[64] = 0xffff007f;
   s->palette[65] = 0xff3b3b57;
   s->palette[66] = 0xff3c4173;
   s->palette[67] = 0xff55558e;
   s->palette[68] = 0xff7373ab;
   s->palette[69] = 0xff8f8fc7;
   s->palette[70] = 0xffababe3;
   s->palette[71] = 0xffdad2f8;
   s->palette[72] = 0xffabc7e3;
   s->palette[73] = 0xff739ec4;
   s->palette[74] = 0xff57738f;
   s->palette[75] = 0xff3b5773;
   s->palette[76] = 0xff1f2d3b;
   s->palette[77] = 0xff234141;
   s->palette[78] = 0xff3b7373;
   s->palette[79] = 0xff578f8f;
   s->palette[80] = 0xff55a2a2;
   s->palette[81] = 0xff72b5b5;
   s->palette[82] = 0xff8fc7c7;
   s->palette[83] = 0xffabdada;
   s->palette[84] = 0xffc7eded;
   s->palette[85] = 0xffabe3c7;
   s->palette[86] = 0xff8fc7ab;
   s->palette[87] = 0xff55be8e;
   s->palette[88] = 0xff578f73;
   s->palette[89] = 0xff3e7d58;
   s->palette[90] = 0xff325046;
   s->palette[91] = 0xff0f1e19;
   s->palette[92] = 0xff375023;
   s->palette[93] = 0xff3b573b;
   s->palette[94] = 0xff506450;
   s->palette[95] = 0xff49733b;
   s->palette[96] = 0xff578f57;
   s->palette[97] = 0xff73ab73;
   s->palette[98] = 0xff82c064;
   s->palette[99] = 0xff8fc78f;
   s->palette[100] = 0xffa2d8a2;
   s->palette[101] = 0xfffaf8e1;
   s->palette[102] = 0xffcaeeb4;
   s->palette[103] = 0xffc5e3ab;
   s->palette[104] = 0xff8eb487;
   s->palette[105] = 0xff5f7d50;
   s->palette[106] = 0xff46690f;
   s->palette[107] = 0xff232d1e;
   s->palette[108] = 0xff464123;
   s->palette[109] = 0xff73733b;
   s->palette[110] = 0xffabab64;
   s->palette[111] = 0xffc7c78f;
   s->palette[112] = 0xffe3e3ab;
   s->palette[113] = 0xfff1f1c7;
   s->palette[114] = 0xfff0d2be;
   s->palette[115] = 0xffe3c7ab;
   s->palette[116] = 0xffdcb9a8;
   s->palette[117] = 0xffc7ab8f;
   s->palette[118] = 0xffc78f57;
   s->palette[119] = 0xff8f7357;
   s->palette[120] = 0xff73573b;
   s->palette[121] = 0xff2d190f;
   s->palette[122] = 0xff3b1f1f;
   s->palette[123] = 0xff573b3b;
   s->palette[124] = 0xff734949;
   s->palette[125] = 0xff8f5757;
   s->palette[126] = 0xffaa6e73;
   s->palette[127] = 0xffca7676;
   s->palette[128] = 0xffc78f8f;
   s->palette[129] = 0xffe3abab;
   s->palette[130] = 0xfff8dad0;
   s->palette[131] = 0xffffe3e3;
   s->palette[132] = 0xffc78fab;
   s->palette[133] = 0xffc7578f;
   s->palette[134] = 0xff8f5773;
   s->palette[135] = 0xff733b57;
   s->palette[136] = 0xff3c233c;
   s->palette[137] = 0xff463246;
   s->palette[138] = 0xff724072;
   s->palette[139] = 0xff8f578f;
   s->palette[140] = 0xffab57ab;
   s->palette[141] = 0xffab73ab;
   s->palette[142] = 0xffe1aceb;
   s->palette[143] = 0xfff5dcff;
   s->palette[144] = 0xffe3c7e3;
   s->palette[145] = 0xffd2b9e1;
   s->palette[146] = 0xffbea0d7;
   s->palette[147] = 0xffb98fc7;
   s->palette[148] = 0xffa07dc8;
   s->palette[149] = 0xff915ac3;
   s->palette[150] = 0xff37284b;
   s->palette[151] = 0xff231632;
   s->palette[152] = 0xff1e0a28;
   s->palette[153] = 0xff111840;
   s->palette[154] = 0xff001862;
   s->palette[155] = 0xff0a14a5;
   s->palette[156] = 0xff1020da;
   s->palette[157] = 0xff4a52d5;
   s->palette[158] = 0xff0a3cff;
   s->palette[159] = 0xff325af5;
   s->palette[160] = 0xff6262ff;
   s->palette[161] = 0xff31bdf6;
   s->palette[162] = 0xff3ca5ff;
   s->palette[163] = 0xff0f9bd7;
   s->palette[164] = 0xff0a6eda;
   s->palette[165] = 0xff005ab4;
   s->palette[166] = 0xff054ba0;
   s->palette[167] = 0xff14325f;
   s->palette[168] = 0xff0a5053;
   s->palette[169] = 0xff006262;
   s->palette[170] = 0xff5a808c;
   s->palette[171] = 0xff0094ac;
   s->palette[172] = 0xff0ab1b1;
   s->palette[173] = 0xff5ad5e6;
   s->palette[174] = 0xff10d5ff;
   s->palette[175] = 0xff4aeaff;
   s->palette[176] = 0xff41ffc8;
   s->palette[177] = 0xff46f09b;
   s->palette[178] = 0xff19dc96;
   s->palette[179] = 0xff05c873;
   s->palette[180] = 0xff05a86a;
   s->palette[181] = 0xff146e3c;
   s->palette[182] = 0xff053428;
   s->palette[183] = 0xff084620;
   s->palette[184] = 0xff0c5c0c;
   s->palette[185] = 0xff059614;
   s->palette[186] = 0xff0ad70a;
   s->palette[187] = 0xff0ae614;
   s->palette[188] = 0xff73ff7d;
   s->palette[189] = 0xff5af04b;
   s->palette[190] = 0xff14c500;
   s->palette[191] = 0xff50b405;
   s->palette[192] = 0xff4e8c1c;
   s->palette[193] = 0xff323812;
   s->palette[194] = 0xff809812;
   s->palette[195] = 0xff91c406;
   s->palette[196] = 0xff6ade00;
   s->palette[197] = 0xffa8eb2d;
   s->palette[198] = 0xffa5fe3c;
   s->palette[199] = 0xffcdff6a;
   s->palette[200] = 0xffffeb91;
   s->palette[201] = 0xffffe655;
   s->palette[202] = 0xfff0d77d;
   s->palette[203] = 0xffd5de08;
   s->palette[204] = 0xffde9c10;
   s->palette[205] = 0xff5c5a05;
   s->palette[206] = 0xff522c16;
   s->palette[207] = 0xff7d370f;
   s->palette[208] = 0xff9c4a00;
   s->palette[209] = 0xff966432;
   s->palette[210] = 0xfff65200;
   s->palette[211] = 0xffbd6a18;
   s->palette[212] = 0xffdc7823;
   s->palette[213] = 0xffc39d69;
   s->palette[214] = 0xffffa44a;
   s->palette[215] = 0xffffb090;
   s->palette[216] = 0xffffc55a;
   s->palette[217] = 0xfffab9be;
   s->palette[218] = 0xfff06e78;
   s->palette[219] = 0xffff5a4a;
   s->palette[220] = 0xfff64162;
   s->palette[221] = 0xfff53c3c;
   s->palette[222] = 0xffda1c10;
   s->palette[223] = 0xffbd1000;
   s->palette[224] = 0xff941023;
   s->palette[225] = 0xff48210c;
   s->palette[226] = 0xffb01050;
   s->palette[227] = 0xffd01060;
   s->palette[228] = 0xffd23287;
   s->palette[229] = 0xffff419c;
   s->palette[230] = 0xffff62bd;
   s->palette[231] = 0xffff91b9;
   s->palette[232] = 0xffffa5d7;
   s->palette[233] = 0xfffac3d7;
   s->palette[234] = 0xfffcc6f8;
   s->palette[235] = 0xffff73e6;
   s->palette[236] = 0xffff52ff;
   s->palette[237] = 0xffe020da;
   s->palette[238] = 0xffff29bd;
   s->palette[239] = 0xffc510bd;
   s->palette[240] = 0xffbe148c;
   s->palette[241] = 0xff7b185a;
   s->palette[242] = 0xff641464;
   s->palette[243] = 0xff620041;
   s->palette[244] = 0xff460a32;
   s->palette[245] = 0xff371955;
   s->palette[246] = 0xff8219a0;
   s->palette[247] = 0xff7800c8;
   s->palette[248] = 0xffbf50ff;
   s->palette[249] = 0xffc56aff;
   s->palette[250] = 0xffb9a0fa;
   s->palette[251] = 0xff8c3afc;
   s->palette[252] = 0xff781ee6;
   s->palette[253] = 0xff3910bd;
   s->palette[254] = 0xff4d3498;
   s->palette[255] = 0xff371491;
   //-------------------------------------

   //Brushes
   //-------------------------------------
   
   //Square 1x1
   s->brushes[0] = calloc(1,sizeof(*s->brushes[0])+1);
   s->brushes[0]->width = 1;
   s->brushes[0]->height = 1;
   s->brushes[0]->data[0] = 1;

   //Square 2x2
   s->brushes[1] = calloc(1,sizeof(*s->brushes[1])+4);
   s->brushes[1]->width = 2;
   s->brushes[1]->height = 2;
   for(int i = 0;i<4;i++) s->brushes[1]->data[i] = 1;

   //Square 3x3
   s->brushes[2] = calloc(1,sizeof(*s->brushes[2])+9);
   s->brushes[2]->width = 3;
   s->brushes[2]->height = 3;
   for(int i = 0;i<9;i++) s->brushes[2]->data[i] = 1;

   //Square 4x4
   s->brushes[3] = calloc(1,sizeof(*s->brushes[3])+16);
   s->brushes[3]->width = 4;
   s->brushes[3]->height = 4;
   for(int i = 0;i<16;i++) s->brushes[3]->data[i] = 1;

   //Square 5x5
   s->brushes[4] = calloc(1,sizeof(*s->brushes[4])+25);
   s->brushes[4]->width = 5;
   s->brushes[4]->height = 5;
   for(int i = 0;i<25;i++) s->brushes[4]->data[i] = 1;

   //Square 7x7
   s->brushes[5] = calloc(1,sizeof(*s->brushes[5])+49);
   s->brushes[5]->width = 7;
   s->brushes[5]->height = 7;
   for(int i = 0;i<49;i++) s->brushes[5]->data[i] = 1;

   //Square 8x8
   s->brushes[6] = calloc(1,sizeof(*s->brushes[6])+64);
   s->brushes[6]->width = 8;
   s->brushes[6]->height = 8;
   for(int i = 0;i<64;i++) s->brushes[6]->data[i] = 1;

   //Square 12x12
   s->brushes[7] = calloc(1,sizeof(*s->brushes[7])+144);
   s->brushes[7]->width = 12;
   s->brushes[7]->height = 12;
   for(int i = 0;i<144;i++) s->brushes[7]->data[i] = 1;

   //Square 16x16
   s->brushes[8] = calloc(1,sizeof(*s->brushes[8])+256);
   s->brushes[8]->width = 16;
   s->brushes[8]->height = 16;
   for(int i = 0;i<256;i++) s->brushes[8]->data[i] = 1;

   //Square sieve 16x16
   s->brushes[9] = calloc(1,sizeof(*s->brushes[9])+256);
   s->brushes[9]->width = 16;
   s->brushes[9]->height = 16;
   for(int i = 0;i<256;i++) if(((i&1)+i/16)&1) s->brushes[9]->data[i] = 1;
   //-------------------------------------

   s->palette_selected = 1;

   return s;
}

void settings_build_lut(Settings *s)
{
   s->color_white = color_closest(s,255,255,255);
}

void settings_free(Settings *s)
{
   if(s==NULL)
      return;

   for(int i = 0;i<64;i++)
   {
      if(s->brushes[i]!=NULL)
         free(s->brushes[i]);
      s->brushes[i] = NULL;
   }
}

static uint8_t color_closest(const Settings *settings, uint8_t r, uint8_t g, uint8_t b)
{
   uint8_t index_min = 0;
   uint32_t dist_min = UINT32_MAX;

   for(int i = 0;i<settings->palette_colors;i++)
   {
      uint32_t pr = color32_r(settings->palette[i]);
      uint32_t pg = color32_g(settings->palette[i]);
      uint32_t pb = color32_b(settings->palette[i]);
      uint32_t dist = (pr-r)*(pr-r)+(pg-g)*(pg-g)+(pb-b)*(pb-b);
      if(dist<dist_min)
      {
         dist_min = dist;
         index_min = (uint8_t)i;
      }
   }

   return index_min;
}
//-------------------------------------
