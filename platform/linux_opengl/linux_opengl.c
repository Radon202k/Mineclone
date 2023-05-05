/* thanks to https://gist.github.com/mmozeiko/911347b5e3d998621295794e0ba334c4 */

#include "linux_opengl.h"

global char *globalHomeDir;

static void FatalError(const char* message)
{
    fprintf(stderr, "%s\n", message);
    
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "zenity --error --no-wrap --text=\"%s\"", message);
    system(cmd);
    
    exit(0);
}

#ifndef NDEBUG
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                   GLsizei length, const GLchar* message, const void* user)
{
    fprintf(stderr, "%s\n", message);
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        assert(!"OpenGL API usage error! Use debugger to examine call stack!");
    }
}
#endif

int main()
{
    /* get home dir to build absolute paths */
    globalHomeDir = getenv("HOME");
    printf("Home dir is: %s\n", globalHomeDir);
    if (globalHomeDir == NULL) {
        fprintf(stderr, "Error: Unable to retrieve the home directory path.\n");
        return 1;
    }

    Display* dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        FatalError("Cannot open X display");
    }
    
    XSetWindowAttributes attributes =
    {
        .event_mask = PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask,
    };
    
    // create window
    renderer.width = 1280;
    renderer.height = 720;
    Window window = XCreateWindow(dpy, DefaultRootWindow(dpy),
                                  0, 0, renderer.width, renderer.height, 
                                  0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask,
                                  &attributes);
    Assert(window && "Failed to create window");
    
    // uncomment in case you want fixed size window
    //XSizeHints* hints = XAllocSizeHints();
    //Assert(hints);
    //hints->flags |= PMinSize | PMaxSize;
    //hints->min_width  = hints->max_width  = width;
    //hints->min_height = hints->max_height = height;
    //XSetWMNormalHints(dpy, window, hints);
    //XFree(hints);
    
    // set window title
    XStoreName(dpy, window, "OpenGL Window");
    
    // subscribe to window close notification
    Atom WM_PROTOCOLS = XInternAtom(dpy, "WM_PROTOCOLS", False);
    Atom WM_DELETE_WINDOW = XInternAtom(dpy , "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, window, &WM_DELETE_WINDOW, 1);
    
    // initialize EGL
    EGLDisplay* display;
    {
        display = eglGetDisplay((EGLNativeDisplayType)dpy);
        Assert(display != EGL_NO_DISPLAY && "Failed to get EGL display");
        
        EGLint major, minor;
        if (!eglInitialize(display, &major, &minor))
        {
            FatalError("Cannot initialize EGL display");
        }
        if (major < 1 || (major == 1 && minor < 5))
        {
            FatalError("EGL version 1.5 or higher required");
        }
    }
    
    // choose OpenGL API for EGL, by default it uses OpenGL ES
    EGLBoolean ok = eglBindAPI(EGL_OPENGL_API);
    Assert(ok && "Failed to select OpenGL API for EGL");
    
    // choose EGL configuration
    EGLConfig config;
    {
        EGLint attr[] =
        {
            EGL_SURFACE_TYPE,      EGL_WINDOW_BIT,
            EGL_CONFORMANT,        EGL_OPENGL_BIT,
            EGL_RENDERABLE_TYPE,   EGL_OPENGL_BIT,
            EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
            
            EGL_RED_SIZE,      8,
            EGL_GREEN_SIZE,    8,
            EGL_BLUE_SIZE,     8,
            EGL_DEPTH_SIZE,   24,
            EGL_STENCIL_SIZE,  8,
            
            // uncomment for multisampled framebuffer
            //EGL_SAMPLE_BUFFERS, 1,
            //EGL_SAMPLES,        4, // 4x MSAA
            
            EGL_NONE,
        };
        
        EGLint count;
        if (!eglChooseConfig(display, attr, &config, 1, &count) || count != 1)
        {
            FatalError("Cannot choose EGL config");
        }
    }
    
    // create EGL surface
    EGLSurface* surface;
    {
        EGLint attr[] =
        {
            EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR, // or use EGL_GL_COLORSPACE_SRGB for sRGB framebuffer
            EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
            EGL_NONE,
        };
        
        surface = eglCreateWindowSurface(display, config, window, attr);
        if (surface == EGL_NO_SURFACE)
        {
            FatalError("Cannot create EGL surface");
        }
    }
    
    // create EGL context
    EGLContext* context;
    {
        EGLint attr[] =
        {
            EGL_CONTEXT_MAJOR_VERSION, 4,
            EGL_CONTEXT_MINOR_VERSION, 5,
            EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifndef NDEBUG
            // ask for debug context for non "Release" builds
            // this is so we can enable debug callback
            EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
            EGL_NONE,
        };
        
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, attr);
        if (context == EGL_NO_CONTEXT)
        {
            FatalError("Cannot create EGL context, OpenGL 4.5 not supported?");
        }
    }
    
    ok = eglMakeCurrent(display, surface, surface, context);
    Assert(ok && "Failed to make context current");

    // load OpenGL functions
#define X(type, name) name = (type)eglGetProcAddress(#name); Assert(name);
    GL_FUNCTIONS(X)
#undef X
    
