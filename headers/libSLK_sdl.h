#include <SDL2/SDL.h>

#define CUTE_PNG_IMPLEMENTATION
#define CUTE_PNG_ALLOC backend_malloc
#define CUTE_PNG_CALLOC backend_calloc
#define CUTE_PNG_FREE backend_free
#include "../external/cute_png.h"
//https://github.com/RandyGaul/cute_headers

#define HLH_SLK_IMPLEMENTATION
#define HLH_SLK_MALLOC backend_malloc
#define HLH_SLK_FREE backend_free
#include "../external/HLH_slk.h"

#define MAX_CONTROLLERS 4

typedef struct
{
   SDL_GameController *gamepad;
   SDL_Haptic *haptic;
   SDL_JoystickID id;
   int connected;
   uint8_t new_button_state[SLK_PAD_MAX];
   uint8_t old_button_state[SLK_PAD_MAX];
}Gamepad;

static SDL_Window *sdl_window;
static SDL_Renderer *renderer;
static SDL_Texture **layer_textures;
static int screen_width;
static int screen_height;
static int pixel_scale;
static int window_width;
static int window_height;
static int view_x;
static int view_y;
static int view_width;
static int view_height;
static int fps;
static int frametime;
static int framedelay;
static int framestart;
static float delta;
static uint8_t key_map[SDL_NUM_SCANCODES];
static uint8_t mouse_map[6];
static uint8_t gamepad_map[SDL_CONTROLLER_BUTTON_MAX];
static uint8_t new_key_state[SLK_KEY_MAX];
static uint8_t old_key_state[SLK_KEY_MAX];
static uint8_t new_mouse_state[SLK_BUTTON_MAX];
static uint8_t old_mouse_state[SLK_BUTTON_MAX];
static Gamepad gamepads[MAX_CONTROLLERS];
static int mouse_x_rel;
static int mouse_y_rel;
static char *text_input;
static int text_input_active;
static int text_input_max;
static int mouse_x;
static int mouse_y;
static int mouse_wheel;

static void *(*bmalloc)(size_t size) = backend_system_malloc;
static void (*bfree)(void *ptr) = backend_system_free;
static void *(*brealloc)(void *ptr, size_t size) = backend_system_realloc;

static int get_gamepad_index(int which);

//(should) center the viewport.
void backend_update_viewport()
{
   SDL_GetWindowSize(sdl_window,&window_width,&window_height);

   if(layer_dynamic)
   {
      view_width = window_width;
      view_height = window_height;
      view_x = 0;
      view_y = 0;
   }
   else
   {
      view_width = screen_width*pixel_scale;
      view_height = screen_height*pixel_scale;

      view_x = (window_width-view_width)/2;
      view_y = (window_height-view_height)/2;
   }

   SDL_Rect v;
   v.x = view_x;
   v.y = view_y;
   v.w = view_width;
   v.h = view_height;
   if(SDL_RenderSetViewport(renderer,&v)<0)
      SLK_log_line("SDL_RenderSetViewport","failed to set render viewport: %s",SDL_GetError());
}

