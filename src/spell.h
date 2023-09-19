#pragma once

#include <string>

namespace spell
{
    enum ID : int
    {
        ARCANE_TORRENT = 28730,
        ARCANE_BARRAGE = 44781,
        ARCANE_BLAST = 42897,
        ARCANE_EXPLOSION = 42921,
        ARCANE_MISSILES = 42846,
        BLIZZARD = 42940,
        FROSTBOLT = 42842,
        FIREBALL = 42833,
        FIREBALL_DOT = 4283300,
        FROSTFIRE_BOLT = 47610,
        FROSTFIRE_BOLT_DOT = 4761000,
        SCORCH = 42859,
        FIRE_BLAST = 42873,
        PYROBLAST = 42891,
        PYROBLAST_DOT = 4289100,
        LIVING_BOMB = 55360,
        LIVING_BOMB_EXPLOSION = 55362,
        FLAMESTRIKE = 42926,
        FLAMESTRIKE_DOT = 4292600,
        FLAMESTRIKE_DR = 42925,
        FLAMESTRIKE_DR_DOT = 4292500,
        BLAST_WAVE = 42945,
        DRAGONS_BREATH = 42950,
        ICE_LANCE = 42914,
        CONE_OF_COLD = 42931,
        DEEP_FREEZE = 44572,
        COLD_SNAP = 11958,
        FIRE_WARD = 43010,
        MANA_SHIELD = 43020,
        IGNITE = 12848,
        MANA_GEM = 42987,
        EVOCATION = 12051,
        SAPPER_CHARGE = 56488,
        PENDULUM_TELLURIC_CURRENTS = 60483,
        DARKMOON_DEATH = 60203,
        EXTRACT_NECROMANTIC_POWER = 60488,
        PILLAR_OF_FLAME_HC = 67760,
        PILLAR_OF_FLAME_NM = 67714,

        MIRROR_IMAGE = 55342,
        MIRROR_FIRE_BLAST = 59637,
        MIRROR_FROSTBOLT = 59638,

        WATER_ELEMENTAL = 31687,
        WATERBOLT = 31707,

        VALKYR_PROTECTOR = 71844,
        VALKYR_PROTECTOR_SMITE = 71842,
        VALKYR_GUARDIAN = 71843,
        VALKYR_GUARDIAN_SMITE = 71841,
    };

    enum Result : int
    {
        NONE,
        HIT,
        CRIT,
        MISS
    };

    struct Spell
    {
        const ID id;
        const std::string name;
        double cost = 0;
        double min_dmg = 0;
        double max_dmg = 0;
        double cast_time = 0;
        double delay = 0;
        double coeff = 1;
        double gcd = 1.5;
        double speed = 0;
        bool channeling = false;
        bool dot = false;
        bool proc = false;
        bool binary = false;
        bool aoe = false;
        bool aoe_capped = true;
        bool fixed_dmg = false;
        bool active_use = true;
        bool off_gcd = false;
        bool is_trigger = false;
        bool can_proc = true;
        int ticks = 0;
        int t_interval = 1;
        const School school = SCHOOL_NONE;

        double actual_cost = 0;
        double actual_cast_time = 0;
        int tick = 0;
        bool done = false;

        double avgDmg() const
        {
            return (min_dmg + max_dmg)/2.0;
        }

        // TODO: what other fields can be const-init?
        Spell(ID _id, const std::string& _name, School _school)
            : id(_id), name(_name), school(_school) {}
    };

    struct SpellInstance
    {
        std::shared_ptr<Spell> spell;
        Result result = Result::NONE;
        double dmg = 0;
        double resist = 0;
        int tick = 0;
    };

    struct ArcaneBarrage : Spell
    {
        ArcaneBarrage() : Spell(ARCANE_BARRAGE, "Arcane Barrage", SCHOOL_ARCANE)
        {
            cost = 18;
            min_dmg = 936;
            max_dmg = 1144;
            cast_time = 0;
            coeff = 2.5/3.5;
            speed = 24;
        }
    };

    struct ArcaneBlast : Spell
    {
        ArcaneBlast() : Spell(ARCANE_BLAST, "Arcane Blast", SCHOOL_ARCANE)
        {
            cost = 7;
            min_dmg = 1185;
            max_dmg = 1377;
            cast_time = 2.5;
            coeff = 2.5/3.5;
        }
    };

    struct ArcaneExplosion : Spell
    {
        ArcaneExplosion() : Spell(ARCANE_EXPLOSION, "Arcane Explosion", SCHOOL_ARCANE)
        {
            cost = 22;
            min_dmg = 538;
            max_dmg = 582;
            cast_time = 0;
            coeff = 1.5/3.5/2;
            aoe = true;
        }
    };

