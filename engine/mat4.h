#ifndef MAT4_H
#define MAT4_H

typedef union mat4 {
    struct { v4 col0, col1, col2, col3; };
    f32 e[4][4];
    f32 d[16];
} mat4;

internal mat4
mat4_mul(mat4 a, mat4 b) {
    
}

internal v4
mat4_mulv(mat4 m, v4 v) {
    
}

internal mat4
mat4_inv(mat4 a) {
}

internal v4
mat4_unproject(mat4 m, v4 v) {
}

internal mat4
mat4_identity(void) {
}

internal mat4
mat4_perspective(f32 fovy, f32 aspect, f32 nearZ, f32 farZ) {
    mat4 result = {0};
    
    f32 f  = 1.0f / tanf(fovy * 0.5f);
    f32 fn = 1.0f / (nearZ - farZ);
    
    result.e[0][0] = f / aspect;
    result.e[1][1] = f;
    result.e[2][2] = (nearZ + farZ) * fn;
    result.e[2][3] =-1.0f;
    result.e[3][2] = 2.0f * nearZ * farZ * fn;
    return result;
}

internal mat4
mat4_orthographic() {
}

internal mat4
mat4_lookat(v3 eye, v3 center, v3 up) {
    v3 f = v3_sub(center, eye);
    v3_normalize(&f);
    
    v3 s = v3_crossn(f, up);
    v3 u = v3_cross(s, f);
    
    mat4 result = {0};
    result.e[0][0] = s.e[0];
    result.e[0][1] = u.e[0];
    result.e[0][2] =-f.e[0];
    result.e[1][0] = s.e[1];
    result.e[1][1] = u.e[1];
    result.e[1][2] =-f.e[1];
    result.e[2][0] = s.e[2];
    result.e[2][1] = u.e[2];
    result.e[2][2] =-f.e[2];
    result.e[3][0] =-v3_dot(s, eye);
    result.e[3][1] =-v3_dot(u, eye);
    result.e[3][2] = v3_dot(f, eye);
    result.e[0][3] = result.e[1][3] = result.e[2][3] = 0.0f;
    result.e[3][3] = 1.0f;
    return result;
}

#endif //MAT4_H
