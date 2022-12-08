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

#include "../types/base.hh"

#include <list>

namespace wltk
{
    class render_queue
    {
    public:
        struct render_instruction
        {
            /**
             * operation中
             * dot指令和非抗锯齿指令均为软件渲染；
             * 抗锯齿指令为硬件渲染。
             */
            enum operation
            {
                dot,    // 点 d1=(x,y)
                line,   // 直线 d1=(x1,y1);d2=(x2,y2)
                aaline, // 抗锯齿直线 d1=(x1,y1);d2=(x2,y2)
                rect,   // 矩形 d1=(x,y);d2=(width,height)
                tran,   // 三角形 d1=(x1,y1);d2=(x2,y2);d3=(x3,y3)
                aatran, // 抗锯齿三角形 d1=(x1,y1);d2=(x2,y2);d3=(x3,y3)
                circ,   // 圆 d1=(x,y);mod=r
                aacirc, // 抗锯齿圆 d1=(x,y);mod=r
                elli,   // 椭圆 d1=(x,y);d2=(hw,hh)
                aaelli, // 抗锯齿椭圆 d1=(x,y);d2=(hw,hh)
                arc,    // 弧 d1=(x,y);d2=(hw,hh);a1=rang1;a2=rang2
                aaarc,  // 抗锯齿的弧 d1=(x,y);d2=(hw,hh);a1=rang1;a2=rang2
            };
            operation oper;
            struct vector
            {
                i64 x, y;
            };
            vector d1, d2, d3;
            f64 a1, a2;
            u64 mod;
        };

        typedef std::list<render_instruction> instructions_list;

    private:
        instructions_list inst_list;
        u64 width;

    public:
        void add(render_instruction inst);
        render_instruction get();
    };
};

#endif
