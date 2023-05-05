// thanks to https://gist.github.com/mmozeiko/ed2ad27f75edf9c26053ce332a1f6647

#include "windows_opengl.h"

static void FatalError(const char* message)
{
    MessageBoxA(NULL, message, "Error", MB_ICONEXCLAMATION);
    ExitProcess(0);
}

#ifndef NDEBUG
static void APIENTRY DebugCallback(
                                   GLenum source, GLenum type, GLuint id, GLenum severity,
                                   GLsizei length, const GLchar* message, const void* user)
{
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        if (IsDebuggerPresent())
        {
            Assert(!"OpenGL error - check the callstack in debugger");
        }
        FatalError("OpenGL API usage error! Use debugger to examine call stack!");
    }
}
#endif

static LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_MOUSEMOVE: {
            mouse.p = (v2) {
                (f32)GET_X_LPARAM(lparam),
                (f32)GET_Y_LPARAM(lparam),
            };
        } break;
        
        case WM_LBUTTONDOWN: {
            mouse.left.down = true;
            mouse.left.pressed = true;
        } break;
        
        case WM_LBUTTONUP: {
            mouse.left.down = false;
            mouse.left.released = true;
        } break;
        
        case WM_RBUTTONDOWN: {
            mouse.right.down = true;
            mouse.right.pressed = true;
        } break;
        
        case WM_RBUTTONUP: {
            mouse.right.down = false;
            mouse.right.released = true;
        } break;
        
        case WM_KEYDOWN: {
            Button *key = 0;
            if      (wparam == 'W')        key = &keyboard.w;
            else if (wparam == 'A')        key = &keyboard.a;
            else if (wparam == 'S')        key = &keyboard.s;
            else if (wparam == 'D')        key = &keyboard.d;
            else if (wparam == VK_SPACE)   key = &keyboard.space;
            else if (wparam == VK_CONTROL) key = &keyboard.control;
            else if (wparam == VK_MENU)    key = &keyboard.alt;
            
            if (key) {
                key->down = true;
                key->pressed = true;
            }
        } break;
        
        case WM_KEYUP: {
            Button *key = 0;
            if      (wparam == 'W')        key = &keyboard.w;
            else if (wparam == 'A')        key = &keyboard.a;
            else if (wparam == 'S')        key = &keyboard.s;
            else if (wparam == 'D')        key = &keyboard.d;
            else if (wparam == VK_SPACE)   key = &keyboard.space;
            else if (wparam == VK_CONTROL) key = &keyboard.control;
            else if (wparam == VK_MENU)    key = &keyboard.alt;
            
            if (key) {
                key->down = false;
                key->released = true;
            }
        } break;
        
        case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(wnd, msg, wparam, lparam);
}

// compares src string with dstlen characters from dst, returns 1 if they are equal, 0 if not
static int StringsAreEqual(const char* src, const char* dst, size_t dstlen)
{
    while (*src && dstlen-- && *dst)
    {
        if (*src++ != *dst++)
        {
            return 0;
        }
    }
    
    return (dstlen && *src == *dst) || (!dstlen && *src == 0);
}

static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

