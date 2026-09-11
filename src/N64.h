#ifndef N64_H__
#define N64_H__

#include <libdragon.h>
#include <mikmod.h>
//#include <stdbool.h>
//#include <stdlib.h>


// #define True (1)
// #define False (0)
// #define true True;
// #define false False;

#define SDL_TRUE 1
#define SDL_FALSE 0
#define BREW 1
#define MAXTICKS 93750//(0x16000) 93750000

typedef char Sint8;
typedef unsigned char Uint8;
typedef short Sint16;
typedef unsigned short Uint16;
typedef long Sint32;
typedef unsigned long Uint32;
typedef long long Sint64;
typedef unsigned long long Uint64;

extern void* __safe_buffer[];
#define __get_buffer(x) __safe_buffer[(x)-1]
#define __set_pixel(buffer, x, y, color) (buffer)[(x) + ((y) * 320)] = color //__width
Uint16* fbuffer;
int openfiles;

struct controller_data gKeys;
struct SI_condat Pressed1;
struct SI_condat Down1;
int resettics;
bool consoledebug;

//AUDIO
#define sample_rate 8000
#define MONO_PCM_SAMPLE_SIZE sizeof(signed short)
#define STEREO_PCM_SAMPLE_SIZE (MONO_PCM_SAMPLE_SIZE << 1)
#define MIX_MAX_VOLUME 128 //100
#define GAMEID 0x4535

MODULE* Mod;

//RDP MODES
// enable palette (tlut)
#define EN_TLUT 0x00800000000000
// enable atomic prim, 1st primitive bandwitdh save
#define ATOMIC_PRIM 0x80000000000000
// enable perspective correction
#define PERSP_TEX_EN 0x08000000000000
// select alpha dither
#define ALPHA_DITHER_SEL_PATTERN 0x00000000000000
#define ALPHA_DITHER_SEL_PATTERNB 0x00001000000000
#define ALPHA_DITHER_SEL_NOISE 0x00002000000000
#define ALPHA_DITHER_SEL_NO_DITHER 0x00003000000000
// select rgb dither
#define RGB_DITHER_SEL_MAGIC_SQUARE_MATRIX 0x00000000000000
#define RGB_DITHER_SEL_STANDARD_BAYER_MATRIX 0x00004000000000
#define RGB_DITHER_SEL_NOISE 0x00008000000000
#define RGB_DITHER_SEL_NO_DITHER 0x0000C000000000
// enable texture filtering
#define SAMPLE_TYPE 0x00200000000000


typedef struct SDL_Point{
    int h;
    int v;
}SDL_Point;

typedef struct SDL_Rect{
    int x;
    int y;
    int w;
    int h;
}SDL_Rect;

typedef struct SDL_Window{
    int event;
    int windowID;
}SDL_Window;

typedef struct SDL_MessageBoxButtonData{
    int flags;
    int buttonid;
    char* text;
}SDL_MessageBoxButtonData;

typedef struct SDL_Wheel{
    int y;
}SDL_Wheel;

typedef struct RGBColour{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
}RGBColour;

typedef struct SDL_Palette{
    RGBColour* colors;
    int ncolors;
}SDL_Palette;

typedef struct SDL_PixelFormat{
    SDL_Palette* palette;
}SDL_PixelFormat;

typedef struct SDL_MessageBoxColorScheme{
    // RGBColour SDL_MESSAGEBOX_COLOR_BACKGROUND;
    // RGBColour SDL_MESSAGEBOX_COLOR_TEXT;
    // RGBColour SDL_MESSAGEBOX_COLOR_BUTTON_BORDER;
    // RGBColour SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND;
    // RGBColour SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED;
    RGBColour MBColours[5];
}SDL_MessageBoxColorScheme;

typedef struct SDL_MessageBoxData{
    	int flags;
		SDL_Window window;
		char* title;
		int message;
		int numbuttons;
		int buttons;
		RGBColour* colorScheme;
}SDL_MessageBoxData;

typedef struct SDL_Event{
    SDL_Window window;
    SDL_Wheel wheel;
    int what;
    int where;
    int type;
}SDL_Event;

#define SDL_MESSAGEBOX_ERROR (-1)
#define SDL_INIT_EVERYTHING (0)
#define SDL_BYTESPERPIXEL(val) (val)

enum{
    SDL_RENDERER_ACCELERATED
}SDL_RENDERER;

enum{
    SDL_SCANCODE_UP,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_A,
    SDL_SCANCODE_D,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_C,
    SDL_SCANCODE_TAB,
    SDL_SCANCODE_ESCAPE,
    SDL_SCANCODE_1,
    SDL_SCANCODE_0,
    SDL_SCANCODE_KP_1,
    SDL_SCANCODE_KP_0
}SDL_SCANCODE;
#define SDL_NUM_SCANCODES 16

enum{
    SDL_CONTROLLER_NO_INPUT, //new
    SDL_CONTROLLER_BUTTON_A,
    SDL_CONTROLLER_BUTTON_B,
    SDL_CONTROLLER_BUTTON_X,
    SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK,
    SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSTICK,
    SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_DPAD_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
}SDL_CONTROLLER_BUTTON;