//Handles window and input events.
void backend_handle_events()
{
   mouse_wheel = 0;
   memcpy(old_key_state,new_key_state,sizeof(new_key_state));
   memcpy(old_mouse_state,new_mouse_state,sizeof(new_mouse_state));
   for(int i = 0;i<MAX_CONTROLLERS;i++)
      memcpy(gamepads[i].old_button_state,gamepads[i].new_button_state,sizeof(gamepads[0].new_button_state));

   //Event managing
   SDL_Event event;
   while(SDL_PollEvent(&event))
   {
      switch(event.type)
      {
      case SDL_QUIT:
         SLK_core_quit();
         break;
      case SDL_KEYDOWN:
         if(text_input_active&&event.key.keysym.sym==SDLK_BACKSPACE&&text_input[0]!='\0')
            text_input[strlen(text_input)-1] = '\0';
         if(event.key.state==SDL_PRESSED)
            new_key_state[key_map[event.key.keysym.scancode]] = 1;
         break;
      case SDL_KEYUP:
         if(event.key.state==SDL_RELEASED)
            new_key_state[key_map[event.key.keysym.scancode]] = 0;
         break;
      case SDL_MOUSEBUTTONDOWN:
         if(event.button.state==SDL_PRESSED)
            new_mouse_state[mouse_map[event.button.button]] = 1;
         break;
      case SDL_MOUSEBUTTONUP:
         if(event.button.state==SDL_RELEASED)
            new_mouse_state[mouse_map[event.button.button]] = 0;
         break;       
      case SDL_TEXTINPUT:
         if(text_input_active&&strlen(text_input)+strlen(event.text.text)<text_input_max)
            strcat(text_input,event.text.text);
         break;
      case SDL_MOUSEWHEEL:
         mouse_wheel = event.wheel.y;
         break;
      case SDL_CONTROLLERBUTTONDOWN:
         if(event.cbutton.state==SDL_PRESSED)
         {
            int id = get_gamepad_index(event.cbutton.which);
            gamepads[id].new_button_state[gamepad_map[event.cbutton.button]] = 1;
         }
         break;
      case SDL_CONTROLLERBUTTONUP:
         if(event.cbutton.state==SDL_RELEASED)
         {
            int id = get_gamepad_index(event.cbutton.which);
            gamepads[id].new_button_state[gamepad_map[event.cbutton.button]] = 0;
         }
         break;
      case SDL_CONTROLLERDEVICEADDED:
         {
            int which = event.cdevice.which;
            if(which<MAX_CONTROLLERS)
            {
               gamepads[which].gamepad = SDL_GameControllerOpen(which);
               gamepads[which].connected = 1;
               SDL_Joystick *j = SDL_GameControllerGetJoystick(gamepads[which].gamepad);
               gamepads[which].id = SDL_JoystickInstanceID(j);
            }
         }
         break;
      case SDL_CONTROLLERDEVICEREMOVED:
         {
            int which = event.cdevice.which;
            if(which<0)
               break;
            int id = get_gamepad_index(which);
            gamepads[id].connected = 0;
            SDL_GameControllerClose(gamepads[id].gamepad);
         }
         break;
      case SDL_WINDOWEVENT:
         if(event.window.event==SDL_WINDOWEVENT_RESIZED)
         {
            if(layer_dynamic)
            {
               int new_width = event.window.data1/pixel_scale+1;
               int new_height = event.window.data2/pixel_scale+1;
               screen_width = new_width;
               screen_height = new_height;

               for(int l = 0;l<layer_count;l++)
               {
                  if(layers[l].dynamic)
                     SLK_layer_set_size(l,new_width,new_height);
               }
            }
         }
         backend_update_viewport();
         break;
      }
   }
   //-------------------------------------------
   
   int x,y;
   SDL_GetMouseState(&x,&y);

   x-=view_x;
   y-=view_y;
   mouse_x = x/pixel_scale;
   mouse_y = y/pixel_scale;

   SDL_GetRelativeMouseState(&mouse_x_rel,&mouse_y_rel);
   mouse_x_rel = mouse_x_rel/pixel_scale;
   mouse_y_rel = mouse_y_rel/pixel_scale;

   if(mouse_x>=screen_width)
     mouse_x = screen_width-1;
   if(mouse_y>=screen_height)
     mouse_y = screen_height-1;

   if(mouse_x<0)
     mouse_x = 0;
   if(mouse_y<0)
     mouse_y = 0;
}

//Creates the window, etc.
void backend_setup(int width, int height, int layer_num, const char *title, int fullscreen, int scale, int resizable)
{
   pixel_scale = scale;
   screen_width = width;
   screen_height = height;
   layer_count = layer_num;
   layer_dynamic = resizable;

   Uint32 flags = SDL_INIT_VIDEO|SDL_INIT_EVENTS;

   if(SDL_Init(flags)<0)
   {
      SLK_log_line("SDL_Init","%s",SDL_GetError());
      exit(-1);
   }

   if(pixel_scale==SLK_WINDOW_MAX)
   {
      SDL_Rect max_size;

      if(SDL_GetDisplayUsableBounds(0,&max_size)<0)
      {
         SLK_log_line("SDL_GetDisplayUsableBounds","%s",SDL_GetError());
      }
      else
      {
         int max_x,max_y;

         max_x = max_size.w/screen_width;
         max_y = max_size.h/screen_height;

         pixel_scale = (max_x>max_y)?max_y:max_x;
      }
      
   }

   if(pixel_scale<=0)
      pixel_scale = 1;

   if(resizable)
      sdl_window = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width*pixel_scale,height*pixel_scale,SDL_WINDOW_RESIZABLE);
   else
      sdl_window = SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width*pixel_scale,height*pixel_scale,0);

   if(sdl_window==NULL)
   {
      SLK_log_line("SDL_CreateWindow","%s",SDL_GetError());
      exit(-1);
   }

   renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
   if(!renderer)
   {
      SLK_log_line("SDL_CreateRenderer","%s",SDL_GetError());
      exit(-1);
   }

   SDL_SetRenderDrawColor(renderer,0,0,0,0);

   layer_textures = backend_malloc(sizeof(*layer_textures)*layer_num);
   if(layer_textures==NULL)
      SLK_log("malloc of size %zu failed, out of memory!",sizeof(*layer_textures)*layer_num);

   memset(layer_textures,0,sizeof(*layer_textures)*layer_num);
   backend_update_viewport();
}

