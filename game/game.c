internal void
generate_chunk_voxels(s32 cx, s32 cy, s32 cz) {
    u32 *voxels = malloc(CHUNK_VOXEL_COUNT*sizeof(u32));
    memset(voxels, 0, CHUNK_VOXEL_COUNT*sizeof(u32));
    for (s32 x=0; x<CHUNK_DIM; ++x) {
        for (s32 z=0; z<CHUNK_DIM; ++z) {
            f32 height = (1 + perlin2d((f32)(cx*CHUNK_DIM + x), 
                                       (f32)(cz*CHUNK_DIM + z), 
                                       0.1f, 8)) / 2;
            for (s32 y=0; y<(s32)(height*CHUNK_DIM/2); ++y) {
                voxel_set(voxels, x,y,z, 1);
            }
        }
    }
    
    chunk_voxels_htable_insert(cx,cy,cz, voxels);
}

internal void
game_construct(void) {
    s32 r = 2;
    
    /* Generate voxels for the chunks */
    for (s32 x=-r; x<r; ++x) {
        for (s32 z=-r; z<r; ++z) {
            generate_chunk_voxels(x,0,z);
        }
    }
    
    /* Generate naive mesh from the voxels (2 triangles per block) */
    // TODO: Greedy mesh
    for (s32 x=-r; x<r; ++x) {
        for (s32 z=-r; z<r; ++z) {
            ChunkVoxels *chunk = chunk_voxels_htable_find(x,0,z);
            ChunkMesh mesh = generate_chunk_naive_mesh(chunk);
            renderer_chunk_htable_insert(x,0,z, &mesh);
        }
    }
}

internal void
game_update(void) {
    if (keyboard.w.down) {
        camera_first_person_move_north(&game.playerCamera);
    }
    
    if (keyboard.a.down) {
        camera_first_person_move_east(&game.playerCamera);
    }
    
    if (keyboard.s.down) {
        camera_first_person_move_south(&game.playerCamera);
    }
    
    if (keyboard.d.down) {
        camera_first_person_move_west(&game.playerCamera);
    }
    
    if (keyboard.space.down) {
        camera_first_person_move_up(&game.playerCamera);
    }
    
    if (keyboard.lshift.down) {
        camera_first_person_move_down(&game.playerCamera);
    }
    
    v2 deltaP = v2_sub(mouse.p, game.lastMouseP);
    game.lastMouseP = mouse.p;
    
    game.playerCamera.yaw -= 0.2f * deltaP.x;
    game.playerCamera.pitch -= 0.1f * deltaP.y;
}