enum{
    SDL_CONTROLLER_AXIS_TRIGGERLEFT,
    SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
    SDL_CONTROLLER_AXIS_LEFTY,
    SDL_CONTROLLER_AXIS_LEFTX,
    SDL_CONTROLLER_AXIS_RIGHTY,
    SDL_CONTROLLER_AXIS_RIGHTX
}SDL_CONTROLLER_AXIS;

enum{
    SDL_BUTTON_LMASK,
    SDL_BUTTON_MMASK,
    SDL_BUTTON_RMASK,
    SDL_BUTTON_X1MASK,
    SDL_BUTTON_X2MASK
}SDL_BUTTON_MASK;

enum{
    SDL_MOUSEBUTTONUP,
    SDL_MOUSEBUTTONDOWN,
    SDL_MOUSEWHEEL,
    SDL_MOUSEMOTION,
    SDL_WINDOWEVENT,
    SDL_WINDOWEVENT_RESIZED,
    SDL_WINDOWEVENT_CLOSE,
    SDL_QUIT
}SDL_EVENT;

enum{
    SDL_WINDOW_OPENGL,
    SDL_WINDOW_SHOWN,
    SDL_WINDOW_RESIZABLE,
    SDL_WINDOW_ALLOW_HIGHDPI,
    SDL_WINDOWPOS_UNDEFINED
}SDL_WINDOW_FLAGS;

enum{
    SDL_HINT_RENDER_DRIVER,    
    SDL_HINT_RENDER_VSYNC,
    SDL_HINT_NO_SIGNAL_HANDLERS,
    SDL_HINT_RENDER_SCALE_QUALITY,
    SDL_HINT_JOYSTICK_RAWINPUT
}SDL_HINT;

enum{
    SDL_BLENDMODE_NONE,
    SDL_BLENDMODE_BLEND
}SDL_BLENDMODE;

enum{
    SDL_PIXELFORMAT_RGB565 = 2
}SDL_PIXELFORMAT;

enum{
    SDL_TEXTUREACCESS_STREAMING
}SDL_TEXTUREACCESS;


typedef Uint16* SDL_Renderer;
typedef struct SDL_Texture{
    Uint16 w;
    Uint16 h;
    Uint16* data;
}SDL_Texture;

typedef struct SDL_Surface{
    SDL_Texture* imgBitmap;
    SDL_PixelFormat* format;
    Uint16 w;
    Uint16 h;
    Uint16 compression;
    Uint16 bitsperpixel;
    Uint8* data;
    Uint32 size;
}SDL_Surface;

typedef unsigned char SDL_RWops;
typedef int SDL_GameController;
typedef int SDL_Joystick;
typedef int SDL_Haptic;

int SDL_Init();
void SDL_Quit();

char test[50];
void consolevisible(int bol);
void testsub(const char* str);
void getstate(const char* state);
void getpad();

void rdp_setdeblur(int option);
void rdpsquare(int y1, int y2, Uint16 color);
void rdp_test(Uint16* renderer);
void rdp_loadtest(Uint16* paldata, Uint8* pixdata, int pixdataoffset);
void rdp_coltest(Uint16* paldata, Uint8* pixdata, int pixdataoffset, int x, int y1, int height, int scale);
void rdp_drawspan2(int texcol, int dc_x, int dc_yl, int centery, int dc_yh, double dc_iscale, int dc_texturemid);
void testchannel(int chan, int size);
void SetFPSCounter(int option);

SDL_Window* SDL_CreateWindow(const char* title, int posx, int posy, int w, int h, int flags);
void SDL_SetWindowMinimumSize(SDL_Window* Window, int w, int h);
void SDL_GetWindowSize(SDL_Window* Window, int* w, int* h);
void SDL_SetWindowFullscreen(SDL_Window* Window, int flag);
void SDL_WarpMouseInWindow(SDL_Window* Window, int* posx, int* posy);
void SDL_DestroyWindow(SDL_Window* Window);

Uint32 SDL_PollEvent(SDL_Event* ev);
Uint16* SDL_CreateRenderer(SDL_Window* window, int val, int flag);
void SDL_RenderSetLogicalSize(SDL_Renderer* renderer, int w, int h);
void SDL_SetRenderDrawColor(SDL_Renderer renderer, int r, int g, int b, int a);
Uint32 SDL_MapRGB(int* format, int r, int g, int b);
void SDL_SetColorKey(SDL_Surface* Surface, int flag, Uint32 colorkey);
void SDL_RenderSetIntegerScale(Uint16* renderer, int scale);
void SDL_RenderCopy(Uint16* renderer, SDL_Texture* image, SDL_Rect* cliprect, SDL_Rect* renderrect);
void SDL_RenderCopy2(Uint16* renderer, SDL_Texture* image, SDL_Rect* cliprect, SDL_Rect* renderrect);
void SDL_RenderPresent(Uint16* renderer);
void SDL_RenderClear(Uint16* renderer);
void SDL_DrawTexturedTile(Uint16* imgBitmap, int xDst, int yDst, int width, int height, int flags); //new
void SDL_DrawTexturedHTiles(SDL_Texture* imgBitmap, int width, int height, int xDst, int yDst); //new
void SDL_SetRenderDrawBlendMode(Uint16* renderer, int blend);
void SDL_RenderSetClipRect(Uint16* renderer, SDL_Rect* cliprect);
void SDL_RenderFillRect(Uint16* renderer, SDL_Rect* cliprect);
void SDL_RenderFillRect2(Uint16* renderer, SDL_Rect* cliprect);
void SDL_RenderDrawRect(Uint16* renderer, SDL_Rect* cliprect);
void SDL_RenderDrawPoints(Uint16* renderer, SDL_Point* points, int count);
void SDL_RenderDrawLine(Uint16* renderer, int x1, int y1, int x2, int y2);
void SDL_DestroyRenderer(Uint16* renderer);

