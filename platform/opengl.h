#ifndef OPENGL_H
#define OPENGL_H

/* this file has all the cross platform opengl */

typedef struct RendererChunk {
    s32 x, y, z; /* key */
    u32 vao, vbo, ebo, indexCount; /* value */
    struct RendererChunk *next;
} RendererChunk;

typedef struct Shader {
    GLuint program;
} Shader;

typedef struct Renderer {
    bool running;
    u32 width;
    u32 height;
    
    BlockTextureInfo blockTextureInfo[BLOCK_TEXTURE_COUNT];
    
    Shader voxelsShader;
    
    v2 mouseDelta;
    f32 dt;
    
    Camera orbitCamera;
    
    v2 lastMouseP;
    bool draggingCamera;
    
    RendererChunk *chunkHashTable[256];
    
    GLuint textureAtlas;
    GLint sunDirectionLoc;
    GLint sunColorLoc;
    
    v3 sunDirection;
    
    /* 3d lines */
    u32 lineVertexCount;
    Shader linesShader;
    GLuint lineVAO;
    GLuint lineVBO;
    
} Renderer;

global Renderer renderer;

internal void
opengl_load_textures() {
    
    glCreateTextures(GL_TEXTURE_2D, 1, &renderer.textureAtlas);
    
    // Allocate atlas storage
    
    s32 atlasSize = 1024;
    s32 blockSize = 32;
    
    glTextureParameteri(renderer.textureAtlas, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(renderer.textureAtlas, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(renderer.textureAtlas, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(renderer.textureAtlas, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureStorage2D(renderer.textureAtlas, 1, GL_RGBA8, atlasSize, atlasSize);
    
    // TODO: Some packing scheme
    // Load block textures into the atlas
    s32 yOffset = 0;
    for (s32 i = 0; i < BLOCK_TEXTURE_COUNT; i++) {
        s32 w, h, n;
        char *texturePath = platform_build_absolute_path(game.textureFilenames[i]);
        u8 *data = stbi_load(texturePath, &w, &h, &n, 0);
        
        if (data) {
            assert(n==4 && w > 0 && h > 0);
            
            glTextureSubImage2D(renderer.textureAtlas, 0, 0, 
                                yOffset, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
            
            // Calculate the UV coordinates for this block texture
            renderer.blockTextureInfo[i].u_min = 0.0f;
            renderer.blockTextureInfo[i].u_max = (f32)blockSize/(f32)atlasSize;
            renderer.blockTextureInfo[i].v_min = (f32)yOffset / (f32)atlasSize;
            renderer.blockTextureInfo[i].v_max = (f32)(yOffset + h) / (f32)atlasSize;
            
            yOffset += h;
            stbi_image_free(data);
        } else {
            // TODO: handle texture loading failure
            assert(!"Failed to load block");
        }
    }
}

internal GLuint
compile_shader(char *path, GLenum type) {
    u8 *glsl = platform_file_read(path);
    if (!glsl) {
        fprintf(stderr, "Failed to load shader source file: %s\n", path);
        return 0;
    }
    
    GLuint result = glCreateShader(type);
    glShaderSource(result, 1, (const GLchar **)&glsl, NULL);
    glCompileShader(result);
    
    GLint compileStatus;
    glGetShaderiv(result, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus) {
        char infoLog[1024];
        glGetShaderInfoLog(result, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Shader compilation failed for %s: %s\n", path, infoLog);
        glDeleteShader(result);
        return 0;
    }
    
    return result;
}


internal Shader
opengl_shader_from_files(char *vsPath, char *fsPath, char *gsPath) {
    Shader result = {0};
    
    /* Create shader objects */
    GLuint vertexShader = compile_shader(vsPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = compile_shader(fsPath, GL_FRAGMENT_SHADER);
    GLuint geometryShader = gsPath ? compile_shader(gsPath, GL_GEOMETRY_SHADER) : UINT_MAX;
    
    /* Create shader program */
    result.program = glCreateProgram();
    
    /* Attach shaders */
    glAttachShader(result.program, vertexShader);
    glAttachShader(result.program, fragmentShader);
    if (gsPath) glAttachShader(result.program, geometryShader);
    
    /* Link program */
    glLinkProgram(result.program);
    
    GLint linkStatus;
    glGetProgramiv(result.program, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        char infoLog[1024];
        glGetProgramInfoLog(result.program, sizeof(infoLog), NULL, infoLog);
        Assert(!"Shader program linking failed!");
    }
    
    /* Clean up */
    glDetachShader(result.program, vertexShader);
    glDetachShader(result.program, fragmentShader);
    if (gsPath) glDetachShader(result.program, geometryShader);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (gsPath) glDeleteShader(geometryShader);
    
    return result;
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
    
    {
        // Use the shader program
        glUseProgram(renderer.voxelsShader.program);
        
        // Set the sun direction
        glUniform3fv(renderer.sunDirectionLoc, 1, renderer.sunDirection.e);
        
        // Set the sun color
        float sunColor[] = {1.0f, 1.0f, 1.0f};
        glUniform3fv(renderer.sunColorLoc, 1, sunColor);
        
        /* player camera */
        Camera *cam = &game.playerCamera;
        v3 eye = cam->p;
        v3 center = {
            eye.x - sinf(deg2rad(cam->yaw)), 
            eye.y + tanf(deg2rad(cam->pitch)), 
            eye.z - cosf(deg2rad(cam->yaw))};
        v3 up = {0,1,0};
        
        f32 aspect = (float)renderer.width / renderer.height;
        
        /* matrices */
        mat4 proj = mat4_perspective((f32)deg2rad(45), aspect, 0.1f, 1000.0f);
        mat4 view = mat4_lookat(eye, center, up); 
        
        /* voxels shader setup */
        {
            glUniformMatrix4fv(0, 1, GL_FALSE, proj.d);
            glUniformMatrix4fv(1, 1, GL_FALSE, view.d);
        }
    }
}

internal void
opengl_render_chunks() {
    glUseProgram(renderer.voxelsShader.program);
    
    for (u32 chunkIndex=0; chunkIndex<narray(renderer.chunkHashTable); ++chunkIndex){
        RendererChunk *renderChunk = renderer.chunkHashTable[chunkIndex];
        if (renderChunk) {
            /* bind buffers */
            glBindVertexArray(renderChunk->vao);
            glBindBuffer(GL_ARRAY_BUFFER, renderChunk->vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderChunk->ebo);
            /* bind texture */
            GLint s_texture = 0; // texture unit that sampler2D will use in GLSL code
            glBindTextureUnit(s_texture, renderer.textureAtlas);
            /* draw */
            glDrawElements(GL_TRIANGLES, renderChunk->indexCount, GL_UNSIGNED_INT, 0);
        }
    }
}

internal void
opengl_render_lines(void) {
    if (renderer.lineVertexCount > 0 && renderer.lineVertexCount < MAX_LINES) {
        Shader *shader = &renderer.linesShader;
        glUseProgram(shader->program);
        /* lines shader setup */
        {
            /* player camera */
            Camera *cam = &game.playerCamera;
            v3 eye = cam->p;
            v3 center = {
                eye.x - sinf(deg2rad(cam->yaw)), 
                eye.y + tanf(deg2rad(cam->pitch)), 
                eye.z - cosf(deg2rad(cam->yaw))};
            v3 up = {0,1,0};
            
            f32 aspect = (float)renderer.width / renderer.height;
            
            /* matrices */
            mat4 proj = mat4_perspective((f32)deg2rad(45), aspect, 0.1f, 1000.0f);
            mat4 view = mat4_lookat(eye, center, up); 
            
            GLint projLoc = glGetUniformLocation(shader->program, "proj");
            GLint viewLoc = glGetUniformLocation(shader->program, "view");
            
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, proj.d);
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.d);
        }
        glBindVertexArray(renderer.lineVAO);
        glDrawArrays(GL_LINES, 0, renderer.lineVertexCount);
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

internal bool
renderer_chunk_htable_exists(s32 x, s32 y, s32 z) {
    u32 bucket = renderer_chunk_hash(x, y, z);
    
    RendererChunk *at = renderer.chunkHashTable[bucket];
    while (at) {
        if (at->x == x && at->y == y && at->z == z) {
            return true;
        }
        
        at = at->next;
    }
    
    return false;
}

internal void
renderer_lines3D_update(Lines3D *lines)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer.lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, lines->vertexIndex * sizeof(LineVertex), lines->vertices);
    
    renderer.lineVertexCount = lines->vertexIndex;
}

internal void
renderer_lines3D_destruct()
{
    /* delete VAO */
    glDeleteVertexArrays(1, &renderer.lineVAO);
    
    /* delete VBOs */
    glDeleteBuffers(1, &renderer.lineVBO);
}

internal void
renderer_lines3D_construct(void)
{
    /* generate vbo */
    glGenBuffers(1, &renderer.lineVBO);
    
    /* allocate memory in the gpu for it */
    {
        /* line start */
        glBindBuffer(GL_ARRAY_BUFFER, renderer.lineVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_LINES*sizeof(LineVertex), 0, GL_DYNAMIC_DRAW);
    }
    
    /* vao */
    glGenVertexArrays(1, &renderer.lineVAO);
    glBindVertexArray(renderer.lineVAO);
    
    /* configure vertex attributes */
    glBindBuffer(GL_ARRAY_BUFFER, renderer.lineVBO);
    {
        /* position */
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, pos));
        
        /* color */
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));
    }
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

internal void
renderer_chunk_htable_remove(s32 x, s32 y, s32 z) {
    u32 bucket = renderer_chunk_hash(x,y,z);
    RendererChunk *at = renderer.chunkHashTable[bucket];
    RendererChunk *before = 0;
    RendererChunk *found = 0;
    while (at) {
        if (at->x == x && at->y == y && at->z == z) {
            found = at;
            break;
        }
        
        before = at;
        at = at->next;
    }
    
    if (found) {
        if (before)
            before->next = found->next;
        else
            renderer.chunkHashTable[bucket] = found->next;
        
        /* delete VAO, VBO, and EBO */
        glDeleteVertexArrays(1, &found->vao);
        glDeleteBuffers(1, &found->vbo);
        glDeleteBuffers(1, &found->ebo);
        
        free(found);
    }
}

// old orbit camera code
#if 0
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

v3 eye = camera_orbit_position(renderer.orbitCamera.distance,
                               renderer.orbitCamera.azimuth,
                               renderer.orbitCamera.elevation);
v3 worldUp = (v3){0,1,0};
mat4 view = mat4_lookat(eye, 
                        renderer.orbitCamera.target,
                        worldUp);
#else 

#endif


#endif //OPENGL_H
