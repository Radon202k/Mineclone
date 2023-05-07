internal u32
voxel_index(s32 x, s32 y, s32 z) {
    x += CHUNK_HALF_DIM_X;
    y += CHUNK_HALF_DIM_Y;
    z += CHUNK_HALF_DIM_Z;
    
    u32 index = (z*CHUNK_DIM_X*CHUNK_DIM_Y + y*CHUNK_DIM_X + x);
    return index;
}

internal u32
voxel_get(ChunkVoxels *chunk, s32 x, s32 y, s32 z)
{
    /* if trying to look at a voxel too far to the left */
    if (x == -CHUNK_HALF_DIM_X-1)
    {
        /* get the chunk to the left of this if it exists */
        ChunkVoxels *leftChunk = chunk_voxels_htable_find(chunk->x-1, 0, chunk->z);
        if (leftChunk) {
            /* adjust the x to point to the last block */
            x = CHUNK_HALF_DIM_X;
            /* return that voxel value in the left chunk */
            return leftChunk->voxels[voxel_index(x,y,z)];
        }
    }
    /* if trying to look at a voxel too far to the right */
    else if (x == CHUNK_HALF_DIM_X+1)
    {
        /* get the chunk to the right of this if it exists */
        ChunkVoxels *rightChunk = chunk_voxels_htable_find(chunk->x+1, 0, chunk->z);
        if (rightChunk) {
            /* adjust the x to point to the first block */
            x = -CHUNK_HALF_DIM_X;
            /* return that voxel value in the left chunk */
            return rightChunk->voxels[voxel_index(x,y,z)];
        }
    }
    /* if trying to look at a voxel too far to the back */
    else if (z == -CHUNK_HALF_DIM_Z-1)
    {
        /* get the chunk to the back of this if it exists */
        ChunkVoxels *backChunk = chunk_voxels_htable_find(chunk->x, 0, chunk->z-1);
        if (backChunk) {
            /* adjust the z to point to the first block */
            z = -CHUNK_HALF_DIM_Z;
            /* return that voxel value in the left chunk */
            return backChunk->voxels[voxel_index(x,y,z)];
        }
    }
    /* if trying to look at a voxel too far to the front */
    else if (z == CHUNK_HALF_DIM_Z+1)
    {
        /* get the chunk to the front of this if it exists */
        ChunkVoxels *frontChunk = chunk_voxels_htable_find(chunk->x, 0, chunk->z+1);
        if (frontChunk) {
            /* adjust the z to point to the last block */
            z = CHUNK_HALF_DIM_Z;
            /* return that voxel value in the left chunk */
            return frontChunk->voxels[voxel_index(x,y,z)];
        }
    }
    else
    {
        if (y >= -CHUNK_HALF_DIM_Y && y <= CHUNK_HALF_DIM_Y)
        {
            return chunk->voxels[voxel_index(x,y,z)];
        }
    }
    return 0;
}

internal void
voxel_set(u32 *voxels, s32 x, s32 y, s32 z, u32 voxelType) {
    u32 index = ((z + CHUNK_HALF_DIM_Z)*CHUNK_DIM_X*CHUNK_DIM_Y +
                 (y + CHUNK_HALF_DIM_Y)*CHUNK_DIM_X +
                 (x + CHUNK_HALF_DIM_X));
    
    voxels[index] = voxelType;
}