//Clears the window and redraws the scene.
//Drawing is performed from back to front, layer 0 is always drawn last.
void backend_render_update()
{
   SDL_RenderClear(renderer);

   for(int l = layer_count-1;l>=0;l--)
   {
      layers[l].resized = 0;

      if(layers[l].active)
      {
         switch(layers[l].type)
         {
         case SLK_LAYER_PAL:
         {
            float width = (float)layers[l].as.type_0.target->width*layers[l].scale*pixel_scale;
            float height = (float)layers[l].as.type_0.target->height*layers[l].scale*pixel_scale;
            float x = (float)layers[l].x*pixel_scale;
            float y = (float)layers[l].y*pixel_scale;
            SDL_Rect dst_rect;
            dst_rect.x = x;
            dst_rect.y = y;
            dst_rect.w = width;
            dst_rect.h = height;

            for(int i = 0;i<layers[l].as.type_0.render->width*layers[l].as.type_0.render->height;i++)
               layers[l].as.type_0.render->data[i] = layers[l].as.type_0.palette->colors[layers[l].as.type_0.target->data[i]];

            int w, h;
            SDL_QueryTexture(layer_textures[l], NULL, NULL, &w, &h);

            if(w!=layers[l].as.type_0.target->width||h!=layers[l].as.type_0.target->height)
            {
               SDL_DestroyTexture(layer_textures[l]);
               layer_textures[l] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,layers[l].as.type_0.target->width,layers[l].as.type_0.target->height);
               SDL_SetTextureBlendMode(layer_textures[l],SDL_BLENDMODE_BLEND);
            }

            void *data;
            int stride;
            SDL_LockTexture(layer_textures[l],NULL,&data,&stride);
            memcpy(data,layers[l].as.type_0.render->data,sizeof(*layers[l].as.type_0.render->data)*layers[l].as.type_0.target->width*layers[l].as.type_0.target->height);
            SDL_UnlockTexture(layer_textures[l]);

            SDL_SetTextureColorMod(layer_textures[l],layers[l].tint.rgb.r,layers[l].tint.rgb.g,layers[l].tint.rgb.b);
            SDL_SetTextureAlphaMod(layer_textures[l],layers[l].tint.rgb.a);
            SDL_RenderCopy(renderer,layer_textures[l],NULL,&dst_rect);

            break;
         }
         case SLK_LAYER_RGB:
         {
            int width = (float)layers[l].as.type_1.target->width*layers[l].scale*pixel_scale;
            int height = (float)layers[l].as.type_1.target->height*layers[l].scale*pixel_scale;
            int x = (float)layers[l].x*pixel_scale;
            int y = (float)layers[l].y*pixel_scale;
            SDL_Rect dst_rect;
            dst_rect.x = x;
            dst_rect.y = y;
            dst_rect.w = width;
            dst_rect.h = height;

            if(layers[l].as.type_1.target->changed)
            {
               int w, h;
               SDL_QueryTexture(layer_textures[l], NULL, NULL, &w, &h);

               if(w!=layers[l].as.type_1.target->width||h!=layers[l].as.type_1.target->height)
               {
                  SDL_DestroyTexture(layer_textures[l]);
                  layer_textures[l] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,layers[l].as.type_1.target->width,layers[l].as.type_1.target->height);
                  SDL_SetTextureBlendMode(layer_textures[l],SDL_BLENDMODE_BLEND);
               }
               void *data;
               int stride;
               SDL_LockTexture(layer_textures[l],NULL,&data,&stride);
               memcpy(data,layers[l].as.type_1.target->data,sizeof(*layers[l].as.type_1.target->data)*layers[l].as.type_1.target->width*layers[l].as.type_1.target->height);
               SDL_UnlockTexture(layer_textures[l]);
               layers[l].as.type_1.target->changed = 0;
            }
            SDL_SetTextureColorMod(layer_textures[l],layers[l].tint.rgb.r,layers[l].tint.rgb.g,layers[l].tint.rgb.b);
            SDL_SetTextureAlphaMod(layer_textures[l],layers[l].tint.rgb.a);
            SDL_RenderCopy(renderer,layer_textures[l],NULL,&dst_rect);

            break;
         }
         }
      }
   }
   
   SDL_RenderPresent(renderer);
}

void backend_create_layer(unsigned index, int type)
{
   if(index>=layer_count)
      return;

   switch(type)
   {
   case SLK_LAYER_PAL:
      layer_textures[index] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,screen_width,screen_height);
      if(layer_textures[index]==NULL)
         SLK_log("failed to create texture for layer %d: %s",index,SDL_GetError());

      if(SDL_SetTextureBlendMode(layer_textures[index],SDL_BLENDMODE_BLEND)<0)
         SLK_log("failed to set texture blend mode: %s",SDL_GetError());
      break;
   case SLK_LAYER_RGB:
      layer_textures[index] = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA32,SDL_TEXTUREACCESS_STREAMING,screen_width,screen_height);
      if(layer_textures[index]==NULL)
         SLK_log("failed to create texture for layer %d: %s",index,SDL_GetError());

      if(SDL_SetTextureBlendMode(layer_textures[index],SDL_BLENDMODE_BLEND)<0)
         SLK_log("failed to set texture blend mode: %s",SDL_GetError());
      break;
   }
}

