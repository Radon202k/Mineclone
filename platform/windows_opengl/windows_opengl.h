#ifndef WINDOWS_OPENGL_H
#define WINDOWS_OPENGL_H

#include "../../engine/engine.h"
#include "../../game/game.h"
#include "windows_opengl.h"

#include "../../engine/chunk.c"
#include "../../engine/generation.c"
#include "../../game/game.c"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <GL/gl.h>
#include "glcorearb.h"
#include "wglext.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stddef.h>

// replace this with your favorite Assert() implementation
#include <intrin.h>
#define Assert(cond) do { if (!(cond)) __debugbreak(); } while (0)

#pragma comment (lib, "gdi32.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib, "opengl32.lib")

#include "../opengl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#endif //WINDOWS_OPENGL_H
