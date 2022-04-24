#include <time.h>
#define WIN32_EXTRA_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <GL/gl.h>

#define CUTE_PNG_IMPLEMENTATION
#define CUTE_PNG_ALLOC backend_malloc
#define CUTE_PNG_CALLOC backend_calloc
#define CUTE_PNG_FREE backend_free
#include "../external/cute_png.h"

#define HLH_SLK_IMPLEMENTATION
#define HLH_SLK_MALLOC backend_malloc
#define HLH_SLK_FREE backend_free
#include "../external/HLH_slk.h"

typedef BOOL(WINAPI wglSwapInterval_t) (int interval);

static int pixel_scale;
static int screen_width;
static int screen_height;
static int window_width;
static int window_height;
static int view_x;
static int view_y;
static int view_width;
static int view_height;
static HWND window;
static int mouse_x_rel;
static int mouse_y_rel;
static int mouse_x;
static int mouse_y;
static int mouse_x_ev;
static int mouse_y_ev;
static int mouse_wheel;
uint8_t new_key_state[256];
uint8_t old_key_state[256];
uint8_t key_map[256];
uint8_t new_mouse_state[5];
uint8_t old_mouse_state[5];
static HDC gl_device_context;
static GLuint *layer_textures;
static int fps;
static int framedelay;
static float delta;
static int mouse_relative = 0;

static void *(*bmalloc)(size_t size) = backend_system_malloc;
static void (*bfree)(void *ptr) = backend_system_free;
static void *(*brealloc)(void *ptr, size_t size) = backend_system_realloc;

static LRESULT CALLBACK window_event(HWND win, UINT message, WPARAM wParam, LPARAM lParam);

//Set the window title.
void backend_set_title(const char *title)
{

}

//Toggles fullscreen.
void backend_set_fullscreen(int fullscreen)
{

}

//(should) center the viewport.
void backend_update_viewport()
{
   RECT rect;
   if(GetClientRect(window,&rect))
   {
      window_width = rect.right-rect.left;
      window_height = rect.bottom-rect.top;
   }

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

   glViewport(view_x,view_y,view_width,view_height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0,screen_width,screen_height,0,1.0,-1.0);
}

//Sets wether the window is visible.
void backend_set_visible(int visible)
{

}

//Sets the window icon.
void backend_set_icon(const SLK_RGB_sprite *icon)
{

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
   static clock_t time_start;
   static clock_t time_end;

   time_end = ((clock()*1000)/CLOCKS_PER_SEC);
   int difference = time_end-time_start;
   struct timespec ts = {};
   ts.tv_nsec = (framedelay-difference)*1000000;
   if(framedelay>difference)
      nanosleep(&ts,NULL);
   delta = (float)(((clock()*1000)/CLOCKS_PER_SEC)-time_start)/1000.0f;
   time_start = ((clock()*1000)/CLOCKS_PER_SEC);
}

//Returns the delta time of the last frame.
float backend_timer_get_delta()
{
   return delta;
}

//Handles window and input events.
void backend_handle_events()
{
   memcpy(old_key_state,new_key_state,sizeof(new_key_state));
   memcpy(old_mouse_state,new_mouse_state,sizeof(new_mouse_state));
   mouse_wheel = 0;

   MSG message;
   while(PeekMessage(&message,0,0,0,PM_REMOVE))
   {
      TranslateMessage(&message);
      DispatchMessage(&message);
   }

   POINT p;
   p.x = mouse_x_ev;
   p.y = mouse_y_ev;
   p.x-=view_x;
   p.y-=view_y;

   p.x = p.x/pixel_scale;
   p.y = p.y/pixel_scale;

   if(p.x>=screen_width)
      p.x = screen_width-1;
   if(p.y>=screen_height)
      p.y = screen_height-1;

   if(p.x<0)
      p.x = 0;
   if(p.y<0)
      p.y = 0;

   mouse_x_rel = p.x-mouse_x;
   mouse_y_rel = p.y-mouse_y;

   mouse_x = p.x;
   mouse_y = p.y;

   if(mouse_relative)
   {
      POINT pt = {.x = window_width/2, .y = window_height/2};
      ClientToScreen(window,&pt);
      SetCursorPos(pt.x,pt.y);

      pt.x = window_width/2;
      pt.y = window_height/2;
      pt.x-=view_x;
      pt.y-=view_y;
      pt.x = pt.x/pixel_scale;
      pt.y = pt.y/pixel_scale;

      if(pt.x>=screen_width)
         pt.x = screen_width-1;
      if(pt.y>=screen_height)
         pt.y = screen_height-1;

      if(pt.x<0)
         pt.x = 0;
      if(pt.y<0)
         pt.y = 0;
      mouse_x = pt.x;
      mouse_y = pt.y;
   }
}

