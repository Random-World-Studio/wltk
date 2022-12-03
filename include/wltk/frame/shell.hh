/**
 * @file shell.hh
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-12-03
 *
 * @copyright Copyright (c) 2022 Random World Studio
 *
 */

#ifndef __shell_hh__
#define __shell_hh__

#include "../types/base.hh"

#include <wayland-client.h>
#include <wayland-server.h>

namespace wltk
{
    class shell_controller
    {
    public:
    private:
        bool bind_succeed;

    public:
        shell_controller();

        void bind(
            wl_registry *registry,
            u32 name,
            u32 version);

        bool binded();

        void set_title(str name);
    };
};

#endif
