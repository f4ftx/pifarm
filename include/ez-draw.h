/*
 * ez-draw.h: basic EZ-Draw module
 *
 * Edouard.Thiel@lif.univ-mrs.fr - 07/07/2013 - version 1.2
 *
 * This program is free software under the terms of the
 * GNU Lesser General Public License (LGPL) version 2.1.
*/

#ifndef EZ_DRAW__H
#define EZ_DRAW__H

#ifndef _WIN32
#define EZ_BASE_XLIB 1
#else
#define EZ_BASE_WIN32 1
#endif

/* To let vsnprintf be compatible with -ansi */
#if defined __STRICT_ANSI__ && !defined _GNU_SOURCE
#define _GNU_SOURCE
#include <stdio.h>
#undef _GNU_SOURCE
#else
#include <stdio.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifdef EZ_BASE_XLIB

#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/keysym.h>
#include <X11/extensions/Xdbe.h>

#elif defined EZ_BASE_WIN32

#ifdef __MINGW32__  /* MinGW underestimate Windows version */
#  undef WINVER
#  define WINVER 0x0501
#endif

#include <windows.h>
#include <windowsx.h>
#include <wingdi.h>

#endif /* EZ_BASE_ */

/* Miscellaneous constants */
#define EZ_FONT_MAX    16
#define EZ_WIN_MAX   1024

typedef   signed char  Ez_int8;
typedef unsigned char  Ez_uint8;
typedef unsigned short Ez_uint16;
typedef   signed short Ez_int16;
typedef unsigned int   Ez_uint32;
typedef   signed int   Ez_int32;
typedef unsigned int   Ez_uint;

/* Produce a compiler error if size is wrong */
typedef Ez_uint8 Ez_validate_uint32[sizeof (Ez_uint32)==4 ? 1 : -1];

#define EZ_ROUND(x) ((x)<0 ? (int)((x)-0.5) : (int)((x)+0.5))

/* Type Ez_window */
#ifdef EZ_BASE_XLIB
typedef Window Ez_window;
#elif defined EZ_BASE_WIN32
typedef HWND Ez_window;
#endif /* EZ_BASE_ */

/* Colors */
extern Ez_uint32 ez_black, ez_white, ez_grey, ez_red, ez_green, ez_blue,
              ez_yellow, ez_cyan, ez_magenta;

#ifdef EZ_BASE_XLIB
/* Colors handling on X11 */
typedef struct {
    Ez_uint32 mask, shift, length, max;
} Ez_channel;

typedef struct {
    Ez_channel green, red, blue;
} Ez_TrueColor;

typedef struct {
    Colormap colormap;
    Ez_uint32 palette[6][6][6];
    XColor samples[256];
} Ez_PseudoColor;
#endif /* EZ_BASE_ */

/* Timers handling */
#define EZ_TIMER_MAX 100

typedef struct {
    Ez_window win;
    struct timeval expiration;
} Ez_timer;

/* To display text */
typedef enum {
    EZ_AA = 183200,
    EZ_TL,  EZ_TC,  EZ_TR,     /* Top Left, Top Center, Top Right */
    EZ_ML,  EZ_MC,  EZ_MR,     /* Middle Left, Middle Center, Middle Right */
    EZ_BL,  EZ_BC,  EZ_BR,     /* Bottom Left, Bottom Center, Bottom Right */
    EZ_BB,
    EZ_TLF, EZ_TCF, EZ_TRF,    /* same, fill background */
    EZ_MLF, EZ_MCF, EZ_MRF,    /* same, fill background */
    EZ_BLF, EZ_BCF, EZ_BRF,    /* same, fill background */
    EZ_CC
} Ez_Align;

/* Convert Xlib types for Win32 */
#ifdef EZ_BASE_WIN32
typedef HDC XdbeBackBuffer;
typedef MSG XEvent;
typedef int KeySym;
typedef LPCTSTR XContext;
typedef POINT XPoint;
#define True  TRUE
#define False FALSE
#define None  NULL
#endif /* EZ_BASE_ */