//Set the window title.
void backend_set_title(const char *title)
{
   SDL_SetWindowTitle(sdl_window,title);
}

//Toggles fullscreen.
void backend_set_fullscreen(int fullscreen)
{
   if(fullscreen)
   {
      if(SDL_SetWindowFullscreen(sdl_window,SDL_WINDOW_FULLSCREEN_DESKTOP)!=0)
         SLK_log("failed to fullscreen window: %s",SDL_GetError());
   }
   else
   {
      if(SDL_SetWindowFullscreen(sdl_window,0)!=0)
         SLK_log("failed to exit fullscreen: %s",SDL_GetError());

      SDL_SetWindowSize(sdl_window,screen_width*pixel_scale,screen_height*pixel_scale);
   }

   backend_update_viewport();
}

//Sets wether the window is visible.
void backend_set_visible(int visible)
{
   if(visible)
      SDL_ShowWindow(sdl_window);
   else
      SDL_HideWindow(sdl_window);
}

//Sets the window icon.
void backend_set_icon(const SLK_RGB_sprite *icon)
{
   SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(icon->data,icon->width,icon->height,32,icon->width*4,0xf000,0x0f00,0x00f0,0x000f);
   if(surface==NULL)
   {
      SLK_log("failed to create sdl surface from rgb sprite: %s",SDL_GetError());
      return;
   }

   SDL_SetWindowIcon(sdl_window,surface);
   SDL_FreeSurface(surface);
}

//Returns the viewport width adjusted to pixel scale.
int backend_get_width()
{
   return screen_width;
}

//Returns the viewport height adjusted to pixel scale.
int backend_get_height()
{
   return screen_height;
}

//Returns the view width.
int backend_get_view_width()
{
   return view_width;
}

//Returns the view height.
int backend_get_view_height()
{
   return view_height;
}

//Returns the view x pos.
int backend_get_view_x()
{
   return view_x;
}

//Returns the view y pos.
int backend_get_view_y()
{
   return view_y;
}

//Returns the window width.
int backend_get_win_width()
{
   return window_width;
}

//Returns the window height.
int backend_get_win_height()
{
   return window_height;
}

//Sets the target/maximum fps.
void backend_set_fps(int FPS)
{
   if(FPS<1||FPS>1000)
      fps = 1000;
   else
      fps = FPS;

   framedelay = 1000/fps;
}

//Returns the current target fps.
int backend_get_fps()
{
   return fps;
}

//Limits the fps to the target fps.
void backend_timer_update()
{
   frametime = SDL_GetTicks()-framestart;

   if(framedelay>frametime)
      SDL_Delay(framedelay-frametime);

   delta = (float)(SDL_GetTicks()-framestart)/1000.0f;
   framestart = SDL_GetTicks();
}

//Returns the delta time of the last frame.
float backend_timer_get_delta()
{
   return delta;
}

