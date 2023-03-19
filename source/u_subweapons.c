// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Subweapons upgrade.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

#define udata pl.upgrdata.subweapons

void Upgr_Subweapons_Update(void) {
   if(udata.shots < 2) {
      if(udata.charge >= 100) {
         AmbientSound(ss_weapons_subweapon_charge, 1.0);
         udata.shots++;
         udata.charge = 0;
      } else {
         udata.charge++;
      }
   }

   if(pl.buttons & BT_USER4 && !(pl.old.buttons & BT_USER4)) {
      for(i32 next = udata.which + 1;; next++) {
         if(next >= _subw_max) {
            next = 0;
         }

         if(get_bit(udata.have, next)) {
            udata.which = next;
            break;
         }
      }
   }
}

void Upgr_Subweapons_Render(void) {
   if(!pl.hudenabled) return;

   PrintSprite(sp_SubWepBack, pl.hudlpos+66,1, 239,2);

   i32 prc = 29 * udata.charge / (k32)100.0;
   i32 srw;
   if(udata.shots == 0) {srw = prc;} else {srw = 29;}
   /*                */ PrintSpriteClip(sp_SubWepBar1, pl.hudlpos+72,1, 224,2, 0,0,srw,1);
   if(udata.shots == 1) {srw = prc;} else {srw = 29;}
   if(udata.shots >  0) PrintSpriteClip(sp_SubWepBar2, pl.hudlpos+72,1, 224,2, 0,0,srw,1);

   for(i32 i = 0; i < _subw_max; i++) {
      i32 x   = pl.hudlpos + 68 + i * 9;
      i32 fid = fid_subwepS + i;

      if(get_bit(udata.have, i)) PrintSprite(sa_subwep_act[i], x,1, 238,2);
      if(udata.which == i)       SetFade(fid, 1, 6);
      if(CheckFade(fid))         PrintSprite(sp_SubWepUse, x,1, 238,2, _u_fade, fid);
   }
}

void Upgr_Subweapons_Enter(struct ugprade *upgr) {
   udata.shots = 2;
   #ifndef NDEBUG
   if(dbgflags(dbgf_items)) {
      udata.have = UINT32_MAX;
   } else {
   #endif
      set_bit(udata.have, _subw_gun);
   #ifndef NDEBUG
   }
   #endif
}

script_str ext("ACS") addr(OBJ "GetSubShots")
i32 Z_GetSubShots(void) {
   return udata.shots;
}

script_str ext("ACS") addr(OBJ "TakeSubShot")
void Z_TakeSubShot(void) {
   udata.shots--;
}

script_str ext("ACS") addr(OBJ "GetSubType")
i32 Z_GetSubType(void) {
   return udata.which;
}

/* EOF */