/* Catchall type used by all functions */
typedef struct {
#ifdef EZ_BASE_XLIB
    Display *display;               /* The display */
    int screen_num;                 /* The screen number */
    GC gc;                          /* Graphical context */
    XdbeBackBuffer dbuf_pix;        /* Current double buffer */
    Ez_window dbuf_win;             /* Current double-buffered window */
    Atom atom_protoc, atom_delwin;  /* To handle windows deletion */
    XFontStruct *font[EZ_FONT_MAX]; /* To store the fonts */
    int depth;                      /* Depth: 8, 15, 16, 24, 32 */
    Visual *visual;                 /* For colors */
    Ez_PseudoColor pseudoColor;     /* Palette indexed on 256 colors */
    Ez_TrueColor   trueColor;       /* RGB channels stored in the pixels */
#elif defined EZ_BASE_WIN32
    HINSTANCE hand_prog;            /* Handle on the program */
    WNDCLASSEX wnd_class;           /* Extended window class */
    HDC hdc;                        /* Drawin context for dc_win */
    Ez_window dc_win;               /* Ez_window associated to hdc */
    HDC dbuf_dc;                    /* The dc of the current double buffer */
    Ez_window dbuf_win;             /* Current double-buffered window */
    int dbuf_w, dbuf_h;             /* Size of double buffer */
    HBITMAP hOldBmp, hMemBmp;       /* Bitmap of window and double buffer */
    HPEN hpen;                      /* Logical pen for color drawing */
    HBRUSH hbrush;                  /* Brush for color filling */
    HFONT font[EZ_FONT_MAX];        /* To store fonts */
    KeySym key_sym;                 /* Key symbol: XK_Space, XK_q, etc */
    char  *key_name;                /* For printing: "XK_Space", "XK_q", .. */
    char  *key_string;              /* Corresponding string: " ", "q", etc */
    struct timeval start_time;      /* Initial date */
    LARGE_INTEGER start_count;      /* Counter to compute time */
    double perf_freq;               /* Frequency to compute time */
#endif /* EZ_BASE_ */
    int display_width;              /* Display width */
    int display_height;             /* Display height */
    Ez_window root_win;             /* Root window */
    XContext info_prop;             /* xid or prop to store data in the window */
    int mv_x, mv_y;                 /* Coord to filter mouse moves */
    Ez_window mv_win;               /* Ez_window to filter mouse moves */
    Ez_uint32 color;                /* Current color */
    int thick;                      /* Current thickness */
    int nfont;                      /* Current font number */
    Ez_timer timer_l[EZ_TIMER_MAX]; /* Timers list */
    int timer_nb;                   /* Timers number */
    int main_loop;                  /* Main loop flag */
    int last_expose;                /* Last Expose flag */
    int auto_quit;                  /* Close button flag */
    int mouse_b;                    /* Mouse button pressed */
    Ez_window win_l[EZ_WIN_MAX];    /* Windows list */
    int win_nb;                     /* Windows number */
} Ez_X;

#ifdef EZ_BASE_WIN32
/* Events of X11/X.h for Win32 */
#define  KeyPress           2
#define  KeyRelease         3
#define  ButtonPress        4
#define  ButtonRelease      5
#define  MotionNotify       6
#define  Expose            12
#define  ConfigureNotify   22
#define  ClientMessage     33
#define  LASTEvent         35
/* Timer */
#define  EZ_TIMER1        208
/* Private messages */
enum { EZ_MSG_PAINT = WM_APP+1, EZ_MSG_LAST };
#endif /* EZ_BASE_ */

/* Additional events */
enum { WindowClose = LASTEvent+1, TimerNotify, EzLastEvent };


typedef struct {
    int type;                       /* Expose, ButtonPress, etc */
    Ez_window win;                  /* Identifier of the window */
    int mx, my;                     /* Mouse coordinates */
    int mb;                         /* Mouse button number, 0 = None */
    int width, height;              /* Width and height */
    KeySym key_sym;                 /* Key symbol: XK_Space, XK_q, etc */
    char   key_name[80];            /* For printing: "XK_Space", "XK_q", .. */
    char   key_string[80];          /* Corresponding string: " ", "q", etc */
    int    key_count;               /* String length */
    XEvent xev;                     /* Original event */
} Ez_event;

/* Type of a callback */
typedef void (*Ez_func)(Ez_event *ev);

