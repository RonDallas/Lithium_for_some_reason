// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#if defined(LITH_X)
  LITH_X(WEAPONS,      "Weapons")
  LITH_X(ENEMIES,      "Adversaries")
// TODO: Add when things like the Division Sigil are in
//LITH_X(ITEMS,        "Items")
  LITH_X(UPGRADES,     "Upgrades")
  LITH_X(PLACES,       "Places")
  LITH_X(CORPORATIONS, "Companies")
  LITH_X(MAIL,         "Mail")
#undef LITH_X

#elif !defined(LITH_BIP_H)
#define LITH_BIP_H

#include "lith_list.h"

// Extern Functions ----------------------------------------------------------|

[[__call("ScriptS")]] void Lith_PlayerInitBIP(struct player *p);
struct bippage_s *Lith_FindBIPPage(struct bip_s *bip, __str name);
[[__optional_args(1)]] struct bippage_s *Lith_UnlockBIPPage(struct bip_s *bip, __str name, int pclass);
[[__call("ScriptS")]] void Lith_DeallocateBIP(struct bip_s *bip);
[[__optional_args(1)]] void Lith_DeliverMail(struct player *p, __str title, int flags);

// Types ---------------------------------------------------------------------|

enum
{
   BIPC_NONE,
#define LITH_X(name, capt) BIPC_##name,
#include "lith_bip.h"
   BIPC_EXTRA,
   BIPC_MAX,
   BIPC_MAIN,
   BIPC_LOG,
   BIPC_STATS,
   BIPC_SEARCH
};

enum
{
   MAILF_NoPrint    = 1 << 0,
   MAILF_AllPlayers = 1 << 1,
};

typedef __str bip_unlocks_t[5];

struct page_initializer
{
   int pclass;
   __str name;
   bip_unlocks_t unlocks;
   int category;
   bool isfree;
};

typedef struct bippage_s
{
   __str  name;
   __str  image;
   __str  body;
   __str  title;
   int    height;
   int    category;
   bool   unlocked;
   list_t link;
   bip_unlocks_t unlocks;
} bippage_t;

typedef struct bip_s
{
   __prop find       {call: Lith_FindBIPPage(this)}
   __prop unlock     {call: Lith_UnlockBIPPage(this)}
   __prop deallocate {call: Lith_DeallocateBIP(this)}

   // Stats
   int categoryavail[BIPC_MAX];
   int categorymax[BIPC_MAX];
   int pageavail;
   int pagemax;
   int mailreceived, mailtrulyreceived;

   // State
   bool init;
   bippage_t *curpage;
   int curpagenum;
   int curcategory;
   int lastcategory;
   int scroll;
   bippage_t *result[8];
   int resnum;
   int rescur;

   // Info
   list_t infogr[BIPC_MAX];
} bip_t;

#endif
