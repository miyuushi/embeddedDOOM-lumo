// Stubbed Video.c

// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

// static const char
// rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
// #include <netinet/in.h>
#include "d_event.h"
#include "d_main.h"
#include "doomstat.h"
#include "i_system.h"
#include "m_argv.h"
#include "v_video.h"
#include <signal.h>

#include "doomdef.h"

#define POINTER_WARP_COUNTDOWN 1

#include "DrawFunctions.c"
#include "os_generic.c"
#if defined(ESP_PLATFORM)
#include "freertos/idf_additions.h"
#include "lm_system.h"
#include "lm_gamepad.h"

static lm_surface_t *surface;

static const struct {
  int mask;
  int key;
} keymap[] = {
    {LM_BUTTON_UP, KEY_UPARROW},     {LM_BUTTON_DOWN, KEY_DOWNARROW},
    {LM_BUTTON_LEFT, KEY_LEFTARROW}, {LM_BUTTON_RIGHT, KEY_RIGHTARROW},
    {LM_BUTTON_A, KEY_RCTRL},        {LM_BUTTON_A, KEY_ENTER},
    {LM_BUTTON_B, KEY_RSHIFT},       {LM_BUTTON_B, KEY_LALT},
    {LM_BUTTON_B, KEY_BACKSPACE},    {LM_BUTTON_L3, KEY_ESCAPE},
    {LM_BUTTON_R3, KEY_TAB},         {LM_BUTTON_SELECT, KEY_F1},
    {LM_BUTTON_START, KEY_F2}};
#else
#include "XDriver.c"
#endif

void HandleKey(int keycode, int bDown) {
  //   switch(rc = keycode )
  //   {
  //     case XK_Left:	rc = KEY_LEFTARROW;	break;
  //     case XK_Right:	rc = KEY_RIGHTARROW;	break;
  //     case XK_Down:	rc = KEY_DOWNARROW;	break;
  //     case XK_Up:	rc = KEY_UPARROW;	break;
  //     case XK_Escape:	rc = KEY_ESCAPE;	break;
  //     case XK_Return:	rc = KEY_ENTER;		break;
  //     case XK_Tab:	rc = KEY_TAB;		break;
  //     case XK_F1:	rc = KEY_F1;		break;
  //     case XK_F2:	rc = KEY_F2;		break;
  //     case XK_F3:	rc = KEY_F3;		break;
  //     case XK_F4:	rc = KEY_F4;		break;
  //     case XK_F5:	rc = KEY_F5;		break;
  //     case XK_F6:	rc = KEY_F6;		break;
  //     case XK_F7:	rc = KEY_F7;		break;
  //     case XK_F8:	rc = KEY_F8;		break;
  //     case XK_F9:	rc = KEY_F9;		break;
  //     case XK_F10:	rc = KEY_F10;		break;
  //     case XK_F11:	rc = KEY_F11;		break;
  //     case XK_F12:	rc = KEY_F12;		break;

  //     case XK_BackSpace:
  //     case XK_Delete:	rc = KEY_BACKSPACE;	break;

  //     case XK_Pause:	rc = KEY_PAUSE;		break;

  //     case XK_KP_Equal:
  //     case XK_equal:	rc = KEY_EQUALS;	break;

  //     case XK_KP_Subtract:
  //     case XK_minus:	rc = KEY_MINUS;		break;

  //     case XK_Shift_L:
  //     case XK_Shift_R:
  // rc = KEY_RSHIFT;
  // break;

  //     case XK_Control_L:
  //     case XK_Control_R:
  // rc = KEY_RCTRL;
  // break;

  //     case XK_Alt_L:
  //     case XK_Meta_L:
  //     case XK_Alt_R:
  //     case XK_Meta_R:
  // rc = KEY_RALT;
  // break;

  //     default:
  // if (rc >= XK_space && rc <= XK_asciitilde)
  //     rc = rc - XK_space + ' ';
  // if (rc >= 'A' && rc <= 'Z')
  //     rc = rc - 'A' + 'a';
  // break;
  //   }
}

static int lastmousex = 0;
static int lastmousey = 0;
boolean mousemoved = false;

int mousemask = 0;

void HandleButton(int x, int y, int button, int bDown) {
  int dx = x - lastmousex;
  int dy = y - lastmousey;

  if (bDown) {
    mousemask |= 1 << button;
  } else {
    mousemask &= ~(1 << button);
  }

  event_t event;
  event.type = ev_mouse;
  event.data1 = mousemask;
  event.data2 = dx * 10;
  event.data3 = dy * 10;
  D_PostEvent(&event);

  lastmousex = x;
  lastmousey = y;
}

void HandleMotion(int x, int y, int mask) {
  int dx = x - lastmousex;
  int dy = y - lastmousey;

  mousemask = mask;

  event_t event;
  event.type = ev_mouse;
  event.data1 = mousemask;
  event.data2 = dx * 10;
  event.data3 = dy * 10;
  D_PostEvent(&event);

  lastmousex = x;
  lastmousey = y;
}

//
// I_SetPalette
//

// static byte lpalette[256*3];

// static const byte gammatbl[] = {
//     0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x07,
//     0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09,
//     0x09, 0x09, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0B, 0x0B, 0x0B,
//     0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
//     0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
//     0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
//     0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
//     0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
//     0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
//     0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0C, 0x0C, 0x0C,
//     0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
//     0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
//     0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0A, 0x0A, 0x0A,
//     0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x09, 0x09,
//     0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x08,
//     0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x07, 0x07, 0x07,
//     0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x06, 0x06, 0x06, 0x06,
//     0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
//     0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x03,
//     0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02,
//     0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
//     0x00, 0x00, 0x00, 0x00,
// };

