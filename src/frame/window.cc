/**
 * @file frame.cc
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-11-30
 *
 * @copyright Copyright (c) 2022 Random World Studio
 *
 */

#include "../../include/wltk/frame/window.hh"
#include "../../include/wltk/log/log.hh"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <ctime>
#include <regex>

#include <unistd.h>

u128 thrown;

namespace wltk
{

    void window::__wl_objs::register_handler(
        void *data,
        wl_registry *registry,
        u32 name,
        const char *interface,
        u32 version)
    {
        // 绑定混成器
        if (str(interface) == "wl_compositor")
        {
            window::__wl_objs::compositor =
                (wl_compositor *)wl_registry_bind(registry, name,
                                                  &wl_compositor_interface,
                                                  version);
            logger->info("Wayland compositor binded.");
        }
        // 绑定wayland外壳
        else if (
            std::regex_match(str(interface),
                             std::regex(".+_shell")))
        {
            window::__wl_objs::shell.bind(registry, name, version);
            logger->info("Wayland shell binded.");
        }
    }

    void window::__wl_objs::register_remover(
        void *data,
        wl_registry *registry,
        u32 name)
    {
        logger->info("Object {} removed.", name);
    }

    int window::init_wayland()
    {
        // 连接display
        this->wl_objs.display = wl_display_connect(this->name.c_str());
        if (!this->wl_objs.display)
        {
            logger->error(
                "Cannot connect to wayland display{}.",
                this->name);
            return -1;
        }
        logger->info("Display connected.");
        // 获取registry
        this->wl_objs.registry = wl_display_get_registry(window::__wl_objs::display);
        if (!this->wl_objs.registry)
        {
            logger->error(
                "Failed to get registry from display{}.",
                this->name);
            return -1;
        }
        // 注册事件回调函数
        this->wl_objs.registry_listener.global = window::__wl_objs::register_handler;
        this->wl_objs.registry_listener.global_remove = window::__wl_objs::register_remover;
        wl_registry_add_listener(
            this->wl_objs.registry,
            &this->wl_objs.registry_listener,
            nullptr);

        wl_display_dispatch(this->wl_objs.display);
        wl_display_roundtrip(this->wl_objs.display);

        // 创建surface
        if (!this->wl_objs.compositor)
        {
            logger->error(
                "Cannot find wayland compositor.");
            return -1;
        }
        this->wl_objs.surface = wl_compositor_create_surface(this->wl_objs.compositor);
        if (!this->wl_objs.surface)
        {
            logger->error(
                "Failed to create surface.");
            return -1;
        }
        // 获取shell surface
        //  TODO
        // 创建串口region
        this->wl_objs.region = wl_compositor_create_region(this->wl_objs.compositor);
        wl_region_add(
            this->wl_objs.region,
            this->x, this->y, this->width, this->height);
        wl_surface_set_opaque_region(this->wl_objs.surface, this->wl_objs.region);

        this->wl_objs.shell.set_title(this->name);

        return 0;
    }

    int window::init_egl()
    {
        // 创建EGL display
        window::__egl_objs::display =
            eglGetDisplay((EGLNativeDisplayType)this->wl_objs.display);
        eglInitialize(
            window::__egl_objs::display,
            &window::__egl_objs::major, &window::__egl_objs::minor);
        eglChooseConfig(
            window::__egl_objs::display,
            (EGLint *)window::egl_objs.confattributes,
            &window::__egl_objs::config, 1,
            (EGLint *)&thrown);
        // 创建EGL上下文
        window::__egl_objs::context =
            eglCreateContext(
                window::__egl_objs::display,
                window::__egl_objs::config,
                EGL_NO_CONTEXT,
                window::egl_objs.contattributes);
        // 创建egl窗口
        window::__egl_objs::window =
            wl_egl_window_create(
                window::__wl_objs::surface,
                width, height);
        // 获取surface
        window::__egl_objs::surface =
            eglCreateWindowSurface(
                window::__egl_objs::display,
                window::__egl_objs::config,
                window::__egl_objs::window,
                0);

        eglMakeCurrent(
            window::__egl_objs::display,
            window::__egl_objs::surface,
            window::__egl_objs::surface,
            window::__egl_objs::context);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        eglSwapBuffers(
            window::__egl_objs::display,
            window::__egl_objs::surface);

        return 0;
    }

    void window::__build_wl_egl_surface()
    {
        /* 初始化wayland */
        if (!init_wayland())
        {
            logger->error(
                "Failed to initialize wayland.");
            init_success_flag = false;
            return;
        }
        /* 初始化EGL */
        if (!init_egl())
        {
            logger->error(
                "Failed to initialize egl.");
            init_success_flag = false;
            return;
        }
        init_success_flag = true;
    }

    static void update_surfaces(surface *surf)
    {
        u64 t = time(NULL);
        if (!surf->get_updater().next_frame)
        {
            surf->render();
            surf->get_updater().update_time = t;
            surf->get_updater().next_frame = 1000 / surf->get_updater().fps;
        }
        else
        {
            surf->get_updater().next_frame -=
                t - surf->get_updater().update_time;
            surf->get_updater().update_time = t;
        }
    }

    void window::main_thread()
    { // TODO 渲染和事件，在组件和事件构建后才能完成
        u64 time_flag;
        while (wl_display_dispatch(window::__wl_objs::display) != -1)
        {
            time_flag = time(NULL);
            // 递归遍历surface，刷新界面
            update_surfaces(root_surface);
            // 保持帧率
            i64 slp_time = 1000 / window::fps - time(NULL) + time_flag;
            if (slp_time < 0)
            {
                slp_time = -slp_time;
            }
            // 休眠至下一帧到来
            while (slp_time)
            {
                slp_time = usleep(slp_time);
            }
        }
    }

    void window::__init__()
    {
        // 设置日志路径
        logger = spdlog::rotating_logger_mt("wltk", log_path, 1048576 * 50, 64);
        if (initialized)
        {
            logger->error(
                "The frame class has been initialized,"
                "it cannot be initialized twice or more in one process.");
            init_success_flag = false;
            return;
        }
        // 初始化wayland窗口
        this->__build_wl_egl_surface();
        // 主循环线程
        window_thread = std::thread(main_thread);

        fps = 60;

        initialized = true;
    }

    window::window()
        : name("Wayland Toolkits"), x(100), y(100), width(500), height(500)
    {
        __init__();
    }

    window::window(u64 width, u64 height, str name)
        : name(name), width(width), height(height), x(100), y(100)
    {
        __init__();
    }

    window::window(i64 x, i64 y, u64 width, u64 height, str name)
        : name(name), width(width), height(height), x(x), y(y)
    {
        __init__();
    }

    window::~window()
    {
        window_thread.join();
        wl_display_disconnect(this->wl_objs.display);
        logger->info(
            "Disconnected wayland.");
    }

    bool window::succeed()
    {
        return init_success_flag;
    }

};
