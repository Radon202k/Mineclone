#ifndef OPENGL_LOADER_H
#define OPENGL_LOADER_H

// make sure you use functions that are valid for selected GL version (specified when context is created)
#define GL_FUNCTIONS(X) \
X(PFNGLGENBUFFERSPROC,               glGenBuffers               ) \
X(PFNGLDELETEBUFFERSPROC,            glDeleteBuffers            ) \
X(PFNGLBUFFERDATAPROC,               glBufferData               ) \
X(PFNGLNAMEDBUFFERSTORAGEPROC,       glNamedBufferStorage       ) \
X(PFNGLBINDVERTEXARRAYPROC,          glBindVertexArray          ) \
X(PFNGLDELETEVERTEXARRAYSPROC,       glDeleteVertexArrays       ) \
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

#endif //OPENGL_LOADER_H
