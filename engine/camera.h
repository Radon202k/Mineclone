#ifndef CAMERA_H
#define CAMERA_H

typedef union Camera {
    struct {
        v3 target;
        f32 distance;
        f32 azimuth;
        f32 elevation;
    };
    struct {
        v3 p;
        f32 yaw;
        f32 pitch;
    };
} Camera;

internal void
camera_first_person_move_east(Camera *camera, f32 speed) {
    camera->p.x -= speed * 0.1f * cosf(deg2rad(camera->yaw));
    camera->p.z += speed * 0.1f * sinf(deg2rad(camera->yaw));
}

internal void
camera_first_person_move_west(Camera *camera, f32 speed) {
    camera->p.x += speed * 0.1f * cosf(deg2rad(camera->yaw));
    camera->p.z -= speed * 0.1f * sinf(deg2rad(camera->yaw));
}

internal void
camera_first_person_move_north(Camera *camera, f32 speed) {
    camera->p.x -= speed * 0.1f * sinf(deg2rad(camera->yaw));
    camera->p.z -= speed * 0.1f * cosf(deg2rad(camera->yaw));
}

internal void
camera_first_person_move_south(Camera *camera, f32 speed) {
    camera->p.x += speed * 0.1f * sinf(deg2rad(camera->yaw));
    camera->p.z += speed * 0.1f * cosf(deg2rad(camera->yaw));
}

internal void
camera_first_person_move_up(Camera *camera, f32 speed) {
    camera->p.y += speed * 0.1f;
}

internal void
camera_first_person_move_down(Camera *camera, f32 speed) {
    camera->p.y -= speed * 0.1f;
}

internal v3
camera_orbit_position(f32 distance, f32 azimuth, f32 elevation) {
    return (v3) {
        distance * sinf(elevation) * sinf(azimuth),
        distance * cosf(elevation),
        distance * sinf(elevation) * cosf(azimuth),
    };
}

#endif //CAMERA_H