static void GetWglFunctions(void)
{
    // to get WGL functions we need valid GL context, so create dummy window for dummy GL contetx
    HWND dummy = CreateWindowExW(
                                 0, L"STATIC", L"DummyWindow", WS_OVERLAPPED,
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 NULL, NULL, NULL, NULL);
    Assert(dummy && "Failed to create dummy window");
    
    HDC dc = GetDC(dummy);
    Assert(dc && "Failed to get device context for dummy window");
    
    PIXELFORMATDESCRIPTOR desc =
    {
        .nSize = sizeof(desc),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
    };
    
    int format = ChoosePixelFormat(dc, &desc);
    if (!format)
    {
        FatalError("Cannot choose OpenGL pixel format for dummy window!");
    }
    
    int ok = DescribePixelFormat(dc, format, sizeof(desc), &desc);
    Assert(ok && "Failed to describe OpenGL pixel format");
    
    // reason to create dummy window is that SetPixelFormat can be called only once for the window
    if (!SetPixelFormat(dc, format, &desc))
    {
        FatalError("Cannot set OpenGL pixel format for dummy window!");
    }
    
    HGLRC rc = wglCreateContext(dc);
    Assert(rc && "Failed to create OpenGL context for dummy window");
    
    ok = wglMakeCurrent(dc, rc);
    Assert(ok && "Failed to make current OpenGL context for dummy window");
    
    // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_extensions_string.txt
    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
    (void*)wglGetProcAddress("wglGetExtensionsStringARB");
    if (!wglGetExtensionsStringARB)
    {
        FatalError("OpenGL does not support WGL_ARB_extensions_string extension!");
    }
    
    const char* ext = wglGetExtensionsStringARB(dc);
    Assert(ext && "Failed to get OpenGL WGL extension string");
    
    const char* start = ext;
    for (;;)
    {
        while (*ext != 0 && *ext != ' ')
        {
            ext++;
        }
        
        size_t length = ext - start;
        if (StringsAreEqual("WGL_ARB_pixel_format", start, length))
        {
            // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt
            wglChoosePixelFormatARB = (void*)wglGetProcAddress("wglChoosePixelFormatARB");
        }
        else if (StringsAreEqual("WGL_ARB_create_context", start, length))
        {
            // https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt
            wglCreateContextAttribsARB = (void*)wglGetProcAddress("wglCreateContextAttribsARB");
        }
        else if (StringsAreEqual("WGL_EXT_swap_control", start, length))
        {
            // https://www.khronos.org/registry/OpenGL/extensions/EXT/WGL_EXT_swap_control.txt
            wglSwapIntervalEXT = (void*)wglGetProcAddress("wglSwapIntervalEXT");
        }
        
        if (*ext == 0)
        {
            break;
        }
        
        ext++;
        start = ext;
    }
    
    if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB || !wglSwapIntervalEXT)
    {
        FatalError("OpenGL does not support required WGL extensions for modern context!");
    }
    
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(rc);
    ReleaseDC(dummy, dc);
    DestroyWindow(dummy);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previnstance, LPSTR cmdline, int cmdshow)
{
    // get WGL functions to be able to create modern GL context
    GetWglFunctions();
    
    // register window class to have custom WindowProc callback
    WNDCLASSEXW wc =
    {
        .cbSize = sizeof(wc),
        .lpfnWndProc = WindowProc,
        .hInstance = instance,
        .hIcon = LoadIcon(NULL, IDI_APPLICATION),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = L"opengl_window_class",
    };
    ATOM atom = RegisterClassExW(&wc);
    Assert(atom && "Failed to register window class");
    
    // window properties - width, height and style
    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    DWORD exstyle = WS_EX_APPWINDOW;
    DWORD style = WS_OVERLAPPEDWINDOW;
    
    // uncomment in case you want fixed size window
    //style &= ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    //RECT rect = { 0, 0, 1280, 720 };
    //AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    //width = rect.right - rect.left;
    //height = rect.bottom - rect.top;
    
    // create window
    HWND window = CreateWindowExW(exstyle, wc.lpszClassName, L"OpenGL Window", style,
                                  CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                                  NULL, NULL, wc.hInstance, NULL);
    Assert(window && "Failed to create window");
    
    HDC dc = GetDC(window);
    Assert(dc && "Failed to window device context");
    
    // set pixel format for OpenGL context
    {
        int attrib[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB,     24,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,
            
            // uncomment for sRGB framebuffer, from WGL_ARB_framebuffer_sRGB extension
            // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_framebuffer_sRGB.txt
            //WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
            
            // uncomment for multisampeld framebuffer, from WGL_ARB_multisample extension
            // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_multisample.txt
            //WGL_SAMPLE_BUFFERS_ARB, 1,
            //WGL_SAMPLES_ARB,        4, // 4x MSAA
            
            0,
        };
        
        int format;
        UINT formats;
        if (!wglChoosePixelFormatARB(dc, attrib, NULL, 1, &format, &formats) || formats == 0)
        {
            FatalError("OpenGL does not support required pixel format!");
        }
        
        PIXELFORMATDESCRIPTOR desc = { .nSize = sizeof(desc) };
        int ok = DescribePixelFormat(dc, format, sizeof(desc), &desc);
        Assert(ok && "Failed to describe OpenGL pixel format");
        
        if (!SetPixelFormat(dc, format, &desc))
        {
            FatalError("Cannot set OpenGL selected pixel format!");
        }
    }
    
    // create modern OpenGL context
    {
        int attrib[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifndef NDEBUG
            // ask for debug context for non "Release" builds
            // this is so we can enable debug callback
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
            0,
        };
        
        HGLRC rc = wglCreateContextAttribsARB(dc, NULL, attrib);
        if (!rc)
        {
            FatalError("Cannot create modern OpenGL context! OpenGL version 4.5 not supported?");
        }
        
        BOOL ok = wglMakeCurrent(dc, rc);
        Assert(ok && "Failed to make current OpenGL context");
        
        // load OpenGL functions
#define X(type, name) name = (type)wglGetProcAddress(#name); Assert(name);
        GL_FUNCTIONS(X)
#undef X
        
#ifndef NDEBUG
        // enable debug callback
        glDebugMessageCallback(&DebugCallback, NULL);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
    }
    
    // checkerboard texture, with 50% transparency on black colors
    GLuint texture;
    {
        int w,h,n;
        u8 *data = stbi_load("textures/dirt.png", &w, &h, &n, 0);
        
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTextureStorage2D(texture, 1, GL_RGBA8, w, h);
        glTextureSubImage2D(texture, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        stbi_image_free(data);
    }
    
    // fragment & vertex shaders for drawing triangle
    GLuint pipeline, vshader, fshader;
    {
        u8 *glslVShader = platform_file_read("w:\\Mineclone\\platform\\shaders\\glsl\\voxel.vs");
        u8 *glslFShader = platform_file_read("w:\\Mineclone\\platform\\shaders\\glsl\\voxel.fs");
        vshader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, (const GLchar **)&glslVShader);
        fshader = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, (const GLchar **)&glslFShader);
        
        GLint linked;
        glGetProgramiv(vshader, GL_LINK_STATUS, &linked);
        if (!linked) {
            char message[1024];
            glGetProgramInfoLog(vshader, sizeof(message), NULL, message);
            OutputDebugStringA(message);
            Assert(!"Failed to create vertex shader!");
        }
        
        glGetProgramiv(fshader, GL_LINK_STATUS, &linked);
        if (!linked) {
            char message[1024];
            glGetProgramInfoLog(fshader, sizeof(message), NULL, message);
            OutputDebugStringA(message);
            Assert(!"Failed to create fragment shader!");
        }
        
        glGenProgramPipelines(1, &pipeline);
        glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vshader);
        glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fshader);
    }
    
    // setup global GL state
    {
        // enable alpha blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glEnable(GL_DEPTH_TEST);
        
        // disable culling
        glDisable(GL_CULL_FACE);
    }
    
    // set to FALSE to disable vsync
    BOOL vsync = TRUE;
    wglSwapIntervalEXT(vsync ? 1 : 0);
    
    // show the window
    ShowWindow(window, SW_SHOWDEFAULT);
    
    LARGE_INTEGER freq, c1;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&c1);
    
    renderer.orbitCamera.distance = 5;
    renderer.orbitCamera.elevation = 1;
    
    game_construct();
    
    for (;;)
    {
        // process all incoming Windows messages
        MSG msg;
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            continue;
        }
        
        /* Handle shift keys */
        bool lshiftDown = (GetKeyState(VK_LSHIFT) & 0x8000) != 0;
        if (!keyboard.lshift.down && lshiftDown) keyboard.lshift.pressed = true;
        if (keyboard.lshift.down && !lshiftDown) keyboard.lshift.released = true;
        keyboard.lshift.down = lshiftDown;
        
        // get current window client area size
        RECT rect;
        GetClientRect(window, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
        
        LARGE_INTEGER c2;
        QueryPerformanceCounter(&c2);
        renderer.dt = (float)((double)(c2.QuadPart - c1.QuadPart) / freq.QuadPart);
        c1 = c2;
        
        game_update();
        
        // render only if window size is non-zero
        if (width != 0 && height != 0)
        {
            // setup output size covering all client area of window
            glViewport(0, 0, width, height);
            
            // clear screen
            glClearColor(0.392f, 0.584f, 0.929f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            // setup rotation matrix in uniform
            {
                f32 aspect = (float)width / height;
                mat4 proj = mat4_perspective((f32)deg2rad(45), aspect, 0.1f, 1000.0f);
                
                if (!renderer.draggingCamera && mouse.left.pressed) {
                    renderer.draggingCamera = true;
                    renderer.lastMouseP = mouse.p;
                }
                
                if (renderer.draggingCamera && !mouse.left.down)
                    renderer.draggingCamera = false;
                
                if (renderer.draggingCamera) {
                    v2 deltaP = v2_sub(mouse.p, renderer.lastMouseP);
                    renderer.lastMouseP = mouse.p;
                    
                    if (keyboard.lshift.down) {
                        renderer.orbitCamera.distance += 0.02f * deltaP.y;
                    }
                    else {
                        renderer.orbitCamera.azimuth -= 0.02f * deltaP.x;
                        renderer.orbitCamera.elevation -= 0.02f * deltaP.y;
                    }
                }
                
#if 0
                v3 eye = camera_orbit_position(renderer.orbitCamera.distance,
                                               renderer.orbitCamera.azimuth,
                                               renderer.orbitCamera.elevation);
                v3 worldUp = (v3){0,1,0};
                mat4 view = mat4_lookat(eye, 
                                        renderer.orbitCamera.target,
                                        worldUp);
#else 
                Camera *cam = &game.playerCamera;
                v3 eye = cam->p;
                v3 center = {
                    eye.x - sinf(deg2rad(cam->yaw)), 
                    eye.y + tanf(deg2rad(cam->pitch)), 
                    eye.z - cosf(deg2rad(cam->yaw))};
                v3 up = {0,1,0};
                mat4 view = mat4_lookat(eye, center, up); 
#endif
                
                glProgramUniformMatrix4fv(vshader, 0, 1, GL_FALSE, proj.d);
                glProgramUniformMatrix4fv(vshader, 1, 1, GL_FALSE, view.d);
            }
            
            // activate shaders for next draw call
            glBindProgramPipeline(pipeline);
            
            for (u32 chunkIndex=0; chunkIndex<narray(renderer.chunkHashTable); ++chunkIndex){
                RendererChunk *renderChunk = renderer.chunkHashTable[chunkIndex];
                if (renderChunk) {
                    /* bind buffers */
                    glBindVertexArray(renderChunk->vao);
                    glBindBuffer(GL_ARRAY_BUFFER, renderChunk->vbo);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderChunk->ebo);
                    /* bind texture */
                    GLint s_texture = 0; // texture unit that sampler2D will use in GLSL code
                    glBindTextureUnit(s_texture, texture);
                    /* draw */
                    glDrawElements(GL_TRIANGLES, renderChunk->indexCount, GL_UNSIGNED_INT, 0);
                }
            }
            
            // swap the buffers to show output
            if (!SwapBuffers(dc))
            {
                FatalError("Failed to swap OpenGL buffers!");
            }
            
            /* Clear input */
            mouse.left.pressed = false;
            mouse.right.pressed = false;
            for (u32 keyIndex=0; keyIndex<narray(keyboard.keys); ++keyIndex) {
                keyboard.keys[keyIndex].pressed = false;
            }
        }
        else
        {
            // window is minimized, cannot vsync - instead sleep a bit
            if (vsync)
            {
                Sleep(10);
            }
        }
    }
}

