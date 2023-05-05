#ifndef OPENGL_H
#define OPENGL_H

/* this file has all the cross platform opengl */

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

typedef struct RendererChunk {
    s32 x, y, z; /* key */
    u32 vao, vbo, ebo, indexCount; /* value */
    struct RendererChunk *next;
} RendererChunk;

typedef struct Shader {
    GLuint vertex;
    GLuint fragment;
    GLuint pipeline;
} Shader;

typedef struct Renderer {
    bool running;
    u32 width;
    u32 height;
    
    Shader voxelsShader;
    
    f32 dt;
    
    Camera orbitCamera;
    
    v2 lastMouseP;
    bool draggingCamera;
    
    RendererChunk *chunkHashTable[256];
    
    GLuint texture;
    
} Renderer;

global Renderer renderer;

internal GLuint
opengl_load_textures() {
    int w,h,n;
    char *dirtTexture = platform_build_absolute_path("/Desktop/Mineclone/bin/textures/dirt.png");

    u8 *data = stbi_load(dirtTexture, &w, &h, &n, 0);
    if (data) {
        GLuint texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTextureStorage2D(texture, 1, GL_RGBA8, w, h);
        glTextureSubImage2D(texture, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        stbi_image_free(data);
        return texture;
    }
    else {
        // TODO: make a default texture
        assert(!"Failed to load texture");
    }
}

internal void
opengl_shader_from_files(char *vsPath, char *fsPath){
    u8 *glslVShader = platform_file_read(vsPath);
    u8 *glslFShader = platform_file_read(fsPath);
    renderer.voxelsShader.vertex = 
        glCreateShaderProgramv(GL_VERTEX_SHADER, 1, 
                               (const GLchar **)&glslVShader);
    renderer.voxelsShader.fragment = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, 
                                                            (const GLchar **)&glslFShader);
    
    GLint linked;
    glGetProgramiv(renderer.voxelsShader.vertex, GL_LINK_STATUS, &linked);
    if (!linked) {
        char message[1024];
        glGetProgramInfoLog(renderer.voxelsShader.vertex, sizeof(message), NULL, message);
        platform_debug_print(message);
        Assert(!"Failed to create vertex shader!");
    }
    
    glGetProgramiv(renderer.voxelsShader.fragment, GL_LINK_STATUS, &linked);
    if (!linked) {
        char message[1024];
        glGetProgramInfoLog(renderer.voxelsShader.fragment, sizeof(message), NULL, message);
        platform_debug_print(message);
        Assert(!"Failed to create fragment shader!");
    }
    
    glGenProgramPipelines(1, &renderer.voxelsShader.pipeline);
    glUseProgramStages(renderer.voxelsShader.pipeline, GL_VERTEX_SHADER_BIT, renderer.voxelsShader.vertex);
    glUseProgramStages(renderer.voxelsShader.pipeline, GL_FRAGMENT_SHADER_BIT, renderer.voxelsShader.fragment);
}

internal void
opengl_setup_global_state(void) {
    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    
    // disable culling
    glDisable(GL_CULL_FACE);
    
    renderer.orbitCamera.distance = 5;
    renderer.orbitCamera.elevation = 1;
}

