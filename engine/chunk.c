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
unload_far_away_chunks(void) {
    /* for every loaded chunk */
    for (u32 i=0; i<narray(chunkVoxelsHashTable); ++i) {
        ChunkVoxels *chunk = chunkVoxelsHashTable[i];
        if (chunk) {
            /* if the chunk is too far away from new center */
            if (abs(chunk->x - game.centerChunkX) >= 0.5f*game.chunkLoadingDiameter ||
                abs(chunk->y - game.centerChunkY) >= 0.5f*game.chunkLoadingDiameter ||
                abs(chunk->z - game.centerChunkZ) >= 0.5f*game.chunkLoadingDiameter)
            {
                /* remove from renderer's hash table */
                renderer_chunk_htable_remove(chunk->x, chunk->y, chunk->z);

                /* remove from voxels' hash table */
                chunk_voxels_htable_remove(chunk);
            }
        }
    }
}

internal void
load_chunks_update(void) {
    s32 loadRadius = (s32)ceilf(0.5f*game.chunkLoadingDiameter);
    s32 threshold = (s32)ceilf(0.5f*loadRadius);
    // Calculate the player's current chunk coordinates based on their position
    s32 playerChunkX = (s32)game.playerCamera.p.x / CHUNK_DIM;
    s32 playerChunkY = (s32)game.playerCamera.p.y / CHUNK_DIM;
    s32 playerChunkZ = (s32)game.playerCamera.p.z / CHUNK_DIM;

    // Check if the player has moved UPDATE_THRESHOLD chunks away from the center in any direction
    if (abs(playerChunkX - game.centerChunkX) >= threshold ||
        abs(playerChunkY - game.centerChunkY) >= threshold ||
        abs(playerChunkZ - game.centerChunkZ) >= threshold )
    {
        // Set the new center to the player's current chunk coordinates
        game.centerChunkX = playerChunkX;
        game.centerChunkY = playerChunkY;
        game.centerChunkZ = playerChunkZ;

        unload_far_away_chunks();

        // Iterate through the chunks in the area determined by the load radius
        for (s32 x = game.centerChunkX - loadRadius;
            x <= game.centerChunkX + loadRadius;
            ++x)
        {
        // TODO: multiple y chunks?
    #if 0
            for (s32 y = centerChunkY - loadRadius;
                y <= centerChunkY + loadRadius;
                ++y)
    #else
            s32 y = 0;
    #endif
            {
                for (s32 z = game.centerChunkZ - loadRadius;
                    z <= game.centerChunkZ + loadRadius;
                    ++z)
                {
                    /* if the chunk is not loaded already */
                    ChunkVoxels *find = chunk_voxels_htable_find(x, y, z);
                    if (!find) {
                        /* then load it */
                        load_chunk(x, y, z);
                    }
                }
            }
        }
    }
}

internal void
load_chunk(s32 x, s32 y, s32 z) {
    u32* voxels = generate_chunk_voxels(x, y, z);
    /* insert it into the hash table */
    chunk_voxels_htable_insert(x, y, z, voxels);

    /* should always find the chunk since we inserted in the loop above */
    ChunkVoxels* chunk = chunk_voxels_htable_find(x, y, z);
    assert(chunk);

    /* if the respective renderer chunk doesn't exist */
    if (!renderer_chunk_htable_exists(x, y, z)) {
        /* Generate naive mesh from the voxels (2 triangles per block's face!) */
        // TODO: Greedy mesh
        ChunkMesh mesh = generate_chunk_naive_mesh(chunk);
        /* upload it */
        renderer_chunk_htable_insert(x, 0, z, &mesh);
        /* destroy the mesh */
        free(mesh.vertices);
        free(mesh.indices);
    }
}