internal u8 *
platform_file_read(char *path) {
    u8 *result = 0;
    
    HANDLE fileHandle;
    DWORD fileSize;
    DWORD bytesRead;
    
    // Open the file for reading
    fileHandle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, 
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (fileHandle != INVALID_HANDLE_VALUE) {
        // Get the file size
        fileSize = GetFileSize(fileHandle, NULL);
        
        // Allocate a buffer to hold the entire file
        result = (u8 *)malloc(fileSize + 1);
        if (result != NULL) {
            // Read the entire file into the buffer
            if (ReadFile(fileHandle, result, fileSize, &bytesRead, NULL) ||
                bytesRead == fileSize) {
                // All good
                result[fileSize] = '\0';
            }
            else {
                printf("Error reading file\n");
                free(result);
                CloseHandle(fileHandle);
                return 0;
            }
        }
        else {
            printf("Error allocating memory\n");
            CloseHandle(fileHandle);
            return 0;
        }
    }
    else {
        printf("Error opening file\n");
        return 0;
    }
    
    return result;
}

internal void
platform_file_write(char *path, u8 *contents) {
}

internal void
renderer_chunk_htable_destruct(void) {
    for (s32 i=0; i<narray(renderer.chunkHashTable); ++i) {
        RendererChunk *at = renderer.chunkHashTable[i];
        while (at) {
            RendererChunk *next = at->next;
            free(at);
            at = next;
        }
    }
}

