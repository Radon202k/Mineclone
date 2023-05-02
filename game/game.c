internal void
generate_chunk(s32 cx, s32 cy, s32 cz) {
    u32 *voxels = malloc(CHUNK_VOXEL_COUNT*sizeof(u32));
    for (s32 x=0; x<CHUNK_DIM; ++x) {
        for (s32 z=0; z<CHUNK_DIM; ++z) {
            // TODO: height width perlin 2d
            for (s32 y=0; y<CHUNK_DIM; ++y) {
                voxel_set(voxels, x,y,z, 1);
            }
        }
    }
    
    ChunkVoxels *chunk = chunk_voxels_htable_insert(cx,cy,cz, voxels);
    ChunkMesh mesh = generate_chunk_naive_mesh(chunk);
    
    renderer_chunk_htable_insert(cx,cy,cz, &mesh);
}

internal void
game_construct(void) {
    s32 r = 2;
    for (s32 x=-r; x<r; ++x) {
        for (s32 z=-r; z<r; ++z) {
            generate_chunk(x,0,z);
        }
    }
}