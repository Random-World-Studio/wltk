#include <iostream>

#include <wayland-client.h>
#include <wayland-server.h>

#include <unistd.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

void on_global_added(void *data,
                     struct wl_registry *wl_registry,
                     uint32_t name,
                     const char *interface,
                     uint32_t version)
{
    (void)data;
    (void)wl_registry;
}

void on_global_removed(void *data,
                       struct wl_registry *wl_registry,
                       uint32_t name)
{
    (void)data;
    (void)wl_registry;
    std::cerr << " Global removed: name: " << name
              << std::endl;
}

wl_registry_listener registry_listener = {
    .global = on_global_added,
    .global_remove = on_global_removed
};

int main()
{
    auto logger = spdlog::rotating_logger_mt("test", "test.log", 1048576 * 50, 64);
    logger->info("started.");
    wl_display *display = wl_display_connect(0);

    //获取registry对象用于注册其他全局对象
    wl_registry *registry = wl_display_get_registry(display);

    //绑定注册事件回调函数
    wl_registry_add_listener(registry, &registry_listener, nullptr);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    sleep(5);

    wl_display_disconnect(display);
}
