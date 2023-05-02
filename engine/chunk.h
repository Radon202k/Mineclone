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

#endif //CHUNK_H
