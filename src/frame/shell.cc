/**
 * @file shell.cc
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022 Random World Studio
 * 
 */

#include "../../include/wltk/frame/shell.hh"

namespace wltk
{

    shell_controller::shell_controller()
    {
        bind_succeed = false;
    }

    void shell_controller::bind(
        wl_registry *registry,
        u32 name,
        u32 version)
    {
        //TODO 绑定后获取surface，设置toplevel
        bind_succeed = true;
    }

    bool shell_controller::binded()
    {
        return bind_succeed;
    }

    void set_title(str name)
    {
        //TODO
    }
};
