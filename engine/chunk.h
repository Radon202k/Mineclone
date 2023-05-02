#ifndef CHUNK_H
#define CHUNK_H

#define CHUNK_DIM 16
#define CHUNK_VOXEL_COUNT (CHUNK_DIM*CHUNK_DIM*CHUNK_DIM)

typedef enum FaceOrientation {
    Face_top,
    Face_bottom,
    Face_left,
    Face_right,
    Face_front,
    Face_back,
} FaceOrientation;

internal u32
voxel_get(u32 *voxels, s32 x, s32 y, s32 z) {
    if (x >= 0 && x < CHUNK_DIM &&
        y >= 0 && y < CHUNK_DIM &&
        z >= 0 && z < CHUNK_DIM) {
        u32 index = (z*CHUNK_DIM*CHUNK_DIM +
                     y*CHUNK_DIM +
                     x);
        return voxels[index];
    }
    return 0;
}

internal void
voxel_set(u32 *voxels, s32 x, s32 y, s32 z, u32 voxelType) {
    u32 index = (z*CHUNK_DIM*CHUNK_DIM +
                 y*CHUNK_DIM +
                 x);
    voxels[index] = voxelType;
}

#endif //CHUNK_H
