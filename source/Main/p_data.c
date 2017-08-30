// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_version.h"
#include "lith_world.h"
#include "lith_monster.h"

// Static Functions ----------------------------------------------------------|

static void Lith_GetArmorType(player_t *p);

// Extern Functions ----------------------------------------------------------|

//
// Lith_PlayerCurWeaponType
//
int Lith_PlayerCurWeaponType(player_t *p)
{
   return p->weapon.cur->info->type;
}

//
// Lith_ButtonPressed
//
bool Lith_ButtonPressed(player_t *p, int bt)
{
   return p->buttons & bt && !(p->old.buttons & bt);
}

//
// Lith_SetPlayerVelocity
//
bool Lith_SetPlayerVelocity(player_t *p, fixed velx, fixed vely, fixed velz, bool add, bool setbob)
{
   if(add)
      p->velx += velx, p->vely += vely, p->velz += velz;
   else
      p->velx = velx, p->vely = vely, p->velz = velz;

   return ACS_SetActorVelocity(p->tid, velx, vely, velz, add, setbob);
}

//
// Lith_ValidatePlayerTID
//
void Lith_ValidatePlayerTID(player_t *p)
{
   if(ACS_ActivatorTID() == 0) {
      ACS_Thing_ChangeTID(0, p->tid = ACS_UniqueTID());
      LogDebug(log_dev, "set ptid from 0 to %i", p->tid);
   } else if(p->tid != ACS_ActivatorTID()) {
      LogDebug(log_dev, "set ptid from %i to %i", p->tid, ACS_ActivatorTID());
      p->tid = ACS_ActivatorTID();
   }
}

//
// Lith_PlayerUpdateData
//
// Update all of the player's data.
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateData(player_t *p)
{
   int const warpflags = WARPF_NOCHECKPOSITION | WARPF_MOVEPTR |
      WARPF_WARPINTERPOLATION | WARPF_COPYINTERPOLATION | WARPF_COPYPITCH;

   Lith_ScriptCall("Lith_Server", "SetInput", p->num, false);

   ACS_Warp(p->cameratid,  4, 0, ACS_GetActorViewHeight(0), 0, warpflags);
   ACS_Warp(p->weathertid, 4, 0, ACS_GetActorViewHeight(0), 0, warpflags);

   p->x      = ACS_GetActorX(0);
   p->y      = ACS_GetActorY(0);
   p->z      = ACS_GetActorZ(0);
   p->floorz = ACS_GetActorFloorZ(0);

   p->velx = ACS_GetActorVelX(0);
   p->vely = ACS_GetActorVelY(0);
   p->velz = ACS_GetActorVelZ(0);

   p->pitch = ACS_GetActorPitch(0) - p->addpitch;
   p->yaw   = ACS_GetActorAngle(0) - p->addyaw;

   p->pitchf = ((-p->pitch + 0.25) * 2) * pi;
   p->yawf   = p->yaw * tau - pi;

   p->pitchv = ACS_GetPlayerInputFixed(-1, INPUT_PITCH);
   p->yawv   = ACS_GetPlayerInputFixed(-1, INPUT_YAW);

   p->forwardv = ACS_GetPlayerInputFixed(-1, INPUT_FORWARDMOVE);
   p->sidev    = ACS_GetPlayerInputFixed(-1, INPUT_SIDEMOVE);
   p->upv      = ACS_GetPlayerInputFixed(-1, INPUT_UPMOVE);

   p->buttons = ACS_GetPlayerInput(-1, INPUT_BUTTONS);

   p->health = ACS_GetActorProperty(0, APROP_Health);
   p->armor  = ACS_CheckInventory("BasicArmor");

   p->name        = StrParam("%tS", p->num);
   p->weaponclass = ACS_GetWeapon();
   p->armorclass  = ACS_GetArmorInfoString(ARMORINFO_CLASSNAME);
   p->maxarmor    = ACS_GetArmorInfo(ARMORINFO_SAVEAMOUNT);

   Lith_GetArmorType(p);

   p->scopetoken = ACS_CheckInventory("Lith_WeaponScopedToken");

   p->keys.redcard     = ACS_CheckInventory("RedCard")    || ACS_CheckInventory("KeyGreen");
   p->keys.yellowcard  = ACS_CheckInventory("YellowCard") || ACS_CheckInventory("KeyYellow");
   p->keys.bluecard    = ACS_CheckInventory("BlueCard")   || ACS_CheckInventory("KeyBlue");
   p->keys.redskull    = ACS_CheckInventory("RedSkull");
   p->keys.yellowskull = ACS_CheckInventory("YellowSkull");
   p->keys.blueskull   = ACS_CheckInventory("BlueSkull");
}

//
// Lith_GiveMail
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_GiveMail(int num)
{
   __str names[] = {
      "Intro",
      "Cluster1",
      "Cluster2",
      "Cluster3",
      "Phantom",
      "JamesDefeated",
      "MakarovDefeated",
      "IsaacDefeated"
   };

   num %= countof(names);

   withplayer(LocalPlayer)
      p->deliverMail(names[num]);
}

//
// Lith_ClearTextBuf
//
void Lith_ClearTextBuf(player_t *p)
{
   memset(p->txtbuf, 0, sizeof(p->txtbuf));
   p->tbptr = 0;
}

