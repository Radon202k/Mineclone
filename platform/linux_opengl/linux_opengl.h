#ifndef WINDOWS_OPENGL_H
#define WINDOWS_OPENGL_H

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <EGL/egl.h>
#include <GL/glcorearb.h>

#include "../../engine/engine.h"
#include "../../game/game.h"

#define Assert(cond) assert(cond)

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../opengl.h"

#include "../../engine/chunk.c"
#include "../../engine/generation.c"
#include "../../game/game.c"

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#endif //WINDOWS_OPENGL_H
