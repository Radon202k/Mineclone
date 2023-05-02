#ifndef GENERATION_H
#define GENERATION_H

typedef struct ChunkVoxels {
    s32 x, y, z; /* key */
    u32 *voxels; /* value */
    struct ChunkVoxels *next;
} ChunkVoxels;

global ChunkVoxels *chunkVoxelsHashTable[4096];

internal u32  chunk_voxels_hash                  (s32 x, s32 y, s32 z);
internal ChunkVoxels *chunk_voxels_htable_insert (s32 x, s32 y, s32 z, u32 *voxels);
internal ChunkVoxels *chunk_voxels_htable_find   (s32 x, s32 y, s32 z);
internal void chunk_voxels_htable_update         (void);
internal void chunk_voxels_htable_remove         (void);




#endif //GENERATION_H
