#ifndef RENDERER_H
#define RENDERER_H

typedef struct VoxelVertex
{
    v3 position;
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

#endif //RENDERER_H