internal u32
renderer_chunk_hash(s32 x, s32 y, s32 z) {
    u32 hash = x*31 + y*479 + z*953;
    return hash & (narray(renderer.chunkHashTable)-1);
}

internal void
renderer_chunk_htable_insert(s32 x, s32 y, s32 z, ChunkMesh *mesh) {
    /* allocate new renderer chunk */
    RendererChunk *chunk = malloc(sizeof *chunk);
    memset(chunk, 0, sizeof *chunk);
    chunk->x = x;
    chunk->y = y;
    chunk->z = z;
    chunk->indexCount = mesh->indexIndex;
    /* vbo */
    glGenBuffers(1, &chunk->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertexIndex*sizeof(VoxelVertex), mesh->vertices, GL_STATIC_DRAW);
    /* ebo */
    glGenBuffers(1, &chunk->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexIndex*sizeof(u32), mesh->indices, GL_STATIC_DRAW);
    /* vao */
    glGenVertexArrays(1, &chunk->vao);
    glBindVertexArray(chunk->vao);
    /* bind vbo */
    glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
    /* set up input layout, position */
    GLint a_pos = 0;
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, position));
    /* normal */
    GLint a_nor = 1;
    glEnableVertexAttribArray(a_nor);
    glVertexAttribPointer(a_nor, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, normal));
    /* texcoord */
    GLint a_uv = 2;
    glEnableVertexAttribArray(a_uv);
    glVertexAttribPointer(a_uv, 2, GL_FLOAT, GL_FALSE, sizeof(VoxelVertex), (void*)offsetof(VoxelVertex, texCoord));
    /* unbind buffers */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    /* put at head of collision chain list */
    u32 bucket = renderer_chunk_hash(x,y,z);
    chunk->next = renderer.chunkHashTable[bucket];
    renderer.chunkHashTable[bucket] = chunk;
    free(mesh->vertices);
    free(mesh->indices);
    mesh->vertices = 0;
    mesh->indices = 0;
    mesh->indexIndex = 0;
    mesh->vertexIndex = 0;
}

