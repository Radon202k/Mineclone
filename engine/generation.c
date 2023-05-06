internal void
generate_chunk_voxels(s32 cx, s32 cy, s32 cz) {
    u32 *voxels = malloc(CHUNK_VOXEL_COUNT*sizeof(u32));
    memset(voxels, 0, CHUNK_VOXEL_COUNT*sizeof(u32));
    for (s32 x=0; x<CHUNK_DIM; ++x) {
        for (s32 z=0; z<CHUNK_DIM; ++z) {
            f32 height = (1 + perlin2d((f32)(cx*CHUNK_DIM + x), 
                                       (f32)(cz*CHUNK_DIM + z), 
                                       0.1f, 8)) / 2;
            
            s32 dumbButSafeHeight = min(16, (s32)(height*CHUNK_DIM/2));
            
            for (s32 y=0; y<dumbButSafeHeight; ++y) {
                voxel_set(voxels, x,y,z, 1);
            }
        }
    }
    
    chunk_voxels_htable_insert(cx,cy,cz, voxels);
}

internal bool
face_is_exposed(ChunkVoxels *chunk, s32 x, s32 y, s32 z, FaceOrientation face) {
    u32 voxelType = 0;
    
    switch (face) {
        case Face_top:    voxelType = voxel_get(chunk, x, y+1, z); break;
        case Face_bottom: voxelType = voxel_get(chunk, x, y-1, z); break;
        case Face_left:   voxelType = voxel_get(chunk, x-1, y, z); break;
        case Face_right:  voxelType = voxel_get(chunk, x+1, y, z); break;
        case Face_front:  voxelType = voxel_get(chunk, x, y, z+1); break;
        case Face_back:   voxelType = voxel_get(chunk, x, y, z-1); break;
        default: assert(!"Should not happen");
    }
    
    return voxelType == 0;
}

internal void
generate_chunk_mesh_face(ChunkMesh *mesh, s32 x, s32 y, s32 z, 
                         u32 blockType, FaceOrientation face) {
    
    s32 indexBase = mesh->vertexIndex;
    
    f32 s = 0.5f;
    f32 d = 1.0f;
    switch (face) {
        case Face_top: {
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d+s,z*d-s}, {0,1,0}, {0,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d+s,z*d-s}, {0,1,0}, {1,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d+s,z*d+s}, {0,1,0}, {1,1}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d+s,z*d+s}, {0,1,0}, {0,1}};
        } break;
        
        case Face_bottom: {
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d-s,z*d-s}, {0,-1,0}, {0,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d-s,z*d-s}, {0,-1,0}, {1,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d-s,z*d+s}, {0,-1,0}, {1,1}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d-s,z*d+s}, {0,-1,0}, {0,1}};
        } break;
        
        case Face_left: {
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d+s,z*d-s}, {-1,0,0}, {0,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d+s,z*d+s}, {-1,0,0}, {1,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d-s,z*d+s}, {-1,0,0}, {1,1}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d-s,z*d-s}, {-1,0,0}, {0,1}};
        } break;
        
        case Face_right: {
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d+s,z*d+s}, {1,0,0}, {0,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d+s,z*d-s}, {1,0,0}, {1,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d-s,z*d-s}, {1,0,0}, {1,1}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d-s,z*d+s}, {1,0,0}, {0,1}};
        } break;
        
        case Face_front: {
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d-s,z*d+s}, {0,0,1}, {0,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d-s,z*d+s}, {0,0,1}, {1,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d+s,z*d+s}, {0,0,1}, {1,1}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d+s,z*d+s}, {0,0,1}, {0,1}};
        } break;
        
        case Face_back: {
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d-s,z*d-s}, {0,0,-1}, {0,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d-s,z*d-s}, {0,0,-1}, {1,0}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d+s,y*d+s,z*d-s}, {0,0,-1}, {1,1}};
            mesh->vertices[mesh->vertexIndex++] = (VoxelVertex){{x*d-s,y*d+s,z*d-s}, {0,0,-1}, {0,1}};
        } break;
        
        default: {
            assert(!"Should not happen");
        } break;
    }
    
    mesh->indices[mesh->indexIndex++] = indexBase + 0;
    mesh->indices[mesh->indexIndex++] = indexBase + 1;
    mesh->indices[mesh->indexIndex++] = indexBase + 2;
    mesh->indices[mesh->indexIndex++] = indexBase + 2;
    mesh->indices[mesh->indexIndex++] = indexBase + 3;
    mesh->indices[mesh->indexIndex++] = indexBase + 0;
}

internal ChunkMesh
generate_chunk_naive_mesh(ChunkVoxels *chunk) {
    u32 faceCount = 6;
    ChunkMesh result = {
        malloc(CHUNK_VOXEL_COUNT*faceCount*4*sizeof(VoxelVertex)),
        0,
        malloc(CHUNK_VOXEL_COUNT*faceCount*6*sizeof(u32)),
        0,
    };
    
    for (u32 x=0; x<CHUNK_DIM; ++x) {
        for (u32 z=0; z<CHUNK_DIM; ++z) {
            for (u32 y=0; y<CHUNK_DIM; ++y) {
                u32 blockType = voxel_get(chunk, x,y,z);
                if (blockType != 0) {
                    FaceOrientation faces[6] = {Face_top, Face_bottom, Face_left, Face_right, Face_front, Face_back};
                    for (u32 faceIndex=0; faceIndex<6; ++faceIndex) {
                        if (face_is_exposed(chunk, x,y,z, faces[faceIndex])) {
                            generate_chunk_mesh_face(&result, 
                                                     chunk->x*CHUNK_DIM + x,
                                                     chunk->y*CHUNK_DIM + y,
                                                     chunk->z*CHUNK_DIM + z,
                                                     blockType,
                                                     faces[faceIndex]);
                        }
                    }
                }
            }
        }
    }
    
    return result;
}