    struct ArcaneMissiles : Spell
    {
        ArcaneMissiles() : Spell(ARCANE_MISSILES, "Arcane Missiles", SCHOOL_ARCANE)
        {
            cost = 31;
            min_dmg = 362;
            max_dmg = 362;
            cast_time = 5;
            coeff = 5.0/3.5;
            channeling = true;
            ticks = 5;
            speed = 20;
        }
    };

    struct Blizzard : Spell
    {
        Blizzard() : Spell(BLIZZARD, "Blizzard", SCHOOL_FROST)
        {
            cost = 74;
            min_dmg = 426;
            max_dmg = 426;
            cast_time = 8;
            coeff = 4.0/3.5;
            channeling = true;
            ticks = 8;
            aoe = true;
        }
    };

    struct Frostbolt : Spell
    {
        Frostbolt() : Spell(FROSTBOLT, "Frostbolt", SCHOOL_FROST)
        {
            cost = 11;
            min_dmg = 804;
            max_dmg = 866;
            cast_time = 3;
            coeff = 3/3.5;
            speed = 28;
        }
    };

    struct Fireball : Spell
    {
        Fireball() : Spell(FIREBALL, "Fireball", SCHOOL_FIRE)
        {
            cost = 19;
            min_dmg = 898;
            max_dmg = 1143;
            cast_time = 3.5;
            coeff = 1;
            speed = 24;
        }
    };

    struct FireballDot : Spell
    {
        FireballDot() : Spell(FIREBALL_DOT, "Fireball", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            coeff = 0;
            t_interval = 2;
            ticks = 4;
            min_dmg = 29;
            max_dmg = 29;
        }
    };

    struct FrostfireBolt : Spell
    {
        FrostfireBolt() : Spell(FROSTFIRE_BOLT, "Frostfire Bolt", SCHOOL_FROSTFIRE)
        {
            cost = 14;
            min_dmg = 722;
            max_dmg = 838;
            cast_time = 3;
            coeff = 3/3.5;
            speed = 28;
        }
    };

    struct FrostfireBoltDot : Spell
    {
        FrostfireBoltDot() : Spell(FROSTFIRE_BOLT_DOT, "Frostfire Bolt", SCHOOL_FROSTFIRE)
        {
            dot = true;
            active_use = false;
            t_interval = 3;
            ticks = 3;
            coeff = 0;
            min_dmg = 30;
            max_dmg = 30;
        }
    };

    struct Scorch : Spell
    {
        Scorch() : Spell(SCORCH, "Scorch", SCHOOL_FIRE)
        {
            cost = 8;
            min_dmg = 382;
            max_dmg = 451;
            cast_time = 1.5;
            coeff = 1.5/3.5;
        }
    };

    struct FireBlast : Spell
    {
        FireBlast() : Spell(FIRE_BLAST, "Fire Blast", SCHOOL_FIRE)
        {
            cost = 21;
            min_dmg = 925;
            max_dmg = 1095;
            cast_time = 0;
            coeff = 1.5/3.5;
        }
    };

    struct IceLance : Spell
    {
        IceLance() : Spell(ICE_LANCE, "Ice Lance", SCHOOL_FROST)
        {
            cost = 6;
            min_dmg = 224;
            max_dmg = 258;
            cast_time = 0;
            coeff = 1.5/3.5/3.0;
            speed = 38;
        }
    };

    struct Pyroblast : Spell
    {
        Pyroblast() : Spell(PYROBLAST, "Pyroblast", SCHOOL_FIRE)
        {
            cost = 22;
            min_dmg = 1210;
            max_dmg = 1531;
            cast_time = 5;
            coeff = 1.15;
            speed = 24;
        }
    };

    struct PyroblastDot : Spell
    {
        PyroblastDot() : Spell(PYROBLAST_DOT, "Pyroblast", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            t_interval = 3;
            ticks = 4;
            min_dmg = 113;
            max_dmg = 113;
            coeff = 0.05;
        }
    };

    struct Flamestrike : Spell
    {
        Flamestrike() : Spell(FLAMESTRIKE, "Flamestrike", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 30;
            min_dmg = 876;
            max_dmg = 1071;
            cast_time = 2;
            coeff = 0.2357;
        }
    };

    struct FlamestrikeDot : Spell
    {
        FlamestrikeDot() : Spell(FLAMESTRIKE_DOT, "Flamestrike", SCHOOL_FIRE)
        {
            aoe = true;
            aoe_capped = false;
            dot = true;
            active_use = false;
            t_interval = 2;
            ticks = 4;
            min_dmg = 195;
            max_dmg = 195;
            coeff = 0.122;
        }
    };

