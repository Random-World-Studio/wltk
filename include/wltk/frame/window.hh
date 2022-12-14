/**
 * @file window.hh
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-12-01
 *
 * @copyright Copyright (c) 2022 Random World Studio
 *
 */

#ifndef __window_hh__
#define __window_hh__

#include "../types/base.hh"
#include "shell.hh"
#include "../surface/surface.hh"

#include <thread>

#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-egl.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace wltk
{

    const str log_path = "/proc/self/stdout";

    /* 普通窗口 */
    //此类只能构造一次
    class window
    {
    public:
    private:
        str name;
        u64 width, height;
        i64 x, y;

        static u64 fps;

        //窗口根surface对象，所有绘图操作最终都经过此对象
        static surface *root_surface;

        //捕获事件，渲染窗口的主线程
        std::thread window_thread;
        static void main_thread();

        //表示窗口是否成功初始化
        bool init_success_flag;

        //表示类是否已经构造过
        static bool initialized;

        struct __wl_objs // wayland对象和回调函数
        {
            static wl_display *display;
            static wl_registry *registry;
            static wl_surface *surface;

            static wl_registry_listener registry_listener;
            static void register_handler(
                void *data, wl_registry *wl_registry,
                u32 name, const char *interface, u32 version);
            static void register_remover(
                void *data, wl_registry *wl_registry, u32 name);

            static wl_compositor *compositor;
            static wl_region *region;

            static shell_controller shell;
        } wl_objs;

        static struct __egl_objs
        {
            static EGLint major, minor;
            static EGLint config_count;
            EGLint confattributes[11] = {
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_NONE};
            EGLint contattributes[3] = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE};

            static EGLDisplay display;
            static EGLConfig config;
            static EGLContext context;
            static EGLSurface surface;
            static wl_egl_window *window;
        } egl_objs;

        int init_wayland();
        int init_egl();
        void __build_wl_egl_surface();

        void __init__();

    public:
        window();
        window(u64 width, u64 height, str name);
        window(i64 x, i64 y, u64 width, u64 height, str name);

        ~window();

        bool succeed(); //对象构造后，可调用此函数判断是否初始化成功
    };

};

#endif
