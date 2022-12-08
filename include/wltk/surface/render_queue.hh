/**
 * @file render_queue.hh
 * @author pointer-to-bios (pointer-to-bios@outlook.com)
 * @brief
 * @version 0.1
 * @date 2022-12-03
 *
 * @copyright Copyright (c) 2022 Random World Studio
 *
 */

#ifndef __render_queue_hh__
#define __render_queue_hh__

namespace wltk
{
    class render_queue
    {
    public:
        struct render_instruction
        {
        };

    private:
    public:
        void add(render_instruction inst);
        void get(render_instruction inst);
    };
};

#endif