//Creates the window, etc.
void backend_setup(int width, int height, int layer_num, const char *title, int fullscreen, int scale, int resizable)
{
   ULONG_PTR token;
   GdiplusStartupInput startup_input;
   wglSwapInterval_t *wglSwapInterval;
   HGLRC gl_render_context;

   pixel_scale = scale;
   screen_width = width;
   screen_height = height;
   layer_count = layer_num;
   layer_dynamic = resizable;

   startup_input.GdiplusVersion = 1;
   startup_input.DebugEventCallback = NULL;
   startup_input.SuppressBackgroundThread = FALSE;
   uint16_t error = GdiplusStartup(&token,&startup_input,NULL);
   if(error!=0)
      printf("GdiplusStartup failed, error code: %d\n",error);

   if(pixel_scale==SLK_WINDOW_MAX)
   {
      HMONITOR hmon = MonitorFromWindow(window,MONITOR_DEFAULTTONEAREST);
      if(!hmon)
         printf("Failed to get monitor from window: %ld\n",GetLastError());
      MONITORINFO mi = {.cbSize = sizeof(mi)};
      if(GetMonitorInfo(hmon, &mi))
      {
         int max_x,max_y;

         max_x = mi.rcMonitor.right/screen_width;
         max_y = mi.rcMonitor.bottom/screen_height;

         pixel_scale = (max_x>max_y)?max_y:max_x;
      }
      else
      {
         printf("Failed to get monitor info: %ld\n",GetLastError());
      }
   }

   if(pixel_scale<=0)
      pixel_scale = 1;

   WNDCLASS wc = {};
   wc.hCursor = LoadCursor(NULL,IDC_ARROW);
   wc.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
   wc.lpfnWndProc = window_event;
   wc.hInstance = GetModuleHandle(NULL);
   wc.lpszClassName = title;

   if(!RegisterClass(&wc))
      printf("Failed to register window class: %ld\n",GetLastError());

   window_width = screen_width*pixel_scale;
   window_height = screen_height*pixel_scale;

   DWORD dwExStyle = 0;
   DWORD dwStyle = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_MINIMIZEBOX;

   view_height = window_height;
   view_width = window_width;
   window = NULL;

   if(fullscreen)
   {
      HMONITOR hmon = MonitorFromWindow(window,MONITOR_DEFAULTTONEAREST);
      if(!hmon)
         printf("Failed to get monitor from window: %ld\n",GetLastError());
      MONITORINFO mi = {.cbSize = sizeof(mi)};
      if(GetMonitorInfo(hmon, &mi))
      {
         dwExStyle = 0;
         dwStyle = WS_VISIBLE|WS_POPUP;
         window_width = mi.rcMonitor.right;
         window_height = mi.rcMonitor.bottom;
      }
      else
      {
         printf("Failed to get monitor info: %ld\n",GetLastError());
      }
   }

   RECT window_rect = {0,0,window_width,window_height};
   AdjustWindowRectEx(&window_rect, dwStyle, FALSE, dwExStyle);
   width = window_rect.right-window_rect.left;
   height = window_rect.bottom-window_rect.top;

   window = CreateWindowEx(dwExStyle,title,title,dwStyle,CW_USEDEFAULT,
   CW_USEDEFAULT,width,height,NULL,NULL,
   GetModuleHandle(NULL),NULL);

   if(window==NULL)
      printf("Failed to create window: %ld\n",GetLastError());

   gl_device_context = GetDC(window);

   PIXELFORMATDESCRIPTOR pfd =
   {
      sizeof(PIXELFORMATDESCRIPTOR),1,
      PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
      PFD_MAIN_PLANE,0,0,0,0
   };

   int pf = 0;
   pf = ChoosePixelFormat(gl_device_context,&pfd);
   if(!pf)
      printf("Failed to choose pixel format\n");
   SetPixelFormat(gl_device_context,pf,&pfd);

   gl_render_context = wglCreateContext(gl_device_context);
   if(!gl_render_context)
      printf("Failed to create openGL context\n");
   wglMakeCurrent(gl_device_context,gl_render_context);

   glViewport(view_x,view_y,view_width,view_height);

   wglSwapInterval = (wglSwapInterval_t*)wglGetProcAddress("wglSwapIntervalEXT");
   if(wglSwapInterval)
      wglSwapInterval(0);

   glEnable(GL_TEXTURE_2D);
   glViewport(0,0,screen_width,screen_height);
   glClearColor(0.0f,0.0f,0.0f,0.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   layer_textures = backend_malloc(sizeof(*layer_textures)*layer_num);
   memset(layer_textures,0,sizeof(*layer_textures)*layer_num);

   backend_update_viewport();
}

//Creates the keymap.
void backend_input_init()
{
   key_map[0x00] = SLK_KEY_NONE;
   key_map[0x41] = SLK_KEY_A;
   key_map[0x42] = SLK_KEY_B;
   key_map[0x43] = SLK_KEY_C;
   key_map[0x44] = SLK_KEY_D;
   key_map[0x45] = SLK_KEY_E;
   key_map[0x46] = SLK_KEY_F;
   key_map[0x47] = SLK_KEY_G;
   key_map[0x48] = SLK_KEY_H;
   key_map[0x49] = SLK_KEY_I;
   key_map[0x4A] = SLK_KEY_J;
   key_map[0x4B] = SLK_KEY_K;
   key_map[0x4C] = SLK_KEY_L;
   key_map[0x4D] = SLK_KEY_M;
   key_map[0x4E] = SLK_KEY_N;
   key_map[0x4F] = SLK_KEY_O;
   key_map[0x50] = SLK_KEY_P;
   key_map[0x51] = SLK_KEY_Q;
   key_map[0x52] = SLK_KEY_R;
   key_map[0x53] = SLK_KEY_S;
   key_map[0x54] = SLK_KEY_T;
   key_map[0x55] = SLK_KEY_U;
   key_map[0x56] = SLK_KEY_V;
   key_map[0x57] = SLK_KEY_W;
   key_map[0x58] = SLK_KEY_X;
   key_map[0x59] = SLK_KEY_Y;
   key_map[0x5A] = SLK_KEY_Z;

   key_map[VK_F1] = SLK_KEY_F1;
   key_map[VK_F2] = SLK_KEY_F2;
   key_map[VK_F3] = SLK_KEY_F3;
   key_map[VK_F4] = SLK_KEY_F4;
   key_map[VK_F5] = SLK_KEY_F5;
   key_map[VK_F6] = SLK_KEY_F6;
   key_map[VK_F7] = SLK_KEY_F7;
   key_map[VK_F8] = SLK_KEY_F8;
   key_map[VK_F9] = SLK_KEY_F9;
   key_map[VK_F10] = SLK_KEY_F10;
   key_map[VK_F11] = SLK_KEY_F11;
   key_map[VK_F12] = SLK_KEY_F12;

   key_map[VK_DOWN] = SLK_KEY_DOWN;
   key_map[VK_LEFT] = SLK_KEY_LEFT;
   key_map[VK_RIGHT] = SLK_KEY_RIGHT;
   key_map[VK_UP] = SLK_KEY_UP;
   key_map[VK_RETURN] = SLK_KEY_ENTER;

   key_map[VK_BACK] = SLK_KEY_BACK;
   key_map[VK_ESCAPE] = SLK_KEY_ESCAPE;
   key_map[VK_RETURN] = SLK_KEY_ENTER;
   key_map[VK_PAUSE] = SLK_KEY_PAUSE;
   key_map[VK_SCROLL] = SLK_KEY_SCROLL;
   key_map[VK_TAB] = SLK_KEY_TAB;
   key_map[VK_DELETE] = SLK_KEY_DEL;
   key_map[VK_HOME] = SLK_KEY_HOME;
   key_map[VK_END] = SLK_KEY_END;
   key_map[VK_PRIOR] = SLK_KEY_PGUP;
   key_map[VK_NEXT] = SLK_KEY_PGDN;
   key_map[VK_INSERT] = SLK_KEY_INS;
   key_map[VK_SHIFT] = SLK_KEY_SHIFT;
   key_map[VK_CONTROL] = SLK_KEY_CTRL;
   key_map[VK_SPACE] = SLK_KEY_SPACE;

   key_map[0x30] = SLK_KEY_0;
   key_map[0x31] = SLK_KEY_1;
   key_map[0x32] = SLK_KEY_2;
   key_map[0x33] = SLK_KEY_3;
   key_map[0x34] = SLK_KEY_4;
   key_map[0x35] = SLK_KEY_5;
   key_map[0x36] = SLK_KEY_6;
   key_map[0x37] = SLK_KEY_7;
   key_map[0x38] = SLK_KEY_8;
   key_map[0x39] = SLK_KEY_9;

   key_map[VK_NUMPAD0] = SLK_KEY_NP0;
   key_map[VK_NUMPAD1] = SLK_KEY_NP1;
   key_map[VK_NUMPAD2] = SLK_KEY_NP2;
   key_map[VK_NUMPAD3] = SLK_KEY_NP3;
   key_map[VK_NUMPAD4] = SLK_KEY_NP4;
   key_map[VK_NUMPAD5] = SLK_KEY_NP5;
   key_map[VK_NUMPAD6] = SLK_KEY_NP6;
   key_map[VK_NUMPAD7] = SLK_KEY_NP7;
   key_map[VK_NUMPAD8] = SLK_KEY_NP8;
   key_map[VK_NUMPAD9] = SLK_KEY_NP9;
   key_map[VK_MULTIPLY] = SLK_KEY_NP_MUL;
   key_map[VK_ADD] = SLK_KEY_NP_ADD;
   key_map[VK_DIVIDE] = SLK_KEY_NP_DIV;
   key_map[VK_SUBTRACT] = SLK_KEY_NP_SUB;
   key_map[VK_DECIMAL] = SLK_KEY_NP_DECIMAL;
}

//Shows or hides the mouse cursor.
void backend_show_cursor(int shown)
{
   ShowCursor(shown?TRUE:FALSE);
}

//Sets wether the mouse cursor is captured and only relative
//mouse motion is registerd.
void backend_mouse_set_relative(int relative)
{
   if(relative)
   {
      backend_show_cursor(0);
      mouse_relative = 1;
   }
   else
   {
      backend_show_cursor(1);
      mouse_relative = 0;
   }
}

//Sets wether to track mouse events globally.
void backend_mouse_capture(int capture)
{

}

//Starts text input.
void backend_start_text_input(char *text, int max_length)
{

}

//Stops text input.
void backend_stop_text_input()
{

}


//Clears the window and redraws the scene.
//Drawing is performed from back to front, layer 0 is always drawn last.
void backend_render_update()
{
   glClear(GL_COLOR_BUFFER_BIT);
   //glViewport(view_x,view_y,view_width,view_height);

   for(int l = layer_count-1;l>=0;l--)
   {
      layers[l].resized = 0;

      if(layers[l].active)
      {
         switch(layers[l].type)
         {
         case SLK_LAYER_PAL:
         {
            float width = (float)layers[l].as.type_0.target->width*layers[l].scale;
            float height = (float)layers[l].as.type_0.target->height*layers[l].scale;
            float x = (float)layers[l].x;
            float y = (float)layers[l].y;

            for(int i = 0;i<layers[l].as.type_0.render->width*layers[l].as.type_0.render->height;i++)
               layers[l].as.type_0.render->data[i] = layers[l].as.type_0.palette->colors[layers[l].as.type_0.target->data[i]];

            glBindTexture(GL_TEXTURE_2D,layer_textures[l]);
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,layers[l].as.type_0.render->width,layers[l].as.type_0.render->height,0,GL_RGBA,GL_UNSIGNED_BYTE,layers[l].as.type_0.render->data);

            glBegin(GL_QUADS);
               glColor4ub(layers[l].tint.rgb.r,layers[l].tint.rgb.g,layers[l].tint.rgb.b,layers[l].tint.rgb.a);
               glTexCoord2i(0,0);
               glVertex3f(x,y,0.0f);
               glTexCoord2i(0,1);
               glVertex3f(x,y+height,0.0f);
               glTexCoord2f(1,1);
               glVertex3f(width+x,y+height,0.0f);
               glTexCoord2f(1,0);
               glVertex3f(width+x,y,0.0f);
            glEnd();

            break;
         }
         case SLK_LAYER_RGB:
         {
            float width = (float)layers[l].as.type_1.target->width*layers[l].scale;
            float height = (float)layers[l].as.type_1.target->height*layers[l].scale;
            float x = (float)layers[l].x;
            float y = (float)layers[l].y;

            glBindTexture(GL_TEXTURE_2D,layer_textures[l]);
            if(layers[l].as.type_1.target->changed)
            {
               glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,layers[l].as.type_1.target->width,layers[l].as.type_1.target->height,0,GL_RGBA,GL_UNSIGNED_BYTE,layers[l].as.type_1.target->data);
               layers[l].as.type_1.target->changed = 0;
            }

            glBegin(GL_QUADS);
               glColor4ub(layers[l].tint.rgb.r,layers[l].tint.rgb.g,layers[l].tint.rgb.b,layers[l].tint.rgb.a);
               glTexCoord2i(0,0);
               glVertex3f(x,y,0.0f);
               glTexCoord2i(0,1);
               glVertex3f(x,y+height,0.0f);
               glTexCoord2f(1,1);
               glVertex3f(width+x,y+height,0.0f);
               glTexCoord2f(1,0);
               glVertex3f(width+x,y,0.0f);
            glEnd();

            break;
         }
         }
      }
   }

   SwapBuffers(gl_device_context);
}

