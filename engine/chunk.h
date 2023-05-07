#ifndef CHUNK_H
#define CHUNK_H

#define CHUNK_DIM_X 15
#define CHUNK_DIM_Z 15
#define CHUNK_DIM_Y 31

#define CHUNK_HALF_DIM_X CHUNK_DIM_X/2
#define CHUNK_HALF_DIM_Y CHUNK_DIM_Y/2
#define CHUNK_HALF_DIM_Z CHUNK_DIM_Z/2

#define CHUNK_VOXEL_COUNT (CHUNK_DIM_X*CHUNK_DIM_Y*CHUNK_DIM_Z)

typedef struct ChunkVoxels {
    s32 x, y, z; /* key */
    u32 *voxels; /* value */
    struct ChunkVoxels *next;
} ChunkVoxels;

global ChunkVoxels *chunkVoxelsHashTable[256];

internal u32           chunk_voxels_hash          (s32 x, s32 y, s32 z);
internal ChunkVoxels * chunk_voxels_htable_insert (s32 x, s32 y, s32 z, u32 *voxels);
internal ChunkVoxels * chunk_voxels_htable_find   (s32 x, s32 y, s32 z);
internal void          chunk_voxels_htable_update (void);
internal void          chunk_voxels_htable_remove (ChunkVoxels *chunk);

internal void          unload_far_away_chunks     (void);
internal void          load_chunks_update         (void);
internal void          load_chunk                 (s32 x, s32 y, s32 z);

#endif //CHUNK_H