/* Data associated to a window using a xid or a property */
typedef struct {
    Ez_func func;                   /* Callback of window */
    void *data;                     /* User-data associated to window */
    XdbeBackBuffer dbuf;            /* Back-buffer of window */
    int show;                       /* For delayed display */
} Ez_win_info;


/* Public functions */

int ez_init (void) ;
Ez_window ez_window_create (int w, int h, const char *name, Ez_func func);
int ez_window_get_id (Ez_window win);
void ez_window_destroy (Ez_window win);
void ez_window_show (Ez_window win, int val);
void ez_window_set_size (Ez_window win, int w, int h);
void ez_window_get_size (Ez_window win, int *w, int *h);
void ez_window_clear (Ez_window win);
void ez_window_dbuf (Ez_window win, int val);
void ez_set_data (Ez_window win, void *data);
void *ez_get_data (Ez_window win);
void ez_quit (void) ;
void ez_auto_quit (int val);
void ez_send_expose (Ez_window win);
void ez_start_timer (Ez_window win, int delay);
void ez_main_loop (void) ;
int ez_random (int n);
double ez_get_time (void) ;

Ez_uint32 (*ez_get_RGB)(Ez_uint8 r, Ez_uint8 g, Ez_uint8 b);
Ez_uint32 ez_get_grey (Ez_uint8 g);
void ez_HSV_to_RGB (double h, double s, double v,
    Ez_uint8 *r, Ez_uint8 *g, Ez_uint8 *b);
Ez_uint32 ez_get_HSV (double h, double s, double v);
void ez_set_color (Ez_uint32 color);
void ez_set_thick (int thick);
void ez_draw_point (Ez_window win, int x1, int y1);
void ez_draw_line (Ez_window win, int x1, int y1, int x2, int y2);
void ez_draw_rectangle (Ez_window win, int x1, int y1, int x2, int y2);
void ez_fill_rectangle (Ez_window win, int x1, int y1, int x2, int y2);
void ez_draw_triangle (Ez_window win, int x1, int y1, int x2, int y2, int x3, int y3);
void ez_fill_triangle (Ez_window win, int x1, int y1, int x2, int y2, int x3, int y3);
void ez_draw_circle (Ez_window win, int x1, int y1, int x2, int y2);
void ez_fill_circle (Ez_window win, int x1, int y1, int x2, int y2);

int ez_font_load (int num, const char *name);
void ez_set_nfont (int num);
void ez_draw_text (Ez_window win, Ez_Align align, int x1, int y1,
    const char *format, ...);


/* Private functions */
#ifdef EZ_PRIVATE_DEFS

int ez_draw_debug (void);

int ez_error_dfl (const char *fmt, va_list ap);
int ez_error_ign (const char *fmt, va_list ap);
extern int (*ez_error_handler) (const char *fmt, va_list ap);
int ez_error (const char *fmt, ...);
int ez_check_state (const char *funcname);

void ez_win_delete (Ez_window win);
void ez_win_delete_all (void) ;
extern int ez_win_delete_final;
void ez_close_disp (void) ;

#ifdef EZ_BASE_WIN32
void ez_init_timeofday (void);
void ez_get_systemtime (struct timeval *t);
#endif /* EZ_BASE_ */

void ez_gettimeofday (struct timeval *t);
int ez_timer_add (Ez_window win, int delay);
int ez_timer_remove (Ez_window win);
struct timeval *ez_timer_delay (void) ;

#ifdef EZ_BASE_XLIB
void ez_event_next (Ez_event *ev);
Bool ez_predicat_expose (Display *display, XEvent *xev, XPointer arg);
int ez_is_last_expose (XEvent *xev);
void ez_event_dispatch (Ez_event *ev);
#elif defined EZ_BASE_WIN32
void ez_msg_next (MSG *msg);
LRESULT CALLBACK ez_win_proc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int ez_is_modifier (KeySym key_sym);
int ez_is_repetition (LPARAM lParam);
void ez_cur_win (Ez_window win);
void ez_update_pen (void) ;
#endif /* EZ_BASE_ */

void ez_random_init (void) ;

int ez_win_list_find (Ez_window win);
int ez_win_list_insert (Ez_window win);
int ez_win_list_remove (Ez_window win);

