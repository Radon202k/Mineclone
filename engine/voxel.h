#ifndef VOXEL_H
#define VOXEL_H

internal u32  voxel_get (ChunkVoxels *chunk, s32 x, s32 y, s32 z);
internal void voxel_set (u32 *voxels, s32 x, s32 y, s32 z, u32 voxelType);

#endif //VOXEL_H