    struct FlamestrikeDR : Spell
    {
        FlamestrikeDR() : Spell(FLAMESTRIKE_DR, "Flamestrike (Rank 8)", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 30;
            min_dmg = 699;
            max_dmg = 854;
            cast_time = 2;
            coeff = 0.2357;
        }
    };

    struct FlamestrikeDRDot : Spell
    {
        FlamestrikeDRDot() : Spell(FLAMESTRIKE_DR_DOT, "Flamestrike (Rank 8)", SCHOOL_FIRE)
        {
            aoe = true;
            aoe_capped = false;
            dot = true;
            active_use = false;
            t_interval = 2;
            ticks = 4;
            min_dmg = 155;
            max_dmg = 155;
            coeff = 0.122;
        }
    };

    struct LivingBomb : Spell
    {
        LivingBomb() : Spell(LIVING_BOMB, "Living Bomb", SCHOOL_FIRE)
        {
            cost = 22;
            dot = true;
            t_interval = 3;
            ticks = 4;
            min_dmg = 345;
            max_dmg = 345;
            coeff = 0.2;
        }
    };

    struct LivingBombExplosion : Spell
    {
        LivingBombExplosion() : Spell(LIVING_BOMB_EXPLOSION, "Living Bomb", SCHOOL_FIRE)
        {
            aoe = true;
            active_use = false;
            min_dmg = 690;
            max_dmg = 690;
            coeff = 0.4;
        }
    };

    struct BlastWave : Spell
    {
        BlastWave() : Spell(BLAST_WAVE, "Blast Wave", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 7;
            min_dmg = 1047;
            max_dmg = 1233;
            cast_time = 0;
            coeff = 0.1936;
        }
    };

    struct DragonsBreath : Spell
    {
        DragonsBreath() : Spell(DRAGONS_BREATH, "Dragon's Breath", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 7;
            min_dmg = 1101;
            max_dmg = 1279;
            cast_time = 0;
            coeff = 0.1936;
        }
    };

    struct ConeOfCold : Spell
    {
        ConeOfCold() : Spell(CONE_OF_COLD, "Cone of Cold", SCHOOL_FROST)
        {
            aoe = true;
            cost = 25;
            min_dmg = 710;
            max_dmg = 776;
            cast_time = 0;
            coeff = 0.214;
        }
    };

    struct DeepFreeze : Spell
    {
        DeepFreeze() : Spell(DEEP_FREEZE, "Deep Freeze", SCHOOL_FROST)
        {
            cost = 9;
            min_dmg = 2369;
            max_dmg = 2641;
            cast_time = 0;
            coeff = 7.5/3.5;
        }
    };

    struct ColdSnap : Spell
    {
        ColdSnap() : Spell(COLD_SNAP, "Cold Snap", SCHOOL_FROST)
        {
            cost = 0;
            is_trigger = true;
            gcd = 0;
        }
    };

    struct FireWard : Spell
    {
        FireWard() : Spell(FIRE_WARD, "Fire Ward", SCHOOL_FIRE)
        {
            cost = 16;
            is_trigger = true;
            can_proc = false;
        }
    };

    struct ManaShield : Spell
    {
        ManaShield() : Spell(MANA_SHIELD, "Mana Shield", SCHOOL_ARCANE)
        {
            cost = 7;
            is_trigger = true;
            can_proc = false;
        }
    };

    struct Ignite : Spell
    {
        Ignite(double _dmg) : Spell(IGNITE, "Ignite", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            t_interval = 2;
            ticks = 2;
            coeff = 0;
            min_dmg = max_dmg = _dmg;
            fixed_dmg = true;
        }
    };

    struct ManaGem : Spell
    {
        ManaGem() : Spell(MANA_GEM, "Mana Gem", SCHOOL_ARCANE)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct Evocation : Spell
    {
        Evocation(int _ticks = 4) : Spell(EVOCATION, "Evocation", SCHOOL_ARCANE)
        {
            is_trigger = true;
            channeling = true;
            ticks = _ticks;
            cast_time = _ticks*2;
        }
    };

    struct SapperCharge : Spell
    {
        SapperCharge() : Spell(SAPPER_CHARGE, "Global Thermal Sapper Charge", SCHOOL_FIRE)
        {
            min_dmg = 2188;
            max_dmg = 2812;
            coeff = 0;
            gcd = 0;
            off_gcd = true;
        }
    };

    struct PendulumTelluricCurrents : Spell
    {
        PendulumTelluricCurrents() : Spell(PENDULUM_TELLURIC_CURRENTS, "Pendulum of Telluric Currents", SCHOOL_SHADOW)
        {
            proc = true;
            min_dmg = 1168;
            max_dmg = 1752;
            coeff = 0;
            gcd = 0;
            active_use = false;
        }
    };