internal bool
renderer_chunk_htable_exists(s32 x, s32 y, s32 z) {
    u32 bucket = renderer_chunk_hash(x,y,z);
    RendererChunk *at = renderer.chunkHashTable[bucket];
    while (at) {
        if (at->x == x &&
            at->y == y &&
            at->z == z)
            return true;
        at = at->next;
    }
    return false;
}

internal void renderer_chunk_htable_update();

internal void 
renderer_chunk_htable_remove(RendererChunk *chunk) {
    u32 bucket = renderer_chunk_hash(chunk->x, chunk->y, chunk->z);
    if (chunk == renderer.chunkHashTable[bucket]) {
        renderer.chunkHashTable[bucket] = chunk->next;
    }
    else {
        RendererChunk *prev = 0;
        RendererChunk *at = renderer.chunkHashTable[bucket];
        while (at != chunk) {
            prev = at;
            at = at->next;
        }
        prev->next = chunk->next;
    }
    
    glDeleteBuffers(1, &chunk->vbo);
    glDeleteBuffers(1, &chunk->ebo);
    glDeleteVertexArrays(1, &chunk->vao);
    
    free(chunk);
}

/* NOTE: Free all chunks outside a radius */
internal void
renderer_chunk_free_outside(v3 center, s32 radius) {
    for (u32 i=0; i<narray(renderer.chunkHashTable); ++i) {
        RendererChunk *chunk = renderer.chunkHashTable[i];
        if (chunk) {
            v3 chunkP = (v3){(f32)chunk->x,(f32)chunk->y,(f32)chunk->z,};
            if (v3_dist(center, chunkP) > radius) {
                renderer_chunk_htable_remove(chunk);
            }
        }
    }
}

internal void
platform_debug_print(char *message) {
    OutputDebugStringA(message);
}