//Creates the keymap.
void backend_input_init()
{
   key_map[0x00] = SLK_KEY_NONE;
   key_map[SDL_SCANCODE_A] = SLK_KEY_A;
   key_map[SDL_SCANCODE_B] = SLK_KEY_B;
   key_map[SDL_SCANCODE_C] = SLK_KEY_C;
   key_map[SDL_SCANCODE_D] = SLK_KEY_D;
   key_map[SDL_SCANCODE_E] = SLK_KEY_E;
   key_map[SDL_SCANCODE_F] = SLK_KEY_F;
   key_map[SDL_SCANCODE_G] = SLK_KEY_G;
   key_map[SDL_SCANCODE_H] = SLK_KEY_H;
   key_map[SDL_SCANCODE_I] = SLK_KEY_I;
   key_map[SDL_SCANCODE_J] = SLK_KEY_J;
   key_map[SDL_SCANCODE_K] = SLK_KEY_K;
   key_map[SDL_SCANCODE_L] = SLK_KEY_L;
   key_map[SDL_SCANCODE_M] = SLK_KEY_M;
   key_map[SDL_SCANCODE_N] = SLK_KEY_N;
   key_map[SDL_SCANCODE_O] = SLK_KEY_O;
   key_map[SDL_SCANCODE_P] = SLK_KEY_P;
   key_map[SDL_SCANCODE_Q] = SLK_KEY_Q;
   key_map[SDL_SCANCODE_R] = SLK_KEY_R;
   key_map[SDL_SCANCODE_S] = SLK_KEY_S;
   key_map[SDL_SCANCODE_T] = SLK_KEY_T;
   key_map[SDL_SCANCODE_U] = SLK_KEY_U;
   key_map[SDL_SCANCODE_V] = SLK_KEY_V;
   key_map[SDL_SCANCODE_W] = SLK_KEY_W;
   key_map[SDL_SCANCODE_X] = SLK_KEY_X;
   key_map[SDL_SCANCODE_Y] = SLK_KEY_Y;
   key_map[SDL_SCANCODE_Z] = SLK_KEY_Z;

   key_map[SDL_SCANCODE_F1] = SLK_KEY_F1;
   key_map[SDL_SCANCODE_F2] = SLK_KEY_F2;
   key_map[SDL_SCANCODE_F3] = SLK_KEY_F3;
   key_map[SDL_SCANCODE_F4] = SLK_KEY_F4;
   key_map[SDL_SCANCODE_F5] = SLK_KEY_F5;
   key_map[SDL_SCANCODE_F6] = SLK_KEY_F6;
   key_map[SDL_SCANCODE_F7] = SLK_KEY_F7;
   key_map[SDL_SCANCODE_F8] = SLK_KEY_F8;
   key_map[SDL_SCANCODE_F9] = SLK_KEY_F9;
   key_map[SDL_SCANCODE_F10] = SLK_KEY_F10;
   key_map[SDL_SCANCODE_F11] = SLK_KEY_F11;
   key_map[SDL_SCANCODE_F12] = SLK_KEY_F12;

   key_map[SDL_SCANCODE_DOWN] = SLK_KEY_DOWN;
   key_map[SDL_SCANCODE_LEFT] = SLK_KEY_LEFT;
   key_map[SDL_SCANCODE_RIGHT] = SLK_KEY_RIGHT;
   key_map[SDL_SCANCODE_UP] = SLK_KEY_UP;
   key_map[SDL_SCANCODE_RETURN] = SLK_KEY_ENTER;

   key_map[SDL_SCANCODE_BACKSPACE] = SLK_KEY_BACK;
   key_map[SDL_SCANCODE_ESCAPE] = SLK_KEY_ESCAPE;
   key_map[SDL_SCANCODE_TAB] = SLK_KEY_TAB;
   key_map[SDL_SCANCODE_LGUI] = SLK_KEY_HOME;
   key_map[SDL_SCANCODE_END] = SLK_KEY_END;
   key_map[SDL_SCANCODE_PAGEUP] = SLK_KEY_PGUP;
   key_map[SDL_SCANCODE_PAGEDOWN] = SLK_KEY_PGDN;
   key_map[SDL_SCANCODE_INSERT] = SLK_KEY_INS;
   key_map[SDL_SCANCODE_LSHIFT] = SLK_KEY_SHIFT;
   key_map[SDL_SCANCODE_RSHIFT] = SLK_KEY_SHIFT;
   key_map[SDL_SCANCODE_LCTRL] = SLK_KEY_CTRL;
   key_map[SDL_SCANCODE_RCTRL] = SLK_KEY_CTRL;
   key_map[SDL_SCANCODE_SPACE] = SLK_KEY_SPACE;

   key_map[SDL_SCANCODE_0] = SLK_KEY_0;
   key_map[SDL_SCANCODE_1] = SLK_KEY_1;
   key_map[SDL_SCANCODE_2] = SLK_KEY_2;
   key_map[SDL_SCANCODE_3] = SLK_KEY_3;
   key_map[SDL_SCANCODE_4] = SLK_KEY_4;
   key_map[SDL_SCANCODE_5] = SLK_KEY_5;
   key_map[SDL_SCANCODE_6] = SLK_KEY_6;
   key_map[SDL_SCANCODE_7] = SLK_KEY_7;
   key_map[SDL_SCANCODE_8] = SLK_KEY_8;
   key_map[SDL_SCANCODE_9] = SLK_KEY_9;

   key_map[SDL_SCANCODE_KP_0] = SLK_KEY_NP0;
   key_map[SDL_SCANCODE_KP_1] = SLK_KEY_NP1;
   key_map[SDL_SCANCODE_KP_2] = SLK_KEY_NP2;
   key_map[SDL_SCANCODE_KP_3] = SLK_KEY_NP3;
   key_map[SDL_SCANCODE_KP_4] = SLK_KEY_NP4;
   key_map[SDL_SCANCODE_KP_5] = SLK_KEY_NP5;
   key_map[SDL_SCANCODE_KP_6] = SLK_KEY_NP6;
   key_map[SDL_SCANCODE_KP_7] = SLK_KEY_NP7;
   key_map[SDL_SCANCODE_KP_8] = SLK_KEY_NP8;
   key_map[SDL_SCANCODE_KP_9] = SLK_KEY_NP9;
   key_map[SDL_SCANCODE_KP_MULTIPLY] = SLK_KEY_NP_MUL;
   key_map[SDL_SCANCODE_KP_PLUS] = SLK_KEY_NP_ADD;
   key_map[SDL_SCANCODE_KP_DIVIDE] = SLK_KEY_NP_DIV;
   key_map[SDL_SCANCODE_KP_MINUS] = SLK_KEY_NP_SUB;
   key_map[SDL_SCANCODE_KP_PERIOD] = SLK_KEY_NP_DECIMAL;

   mouse_map[SDL_BUTTON_LEFT] = SLK_BUTTON_LEFT;
   mouse_map[SDL_BUTTON_RIGHT] = SLK_BUTTON_RIGHT;
   mouse_map[SDL_BUTTON_MIDDLE] = SLK_BUTTON_MIDDLE;
   mouse_map[SDL_BUTTON_X1] = SLK_BUTTON_X1;
   mouse_map[SDL_BUTTON_X2] = SLK_BUTTON_X2;

   gamepad_map[SDL_CONTROLLER_BUTTON_A] = SLK_PAD_A;
   gamepad_map[SDL_CONTROLLER_BUTTON_B] = SLK_PAD_B;
   gamepad_map[SDL_CONTROLLER_BUTTON_X] = SLK_PAD_X;
   gamepad_map[SDL_CONTROLLER_BUTTON_Y] = SLK_PAD_Y;
   gamepad_map[SDL_CONTROLLER_BUTTON_BACK] = SLK_PAD_BACK;
   gamepad_map[SDL_CONTROLLER_BUTTON_GUIDE] = SLK_PAD_GUIDE;
   gamepad_map[SDL_CONTROLLER_BUTTON_START] = SLK_PAD_START;
   gamepad_map[SDL_CONTROLLER_BUTTON_LEFTSTICK] = SLK_PAD_LEFTSTICK;
   gamepad_map[SDL_CONTROLLER_BUTTON_RIGHTSTICK] = SLK_PAD_RIGHTSTICK;
   gamepad_map[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = SLK_PAD_LEFTSHOULDER;
   gamepad_map[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] = SLK_PAD_RIGHTSHOULDER;
   gamepad_map[SDL_CONTROLLER_BUTTON_DPAD_UP] = SLK_PAD_UP;
   gamepad_map[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = SLK_PAD_DOWN;
   gamepad_map[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = SLK_PAD_LEFT;
   gamepad_map[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = SLK_PAD_RIGHT;

   //Clear key states, just in case,
   //should already be empty since known at compile time
   memset(new_key_state,0,sizeof(new_key_state));
   memset(old_key_state,0,sizeof(old_key_state));
   memset(new_mouse_state,0,sizeof(new_mouse_state));
   memset(old_mouse_state,0,sizeof(old_mouse_state));
   for(int i = 0;i<MAX_CONTROLLERS;i++)
   {
      memset(gamepads[i].new_button_state,0,sizeof(gamepads[i].new_button_state));
      memset(gamepads[i].old_button_state,0,sizeof(gamepads[i].old_button_state));
   }
}

//Shows or hides the mouse cursor.
void backend_show_cursor(int shown)
{
   if(SDL_ShowCursor(shown?SDL_ENABLE:SDL_DISABLE)<0)
      SLK_log("failed to show/hide cursor: %s",SDL_GetError());
}

//Sets wether the mouse cursor is captured and only relative
//mouse motion is registerd.
void backend_mouse_set_relative(int relative)
{
   if(SDL_SetRelativeMouseMode(relative)<0)
      SLK_log("failed to set relative mouse mode: %s",SDL_GetError());
}

//Sets wether to track mouse events globally.
void backend_mouse_capture(int capture)
{
   if(SDL_CaptureMouse(capture)<0)
      SLK_log("failed to capture/release mouse: %s",SDL_GetError());
}

//Starts text input.
void backend_start_text_input(char *text, int max_length)
{
   text_input = text;
   text_input_active = 1;
   text_input_max = max_length;

   SDL_StartTextInput();
}

//Stops text input.
void backend_stop_text_input()
{
   text_input_active = 0;
   
   SDL_StopTextInput();
}

int backend_key_down(int key)
{
   return new_key_state[key];
}

int backend_key_pressed(int key)
{
   return new_key_state[key]&&!old_key_state[key];
}

int backend_key_released(int key)
{
   return !new_key_state[key]&&old_key_state[key];
}

SLK_Button backend_key_get_state(int key)
{
   SLK_Button out;
   out.pressed = new_key_state[key]&&!old_key_state[key];
   out.released = !new_key_state[key]&&old_key_state[key];
   out.held = new_key_state[key];

   return out;
}

int backend_mouse_down(int key)
{
   return new_mouse_state[key];
}

int backend_mouse_pressed(int key)
{
   return new_mouse_state[key]&&!old_mouse_state[key];
}

int backend_mouse_released(int key)
{
   return !new_mouse_state[key]&&old_mouse_state[key];
}

SLK_Button backend_mouse_get_state(int key)
{
   SLK_Button out;
   out.pressed = new_mouse_state[key]&&!old_mouse_state[key];
   out.held = new_mouse_state[key];
   out.released = !new_mouse_state[key]&&old_mouse_state[key];

   return out;
}

int backend_mouse_wheel_get_scroll()
{
   return mouse_wheel;
}

int backend_gamepad_down(int index, int key)
{
   return gamepads[index].new_button_state[key];
}

int backend_gamepad_pressed(int index, int key)
{
   return gamepads[index].new_button_state[key]&&!gamepads[index].old_button_state[key];
}

int backend_gamepad_released(int index, int key)
{
   return !gamepads[index].new_button_state[key]&&gamepads[index].old_button_state[key];
}

SLK_Button backend_gamepad_get_state(int index, int key)
{
   SLK_Button out;
   out.pressed = gamepads[index].new_button_state[key]&&!gamepads[index].old_button_state[key];
   out.held = gamepads[index].new_button_state[key];
   out.released = !gamepads[index].new_button_state[key]&&gamepads[index].old_button_state[key];

   return out;
}

int backend_get_gamepad_count()
{
   return SDL_NumJoysticks();
}

void backend_mouse_get_pos(int *x, int *y)
{
   *x = mouse_x;
   *y = mouse_y;
}

void backend_mouse_get_relative_pos(int *x, int *y)
{
   *x = mouse_x_rel;
   *y = mouse_y_rel;
}

static int get_gamepad_index(int which)
{

   for(int i = 0;i<MAX_CONTROLLERS;i++)
      if(gamepads[i].connected&&gamepads[i].id==which)
         return i;

   return -1;
}

SLK_RGB_sprite *backend_load_rgb(const char *path)
{
   cp_image_t img = cp_load_png(path);
   SLK_RGB_sprite *out = NULL;
   if(img.pix==NULL)
   {
      SLK_log("failed to load png %s\n",path);
      return NULL;
   }

   out = SLK_rgb_sprite_create(img.w,img.h);
   memcpy(out->data,img.pix,img.w*img.h*sizeof(*out->data));
   cp_free_png(&img);

   return out;
}

SLK_RGB_sprite *backend_load_rgb_file(FILE *f)
{
   int size = 0;
   fseek(f,0,SEEK_END);
   size = ftell(f);
   fseek(f,0,SEEK_SET);
   char *data = backend_malloc(size+1);
   fread(data,size,1,f);
   data[size] = 0;
   SLK_RGB_sprite *out = backend_load_rgb_mem(data,size);
   backend_free(data);

   return out;
}

SLK_RGB_sprite *backend_load_rgb_mem(const void *data, int length)
{
   cp_image_t img = cp_load_png_mem(data,length);
   SLK_RGB_sprite *out = NULL;
   if(img.pix==NULL)
   {
      SLK_log("failed to load png from memory buffer");
      return NULL;
   }

   out = SLK_rgb_sprite_create(img.w,img.h);
   memcpy(out->data,img.pix,img.w*img.h*sizeof(*out->data));
   cp_free_png(&img);

   return out;
}


void backend_save_rgb(const SLK_RGB_sprite *s, const char *path)
{
   if(path==NULL)
      return;
   FILE *f = fopen(path,"wb");
   if(f==NULL)
      SLK_log("failed to open %s for writing",path);

   backend_save_rgb_file(s,f);

   fclose(f);
}

void backend_save_rgb_file(const SLK_RGB_sprite *s, FILE *f)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't write png to disk");
      return;
   }

   cp_image_t img;
   img.w = s->width;
   img.h = s->height;
   img.pix = (cp_pixel_t *)s->data;
   cp_save_png(f,&img);
}

SLK_Pal_sprite *backend_load_pal(const char *path)
{
   FILE *f = fopen(path,"rb");
   if(f==NULL)
   {
      SLK_log("failed to open %s for reading",path);
      return NULL;
   }

   SLK_Pal_sprite *out = backend_load_pal_file(f);
   fclose(f);
   return out;
}

SLK_Pal_sprite *backend_load_pal_file(FILE *f)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't read .slk file");
      return NULL;
   }

   int size = 0;
   char *data = NULL;
   fseek(f,0,SEEK_END);
   size = ftell(f);
   fseek(f,0,SEEK_SET);
   data = backend_malloc(size+1);
   fread(data,size,1,f);
   data[size] = 0;
   SLK_Pal_sprite *s = backend_load_pal_mem(data,size);
   backend_free(data);

   return s;
}

