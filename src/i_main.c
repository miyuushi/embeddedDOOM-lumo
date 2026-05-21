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
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

//static const char
//rcsid[] = "$Id: i_main.c,v 1.4 1997/02/03 22:45:10 b1 Exp $";



#include "doomdef.h"

#include "m_argv.h"
#include "d_main.h"

int
main
( int		argc,
  char**	argv ) 
{ 
    myargc = argc; 
    myargv = argv; 
 
    D_DoomMain (); 

    return 0;
} 

#if defined(ESP_PLATFORM)
#include <lm_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *doom_argv[10];
static lm_app_t* app;

void doom_task(void *pvParameters) {
  const char *iwad = NULL;
  const char *pwad = NULL;

  myargc = 4;
  myargv = doom_argv;
  doom_argv[0] = "doom";
  doom_argv[1] = "-warp";
  doom_argv[2] = "1";
  doom_argv[3] = "1";

  D_DoomMain();
  vTaskDelete(NULL);
}

void app_main(void) {
  app = lm_system_init(NULL, NULL);

  // SCREENWIDTH = 320;
  // SCREENHEIGHT = 200;

  xTaskCreate(doom_task, "doom_task", 8192, NULL, 5, NULL);

}
#endif