#ifndef V3_H
#define V3_H

typedef union v3 {
    struct { f32 x, y, z; };
    f32 e[3];
} v3;

internal  v3 v3_add  ( v3 a,  v3 b) { return (v3){ a.x+b.x, a.y+b.y, a.z+b.z }; }
internal  v3 v3_sub  ( v3 a,  v3 b) { return (v3){ a.x-b.x, a.y-b.y, a.z-b.z }; }
internal  v3 v3_mul  (f32 k,  v3 a) { return (v3){   k*a.x,   k*a.y,   k*a.z }; }
internal  v3 v3_div  ( v3 a, f32 d) { f32 k=1/d; return v3_mul(k,a);            }
internal f32 v3_dot  ( v3 a,  v3 b) { return a.x*b.x + a.y*b.y + a.z*b.z;       }
internal f32 v3_len2 ( v3 a       ) { return v3_dot(a,a);                       }
internal f32 v3_len  ( v3 a       ) { return sqrtf(v3_len2(a));                 }

internal void
v3_normalize(v3 *a) {
    f32 length = v3_len(*a);
    *a = (v3) {
        a->x / length,
        a->y / length,
        a->z / length,
    };
}

internal v3
v3_cross(v3 a, v3 b) {
    return (v3) {
        a.e[1] * b.e[2] - a.e[2] * b.e[1],
        a.e[2] * b.e[0] - a.e[0] * b.e[2],
        a.e[0] * b.e[1] - a.e[1] * b.e[0],
    };
}

internal v3
v3_crossn(v3 a, v3 b) {
    v3 result = v3_cross(a, b);
    v3_normalize(&result);
    return result;
}

#endif //V3_H