Uint64 SDL_GetTicks();
Uint16 GetShort(Uint8* data);
Uint32 GetLong(Uint8* data);
Uint8 SDL_SwapLE8(Uint8 val);
Uint16 SDL_SwapLE16(Uint16 val);
Uint32 SDL_SwapLE32(Uint32 val);

Uint8* SDL_malloc(int size);
Uint8* SDL_calloc(int Count, int size);
Uint8* SDL_realloc(Uint8* Bloc, int size);
void SDL_free(void* data);
void freedata(int size); //new

void SDL_memcpy(Uint8* Dest, Uint8* Src, int size);
int SDL_memcmp(Uint8* Buf1, Uint8* Buf2, int size);
Uint8* SDL_memset(void* data, int value, int size);
void SDL_memmove(Uint8* dest, Uint8* src, int size);

int SDL_strlen(const char* str);
int SDL_strcmp(char* str1, char* str2);
char* SDL_strchr(char* str, int val);
int SDL_strcasecmp(char* str1, char* str2);
int SDL_atoi(const char* str);
#define SDL_snprintf snprintf
//void SDL_snprintf(char* str, int size, char* format);

//SDL Files
SDL_Surface* SDL_LoadBMP_RW(SDL_RWops filename , int bol);
SDL_RWops SDL_RWFromFile(const char* filename, char* setting);
Uint8* SDL_RWFromMem(Uint8* data, int size);
void SDL_RWread(Uint8* buf, Uint8* ret, int size, int count);
void SDL_RWseek(SDL_RWops handle, int Offset, int Origin );
int SDL_RWsize(SDL_RWops handle);
void SDL_RWwrite(SDL_RWops* handle, Uint8* data, int size, int val1);
void SDL_RWclose(SDL_RWops handle);

//SDL images
SDL_Texture* SDL_CreateTexture(Uint16* SDL_Renderer, int PixelFormat, int flag, int w, int h);
SDL_Texture* SDL_CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface);
SDL_Texture* SDL_CreateTiledTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface, int spritewidth, int spriteheight); //new
void SDL_UpdateTexture(SDL_Texture* piDIB , SDL_Rect* rect, Uint16* buffer, int width );
void SDL_UpdateTexture2(Uint16* piDIB , SDL_Rect* rect, Uint16* buffer, int width );
void SDL_DestroyTexture(SDL_Texture* tex);
void SDL_FreeSurface(SDL_Surface* Surface);

//SDL Logging
void SDL_SetHint(int option, const char* setting);
void SDL_ShowMessageBox(void* msgbox, void* ptr);
char* SDL_GetError();
void SDL_Log(const char* str, int id);

//SDL Controller
int SDL_arraysize(int* buttons);
Uint8* SDL_GetKeyboardState(void* keyboard);
char* SDL_GetScancodeName(int scancode);
void SDL_SetRelativeMouseMode(int bol);
int SDL_GetMouseState(void* mouse, void* ptr);
void SDL_GetRelativeMouseState(int* x, int* y);
int SDL_JoystickOpen(int i);
int SDL_NumJoysticks();
int SDL_JoystickIsHaptic(int Joystick);
int SDL_JoystickGetButton(Uint8* JoyStick, int i);
int SDL_JoystickGetAxis(Uint8* JoyStick, int i);
int SDL_JoystickNumAxes(Uint8* JoyStick);
void SDL_JoystickClose(int joystick);
int SDL_IsGameController(int i);
SDL_GameController* SDL_GameControllerOpen(int i);
int SDL_GameControllerGetButton(Uint8* Controller, int i);
int SDL_GameControllerGetAxis(Uint8* Controller, int i);
void SDL_GameControllerClose(int controller);
int SDL_GameControllerHasRumble(int i);
void SDL_GameControllerRumble(int id, Uint16 val1, Uint16 val2, Uint16 val3);
int SDL_HapticRumbleInit(int Joystick);
int SDL_HapticOpenFromJoystick(int Joystick);
void SDL_HapticRumblePlay(int haptic, Uint16 val1, Uint16 val2);
void SDL_HapticRumbleStop(int haptic);
void SDL_HapticClose(int haptic);
//void exit(int i);
#endif