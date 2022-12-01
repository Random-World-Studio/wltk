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

class frame
{
public:
private:
    str name;
    u64 width, height;
    i64 x, y;

    void __build_wl_egl_surface();

public:
    frame();
    frame(str name, u64 width, u64 height)
        : name(name), width(width), height(height), x(0), y(0) {}
};

#endif
