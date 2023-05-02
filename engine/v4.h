#ifndef V4_H
#define V4_H

typedef union v4 {
    struct { f32 x, y, z, w; };
    f32 e[4];
} v4;

internal  v4 v4_add  ( v4 a,  v4 b) { return (v4){ a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w }; }
internal  v4 v4_sub  ( v4 a,  v4 b) { return (v4){ a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w }; }
internal  v4 v4_mul  (f32 k,  v4 a) { return (v4){   k*a.x,   k*a.y,   k*a.z,   k*a.w }; }
internal  v4 v4_div  ( v4 a, f32 d) { f32 k=1/d; return v4_mul(k,a);                     }
internal f32 v4_dot  ( v4 a,  v4 b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;      }
internal f32 v4_len2 ( v4 a       ) { return v4_dot(a,a);                                }
internal f32 v4_len  ( v4 a       ) { return sqrtf(v4_len2(a));                          }

#endif //V4_H