SLK_Pal_sprite *backend_load_pal_mem(const void *data, int length)
{
   HLH_slk *img = HLH_slk_image_load_mem_buffer(data,length);
   SLK_Pal_sprite *s = SLK_pal_sprite_create(img->width,img->height);
   memcpy(s->data,img->data,sizeof(*s->data)*s->width*s->height);
   HLH_slk_image_free(img);

   return s;
}

void backend_save_pal(const SLK_Pal_sprite *s, const char *path, int rle)
{
   FILE *f = fopen(path,"wb");

   if(f==NULL)
   {
      SLK_log("failed to open %s for writing",path);
      return;
   }

   backend_save_pal_file(s,f,rle);
      
   fclose(f);
}

void backend_save_pal_file(const SLK_Pal_sprite *s, FILE *f, int rle)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't write palette to disk");
      return;
   }

   HLH_slk img;
   img.width = s->width;
   img.height = s->height;
   img.data = (uint8_t *)s->data;
   HLH_slk_image_write(&img,f,rle);
}

SLK_Palette *backend_load_palette(const char *path)
{
   FILE *f = fopen(path,"r");
   if(f==NULL)
   {
      SLK_log("failed to open %s for reading",path);
      return NULL; 
   }

   SLK_Palette *palette = backend_load_palette_file(f);
   fclose(f);

   return palette;
}

