// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Drawing functions.                                                       │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

noinit struct globalcolors globalcolors;

alloc_aut(0) stkcall void TextSize(struct i32v2 *draw_text_size, str s, str font, i32 wrap) {
   draw_text_size->x = DrawCallI(sm_LA, wrap, s, font);
   draw_text_size->y = draw_text_size->x >> 16;
   draw_text_size->x = word(draw_text_size->x);
}

alloc_aut(0) stkcall
struct i32v2 const *TextureSize(str t) {
   static struct i32v2 draw_texture_size;
   draw_texture_size.x = DrawCallI(sm_LB, t);
   draw_texture_size.y = draw_texture_size.x >> 16;
   draw_texture_size.x = word(draw_texture_size.x);
   return &draw_texture_size;
}

alloc_aut(0) stkcall
void DrawInit(void) {
   #define GlobalCr(name) \
      globalcolors.name = ServCallI(sm_FindFontColor, so_##name);
   #include "m_drawing.h"
}

alloc_aut(0) stkcall
i32 Draw_GetCr(i32 n) {
   switch(n) {
   case 'a': return CR_BRICK;
   case 'b': return CR_TAN;
   case 'c': return CR_GREY;
   case 'd': return CR_GREEN;
   case 'e': return CR_BROWN;
   case 'f': return CR_GOLD;
   case 'g': return CR_RED;
   case 'h': return CR_BLUE;
   case 'i': return CR_ORANGE;
   case 'j': return CR_WHITE;
   case 'k': return CR_YELLOW;
   case 'l': return CR_UNTRANSLATED;
   case 'm': return CR_BLACK;
   case 'n': return CR_LIGHTBLUE;
   case 'o': return CR_CREAM;
   case 'p': return CR_OLIVE;
   case 'q': return CR_DARKGREEN;
   case 'r': return CR_DARKRED;
   case 's': return CR_DARKBROWN;
   case 't': return CR_PURPLE;
   case 'u': return CR_DARKGREY;
   case 'v': return CR_CYAN;
   case 'w': return CR_ICE;
   case 'x': return CR_FIRE;
   case 'y': return CR_SAPPHIRE;
   case 'z': return CR_TEAL;
   case _gcr_class: return pl.color;
#define GlobalCr(name) case _gcr_##name: return Cr(name);
#define GlobalCrH(name)
#include "m_drawing.h"
   }
   return CR_UNTRANSLATED;
}

alloc_aut(0) script ext("ACS") addr(lsc_drawcr)
i32 Z_DrawCr(i32 cr) {
   return Draw_GetCr(cr);
}

stkcall alloc_aut(0) void PrintSprite(str name, i32 x, i32 xa, i32 y, i32 ya, i32 flg, k32 anum, i32 c) {
   DrawCallV(sm_LS, name, XArg(x, xa), YArg(y, ya), flg, anum, c);
}

stkcall alloc_aut(0) void PrintSpriteClip(str name, i32 x, i32 xa, i32 y, i32 ya, i32 cx, i32 cy, i32 cw, i32 ch, i32 flg, k32 anum, i32 c) {
   DrawCallV(sm_LS, name, XArg(x, xa), YArg(y, ya), flg|_u_clip, anum, c, (cx << 16) | cy, (cw << 16) | ch);
}

stkcall alloc_aut(0) void PrintText_str(str s, str font, i32 cr, i32 x, i32 xa, i32 y, i32 ya, i32 flg, k32 anum, i32 ww) {
   DrawCallV(sm_LT, s, font, XArg(x, xa), YArg(y, ya), cr, flg, anum, ww);
}

/* EOF */
