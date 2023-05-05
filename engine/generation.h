#ifndef GENERATION_H
#define GENERATION_H

internal void      generate_chunk_voxels(s32 cx, s32 cy, s32 cz);
internal bool      face_is_exposed(ChunkVoxels *chunk, s32 x, s32 y, s32 z,
                                   FaceOrientation face);
internal void      generate_chunk_mesh_face(ChunkMesh *mesh, s32 x, s32 y, s32 z, 
                                            u32 voxelType, FaceOrientation face);
internal ChunkMesh generate_chunk_naive_mesh(ChunkVoxels *chunk);



#endif //GENERATION_H
