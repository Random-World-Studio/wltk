/**
 * @file surface.hh
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-12-03
 *
 * @copyright Copyright (c) 2022 Random World Studio
 *
 */

#ifndef __surface_hh__
#define __surface_hh__

#include "render_queue.hh"

#include "../components/component.hh"
#include "../graphics/base/graphics.hh"

#include "../types/base.hh"

#include <list>
#include <mutex>

namespace wltk
{
    extern bool surface_global_initialized;

    class surface
    {
    public:
        typedef std::list<surface *> surface_list;
        typedef u64 surface_id;

    private:
        u64 width, height;
        i64 x, y;

        // surface内容更新控制
        struct updater
        {
            /**
             * 帧率
             * 若为0，则由surface主动刷新
             * 若为非0，则window或popup类的渲染器会按照帧率刷新
             */
            u64 fps;
            /**
             * 帧率控制变量
             * 应为当前时间到下次刷新时间的时间间隔，值由window类
             * 渲染器更新。
             * 一旦为0，window类渲染器刷新一次。
             */
            u64 next_frame;
            /**
             * 更新帧率控制变量的时间
             * 每次window类渲染器遍历都应更新此变量。
             */
            u64 update_time;
        } updating;

        /**
         * 上级surface
         * 根surface的此值为nullptr
         */
        surface *upper_surface;
        /**
         * 子级surface
         * 按先后顺序渲染，排在前面的先渲染，后面的后渲染
         */
        surface_list sub_surfaces;

        component *compo;
        graphics *grap;

        /**
         * surface
         * 使用渲染队列来渲染画面，最终window对象通过递归遍历
         * 每个surface的渲染队列来合成最终画面，渲染指令出队
         * 后不可入队。
         *
         * buffered_queue
         * 用于缓存上一帧画面的渲染队列，此队列除开始渲染下一帧
         * 时以外，渲染指令出队后必须入队。
         * 此队列可用于调试渲染的画面。
         */
        render_queue queue;
        render_queue buffered_queue;

    public:
        surface();
        surface(u64 width, u64 height);

        void add(surface *surf);

        void bind(component *compo);
        void attach(graphics *grap);

        component *component();
        graphics *graphics();

        updater &get_updater();

        void render();
    };
};

#endif