#ifndef NDEBUG
    // enable debug callback
    glDebugMessageCallback(&DebugCallback, NULL);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
    

    /* load textures */
    renderer.texture = opengl_load_textures();
    
    /* load shaders */
    char *voxelsVShader = 
        platform_build_absolute_path("/Desktop/Mineclone/platform/shaders/glsl/voxel.vs");

    char *voxelsFShader = 
        platform_build_absolute_path("/Desktop/Mineclone/platform/shaders/glsl/voxel.fs");

    opengl_shader_from_files(voxelsVShader, voxelsFShader);
    
    // setup global GL state
    opengl_setup_global_state();
    
    // use 0 to disable vsync
    int vsync = 1;
    ok = eglSwapInterval(display, vsync);
    Assert(ok && "Failed to set vsync for EGL");
    
    // show the window
    XMapWindow(dpy, window);
    
    struct timespec c1;
    clock_gettime(CLOCK_MONOTONIC, &c1);
    
    game_construct();
    
    renderer.running = true;
    while (renderer.running)
    {
        // process all incoming X11 events
        while (XPending(dpy))
        {
            XEvent event;
            XNextEvent(dpy, &event);
            switch (event.type)
            {
                case MotionNotify:
                    mouse.p.x = (f32)event.xmotion.x;
                    mouse.p.y = (f32)event.xmotion.y;
                    break;

                case ButtonPress:
                    if (event.xbutton.button == Button1) {
                        mouse.left.down = true;
                        mouse.left.pressed = true;
                    }
                    if (event.xbutton.button == Button3) {
                        mouse.right.down = true;
                        mouse.right.pressed = true;
                    }
                    break;

                case ButtonRelease:
                    if (event.xbutton.button == Button1) {
                        mouse.left.down = false;
                        mouse.left.released = true;
                    }
                    if (event.xbutton.button == Button3) {
                        mouse.right.down = false;
                        mouse.right.released = true;
                    }
                    break;

                case KeyPress:
                case KeyRelease:
                    {
                        KeySym keysym = XLookupKeysym(&event.xkey, 0);
                        Button *key = 0;
                        
                        if      (keysym == XK_w)        key = &keyboard.w;
                        else if (keysym == XK_a)        key = &keyboard.a;
                        else if (keysym == XK_s)        key = &keyboard.s;
                        else if (keysym == XK_d)        key = &keyboard.d;
                        else if (keysym == XK_space)    key = &keyboard.space;
                        else if (keysym == XK_Control_L || keysym == XK_Control_R) key = &keyboard.control;
                        else if (keysym == XK_Alt_L || keysym == XK_Alt_R) key = &keyboard.alt;
                        else if (keysym == XK_Shift_L) key = &keyboard.lshift;
                        else if (keysym == XK_Shift_R) key = &keyboard.rshift;
                        
                        if (key) {
                            key->down = (event.type == KeyPress);
                            key->pressed = (event.type == KeyPress);
                            key->released = (event.type == KeyRelease);
                        }
                    }
                    break;

                case ClientMessage:
                    if (event.xclient.message_type == WM_PROTOCOLS)
                    {
                        Atom protocol = event.xclient.data.l[0];
                        if (protocol == WM_DELETE_WINDOW)
                        {
                            renderer.running = false;
                        }
                    }
                    break;
            }
        }
        
        // get current window size
        XWindowAttributes attr;
        Status status = XGetWindowAttributes(dpy, window, &attr);
        Assert(status && "Failed to get window attributes");
        
        renderer.width = attr.width;
        renderer.height = attr.height;
        
        struct timespec c2;
        clock_gettime(CLOCK_MONOTONIC, &c2);
        renderer.dt = (float)(c2.tv_sec - c1.tv_sec) + 1e-9f * (c2.tv_nsec - c1.tv_nsec);
        c1 = c2;
        
        game_update();
        
        // render only if window size is non-zero
        if (renderer.width != 0 && renderer.height != 0)
        {
            opengl_prepare_frame();
            
            opengl_render_chunks();
            
            // swap the buffers to show output
            if (!eglSwapBuffers(display, surface))
            {
                FatalError("Failed to swap OpenGL buffers!");
            }
        }
        else
        {
            // window is minimized, cannot vsync - instead sleep a bit
            if (vsync)
            {
                usleep(10 * 1000);
            }
        }
    }
}

u8 *
platform_file_read(char *path) {
    u8 *result = 0;
    long fileSize;
    size_t bytesRead;
    
    // Open the file for reading
    FILE *file = fopen(path, "rb");
    
    if (file != NULL) {
        // Get the file size
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        // Allocate a buffer to hold the entire file
        result = (u8 *)malloc(fileSize + 1);
        if (result != NULL) {
            // Read the entire file into the buffer
            bytesRead = fread(result, 1, fileSize, file);
            if (bytesRead == (size_t)fileSize) {
                // All good
                result[fileSize] = '\0';
            }
            else {
                printf("Error reading file\n");
                free(result);
                fclose(file);
                return 0;
            }
        }
        else {
            printf("Error allocating memory\n");
            fclose(file);
            return 0;
        }
    }
    else {
        printf("Error opening file\n");
        return 0;
    }
    
    fclose(file);
    return result;
}

internal void
platform_file_write(char *path, u8 *contents) {
}

void
platform_debug_print(char *message) {
    fprintf(stderr, "%s", message);
}

internal char *
platform_build_absolute_path(char *relativePath) {
    size_t fullPathSize = strlen(globalHomeDir) + strlen(relativePath) + 1;
    char *fullPath = (char *)malloc(fullPathSize);

    if (fullPath == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }

    strcpy(fullPath, globalHomeDir);
    strcat(fullPath, relativePath);

    return fullPath;
}