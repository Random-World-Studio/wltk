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

u128 thrown;

void window::__wl_objs::register_handler(
    void *data,
    wl_registry *registry,
    u32 name,
    const char *interface,
    u32 version)
{
    //绑定混成器
    if (str(interface) == "wl_compositor")
    {
        window::__wl_objs::compositor =
            (wl_compositor *)wl_registry_bind(registry, name,
                                              &wl_compositor_interface,
                                              version);
        logger->info("Wayland compositor binded.");
    }
    //绑定wayland外壳
    else if (str(interface) == "wl_shell")
    {
        window::__wl_objs::shell =
            (wl_shell *)wl_registry_bind(
                registry, name,
                &wl_shell_interface,
                version);
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
    //连接display
    this->wl_objs.display = wl_display_connect(this->name.c_str());
    if (!this->wl_objs.display)
    {
        logger->error(
            "Cannot connect to wayland display{}.",
            this->name);
        return -1;
    }
    logger->info("Display connected.");
    //获取registry
    this->wl_objs.registry = wl_display_get_registry(window::__wl_objs::display);
    if (!this->wl_objs.registry)
    {
        logger->error(
            "Failed to get registry from display{}.",
            this->name);
        return -1;
    }
    //注册事件回调函数
    this->wl_objs.registry_listener.global = window::__wl_objs::register_handler;
    this->wl_objs.registry_listener.global_remove = window::__wl_objs::register_remover;
    wl_registry_add_listener(
        this->wl_objs.registry,
        &this->wl_objs.registry_listener,
        nullptr);

    wl_display_dispatch(this->wl_objs.display);
    wl_display_roundtrip(this->wl_objs.display);

    //创建surface
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
    //获取shell surface
    this->wl_objs.shsurface =
        wl_shell_get_shell_surface(
            this->wl_objs.shell, this->wl_objs.surface);
    if (!this->wl_objs.shsurface)
    {
        logger->error(
            "Cannot get shell surface.");
        return -1;
    }
    //创建串口region
    this->wl_objs.region = wl_compositor_create_region(this->wl_objs.compositor);
    wl_region_add(
        this->wl_objs.region,
        this->x, this->y, this->width, this->height);
    wl_surface_set_opaque_region(this->wl_objs.surface, this->wl_objs.region);

    //设置顶栏surface
    wl_shell_surface_set_toplevel(this->wl_objs.shsurface);
    //设置窗口标题
    wl_shell_surface_set_title(this->wl_objs.shsurface, this->name.c_str());

    return 0;
}

int window::init_egl()
{
    //创建EGL display
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
    //创建EGL上下文
    window::__egl_objs::context =
        eglCreateContext(
            window::__egl_objs::display,
            window::__egl_objs::config,
            EGL_NO_CONTEXT,
            window::egl_objs.contattributes);
    //创建egl窗口
    window::__egl_objs::window =
        wl_egl_window_create(
            window::__wl_objs::surface,
            width, height);
    //获取surface
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

void window::main_thread()
{ // TODO 渲染和事件，在组件和事件构建后才能完成
    while (wl_display_dispatch(window::__wl_objs::display) != -1)
        ;
}

void window::__init__()
{
    //设置日志路径
    logger = spdlog::rotating_logger_mt("wltk", log_path, 1048576 * 50, 64);
    if (initialized)
    {
        logger->error(
            "The frame class has been initialized,"
            "it cannot be initialized twice or more in one process.");
        init_success_flag = false;
        return;
    }
    //初始化wayland窗口
    this->__build_wl_egl_surface();
    //主循环线程
    window_thread = std::thread(main_thread);

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
