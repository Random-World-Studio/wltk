#include <iostream>

#include <wayland-client.h>
#include <wayland-server.h>

int main()
{
    wl_display *display = wl_display_connect(0);
    wl_registry *registry = wl_display_get_registry(display);
    wl_registry_listener *registry_listener;
    wl_registry_add_listener(registry, registry_listener, nullptr);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);
}
