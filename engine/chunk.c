internal void
chunk_voxels_htable_destruct(void) {
    for (s32 i=0; i<narray(chunkVoxelsHashTable); ++i) {
        ChunkVoxels *at = chunkVoxelsHashTable[i];
        while (at) {
            ChunkVoxels *next = at->next;
            free(at);
            at = next;
        }
    }
}

internal u32
chunk_voxels_hash(s32 x, s32 y, s32 z) {
    u32 hash = x*31 + y*479 + z*953;
    return hash & (narray(chunkVoxelsHashTable)-1);
}

internal ChunkVoxels *
chunk_voxels_htable_insert(s32 x, s32 y, s32 z, u32 *voxels) {
    /* allocate new renderer chunk */
    ChunkVoxels *chunk = malloc(sizeof *chunk);
    memset(chunk, 0, sizeof *chunk);
    chunk->x = x;
    chunk->y = y;
    chunk->z = z;
    chunk->voxels = voxels;
    /* put at head of collision chain list */
    u32 bucket = chunk_voxels_hash(x,y,z);
    chunk->next = chunkVoxelsHashTable[bucket];
    chunkVoxelsHashTable[bucket] = chunk;
    return chunk;
}

internal ChunkVoxels *
chunk_voxels_htable_find(s32 x, s32 y, s32 z) {
    u32 bucket = chunk_voxels_hash(x,y,z);
    ChunkVoxels *at = chunkVoxelsHashTable[bucket];
    while (at) {
        if (at->x == x && at->y == y && at->z == z) {
            return at;
        }
        at = at->next;
    }
    
    return 0;
}

internal void
chunk_voxels_htable_remove(ChunkVoxels *chunk) {
    u32 bucket = chunk_voxels_hash(chunk->x, chunk->y, chunk->z);
    if (chunk == chunkVoxelsHashTable[bucket]) {
        chunkVoxelsHashTable[bucket] = chunk->next;
    }
    else {
        ChunkVoxels *prev = 0;
        ChunkVoxels *at = chunkVoxelsHashTable[bucket];
        while (at != chunk) {
            prev = at;
            at = at->next;
        }
        prev->next = chunk->next;
    }
    free(chunk->voxels);
    free(chunk);
}

internal void
unload_farthest_chunks(v3 newLoadCenter, s32 newRadius) {
    /* Call platform to free chunks from gpu */
    renderer_free_chunks_outside_radius(newLoadCenter, newRadius);
    
    /* Free voxel chunks from the cpu */
    for (u32 i=0; i<narray(chunkVoxelsHashTable); ++i) {
        ChunkVoxels *chunk = chunkVoxelsHashTable[i];
        if (chunk) {
            v3 chunkP = (v3){(f32)chunk->x,(f32)chunk->y,(f32)chunk->z};
            if (v3_dist(newLoadCenter, chunkP) > newRadius) {
                chunk_voxels_htable_remove(chunk);
            }
        }
    }
}

internal void
load_chunks(v3 loadCenter, s32 r) {
    s32 cx = (s32)loadCenter.x;
    s32 cy = (s32)loadCenter.y;
    s32 cz = (s32)loadCenter.z;
    /* Generate voxels for the chunks */
    for (s32 x=cx-r; x<cx+r; ++x) {
        for (s32 z=cz-r; z<cz+r; ++z) {
            s32 y = cy;
            ChunkVoxels *chunk = chunk_voxels_htable_find(x,y,z);
            if (!chunk) {
                generate_chunk_voxels(x,0,z);
            }
        }
    }
    
    /* Generate naive mesh from the voxels (2 triangles per block) */
    // TODO: Greedy mesh
    for (s32 x=cx-r; x<cx+r; ++x) {
        for (s32 z=cz-r; z<cz+r; ++z) {
            s32 y = cy;
            ChunkVoxels *chunk = chunk_voxels_htable_find(x,y,z);
            assert(chunk);
            if (!renderer_chunk_htable_exists(x,y,z)) {
                ChunkMesh mesh = generate_chunk_naive_mesh(chunk);
                renderer_chunk_htable_insert(x,0,z, &mesh);
            }
        }
    }
}

