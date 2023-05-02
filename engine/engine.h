#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define internal static
#define global static

typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#define deg2rad(degrees) ((degrees)*M_PI/180)

#define narray(array) (sizeof(array)/sizeof((array)[0]))

#include "v2.h"
#include "v3.h"
#include "v4.h"
#include "mat4.h"
#include "chunk.h"
#include "input.h"
#include "renderer.h"
#include "generation.h"
#include "camera.h"

internal u8 * platform_file_read  (char *path);
internal void platform_file_write (char *path, u8 *contents);

#endif //ENGINE_H
