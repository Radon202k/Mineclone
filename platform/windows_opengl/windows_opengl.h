#ifndef WINDOWS_OPENGL_H
#define WINDOWS_OPENGL_H

#include "../../engine/engine.h"
#include "../../game/game.h"

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

#define GL_FUNCTIONS(X) \
X(PFNGLDELETEBUFFERSPROC,            glDeleteBuffers            ) \
X(PFNGLDELETEVERTEXARRAYSPROC,       glDeleteVertexArrays       ) \
X(PFNGLGENBUFFERSPROC,               glGenBuffers               ) \
X(PFNGLBUFFERDATAPROC,               glBufferData               ) \
X(PFNGLNAMEDBUFFERSTORAGEPROC,       glNamedBufferStorage       ) \
X(PFNGLBINDVERTEXARRAYPROC,          glBindVertexArray          ) \
X(PFNGLBINDBUFFERPROC,               glBindBuffer               ) \
X(PFNGLGENVERTEXARRAYSPROC,          glGenVertexArrays          ) \
X(PFNGLVERTEXATTRIBPOINTERPROC,      glVertexAttribPointer      ) \
X(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glEnableVertexAttribArray  ) \
X(PFNGLCREATESHADERPROGRAMVPROC,     glCreateShaderProgramv     ) \
X(PFNGLGETPROGRAMIVPROC,             glGetProgramiv             ) \
X(PFNGLGETPROGRAMINFOLOGPROC,        glGetProgramInfoLog        ) \
X(PFNGLGENPROGRAMPIPELINESPROC,      glGenProgramPipelines      ) \
X(PFNGLUSEPROGRAMSTAGESPROC,         glUseProgramStages         ) \
X(PFNGLBINDPROGRAMPIPELINEPROC,      glBindProgramPipeline      ) \
X(PFNGLPROGRAMUNIFORMMATRIX2FVPROC,  glProgramUniformMatrix2fv  ) \
X(PFNGLPROGRAMUNIFORMMATRIX4FVPROC,  glProgramUniformMatrix4fv  ) \
X(PFNGLBINDTEXTUREUNITPROC,          glBindTextureUnit          ) \
X(PFNGLCREATETEXTURESPROC,           glCreateTextures           ) \
X(PFNGLTEXTUREPARAMETERIPROC,        glTextureParameteri        ) \
X(PFNGLTEXTURESTORAGE2DPROC,         glTextureStorage2D         ) \
X(PFNGLTEXTURESUBIMAGE2DPROC,        glTextureSubImage2D        ) \
X(PFNGLDEBUGMESSAGECALLBACKPROC,     glDebugMessageCallback     )

#define X(type, name) static type name;
GL_FUNCTIONS(X)
#undef X

#define STR2(x) #x
#define STR(x) STR2(x)

#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "../opengl.h"

#include "../../engine/voxel.c"
#include "../../engine/chunk.c"
#include "../../engine/generation.c"
#include "../../game/game.c"


#endif //WINDOWS_OPENGL_H
