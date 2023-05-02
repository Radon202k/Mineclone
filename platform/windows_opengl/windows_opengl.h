#ifndef WINDOWS_OPENGL_H
#define WINDOWS_OPENGL_H

typedef struct RendererChunk {
    s32 x, y, z; /* key */
    u32 vao, vbo, ebo, indexCount; /* value */
    struct RendererChunk *next;
} RendererChunk;

typedef struct Renderer {
    f32 dt;
    f32 distance;
    f32 azimuth;
    f32 elevation;
    
    v2 lastMouseP;
    bool draggingCamera;
    
    RendererChunk *chunkHashTable[256];
    
} Renderer;

global Renderer renderer;

internal u8 * platform_file_read             (char *path);
internal void platform_file_write            (char *path, u8 *contents);
internal void renderer_chunk_htable_destruct (void);
internal u32  renderer_chunk_hash            (s32 x, s32 y, s32 z);
internal void renderer_chunk_htable_insert   (s32 x, s32 y, s32 z, ChunkMesh *mesh);
internal void renderer_chunk_htable_update   (void);
internal void renderer_chunk_htable_remove   (void);

#endif //WINDOWS_OPENGL_H
