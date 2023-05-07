#ifndef RENDERER_H
#define RENDERER_H

/* TODO: experiment with other strategies for
rendering lines. At the moment we are allocating
a maximum number of lines at start up and updating
the vbo with new data when needed. */
#define MAX_LINES 10*1024

typedef struct {
    f32 u_min;
    f32 u_max;
    f32 v_min;
    f32 v_max;
} BlockTextureInfo;

typedef struct
{
    v3 position;
    v3 normal;
    v2 texCoord;
} VoxelVertex;

/* This mesh is used a temp CPU data structure.
 It will not stay around so we allocate the max
number of vertices and indices it can hold. */
typedef struct {
    /* (CHUNK_DIM^3)*6*4 vertices */
    VoxelVertex *vertices;
    u32 vertexIndex;
    /* (CHUNK_DIM^3)*6*6 indices */
    u32 *indices;
    u32 indexIndex;
} ChunkMesh;

typedef struct
{
    v3 pos;
    v4 color;
} LineVertex;

typedef struct {
    LineVertex *vertices;
    u32 vertexIndex;
} Lines3D;

struct RendererChunk;

global char exePath[1024];
global char *exePathLastSlash;

/* chunk renderer interface */
internal void renderer_chunk_htable_destruct      (void);
internal u32  renderer_chunk_hash                 (s32 x, s32 y, s32 z);
internal void renderer_chunk_htable_insert        (s32 x, s32 y, s32 z, ChunkMesh *mesh);
internal bool renderer_chunk_htable_exists        (s32 x, s32 y, s32 z);
internal void renderer_chunk_htable_update        (void);
internal void renderer_chunk_htable_remove        (s32 x, s32 y, s32 z);

/* line renderer interface */
internal void renderer_lines3D_update(Lines3D *lines);

internal void
lines3D_add(Lines3D *lines, v3 start, v3 end, v4 color)
{
    lines->vertices[lines->vertexIndex++] = (LineVertex){start, color};
    lines->vertices[lines->vertexIndex++] = (LineVertex){end, color};
}

internal void
lines3D_clear(Lines3D *lines)
{
    lines->vertexIndex = 0;
}

internal void
lines3D_destruct(Lines3D *lines)
{
    free(lines->vertices);
    lines->vertexIndex = 0;
}

internal Lines3D
lines3D_construct(s32 initialCapacity)
{
    Lines3D lines;
    lines.vertices = (LineVertex *)malloc(initialCapacity * sizeof(LineVertex));
    lines.vertexIndex = 0;
    return lines;
}

#endif //RENDERER_H
