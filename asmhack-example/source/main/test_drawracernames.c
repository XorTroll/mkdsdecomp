#include <demo.h>

u16 str1[100] = u"Hello";
u16 str2[100] = u"world";
u16 str3[100] = u"from";
u16 str4[100] = u"mkdsdecomp";

void CustomDrawNames() {
    G2d_BgCanvas bg_canvas;
    G2d_TextCanvas txt_canvas;

    // TODO: why does calling GetSomeBgCanvasTileData or Clear1A44SomeTileData abort? doing it manually works...
    void *tile_data = g_Global1A44->bg_canvas_tile_data;
    Mem_CpuFill8(tile_data, 0, 5120);

    G2d_BgCanvas_CreateFromBgTiles(&bg_canvas, tile_data, 32, 4, 4);

    // This setters are probably from an inlined function similar to the one above for bg canvas, these are inlined all over MKDS code
    txt_canvas.inter_char_width = 1;
    txt_canvas.draw_font_ptr = g_LoadedFonts[0];
    txt_canvas.bg_canvas_ptr = &bg_canvas;
    txt_canvas.line_spacing_height = 0;

    // Not really correct x/y values, but good enough to show it's working :P
    G2d_TextCanvas_PrintString(&txt_canvas, 0, 8, 8, 0, str1);
    G2d_TextCanvas_PrintString(&txt_canvas, 64, 8, 10, 0, str2);
    G2d_TextCanvas_PrintString(&txt_canvas, 64*2, 8, 12, 0, str3);
    G2d_TextCanvas_PrintString(&txt_canvas, 64*3, 8, 14, 0, str4);
    G2d_TextCanvas_PrintString(&txt_canvas, 64*4, 8 + 16, 8, 0, str1);
    G2d_TextCanvas_PrintString(&txt_canvas, 64*5, 8 + 16, 10, 0, str2);
    G2d_TextCanvas_PrintString(&txt_canvas, 64*6, 8 + 16, 12, 0, str3);
    G2d_TextCanvas_PrintString(&txt_canvas, 64*7, 8 + 16, 14, 0, str4);
}
ncp_set_call(0x020B6DA8, CustomDrawNames)