int ez_prop_set (Ez_window win, XContext prop, void *value);
int ez_prop_get (Ez_window win, XContext prop, void **value);
int ez_prop_destroy (Ez_window win, XContext prop);

int ez_info_get (Ez_window win, Ez_win_info **info);

int ez_func_set (Ez_window win, Ez_func func);
int ez_func_get (Ez_window win, Ez_func *func);
int ez_func_call (Ez_event *ev);

void ez_dbuf_init (void) ;
int ez_dbuf_set (Ez_window win, XdbeBackBuffer dbuf);
int ez_dbuf_get (Ez_window win, XdbeBackBuffer *dbuf);
void ez_dbuf_preswap (Ez_window win);
void ez_dbuf_swap (Ez_window win);

void ez_font_init (void) ;
void ez_font_delete (void) ;
int ez_color_init (void) ;
#ifdef EZ_BASE_XLIB
void ez_set_palette (Ez_uint32 pixel, int R, int G, int B,
    int max, int inCube);
void ez_init_PseudoColor (void) ;
void ez_init_TrueColor (void) ;
void ez_init_channel (Ez_channel *channel, Ez_uint32 mask);
Ez_uint32 ez_get_RGB_true_color   (Ez_uint8 r, Ez_uint8 g, Ez_uint8 b);
Ez_uint32 ez_get_RGB_pseudo_color (Ez_uint8 r, Ez_uint8 g, Ez_uint8 b);
#elif defined EZ_BASE_WIN32
Ez_uint32 ez_get_RGB_win32 (Ez_uint8 r, Ez_uint8 g, Ez_uint8 b);
int ez_keydown_convert (WPARAM wParam, LPARAM lParam, KeySym *k, char **n, char **s);
int ez_keychar_convert (WPARAM wParam, KeySym *k, char **n, char **s);
char *ez_win_msg_name (unsigned int m);
#endif /* EZ_BASE_ */

#endif /* EZ_PRIVATE_DEFS */