void I_SetPalette(byte *palette) {
#if defined(ESP_PLATFORM)
  for (int i = 0; i < 256; i++) {
    int r = *palette++;
    int g = *palette++;
    int b = *palette++;

    // if (usegamma > 0) {
    //   r = MIN(r + gammatbl[r] * usegamma, 0xFF);
    //   g = MIN(g + gammatbl[g] * usegamma, 0xFF);
    //   b = MIN(b + gammatbl[b] * usegamma, 0xFF);
    // }

    int nr = r >> 3, ng = g >> 2, nb = b >> 3;
    uint16_t n = ((nr << 11) | (ng << 5) | nb);
    uint16_t n_bgr565 = ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
    surface->palette[i] = ~(n_bgr565 << 8 | n_bgr565 >> 8);
#if 0
    This is kept for reference if we ever need to fix truecolor rendering.
    float roundUpR = (r > dontRoundAbove) ? 0 : 0.5f;
    float roundUpG = (g > dontRoundAbove) ? 0 : 0.5f;
    float roundUpB = (b > dontRoundAbove) ? 0 : 0.5f;

    for (int w=0; w<VID_NUMCOLORWEIGHTS; w++) {
      int t = (float)(w)/(float)(VID_NUMCOLORWEIGHTS-1);
      int nr = (int)(r*t+roundUpR);
      int ng = (int)(g*t+roundUpG);
      int nb = (int)(b*t+roundUpB);
      ((uint32_t*)palette)[(i*VID_NUMCOLORWEIGHTS)+w] = ((nr<<16) | (ng<<8) | nb);
    }
#endif
  }
  // memcpy(lpalette, palette, sizeof( lpalette ));
  // UploadNewPalette(X_cmap, palette);
#endif
}

#define OUTSCALE 1

//
// I_UpdateNoBlit
//
void I_UpdateNoBlit(void) {}

void I_InitGraphics(void) {
#if defined(ESP_PLATFORM)
  surface = lm_surface_create(SCREENWIDTH, SCREENHEIGHT, LM_PIXEL_PAL565_BE,
                              MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  screens[0] = surface->data;
#else
  CNFGSetup("EmbeddedDoom", SCREENWIDTH * OUTSCALE, SCREENHEIGHT * OUTSCALE);
#endif
}

void I_StartTic(void) {
#if defined(ESP_PLATFORM)
  static int64_t last_time = 0;
  static int32_t prev_joystick = 0x0000;
  uint32_t joystick = lm_input_read_gamepad().buttons;
  uint32_t changed = prev_joystick ^ joystick;
  event_t event = {0};

  if (changed) {
    for (int i = 0; i < LM_COUNT(keymap); i++) {
      if (changed & keymap[i].mask) {
        event.type = (joystick & keymap[i].mask) ? ev_keydown : ev_keyup;
        event.data1 = keymap[i].key;
        D_PostEvent(&event);
      }
    }
  }

  prev_joystick = joystick;
#else
  CNFGHandleInput();
#endif
}
void I_ReadScreen(byte *scr) {
  memcpy(scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
}
void I_StartFrame() {}

void I_ShutdownGraphics(void) {
#if defined(ESP_PLATFORM)
  screens[0] = NULL;
  lm_surface_free(surface);
#endif
}

void I_FinishUpdate(void) {

  static int lasttic;
  int tics;
  int i;
  // UNUSED static unsigned char *bigscreen=0;

#if 0
    // draws little dots on the bottom of the screen
    if (devparm)
    {
		i = I_GetTime();
		tics = i - lasttic;
		lasttic = i;
		if (tics > 20) tics = 20;

		for (i=0 ; i<tics*2 ; i+=2)
			screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
		for ( ; i<20*2 ; i+=2)
			screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;	
    }
#endif

#if defined(ESP_PLATFORM)
  lm_display_submit(surface, 0);
  lm_display_sync(true);
#endif

  // This is for display on PC only.  Don't worry about the output staging
  // buffer being big!

  // static uint32_t * bmdata;
  // if( !bmdata ) bmdata =
  // malloc(SCREENWIDTH*SCREENHEIGHT*OUTSCALE*OUTSCALE*4);

  // int y, x;
  // for( y = 0; y < SCREENHEIGHT; y++ )
  // {
  // 	const uint8_t * screenline = &screens[0][y*SCREENWIDTH];
  // 	uint32_t * outline = &bmdata[y*SCREENWIDTH*OUTSCALE*OUTSCALE];
  // 	int xt2 = 0;
  // 	for( x = 0; x < SCREENWIDTH; x++, xt2+=OUTSCALE )
  // 	{
  // 		//lpalette
  // 		int col = screenline[x];
  // 		int lx, ly;
  // 		uint32_t rgb =
  // (lpalette[col*3+0]<<16)|(lpalette[col*3+1]<<8)|(lpalette[col*3+2]<<0) |
  // 0xff000000; 		for( ly = 0; ly < OUTSCALE; ly++ )
  // for( lx = 0; lx < OUTSCALE; lx++ )
  // outline[xt2+SCREENWIDTH*OUTSCALE*ly+lx] = rgb;
  // 	}
  // }

  // CNFGUpdateScreenWithBitmap( (unsigned long*)bmdata,SCREENWIDTH*OUTSCALE,
  // SCREENHEIGHT*OUTSCALE );
}
