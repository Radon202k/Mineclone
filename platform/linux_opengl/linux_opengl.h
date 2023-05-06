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

#define GL_FUNCTIONS(X) \
X(PFNGLDRAWELEMENTSPROC,             glDrawElements             ) \
X(PFNGLCLEARCOLORPROC,               glClearColor               ) \
X(PFNGLCLEARPROC,                    glClear                    ) \
X(PFNGLVIEWPORTPROC,                 glViewport                 ) \
X(PFNGLENABLEPROC,                   glEnable                   ) \
X(PFNGLDISABLEPROC,                  glDisable                  ) \
X(PFNGLBLENDFUNCPROC,                glBlendFunc                ) \
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