void backend_create_layer(unsigned index, int type)
{
   if(index>=layer_count)
      return;

   switch(type)
   {
   case SLK_LAYER_PAL:
      glGenTextures(1,&layer_textures[index]);
      glBindTexture(GL_TEXTURE_2D,layer_textures[index]);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,screen_width,screen_height,
                   0,GL_RGBA,GL_UNSIGNED_BYTE,layers[index].as.type_0.render->data);
      break;
   case SLK_LAYER_RGB:
      glGenTextures(1,&layer_textures[index]);
      glBindTexture(GL_TEXTURE_2D,layer_textures[index]);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,screen_width,screen_height,
                   0,GL_RGBA,GL_UNSIGNED_BYTE,layers[index].as.type_1.target->data);
      break;
   }
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
   return 0;
}

int backend_gamepad_pressed(int index, int key)
{
   return 0;
}

int backend_gamepad_released(int index, int key)
{
   return 0;
}

SLK_Button backend_gamepad_get_state(int index, int key)
{
   return (SLK_Button){0};
}

int backend_get_gamepad_count()
{
   return 0;
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

SLK_RGB_sprite *backend_load_rgb(const char *path)
{
   cp_image_t img = cp_load_png(path);
   SLK_RGB_sprite *out = NULL;
   if(img.pix==0)
   {
      printf("Failed to load %s\n",path);
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
   if(img.pix==0)
   {
      puts("Failed to load png from mem");
      return NULL;
   }

   out = SLK_rgb_sprite_create(img.w,img.h);
   memcpy(out->data,img.pix,img.w*img.h*sizeof(*out->data));
   cp_free_png(&img);

   return out;
}


void backend_save_rgb(const SLK_RGB_sprite *s, const char *path)
{
   if(!path)
      return;
   FILE *f = fopen(path,"wb");

   backend_save_rgb_file(s,f);

   fclose(f);
}

void backend_save_rgb_file(const SLK_RGB_sprite *s, FILE *f)
{
   if(!f)
      return;
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
      printf("Failed to load %s\n",path);
      return NULL;
   }
   SLK_Pal_sprite *out = backend_load_pal_file(f);
   fclose(f);
   return out;
}

SLK_Pal_sprite *backend_load_pal_file(FILE *f)
{
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

   if(!f)
      return;

   backend_save_pal_file(s,f,rle);
      
   fclose(f);
}

void backend_save_pal_file(const SLK_Pal_sprite *s, FILE *f, int rle)
{
   HLH_slk img;
   img.width = s->width;
   img.height = s->height;
   img.data = (uint8_t *)s->data;
   HLH_slk_image_write(&img,f,rle);
}


SLK_Palette *backend_load_palette(const char *path)
{
   FILE *f = fopen(path,"r");
   SLK_Palette *palette = backend_load_palette_file(f);
   fclose(f);
   return palette;
}

void backend_save_palette(const char *path, const SLK_Palette *pal)
{
   FILE *f = fopen(path,"w");
   backend_save_palette_file(f,pal);
  fclose(f);
}

SLK_Palette *backend_load_palette_file(FILE *f)
{
   if(!f)
      return NULL;

   char buffer[512];
   int colors = 0,i,found;
   int r,g,b,a;

   SLK_Palette *palette = backend_malloc(sizeof(*palette));
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
   if(!f)
      return;
   fprintf(f,"JASC-PAL\n0100\n%d\n",pal->used);
   for(int i = 0;i<pal->used;i++)
   {
      if(pal->colors[i].rgb.a!=255)
         fprintf(f,"%d %d %d %d\n",pal->colors[i].rgb.r,pal->colors[i].rgb.g,pal->colors[i].rgb.b,pal->colors[i].rgb.a);
      else
         fprintf(f,"%d %d %d\n",pal->colors[i].rgb.r,pal->colors[i].rgb.g,pal->colors[i].rgb.b);
   }
}

static LRESULT CALLBACK window_event(HWND win, UINT message, WPARAM wParam, LPARAM lParam)
{
   LRESULT result = 0;

   switch(message)
   {
   case WM_CLOSE:
      {
         SLK_core_quit();
      }
      break;
   case WM_DESTROY:
      {
         SLK_core_quit();
      }
      break;
   case WM_SIZE:
      //window_width = LOWORD(lParam);
      //window_height = HIWORD(lParam);
      if(layer_dynamic)
      {
         screen_width = window_width/pixel_scale+1;
         screen_height = window_height/pixel_scale+1;
      }
      for(int l = 0;l<layer_count;l++)
      {
         if(layers!=NULL&&layers[l].dynamic)
            SLK_layer_set_size(l,screen_width,screen_height);
      }
      backend_update_viewport();
      break;
   case WM_SYSKEYDOWN:
   case WM_KEYDOWN:
      new_key_state[key_map[wParam]] = 1;
      break;
   case WM_SYSKEYUP:
   case WM_KEYUP:
      new_key_state[key_map[wParam]] = 0;
      break;
   case WM_LBUTTONDOWN:
      new_mouse_state[SLK_BUTTON_LEFT] = 1;
      break;
   case WM_LBUTTONUP:
      new_mouse_state[SLK_BUTTON_LEFT] = 0;
      break;
   case WM_RBUTTONDOWN:
      new_mouse_state[SLK_BUTTON_RIGHT] = 1;
      break;
   case WM_RBUTTONUP:
      new_mouse_state[SLK_BUTTON_RIGHT] = 0;
      break;
   case WM_MBUTTONDOWN:
      new_mouse_state[SLK_BUTTON_MIDDLE] = 1;
      break;
   case WM_MBUTTONUP:
      new_mouse_state[SLK_BUTTON_MIDDLE] = 0;
      break;
   case WM_XBUTTONDOWN:
      if(GET_XBUTTON_WPARAM (wParam)==XBUTTON1)
         new_mouse_state[SLK_BUTTON_X1] = 1;
      else
         new_mouse_state[SLK_BUTTON_X2] = 1;
      break;
   case WM_XBUTTONUP:
      if(GET_XBUTTON_WPARAM (wParam)==XBUTTON1)
         new_mouse_state[SLK_BUTTON_X1] = 0;
      else
         new_mouse_state[SLK_BUTTON_X2] = 0;
      break;
   case WM_MOUSEWHEEL:
      mouse_wheel = GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA;
      break;
   case WM_MOUSEMOVE:
      mouse_x_ev = GET_X_LPARAM(lParam);
      mouse_y_ev = GET_Y_LPARAM(lParam);
      break;
   default:
      result = DefWindowProc(win,message,wParam,lParam);
      break;
   }

   return result;
}

void *backend_system_malloc(size_t size)
{
   return HeapAlloc(GetProcessHeap(),0,size);
}

void backend_system_free(void *ptr)
{
   HeapFree(GetProcessHeap(),0,ptr);
}

void *backend_system_realloc(void *ptr, size_t size)
{
   return HeapReAlloc(GetProcessHeap(),0,ptr,size);
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