void backend_save_palette(const char *path, const SLK_Palette *pal)
{
   FILE *f = fopen(path,"w");
   if(f==NULL)
   {
      SLK_log("failed to open %s for writing",path);
      return;
   }

   backend_save_palette_file(f,pal);

  fclose(f);
}

SLK_Palette *backend_load_palette_file(FILE *f)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't load palette");
      return NULL;
   }

   char buffer[512];
   int colors = 0,i,found;
   int r,g,b,a;

   SLK_Palette *palette = backend_malloc(sizeof(*palette));
   if(palette==NULL)
   {
      SLK_log("malloc of size %zu failed, out of memory!",sizeof(*palette));
      return NULL;
   }

   memset(palette,0,sizeof(*palette));
   for(i = 0;i<259&&fgets(buffer,512,f);i++)
   {
      if(i==2)
      {
         sscanf(buffer,"%d",&found);
         palette->used = found;
      }
      else if(i>2&&buffer[0]!='\0')
      {
         if(sscanf(buffer,"%d %d %d %d",&r,&g,&b,&a)!=4)
         {
            sscanf(buffer,"%d %d %d",&r,&g,&b);
            a = 255;
         }

         palette->colors[colors].rgb.r = r;
         palette->colors[colors].rgb.g = g;
         palette->colors[colors].rgb.b = b;
         palette->colors[colors].rgb.a = a;
         colors++;
      }
   }

   return palette;
}

