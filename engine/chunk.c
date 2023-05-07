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
    /* add axes */
    lines3D_add(&game.lines, (v3){-1000,0,0}, (v3){1000,0,0}, (v4){1,0,0,1});
    lines3D_add(&game.lines, (v3){0,-1000,0}, (v3){0,1000,0}, (v4){0,1,0,1});
    lines3D_add(&game.lines, (v3){0,0,-1000}, (v3){0,0,1000}, (v4){0,0,1,1});
    
    s32 loadRadius = (s32)ceilf(0.5f*game.chunkLoadingDiameter);
    s32 threshold = (s32)ceilf(0.5f*loadRadius);
    // Calculate the player's current chunk coordinates based on their position
    s32 playerChunkX = (s32)game.playerCamera.p.x / CHUNK_DIM_X;
    s32 playerChunkZ = (s32)game.playerCamera.p.z / CHUNK_DIM_Z;
    
    // Check if the player has moved UPDATE_THRESHOLD chunks away from the center in any direction
    if (abs(playerChunkX - game.centerChunkX) >= threshold ||
        abs(playerChunkZ - game.centerChunkZ) >= threshold )
    {
        // Set the new center to the player's current chunk coordinates
        game.centerChunkX = playerChunkX;
        game.centerChunkY = 0;
        game.centerChunkZ = playerChunkZ;
        
        unload_far_away_chunks();
        
        // Iterate through the chunks in the area determined by the load radius
        for (s32 x = game.centerChunkX - loadRadius;
             x <= game.centerChunkX + loadRadius;
             ++x)
        {
            s32 y = 0;
            for (s32 z = game.centerChunkZ - loadRadius;
                 z <= game.centerChunkZ + loadRadius;
                 ++z)
            {
                /* if the chunk is not in the generated voxels hash table */
                ChunkVoxels *find = chunk_voxels_htable_find(x, y, z);
                if (!find)
                {
                    /* then generate it */
                    u32* voxels = generate_chunk_voxels(x, y, z);
                    /* insert it into the hash table */
                    chunk_voxels_htable_insert(x, y, z, voxels);
                }
            }
        }
        
        /* clear lines */
        lines3D_clear(&game.lines);
        
        /* NOTE: We loop twice because we need all chunks generated in order
         to know which faces of the cubes are exposed (in chunk boundaries) */
        
        // Iterate through the chunks in the area determined by the load radius
        for (s32 x = game.centerChunkX - loadRadius;
             x <= game.centerChunkX + loadRadius;
             ++x)
        {
            for (s32 z = game.centerChunkZ - loadRadius;
                 z <= game.centerChunkZ + loadRadius;
                 ++z)
            {
                /* should always find the chunk since we inserted in the loop above */
                ChunkVoxels* chunk = chunk_voxels_htable_find(x, 0, z);
                assert(chunk);
                
                /* if the respective renderer chunk doesn't exist */
                if (!renderer_chunk_htable_exists(x, 0, z))
                {
                    /* Generate naive mesh from the voxels (2 triangles per block's face!) */
                    // TODO: Greedy mesh
                    ChunkMesh mesh = generate_chunk_naive_mesh(chunk);
                    /* upload it */
                    renderer_chunk_htable_insert(x, 0, z, &mesh);
                    /* destroy the mesh */
                    free(mesh.vertices);
                    free(mesh.indices);
                    
#if 0
                    /* generate debug lines around chunk */
                    f32 chunkMinX = (f32)(x-1)*CHUNK_DIM_X - CHUNK_HALF_DIM_X;
                    f32 chunkMinY = (f32)-1*CHUNK_HALF_DIM_Y;
                    f32 chunkMinZ = (f32)(z-1)*CHUNK_DIM_Z  - CHUNK_HALF_DIM_Z;
                    f32 chunkMaxX = chunkMinX + (f32)CHUNK_DIM_X;
                    f32 chunkMaxY = chunkMinY + (f32)CHUNK_DIM_Y;
                    f32 chunkMaxZ = chunkMinZ + (f32)CHUNK_DIM_Z;
                    
                    /* vertical lines */
                    lines3D_add(&game.lines, 
                                (v3){chunkMinX, chunkMinY, chunkMinZ},
                                (v3){chunkMinX, chunkMaxY, chunkMinZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMinX, chunkMinY, chunkMaxZ},
                                (v3){chunkMinX, chunkMaxY, chunkMaxZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMaxX, chunkMinY, chunkMinZ},
                                (v3){chunkMaxX, chunkMaxY, chunkMinZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMaxX, chunkMinY, chunkMaxZ},
                                (v3){chunkMaxX, chunkMaxY, chunkMaxZ},
                                (v4){1,0,1,1});
                    
                    /* horizontal lines bottom */
                    lines3D_add(&game.lines, 
                                (v3){chunkMinX, chunkMinY, chunkMinZ},
                                (v3){chunkMaxX, chunkMinY, chunkMinZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMinX, chunkMinY, chunkMaxZ},
                                (v3){chunkMaxX, chunkMinY, chunkMaxZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMinX, chunkMinY, chunkMinZ},
                                (v3){chunkMinX, chunkMinY, chunkMaxZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMaxX, chunkMinY, chunkMinZ},
                                (v3){chunkMaxX, chunkMinY, chunkMaxZ},
                                (v4){1,0,1,1});
                    
                    /* horizontal lines top */
                    lines3D_add(&game.lines, 
                                (v3){chunkMinX, chunkMaxY, chunkMinZ},
                                (v3){chunkMaxX, chunkMaxY, chunkMinZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMinX, chunkMaxY, chunkMaxZ},
                                (v3){chunkMaxX, chunkMaxY, chunkMaxZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMinX, chunkMaxY, chunkMinZ},
                                (v3){chunkMinX, chunkMaxY, chunkMaxZ},
                                (v4){1,0,1,1});
                    
                    lines3D_add(&game.lines, 
                                (v3){chunkMaxX, chunkMaxY, chunkMinZ},
                                (v3){chunkMaxX, chunkMaxY, chunkMaxZ},
                                (v4){1,0,1,1});
#endif
                    
                }
            }
        }
    }
    
    /* update lines */
    renderer_lines3D_update(&game.lines);
    
}