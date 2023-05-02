#ifndef CAMERA_H
#define CAMERA_H

internal v3
camera_orbit_position(f32 distance, f32 azimuth, f32 elevation) {
    return (v3) {
        distance * sinf(elevation) * sinf(azimuth),
        distance * cosf(elevation),
        distance * sinf(elevation) * cosf(azimuth),
    };
}

#endif //CAMERA_H