void backend_save_palette_file(FILE *f, const SLK_Palette *pal)
{
   if(f==NULL)
   {
      SLK_log("file pointer is NULL, can't write palette to disk");
      return;
   }

   fprintf(f,"JASC-PAL\n0100\n%d\n",pal->used);
   for(int i = 0;i<pal->used;i++)
   {
      if(pal->colors[i].rgb.a!=255)
         fprintf(f,"%d %d %d %d\n",pal->colors[i].rgb.r,pal->colors[i].rgb.g,pal->colors[i].rgb.b,pal->colors[i].rgb.a);
      else
         fprintf(f,"%d %d %d\n",pal->colors[i].rgb.r,pal->colors[i].rgb.g,pal->colors[i].rgb.b);
   }
}

void *backend_system_malloc(size_t size)
{
   return malloc(size);
}

void backend_system_free(void *ptr)
{
   free(ptr);
}

void *backend_system_realloc(void *ptr, size_t size)
{
   return realloc(ptr,size);
}

void backend_set_malloc(void *(*func)(size_t size))
{
   bmalloc = func;
}

void backend_set_free(void (*func)(void *ptr))
{
   bfree = func;
}

void backend_set_realloc(void *(*func)(void *ptr, size_t size))
{
   brealloc = func;
}

void *backend_malloc(size_t size)
{
   return bmalloc(size);
}

void *backend_calloc(size_t num, size_t size)
{
   void *mem = backend_malloc(num*size);
   memset(mem,0,num*size);
   return mem;
}

void backend_free(void *ptr)
{
   bfree(ptr);
}

void *backend_realloc(void *ptr, size_t size)
{
   return brealloc(ptr,size);
}

void backend_log(const char *w, va_list v)
{
   vprintf(w,v);
}

#undef MAX_CONTROLLERS
