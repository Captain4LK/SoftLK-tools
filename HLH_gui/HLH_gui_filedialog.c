/*
HLH_gui - gui framework

Written in 2026 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

//External includes
//-------------------------------------

//Internal includes
#include "HLH_gui.h"
#include "HLH_gui_internal.h"
//-------------------------------------

//#defines
//-------------------------------------

//Typedefs
//-------------------------------------

//Variables
//-------------------------------------

//Function prototypes
//-------------------------------------

//Function implementations
void HLH_gui_open_file_dialog(HLH_gui_window *win, int32_t ident, size_t filter_count,
                              HLH_gui_file_filter filters[static filter_count], HLH_string default_location, bool allow_many)
{
   HLH_gui_dialog_internal_ctx *ctx = calloc(1, sizeof(*ctx));
   ctx->ident = ident;
   ctx->window = win;
   ctx->filters = calloc(filter_count, sizeof(*ctx->filters));

   // Need a copy of filters to ensure lifetime
   for(size_t i = 0; i < filter_count; i += 1)
   {
      ctx->filters[i].name = HLH_gui_strdup(filters[i].name);
      ctx->filters[i].pattern = HLH_gui_strdup(filters[i].pattern);
   }

   const char *default_cstr = HLH_string_clone_to_cstring(default_location);
   SDL_ShowOpenFileDialog(core_open_file_callback, ctx, win->sdl_window, ctx->filters, filter_count, default_cstr, allow_many);
   free((void *)default_cstr);
}

void HLH_gui_save_file_dialog(HLH_gui_window *win, int32_t ident, size_t filter_count,
                              HLH_gui_file_filter filters[static filter_count], HLH_string default_location)
{
   HLH_gui_dialog_internal_ctx *ctx = calloc(1, sizeof(*ctx));
   ctx->ident = ident;
   ctx->window = win;
   ctx->filters = calloc(filter_count, sizeof(*ctx->filters));

   // Need a copy of filters to ensure lifetime
   for(size_t i = 0; i < filter_count; i += 1)
   {
      ctx->filters[i].name = HLH_gui_strdup(filters[i].name);
      ctx->filters[i].pattern = HLH_gui_strdup(filters[i].pattern);
   }

   const char *default_cstr = HLH_string_clone_to_cstring(default_location);
   SDL_ShowSaveFileDialog(core_save_file_callback, ctx, win->sdl_window, ctx->filters, filter_count, default_cstr);
   free((void *)default_cstr);
}

void HLH_gui_open_folder_dialog(HLH_gui_window *win, int32_t ident, HLH_string default_location, bool allow_many)
{
   HLH_gui_dialog_internal_ctx *ctx = calloc(1, sizeof(*ctx));
   ctx->ident = ident;
   ctx->window = win;

   const char *default_cstr = HLH_string_clone_to_cstring(default_location);
   SDL_ShowOpenFolderDialog(core_open_folder_callback, ctx, win->sdl_window, default_cstr, allow_many);
   free((void *)default_cstr);
}
//-------------------------------------
