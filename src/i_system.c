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
//
//-----------------------------------------------------------------------------

// static const char
// rcsid[] = "$Id: m_bbox.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>

#include "doomdef.h"
#include "i_sound.h"
#include "i_video.h"
#include "m_misc.h"

#include "d_net.h"
#include "g_game.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"
#if defined(ESP_PLATFORM)
#include "esp_heap_caps.h"
#include <lm_system.h>
#endif

#ifdef COMBINE_SCREENS
unsigned char CombinedScreens[SCREENWIDTH * SCREENHEIGHT];
#else
unsigned char CombinedScreens[SCREENWIDTH * SCREENHEIGHT * 4];
#endif

int mb_used = 6;

void I_Tactile(int on, int off, int total) {
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t emptycmd;
ticcmd_t* I_BaseTiccmd(void) { return &emptycmd; }

unsigned char* DOOMHeap;

int I_GetHeapSize(void) { return FIXED_HEAP; }

byte* I_ZoneBase(int* size) {
  *size = FIXED_HEAP; // mb_used*1024*1024;
#if defined(ESP_PLATFORM)
  DOOMHeap = (byte*)heap_caps_malloc(FIXED_HEAP, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  if (!DOOMHeap) {
    // 2. Fall back to 96 KB if 112 KB fails.
    DOOMHeap = (byte*)heap_caps_malloc(96 * 1024, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  }

  if (!DOOMHeap) {
    // 3. Fall back to 80 KB. (E1M1 requires roughly this much total zone space
    // to build sectors).
    DOOMHeap = (byte*)heap_caps_malloc(80 * 1024, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  }

  if (!DOOMHeap) {
    // 4. Last ditch effort: 64 KB (might cause Z_Malloc failures on heavy maps)
    DOOMHeap = (byte*)heap_caps_malloc(64 * 1024, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
  }
#else
  DOOMHeap = (byte*)malloc(FIXED_HEAP);
#endif
  return (byte*)DOOMHeap;
}

//
// I_GetTime
// returns time in 1/70th second tics
//
int I_GetTime(void) {
#if defined(ESP_PLATFORM)
  static int last_returned_tic = 0;
  // 1. Calculate the current tic using corrected math
  // (Change 1000000 to 1000 if your timer is in milliseconds)
  int current_tic = (lm_system_timer() * TICRATE) / 1000000;

  // 2. Watchdog Rescue: If DOOM slams this function twice within the SAME game
  // tic, it means it's trapped in its 1993 frame-limiter loop. Let's give
  // FreeRTOS a breather.
  if (current_tic == last_returned_tic) {
    vTaskDelay(1); // Relinquish CPU control for 1 FreeRTOS tick to feed the watchdog

    // 3. Recalculate after waking up so DOOM sees actual time progression
    current_tic = (lm_system_timer() * TICRATE) / 1000000;
  }

  last_returned_tic = current_tic;
  return current_tic;
#else
  struct timeval tp;
  struct timezone tzp;
  int newtics;
  static int basetime = 0;

  gettimeofday(&tp, &tzp);
  if (!basetime)
    basetime = tp.tv_sec;
  newtics = (tp.tv_sec - basetime) * TICRATE + tp.tv_usec * TICRATE / 1000000;
  return newtics;
#endif
}

//
// I_Init
//
void I_Init(void) {
  I_InitSound();
  //  I_InitGraphics();
}

//
// I_Quit
//
void I_Quit(void) {
  D_QuitNetGame();
  I_ShutdownSound();
  I_ShutdownMusic();
  M_SaveDefaults();
  I_ShutdownGraphics();
  exit(0);
}

void I_WaitVBL(int count) {
#if defined(ESP_PLATFORM)
  lm_usleep(count * (1000000 / 700));
// static int64_t last_time = 0;

// lm_system_tick(lm_system_timer() - last_time);
// last_time = lm_system_timer();
#elif defined(SGI)
  sginap(1);
#elif defined(SUN)
  sleep(0);
#else
  usleep(count * (1000000 / 700));
#endif
}

void I_BeginRead(void) {}

void I_EndRead(void) {}

byte* I_AllocLow(int length) {
  byte* mem;
  mem = CombinedScreens;
  memset(mem, 0, length);
  return mem;
}

//
// I_Error
//
extern boolean demorecording;

void I_Error(char* error, ...) {
  va_list argptr;

  // Message first.
  va_start(argptr, error);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, error, argptr);
  fprintf(stderr, "\n");
  va_end(argptr);

  fflush(stderr);

  // Shutdown. Here might be other errors.
  if (demorecording)
    G_CheckDemoStatus();

  D_QuitNetGame();
  I_ShutdownGraphics();

  exit(-1);
}
