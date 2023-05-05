#ifndef WINDOWS_OPENGL_H
#define WINDOWS_OPENGL_H

#include "../../engine/engine.h"
#include "../../game/game.h"
#include "windows_opengl.h"

#include "../../engine/voxel.c"
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

#include "../opengl_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

typedef struct RendererChunk {
    s32 x, y, z; /* key */
    u32 vao, vbo, ebo, indexCount; /* value */
    struct RendererChunk *next;
} RendererChunk;

typedef struct Renderer {
    f32 dt;
    
    Camera orbitCamera;
    
    v2 lastMouseP;
    bool draggingCamera;
    
    RendererChunk *chunkHashTable[256];
    
} Renderer;

global Renderer renderer;



#endif //WINDOWS_OPENGL_H
