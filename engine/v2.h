#ifndef V2_H
#define V2_H

typedef union v2 {
    struct { f32 x, y; };
    f32 e[2];
} v2;

internal  v2 v2_add  ( v2 a,  v2 b) { return (v2){ a.x+b.x, a.y+b.y }; }
internal  v2 v2_sub  ( v2 a,  v2 b) { return (v2){ a.x-b.x, a.y-b.y }; }
internal  v2 v2_mul  (f32 k,  v2 a) { return (v2){   k*a.x,   k*a.y }; }
internal  v2 v2_div  ( v2 a, f32 d) { f32 k=1/d; return v2_mul(k,a);   }
internal f32 v2_dot  ( v2 a,  v2 b) { return a.x*b.x + a.y*b.y;        }
internal f32 v2_len2 ( v2 a       ) { return v2_dot(a,a);              }
internal f32 v2_len  ( v2 a       ) { return sqrtf(v2_len2(a));        }

#endif //V2_H
