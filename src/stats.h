#pragma once

#include "common.h"

// Player statistics, derived from equipped items
struct Stats
{
    double intellect = 0;
    double spirit = 0;
    double mp5 = 0;
    double crit = 0;
    double hit = 0;
    double haste = 0;
    double haste_rating = 0;
    double spell_power = 0;

    // Enchants / procs
    bool black_magic = false;
    bool lightweave_embroidery = false;
    bool darkglow_embroidery = false;
    bool hyperspeed_accelerators = false;
    bool t3_2set = false;
    bool t5_2set = false;
    bool t5_4set = false;
    bool t6_2set = false;
    bool t6_4set = false;
    bool t7_2set = false;
    bool t7_4set = false;
    bool t8_2set = false;
    bool t8_4set = false;
    bool t9_2set = false;
    bool t9_4set = false;
    bool t10_2set = false;
    bool t10_4set = false;
    bool udc_2set = false;
    bool udc_3set = false;
    bool udc_4set = false;
    bool cudc_3set = false;
    bool ashen_band = false;

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;
    MetaGem meta_gem = META_NONE;
};