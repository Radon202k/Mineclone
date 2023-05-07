// thanks to https://gist.github.com/mmozeiko/ed2ad27f75edf9c26053ce332a1f6647

#include "windows_opengl.h"

static void FatalError(const char* message)
{
    MessageBoxA(NULL, message, "Error", MB_ICONEXCLAMATION);
    ExitProcess(0);
}

#ifndef NDEBUG
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
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
        case WM_INPUT:
        {
            UINT dwSize = sizeof(RAWINPUT);
            static RAWINPUT raw;
            
            GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &raw, &dwSize, sizeof(RAWINPUTHEADER));
            
            if (raw.header.dwType == RIM_TYPEMOUSE)
            {
                renderer.mouseDelta.x = (f32)raw.data.mouse.lLastX;
                renderer.mouseDelta.y = (f32)raw.data.mouse.lLastY;
            }
        }
        break;
        
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
    GetModuleFileName(NULL, exePath, MAX_PATH);
    // Find the last backslash and truncate the string
    exePathLastSlash = strrchr(exePath, '\\');
    
    if (exePathLastSlash) {
        *exePathLastSlash = '\0';
    }
    
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
    renderer.width = (u32)CW_USEDEFAULT;
    renderer.height = (u32)CW_USEDEFAULT;
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
                                  CW_USEDEFAULT, CW_USEDEFAULT, renderer.width, renderer.height,
                                  NULL, NULL, wc.hInstance, NULL);
    Assert(window && "Failed to create window");
    
    HDC dc = GetDC(window);
    Assert(dc && "Failed to window device context");
    
    /* hide mouse */
    ShowCursor(FALSE);
    
    /* register for mouse raw input */
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = 0;
    rid.hwndTarget = window;
    RegisterRawInputDevices(&rid, 1, sizeof(rid));
    
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
    
    /* load lines shaders */
    renderer.linesShader = opengl_shader_from_files("w:\\Mineclone\\platform\\shaders\\glsl\\line.vs",
                                                    "w:\\Mineclone\\platform\\shaders\\glsl\\line.fs",
                                                    0);
    
    /* create lines vao an vbos */
    renderer_lines3D_construct();
    
    /* construct the game (the game defines which textures to load at construct time */
    game_construct();
    
    /* load textures */
    opengl_load_textures();
    
    /* load voxel shaders */
    renderer.voxelsShader = opengl_shader_from_files("w:\\Mineclone\\platform\\shaders\\glsl\\voxel.vs",
                                                     "w:\\Mineclone\\platform\\shaders\\glsl\\voxel.fs",
                                                     0); /* no geometry shader */
    
    renderer.sunDirectionLoc = glGetUniformLocation(renderer.voxelsShader.program, "sunDirection");
    renderer.sunColorLoc = glGetUniformLocation(renderer.voxelsShader.program, "sunColor");
    
    // setup global GL state
    opengl_setup_global_state();
    
    // set to FALSE to disable vsync
    BOOL vsync = TRUE;
    wglSwapIntervalEXT(vsync ? 1 : 0);
    
    // show the window
    ShowWindow(window, SW_SHOWDEFAULT);
    
    LARGE_INTEGER freq, c1;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&c1);
    
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
        renderer.width = rect.right - rect.left;
        renderer.height = rect.bottom - rect.top;
        
        LARGE_INTEGER c2;
        QueryPerformanceCounter(&c2);
        renderer.dt = (float)((double)(c2.QuadPart - c1.QuadPart) / freq.QuadPart);
        c1 = c2;
        
        game_update();
        
        // render only if window size is non-zero
        if (renderer.width != 0 && renderer.height != 0)
        {
            /* prepare frame for rendering */
            opengl_prepare_frame();
            
            /* render chunks */
            opengl_render_chunks();
            
            /* render lines */
            opengl_render_lines();
            
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
            
            renderer.mouseDelta.x = 0;
            renderer.mouseDelta.y = 0;
            
            POINT centerPoint = { renderer.width / 2, renderer.height / 2 };
            ClientToScreen(window, &centerPoint);
            SetCursorPos(centerPoint.x, centerPoint.y);
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
    
    renderer_lines3D_destruct();
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
platform_debug_print(char *message) {
    OutputDebugStringA(message);
}

internal char *
platform_build_absolute_path(char *relativePath) {
    size_t fullPathSize = strlen(exePath) + strlen(relativePath) + 2;
    char *fullPath = (char *)malloc(fullPathSize);
    
    strcpy_s(fullPath, fullPathSize, exePath);
    strcat_s(fullPath, fullPathSize, "\\");
    strcat_s(fullPath, fullPathSize, relativePath);
    
    for (size_t i = 0; i < strlen(fullPath); i++) {
        if (fullPath[i] == '/') {
            fullPath[i] = '\\';
        }
    }
    
    return fullPath;
}