internal void
opengl_prepare_frame() {
    // setup output size covering all client area of window
    glViewport(0, 0, renderer.width, renderer.height);
    
    // clear screen
    glClearColor(0.392f, 0.584f, 0.929f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // setup rotation matrix in uniform
    {
        f32 aspect = (float)renderer.width / renderer.height;
        mat4 proj = mat4_perspective((f32)deg2rad(45), aspect, 0.1f, 1000.0f);
        
        if (!renderer.draggingCamera && mouse.left.pressed) {
            renderer.draggingCamera = true;
            renderer.lastMouseP = mouse.p;
        }
        
        if (renderer.draggingCamera && !mouse.left.down)
            renderer.draggingCamera = false;
        
        if (renderer.draggingCamera) {
            v2 deltaP = v2_sub(mouse.p, renderer.lastMouseP);
            renderer.lastMouseP = mouse.p;
            
            if (keyboard.lshift.down) {
                renderer.orbitCamera.distance += 0.02f * deltaP.y;
            }
            else {
                renderer.orbitCamera.azimuth -= 0.02f * deltaP.x;
                renderer.orbitCamera.elevation -= 0.02f * deltaP.y;
            }
        }
        
#if 0
        v3 eye = camera_orbit_position(renderer.orbitCamera.distance,
                                       renderer.orbitCamera.azimuth,
                                       renderer.orbitCamera.elevation);
        v3 worldUp = (v3){0,1,0};
        mat4 view = mat4_lookat(eye, 
                                renderer.orbitCamera.target,
                                worldUp);
#else 
        Camera *cam = &game.playerCamera;
        v3 eye = cam->p;
        v3 center = {
            eye.x - sinf(deg2rad(cam->yaw)), 
            eye.y + tanf(deg2rad(cam->pitch)), 
            eye.z - cosf(deg2rad(cam->yaw))};
        v3 up = {0,1,0};
        mat4 view = mat4_lookat(eye, center, up); 
#endif
        
        glProgramUniformMatrix4fv(renderer.voxelsShader.vertex, 0, 1, GL_FALSE, proj.d);
        glProgramUniformMatrix4fv(renderer.voxelsShader.vertex, 1, 1, GL_FALSE, view.d);
    }
    
    // activate shaders for next draw call
    glBindProgramPipeline(renderer.voxelsShader.pipeline);
}

internal void
opengl_render_chunks() {
    for (u32 chunkIndex=0; chunkIndex<narray(renderer.chunkHashTable); ++chunkIndex){
        RendererChunk *renderChunk = renderer.chunkHashTable[chunkIndex];
        if (renderChunk) {
            /* bind buffers */
            glBindVertexArray(renderChunk->vao);
            glBindBuffer(GL_ARRAY_BUFFER, renderChunk->vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderChunk->ebo);
            /* bind texture */
            GLint s_texture = 0; // texture unit that sampler2D will use in GLSL code
            glBindTextureUnit(s_texture, renderer.texture);
            /* draw */
            glDrawElements(GL_TRIANGLES, renderChunk->indexCount, GL_UNSIGNED_INT, 0);
        }
    }
}

internal void
renderer_chunk_htable_destruct(void) {
    for (s32 i=0; i<narray(renderer.chunkHashTable); ++i) {
        RendererChunk *at = renderer.chunkHashTable[i];
        while (at) {
            RendererChunk *next = at->next;
            free(at);
            at = next;
        }
    }
}

internal u32
renderer_chunk_hash(s32 x, s32 y, s32 z) {
    u32 hash = x*31 + y*479 + z*953;
    return hash & (narray(renderer.chunkHashTable)-1);
}

internal void
renderer_chunk_htable_insert(s32 x, s32 y, s32 z, ChunkMesh *mesh) {
    /* allocate new renderer chunk */
    RendererChunk *chunk = malloc(sizeof *chunk);
    memset(chunk, 0, sizeof *chunk);
    chunk->x = x;
    chunk->y = y;
    chunk->z = z;
    chunk->indexCount = mesh->indexIndex;
    /* vbo */
    glGenBuffers(1, &chunk->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertexIndex*sizeof(VoxelVertex), mesh->vertices, GL_STATIC_DRAW);
    /* ebo */
    glGenBuffers(1, &chunk->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexIndex*sizeof(u32), mesh->indices, GL_STATIC_DRAW);
    /* vao */
    glGenVertexArrays(1, &chunk->vao);
    glBindVertexArray(chunk->vao);
    /* bind vbo */
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
    /* set up input layout, position */
    GLint a_pos = 0;
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, position));
    /* normal */
    GLint a_nor = 1;
    glEnableVertexAttribArray(a_nor);
    glVertexAttribPointer(a_nor, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, normal));
    /* texcoord */
    GLint a_uv = 2;
    glEnableVertexAttribArray(a_uv);
    glVertexAttribPointer(a_uv, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, texCoord));
    /* unbind buffers */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    /* put at head of collision chain list */
    u32 bucket = renderer_chunk_hash(x,y,z);
    chunk->next = renderer.chunkHashTable[bucket];
    renderer.chunkHashTable[bucket] = chunk;
}

internal void renderer_chunk_htable_update();
internal void renderer_chunk_htable_remove();

#endif //OPENGL_H