    struct DarkmoonDeath : Spell
    {
        DarkmoonDeath() : Spell(DARKMOON_DEATH, "Darkmoon Card: Death", SCHOOL_SHADOW)
        {
            proc = true;
            min_dmg = 1750;
            max_dmg = 2250;
            coeff = 0;
            gcd = 0;
            active_use = false;
        }
    };

    struct ExtractNecromanticPower : Spell
    {
        ExtractNecromanticPower() : Spell(EXTRACT_NECROMANTIC_POWER, "Extract of Necromatic Power", SCHOOL_SHADOW)
        {
            proc = true;
            min_dmg = 788;
            max_dmg = 1312;
            coeff = 0;
            gcd = 0;
            active_use = false;
            speed = 20;
        }
    };

    struct PillarOfFlameHc : Spell
    {
        PillarOfFlameHc() : Spell(PILLAR_OF_FLAME_HC, "Pillar of Flame", SCHOOL_FIRE)
        {
            proc = true;
            min_dmg = 1959;
            max_dmg = 2275;
            coeff = 0;
            gcd = 0;
            active_use = false;
            speed = 20;
        }
    };

    struct PillarOfFlameNm : Spell
    {
        PillarOfFlameNm() : Spell(PILLAR_OF_FLAME_NM, "Pillar of Flame", SCHOOL_FIRE)
        {
            proc = true;
            min_dmg = 1741;
            max_dmg = 2023;
            coeff = 0;
            gcd = 0;
            active_use = false;
            speed = 20;
        }
    };

    struct ArcaneTorrent : Spell
    {
        ArcaneTorrent() : Spell(ARCANE_TORRENT, "Arcane Torrent", SCHOOL_ARCANE)
        {
            gcd = 0;
            cost = 0;
            is_trigger = true;
            can_proc = false;
        }
    };

    struct MirrorImage : Spell
    {
        MirrorImage() : Spell(MIRROR_IMAGE, "Mirror Image", SCHOOL_ARCANE)
        {
            cost = 10;
            is_trigger = true;
            can_proc = false;
        }
    };

    struct MirrorFrostbolt : Spell
    {
        MirrorFrostbolt() : Spell(MIRROR_FROSTBOLT, "Frostbolt", SCHOOL_FROST)
        {
            cost = 0;
            min_dmg = 163;
            max_dmg = 169;
            cast_time = 3;
            coeff = 0.3;
            speed = 24;
        }
    };

    struct MirrorFireBlast : Spell
    {
        MirrorFireBlast() : Spell(MIRROR_FIRE_BLAST, "Fire Blast", SCHOOL_FIRE)
        {
            cost = 0;
            min_dmg = 88;
            max_dmg = 98;
            cast_time = 0;
            coeff = 0.15;
        }
    };

    struct WaterElemental : Spell
    {
        WaterElemental() : Spell(WATER_ELEMENTAL, "Water Elemental", SCHOOL_FROST)
        {
            cost = 16;
            is_trigger = true;
        }
    };

    struct Waterbolt : Spell
    {
        Waterbolt() : Spell(WATERBOLT, "Waterbolt", SCHOOL_FROST)
        {
            cost = 1;
            min_dmg = 601;
            max_dmg = 673;
            cast_time = 2.5;
            coeff = 2.5/3.0;
            speed = 16;
        }
    };

    struct ValkyrProtector : Spell
    {
        ValkyrProtector() : Spell(VALKYR_PROTECTOR, "Val'kyr Protector", SCHOOL_HOLY)
        {
            coeff = 0;
            gcd = 0;
            active_use = false;
            is_trigger = true;
            proc = true;
        }
    };

    struct ValkyrGuardian : Spell
    {
        ValkyrGuardian() : Spell(VALKYR_GUARDIAN, "Val'kyr Guardian", SCHOOL_HOLY)
        {
            coeff = 0;
            gcd = 0;
            active_use = false;
            is_trigger = true;
            proc = true;
        }
    };

    struct ValkyrProtectorSmite : Spell
    {
        ValkyrProtectorSmite() : Spell(VALKYR_PROTECTOR_SMITE, "Smite", SCHOOL_HOLY)
        {
            cost = 1;
            min_dmg = 1804;
            max_dmg = 2022;
            cast_time = 1.5;
            coeff = 0;
        }
    };

    struct ValkyrGuardianSmite : Spell
    {
        ValkyrGuardianSmite() : Spell(VALKYR_GUARDIAN_SMITE, "Smite", SCHOOL_HOLY)
        {
            cost = 1;
            min_dmg = 1591;
            max_dmg = 1785;
            cast_time = 1.5;
            coeff = 0;
        }
    };
}