/* LATIN keyboard symbols from X11/keysymdef.h for Win32 */
#ifdef EZ_BASE_WIN32
#define XK_BackSpace                     0xff08  /* Back space, back char */
#define XK_Tab                           0xff09
#define XK_Return                        0xff0d  /* Return, enter */
#define XK_Pause                         0xff13  /* Pause, hold */
#define XK_Scroll_Lock                   0xff14
#define XK_Escape                        0xff1b
#define XK_Delete                        0xffff  /* Delete, rubout */
#define XK_Home                          0xff50
#define XK_Left                          0xff51  /* Move left, left arrow */
#define XK_Up                            0xff52  /* Move up, up arrow */
#define XK_Right                         0xff53  /* Move right, right arrow */
#define XK_Down                          0xff54  /* Move down, down arrow */
#define XK_Prior                         0xff55  /* Prior, previous */
#define XK_Next                          0xff56  /* Next */
#define XK_End                           0xff57  /* EOL */
#define XK_Insert                        0xff63  /* Insert, insert here */
#define XK_Menu                          0xff67
#define XK_Num_Lock                      0xff7f
#define XK_KP_Enter                      0xff8d  /* Enter */
#define XK_KP_Home                       0xff95
#define XK_KP_Left                       0xff96
#define XK_KP_Up                         0xff97
#define XK_KP_Right                      0xff98
#define XK_KP_Down                       0xff99
#define XK_KP_Prior                      0xff9a
#define XK_KP_Next                       0xff9b
#define XK_KP_End                        0xff9c
#define XK_KP_Begin                      0xff9d
#define XK_KP_Equal                      0xffbd  /* Equals */
#define XK_KP_Multiply                   0xffaa
#define XK_KP_Add                        0xffab
#define XK_KP_Separator                  0xffac  /* Separator, often comma */
#define XK_KP_Subtract                   0xffad
#define XK_KP_Divide                     0xffaf
#define XK_KP_0                          0xffb0
#define XK_KP_1                          0xffb1
#define XK_KP_2                          0xffb2
#define XK_KP_3                          0xffb3
#define XK_KP_4                          0xffb4
#define XK_KP_5                          0xffb5
#define XK_KP_6                          0xffb6
#define XK_KP_7                          0xffb7
#define XK_KP_8                          0xffb8
#define XK_KP_9                          0xffb9
#define XK_F1                            0xffbe
#define XK_F2                            0xffbf
#define XK_F3                            0xffc0
#define XK_F4                            0xffc1
#define XK_F5                            0xffc2
#define XK_F6                            0xffc3
#define XK_F7                            0xffc4
#define XK_F8                            0xffc5
#define XK_F9                            0xffc6
#define XK_F10                           0xffc7
#define XK_F11                           0xffc8
#define XK_F12                           0xffc9
#define XK_Shift_L                       0xffe1  /* Left shift */
#define XK_Shift_R                       0xffe2  /* Right shift */
#define XK_Control_L                     0xffe3  /* Left control */
#define XK_Control_R                     0xffe4  /* Right control */
#define XK_Caps_Lock                     0xffe5  /* Caps lock */
#define XK_Meta_L                        0xffe7  /* Left meta */
#define XK_Meta_R                        0xffe8  /* Right meta */
#define XK_Alt_L                         0xffe9  /* Left alt */
#define XK_Alt_R                         0xffea  /* Right alt */
#define XK_space                         0x0020  /* SPACE */
#define XK_exclam                        0x0021  /* EXCLAMATION MARK */
#define XK_quotedbl                      0x0022  /* QUOTATION MARK */
#define XK_numbersign                    0x0023  /* NUMBER SIGN */
#define XK_dollar                        0x0024  /* DOLLAR SIGN */
#define XK_percent                       0x0025  /* PERCENT SIGN */
#define XK_ampersand                     0x0026  /* AMPERSAND */
#define XK_apostrophe                    0x0027  /* APOSTROPHE */
#define XK_parenleft                     0x0028  /* LEFT PARENTHESIS */
#define XK_parenright                    0x0029  /* RIGHT PARENTHESIS */
#define XK_asterisk                      0x002a  /* ASTERISK */
#define XK_plus                          0x002b  /* PLUS SIGN */
#define XK_comma                         0x002c  /* COMMA */
#define XK_minus                         0x002d  /* HYPHEN-MINUS */
#define XK_period                        0x002e  /* FULL STOP */
#define XK_slash                         0x002f  /* SOLIDUS */
#define XK_0                             0x0030  /* DIGIT ZERO */
#define XK_1                             0x0031  /* DIGIT ONE */
#define XK_2                             0x0032  /* DIGIT TWO */
#define XK_3                             0x0033  /* DIGIT THREE */
#define XK_4                             0x0034  /* DIGIT FOUR */
#define XK_5                             0x0035  /* DIGIT FIVE */
#define XK_6                             0x0036  /* DIGIT SIX */
#define XK_7                             0x0037  /* DIGIT SEVEN */
#define XK_8                             0x0038  /* DIGIT EIGHT */
#define XK_9                             0x0039  /* DIGIT NINE */
#define XK_colon                         0x003a  /* COLON */
#define XK_semicolon                     0x003b  /* SEMICOLON */
#define XK_less                          0x003c  /* LESS-THAN SIGN */
#define XK_equal                         0x003d  /* EQUALS SIGN */
#define XK_greater                       0x003e  /* GREATER-THAN SIGN */
#define XK_question                      0x003f  /* QUESTION MARK */
#define XK_at                            0x0040  /* COMMERCIAL AT */
#define XK_A                             0x0041  /* CAPITAL LETTER A */
#define XK_B                             0x0042  /* CAPITAL LETTER B */
#define XK_C                             0x0043  /* CAPITAL LETTER C */
#define XK_D                             0x0044  /* CAPITAL LETTER D */
#define XK_E                             0x0045  /* CAPITAL LETTER E */
#define XK_F                             0x0046  /* CAPITAL LETTER F */
#define XK_G                             0x0047  /* CAPITAL LETTER G */
#define XK_H                             0x0048  /* CAPITAL LETTER H */
#define XK_I                             0x0049  /* CAPITAL LETTER I */
#define XK_J                             0x004a  /* CAPITAL LETTER J */
#define XK_K                             0x004b  /* CAPITAL LETTER K */
#define XK_L                             0x004c  /* CAPITAL LETTER L */
#define XK_M                             0x004d  /* CAPITAL LETTER M */
#define XK_N                             0x004e  /* CAPITAL LETTER N */
#define XK_O                             0x004f  /* CAPITAL LETTER O */
#define XK_P                             0x0050  /* CAPITAL LETTER P */
#define XK_Q                             0x0051  /* CAPITAL LETTER Q */
#define XK_R                             0x0052  /* CAPITAL LETTER R */
#define XK_S                             0x0053  /* CAPITAL LETTER S */
#define XK_T                             0x0054  /* CAPITAL LETTER T */
#define XK_U                             0x0055  /* CAPITAL LETTER U */
#define XK_V                             0x0056  /* CAPITAL LETTER V */
#define XK_W                             0x0057  /* CAPITAL LETTER W */
#define XK_X                             0x0058  /* CAPITAL LETTER X */
#define XK_Y                             0x0059  /* CAPITAL LETTER Y */
#define XK_Z                             0x005a  /* CAPITAL LETTER Z */
#define XK_bracketleft                   0x005b  /* LEFT SQUARE BRACKET */
#define XK_backslash                     0x005c  /* REVERSE SOLIDUS */
#define XK_bracketright                  0x005d  /* RIGHT SQUARE BRACKET */
#define XK_asciicircum                   0x005e  /* CIRCUMFLEX ACCENT */
#define XK_underscore                    0x005f  /* LOW LINE */
#define XK_grave                         0x0060  /* GRAVE ACCENT */
#define XK_a                             0x0061  /* SMALL LETTER A */
#define XK_b                             0x0062  /* SMALL LETTER B */
#define XK_c                             0x0063  /* SMALL LETTER C */
#define XK_d                             0x0064  /* SMALL LETTER D */
#define XK_e                             0x0065  /* SMALL LETTER E */
#define XK_f                             0x0066  /* SMALL LETTER F */
#define XK_g                             0x0067  /* SMALL LETTER G */
#define XK_h                             0x0068  /* SMALL LETTER H */
#define XK_i                             0x0069  /* SMALL LETTER I */
#define XK_j                             0x006a  /* SMALL LETTER J */
#define XK_k                             0x006b  /* SMALL LETTER K */
#define XK_l                             0x006c  /* SMALL LETTER L */
#define XK_m                             0x006d  /* SMALL LETTER M */
#define XK_n                             0x006e  /* SMALL LETTER N */
#define XK_o                             0x006f  /* SMALL LETTER O */
#define XK_p                             0x0070  /* SMALL LETTER P */
#define XK_q                             0x0071  /* SMALL LETTER Q */
#define XK_r                             0x0072  /* SMALL LETTER R */
#define XK_s                             0x0073  /* SMALL LETTER S */
#define XK_t                             0x0074  /* SMALL LETTER T */
#define XK_u                             0x0075  /* SMALL LETTER U */
#define XK_v                             0x0076  /* SMALL LETTER V */
#define XK_w                             0x0077  /* SMALL LETTER W */
#define XK_x                             0x0078  /* SMALL LETTER X */
#define XK_y                             0x0079  /* SMALL LETTER Y */
#define XK_z                             0x007a  /* SMALL LETTER Z */
#define XK_braceleft                     0x007b  /* LEFT CURLY BRACKET */
#define XK_bar                           0x007c  /* VERTICAL LINE */
#define XK_braceright                    0x007d  /* RIGHT CURLY BRACKET */
#define XK_asciitilde                    0x007e  /* TILDE */
#define XK_nobreakspace                  0x00a0  /* NO-BREAK SPACE */
#define XK_exclamdown                    0x00a1  /* INVERTED EXCLAMATION MARK */
#define XK_cent                          0x00a2  /* CENT SIGN */
#define XK_sterling                      0x00a3  /* POUND SIGN */
#define XK_currency                      0x00a4  /* CURRENCY SIGN */
#define XK_yen                           0x00a5  /* YEN SIGN */
#define XK_brokenbar                     0x00a6  /* BROKEN BAR */
#define XK_section                       0x00a7  /* SECTION SIGN */
#define XK_diaeresis                     0x00a8  /* DIAERESIS */
#define XK_copyright                     0x00a9  /* COPYRIGHT SIGN */
#define XK_ordfeminine                   0x00aa  /* FEMININE ORDINAL INDICATOR */
#define XK_guillemotleft                 0x00ab  /* LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
#define XK_notsign                       0x00ac  /* NOT SIGN */
#define XK_hyphen                        0x00ad  /* SOFT HYPHEN */
#define XK_registered                    0x00ae  /* REGISTERED SIGN */
#define XK_macron                        0x00af  /* MACRON */
#define XK_degree                        0x00b0  /* DEGREE SIGN */
#define XK_plusminus                     0x00b1  /* PLUS-MINUS SIGN */
#define XK_twosuperior                   0x00b2  /* SUPERSCRIPT TWO */
#define XK_threesuperior                 0x00b3  /* SUPERSCRIPT THREE */
#define XK_acute                         0x00b4  /* ACUTE ACCENT */
#define XK_mu                            0x00b5  /* MICRO SIGN */
#define XK_paragraph                     0x00b6  /* PILCROW SIGN */
#define XK_periodcentered                0x00b7  /* MIDDLE DOT */
#define XK_cedilla                       0x00b8  /* CEDILLA */
#define XK_onesuperior                   0x00b9  /* SUPERSCRIPT ONE */
#define XK_masculine                     0x00ba  /* MASCULINE ORDINAL INDICATOR */
#define XK_guillemotright                0x00bb  /* RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
#define XK_onequarter                    0x00bc  /* VULGAR FRACTION ONE QUARTER */
#define XK_onehalf                       0x00bd  /* VULGAR FRACTION ONE HALF */
#define XK_threequarters                 0x00be  /* VULGAR FRACTION THREE QUARTERS */
#define XK_questiondown                  0x00bf  /* INVERTED QUESTION MARK */
#define XK_Agrave                        0x00c0  /* CAPITAL LETTER A WITH GRAVE */
#define XK_Aacute                        0x00c1  /* CAPITAL LETTER A WITH ACUTE */
#define XK_Acircumflex                   0x00c2  /* CAPITAL LETTER A WITH CIRCUMFLEX */
#define XK_Atilde                        0x00c3  /* CAPITAL LETTER A WITH TILDE */
#define XK_Adiaeresis                    0x00c4  /* CAPITAL LETTER A WITH DIAERESIS */
#define XK_Aring                         0x00c5  /* CAPITAL LETTER A WITH RING ABOVE */
#define XK_AE                            0x00c6  /* CAPITAL LETTER AE */
#define XK_Ccedilla                      0x00c7  /* CAPITAL LETTER C WITH CEDILLA */
#define XK_Egrave                        0x00c8  /* CAPITAL LETTER E WITH GRAVE */
#define XK_Eacute                        0x00c9  /* CAPITAL LETTER E WITH ACUTE */
#define XK_Ecircumflex                   0x00ca  /* CAPITAL LETTER E WITH CIRCUMFLEX */
#define XK_Ediaeresis                    0x00cb  /* CAPITAL LETTER E WITH DIAERESIS */
#define XK_Igrave                        0x00cc  /* CAPITAL LETTER I WITH GRAVE */
#define XK_Iacute                        0x00cd  /* CAPITAL LETTER I WITH ACUTE */
#define XK_Icircumflex                   0x00ce  /* CAPITAL LETTER I WITH CIRCUMFLEX */
#define XK_Idiaeresis                    0x00cf  /* CAPITAL LETTER I WITH DIAERESIS */
#define XK_ETH                           0x00d0  /* CAPITAL LETTER ETH */
#define XK_Ntilde                        0x00d1  /* CAPITAL LETTER N WITH TILDE */
#define XK_Ograve                        0x00d2  /* CAPITAL LETTER O WITH GRAVE */
#define XK_Oacute                        0x00d3  /* CAPITAL LETTER O WITH ACUTE */
#define XK_Ocircumflex                   0x00d4  /* CAPITAL LETTER O WITH CIRCUMFLEX */
#define XK_Otilde                        0x00d5  /* CAPITAL LETTER O WITH TILDE */
#define XK_Odiaeresis                    0x00d6  /* CAPITAL LETTER O WITH DIAERESIS */
#define XK_multiply                      0x00d7  /* MULTIPLICATION SIGN */
#define XK_Oslash                        0x00d8  /* CAPITAL LETTER O WITH STROKE */
#define XK_Ugrave                        0x00d9  /* CAPITAL LETTER U WITH GRAVE */
#define XK_Uacute                        0x00da  /* CAPITAL LETTER U WITH ACUTE */
#define XK_Ucircumflex                   0x00db  /* CAPITAL LETTER U WITH CIRCUMFLEX */
#define XK_Udiaeresis                    0x00dc  /* CAPITAL LETTER U WITH DIAERESIS */
#define XK_Yacute                        0x00dd  /* CAPITAL LETTER Y WITH ACUTE */
#define XK_THORN                         0x00de  /* CAPITAL LETTER THORN */
#define XK_ssharp                        0x00df  /* SMALL LETTER SHARP S */
#define XK_agrave                        0x00e0  /* SMALL LETTER A WITH GRAVE */
#define XK_aacute                        0x00e1  /* SMALL LETTER A WITH ACUTE */
#define XK_acircumflex                   0x00e2  /* SMALL LETTER A WITH CIRCUMFLEX */
#define XK_atilde                        0x00e3  /* SMALL LETTER A WITH TILDE */
#define XK_adiaeresis                    0x00e4  /* SMALL LETTER A WITH DIAERESIS */
#define XK_aring                         0x00e5  /* SMALL LETTER A WITH RING ABOVE */
#define XK_ae                            0x00e6  /* SMALL LETTER AE */
#define XK_ccedilla                      0x00e7  /* SMALL LETTER C WITH CEDILLA */
#define XK_egrave                        0x00e8  /* SMALL LETTER E WITH GRAVE */
#define XK_eacute                        0x00e9  /* SMALL LETTER E WITH ACUTE */
#define XK_ecircumflex                   0x00ea  /* SMALL LETTER E WITH CIRCUMFLEX */
#define XK_ediaeresis                    0x00eb  /* SMALL LETTER E WITH DIAERESIS */
#define XK_igrave                        0x00ec  /* SMALL LETTER I WITH GRAVE */
#define XK_iacute                        0x00ed  /* SMALL LETTER I WITH ACUTE */
#define XK_icircumflex                   0x00ee  /* SMALL LETTER I WITH CIRCUMFLEX */
#define XK_idiaeresis                    0x00ef  /* SMALL LETTER I WITH DIAERESIS */
#define XK_eth                           0x00f0  /* SMALL LETTER ETH */
#define XK_ntilde                        0x00f1  /* SMALL LETTER N WITH TILDE */
#define XK_ograve                        0x00f2  /* SMALL LETTER O WITH GRAVE */
#define XK_oacute                        0x00f3  /* SMALL LETTER O WITH ACUTE */
#define XK_ocircumflex                   0x00f4  /* SMALL LETTER O WITH CIRCUMFLEX */
#define XK_otilde                        0x00f5  /* SMALL LETTER O WITH TILDE */
#define XK_odiaeresis                    0x00f6  /* SMALL LETTER O WITH DIAERESIS */
#define XK_division                      0x00f7  /* DIVISION SIGN */
#define XK_oslash                        0x00f8  /* SMALL LETTER O WITH STROKE */
#define XK_ugrave                        0x00f9  /* SMALL LETTER U WITH GRAVE */
#define XK_uacute                        0x00fa  /* SMALL LETTER U WITH ACUTE */
#define XK_ucircumflex                   0x00fb  /* SMALL LETTER U WITH CIRCUMFLEX */
#define XK_udiaeresis                    0x00fc  /* SMALL LETTER U WITH DIAERESIS */
#define XK_yacute                        0x00fd  /* SMALL LETTER Y WITH ACUTE */
#define XK_thorn                         0x00fe  /* SMALL LETTER THORN */
#define XK_ydiaeresis                    0x00ff  /* SMALL LETTER Y WITH DIAERESIS */
#define XK_EuroSign                      0x20ac  /* EURO SIGN */
#endif /* EZ_BASE_ */


#endif /* EZ_DRAW__H */

