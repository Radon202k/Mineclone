internal u32
voxel_get(ChunkVoxels *chunk, s32 x, s32 y, s32 z) {
    if (x == -1) {
        ChunkVoxels *leftChunk = 
            chunk_voxels_htable_find(chunk->x-1,chunk->y,chunk->z);
        if (leftChunk) {
            u32 index = (z*CHUNK_DIM*CHUNK_DIM + y*CHUNK_DIM + CHUNK_DIM-1);
            return leftChunk->voxels[index];
        }
        else return 0;
    }
    else if (x == CHUNK_DIM) {
        ChunkVoxels *rightChunk = 
            chunk_voxels_htable_find(chunk->x+1,chunk->y,chunk->z);
        if (rightChunk) {
            u32 index = (z*CHUNK_DIM*CHUNK_DIM + y*CHUNK_DIM);
            return rightChunk->voxels[index];
        }
        else return 0;
    }
    else if (y == -1) {
        ChunkVoxels *bottomChunk = 
            chunk_voxels_htable_find(chunk->x,chunk->y-1,chunk->z);
        if (bottomChunk) {
            u32 index = (z*CHUNK_DIM*CHUNK_DIM + (CHUNK_DIM-1)*CHUNK_DIM + x);
            return bottomChunk->voxels[index];
        }
        else return 0;
    }
    else if (y == CHUNK_DIM) {
        ChunkVoxels *topChunk = 
            chunk_voxels_htable_find(chunk->x,chunk->y+1,chunk->z);
        if (topChunk) {
            u32 index = (z*CHUNK_DIM*CHUNK_DIM + x);
            return topChunk->voxels[index];
        }
        else return 0;
    }
    else if (z == -1) {
        ChunkVoxels *backChunk = 
            chunk_voxels_htable_find(chunk->x,chunk->y,chunk->z-1);
        if (backChunk) {
            u32 index = ((CHUNK_DIM-1)*CHUNK_DIM*CHUNK_DIM + y*CHUNK_DIM + x);
            return backChunk->voxels[index];
        }
        else return 0;
    }
    else if (z == CHUNK_DIM) {
        ChunkVoxels *frontChunk = 
            chunk_voxels_htable_find(chunk->x,chunk->y,chunk->z+1);
        if (frontChunk) {
            u32 index = (y*CHUNK_DIM + x);
            return frontChunk->voxels[index];
        }
        else return 0;
    }
    else {
        u32 index = (z*CHUNK_DIM*CHUNK_DIM + y*CHUNK_DIM + x);
        return chunk->voxels[index];
    }
}

internal void
voxel_set(u32 *voxels, s32 x, s32 y, s32 z, u32 voxelType) {
    u32 index = (z*CHUNK_DIM*CHUNK_DIM +
                 y*CHUNK_DIM +
                 x);
    voxels[index] = voxelType;
}

