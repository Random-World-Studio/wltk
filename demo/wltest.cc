#include <iostream>

#include <wayland-client.h>
#include <wayland-server.h>

#include <unistd.h>

void on_global_added(void *data,
                     struct wl_registry *wl_registry,
                     uint32_t name,
                     const char *interface,
                     uint32_t version)
{
    (void)data;
    (void)wl_registry;
    std::cerr << " Global added: " << interface << ", v" << version
              << " (name " << name << ")"
              << std::endl;
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
    wl_display *display = wl_display_connect(0);

    //获取registry对象用于注册其他全局对象
    wl_registry *registry = wl_display_get_registry(display);

    //注册事件监听回调函数
    wl_registry_add_listener(registry, &registry_listener, nullptr);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    sleep(5);

    wl_display_disconnect(display);
}
