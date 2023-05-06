#ifndef RENDERER_H
#define RENDERER_H

typedef struct VoxelVertex
{
    v3 position;
    v3 normal;
    v2 texCoord;
} VoxelVertex;

/* This mesh is used a temp CPU data structure.
 It will not stay around so we allocate the max
number of vertices and indices it can hold. */
typedef struct ChunkMesh {
    /* (CHUNK_DIM^3)*6*4 vertices */
    VoxelVertex *vertices;
    u32 vertexIndex;
    /* (CHUNK_DIM^3)*6*6 indices */
    u32 *indices;
    u32 indexIndex;
} ChunkMesh;

struct RendererChunk;

global char exePath[1024];
global char *exePathLastSlash;

internal void renderer_chunk_htable_destruct      (void);
internal u32  renderer_chunk_hash                 (s32 x, s32 y, s32 z);
internal void renderer_chunk_htable_insert        (s32 x, s32 y, s32 z, ChunkMesh *mesh);
internal bool renderer_chunk_htable_exists        (s32 x, s32 y, s32 z);
internal void renderer_chunk_htable_update        (void);
internal void renderer_chunk_htable_remove        (struct RendererChunk *chunk);
internal void renderer_free_chunks_outside_radius (v3 center, s32 radius);

#endif //RENDERER_H