//
// Lith_KeyDown
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_KeyDown(int pnum, int ch)
{
   player_t *p = &players[pnum];

   if(p->tbptr + 1 < countof(p->txtbuf))
      p->txtbuf[p->tbptr++] = ch;
}

//
// Lith_ResetPlayer
//
// Reset some things on the player when they spawn.
//
[[__call("ScriptS")]]
void Lith_ResetPlayer(player_t *p)
{
   //
   // Init
   if(!p->wasinit) {
      *p = (player_t){};
      p->wasinit = true;
   }

   //
   // Constant data

   p->active = true;
   p->reinit = p->dead = false;
   p->num    = ACS_PlayerNumber();
   p->bipPtr = &p->bip;

   __with(__str cl = ACS_GetActorClass(0);) {
           if(cl == "Lith_MarinePlayer"   ) p->pclass = pcl_marine;
      else if(cl == "Lith_CyberMagePlayer") p->pclass = pcl_cybermage;
      else if(cl == "Lith_InformantPlayer") p->pclass = pcl_informant;
      else if(cl == "Lith_WandererPlayer" ) p->pclass = pcl_wanderer;
      else if(cl == "Lith_AssassinPlayer" ) p->pclass = pcl_assassin;
      else if(cl == "Lith_DarkLordPlayer" ) p->pclass = pcl_darklord;
      else if(cl == "Lith_ThothPlayer"    ) p->pclass = pcl_thoth;
      else {
         Log("Invalid player class detected!");
         abort();
      }
   }

   //
   // Map-static data

   p->old = (player_delta_t){};

   // If the map sets the TID on the first tic, it could already be set here.
   p->tid = 0;
   Lith_ValidatePlayerTID(p);

   // This keeps spawning more camera actors when you die, but that should be
   // OK as long as you don't die 2 billion times.
   ACS_SpawnForced("Lith_CameraHax", 0, 0, 0, p->cameratid  = ACS_UniqueTID());
   ACS_SpawnForced("Lith_CameraHax", 0, 0, 0, p->weathertid = ACS_UniqueTID());

   if(world.dbgScore)
      p->score = 0xFFFFFFFFFFFFFFFFll;

   //
   // Reset data

   // i cri tears of pain for APROP_SpawnHealth
   if(!p->viewheight) p->viewheight = ACS_GetActorViewHeight(0);
   if(!p->jumpheight) p->jumpheight = ACS_GetActorPropertyFixed(0, APROP_JumpZ);
   if(!p->maxhealth)  p->maxhealth  = ACS_GetActorProperty(0, APROP_Health);
   if(!p->discount)   p->discount   = 1.0;

   // Any linked lists on the player need to be initialized here.
   p->loginfo.hud.free();
   p->hudstrlist.free(free);
   if(!p->loginfo.full.next) p->loginfo.full.construct();
   if(!p->loginfo.maps.next) p->loginfo.maps.construct();

   // pls not exit map with murder thingies out
   // is bad practice
   ACS_SetPlayerProperty(0, false, PROP_INSTANTWEAPONSWITCH);
   ACS_SetActorPropertyFixed(0, APROP_ViewHeight, p->viewheight);
   ACS_TakeInventory("Lith_WeaponScopedToken",  999);

   Lith_PlayerResetCBIGUI(p);

   p->frozen     = 0;
   p->semifrozen = 0;

   p->addpitch = 0.0f;
   p->addyaw   = 0.0f;

   p->bobpitch = 0.0f;
   p->bobyaw   = 0.0f;

   p->extrpitch = 0.0f;
   p->extryaw   = 0.0f;

   p->scoreaccum     = 0;
   p->scoreaccumtime = 0;
   p->scoremul       = 1.3;

   //
   // Static data

   if(!p->bip.init)
      Lith_PlayerInitBIP(p);

   if(!p->upgrinit)
      Lith_PlayerInitUpgrades(p);
   else
      Lith_PlayerReinitUpgrades(p);

   if(!p->staticinit)
   {
      p->log("> Lithium " Lith_Version " :: Compiled %S", __DATE__);

      if(world.dbgLevel) {
         p->logH("> player_t is %u bytes long!", sizeof(player_t) * 4);
         PrintDmonAllocSize(p);
      } else {
         p->logH("> Press \"%jS\" to open the menu.", "lith_k_opencbi");
      }

      p->deliverMail("Intro");

      p->staticinit = true;
   }

   if(world.dbgItems)
   {
      for(int i = weapon_min; i < weapon_max; i++) {
         weaponinfo_t const *info = &weaponinfo[i];
         if(info->classname != null && info->pclass & p->pclass && !(info->flags & wf_magic))
            ACS_GiveInventory(info->classname, 1);
      }
   }
}

// Static Functions ----------------------------------------------------------|

//
// Lith_GetArmorType
//
// Update information on what kind of armour we have.
//
static void Lith_GetArmorType(player_t *p)
{
   __str cl = p->armorclass;
        if(cl == "ArmorBonus")      p->armortype = armor_bonus;
   else if(cl == "GreenArmor" ||
           cl == "SilverShield")    p->armortype = armor_green;
   else if(cl == "BlueArmor" ||
           cl == "BlueArmorForMegasphere" ||
           cl == "EnchantedShield") p->armortype = armor_blue;
   else if(cl == "None")            p->armortype = armor_none;
   else                             p->armortype = armor_unknown;
}

// EOF