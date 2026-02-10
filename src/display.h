#ifndef DISPLAY_H
#define DISPLAY_H

#ifdef USE_DISPLAY
#if DISPLAY_TYPE == 1
#include "display/e_paper_154.h"
#endif
#if DISPLAY_TYPE == 2
#include "display/oled_096.h"
#endif
#if DISPLAY_TYPE == 3
#include "display/mini_tv_144.h"
#endif
#if DISPLAY_TYPE == 4
#include "display/mini_oled_042.h"
#endif
#endif

#endif