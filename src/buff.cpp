#include "buff.h"
#include "common.h"

#include <string>
#include <iomanip>

namespace buff
{

int Buff::addStack()
{
    if (stacks < max_stacks)
        stacks++;
    else
        return 0;

    return stacks;
}

ArcaneBlast::ArcaneBlast()
{
    id = ARCANE_BLAST;
    name = "Arcane Blast";
    duration = 8;
    max_stacks = 4;
    snapshot = true;
}

ArcanePower::ArcanePower(bool glyph)
{
    id = ARCANE_POWER;
    name = "Arcane Power";
    duration = 15;
    if (glyph)
        duration += 3;
}

PresenceOfMind::PresenceOfMind()
{
    id = PRESENCE_OF_MIND;
    name = "Presence of Mind";
    duration = 100;
    snapshot = true;
}

ArcanePotency::ArcanePotency()
{
    id = ARCANE_POTENCY;
    name = "Arcane Potency";
    duration = 1000; // unlimited
}

IcyVeins::IcyVeins()
{
    id = ICY_VEINS;
    name = "Icy Veins";
    duration = 20;
    cost = 3;
}

Combustion::Combustion()
{
    id = COMBUSTION;
    name = "Combustion";
    duration = 2000;
    max_stacks = 20;
}

Evocation::Evocation(double haste, int ticks)
{
    id = EVOCATION;
    name = "Evocation";
    duration = 2.0 * ticks * haste;
}

Clearcast::Clearcast()
{
    id = CLEARCAST;
    name = "Clearcast";
    duration = 15;
    snapshot = true;
}

Bloodlust::Bloodlust()
{
    id = BLOODLUST;
    name = "Bloodlust";
    duration = 40;
}

PowerInfusion::PowerInfusion()
{
    id = POWER_INFUSION;
    name = "Power Infusion";
    duration = 15;
}

Innervate::Innervate()
{
    id = INNERVATE;
    name = "Innervate";
    duration = 20;
}

ManaTide::ManaTide()
{
    id = MANA_TIDE;
    name = "Mana Tide";
    duration = 12;
}

Berserking::Berserking()
{
    id = BERSERKING;
    name = "Berserking";
    duration = 10;
}

MissileBarrage::MissileBarrage()
{
    id = MISSILE_BARRAGE;
    name = "Missile Barrage";
    duration = 15;
}

HotStreak::HotStreak()
{
    id = HOT_STREAK;
    name = "Hot Streak";
    duration = 10;
}

Firestarter::Firestarter()
{
    id = FIRESTARTER;
    name = "Firestarter";
    duration = 10;
}

BrainFreeze::BrainFreeze()
{
    id = BRAIN_FREEZE;
    name = "Brain Freeze";
    duration = 15;
}

FingersOfFrost::FingersOfFrost()
{
    id = FINGERS_OF_FROST;
    name = "Fingers of Frost";
    duration = 15;
}

GhostFingers::GhostFingers()
{
    id = GHOST_FINGERS;
    name = "Fingers of Frost (Ghost proc)";
    duration = 0.01;
    hidden = true;
}

FocusMagic::FocusMagic()
{
    id = FOCUS_MAGIC;
    name = "Focus Magic";
    duration = 6000; // This is actually 10 sec but assume it gets refreshed
    stats.crit = 3;
}

FireWard::FireWard()
{
    id = FIRE_WARD;
    name = "Fire Ward";
    duration = 30;
}

ManaShield::ManaShield()
{
    id = MANA_SHIELD;
    name = "Mana Shield";
    duration = 60;
}

IncantersAbsorption::IncantersAbsorption(double sp)
{
    id = INCANTERS_ABSORPTION;
    duration = 10;
    stats.spell_power = sp;
    show_refresh = true;

    name = "Incanter's Absorption (" + std::to_string(static_cast<unsigned int>(sp)) + ")";
}

HyperspeedAcceleration::HyperspeedAcceleration()
{
    id = HYPERSPEED_ACCELERATION;
    name = "Hyperspeed Acceleration";
    duration = 12;
    stats.haste_rating = 340;
}

BlackMagic::BlackMagic()
{
    id = BLACK_MAGIC;
    name = "Black Magic";
    duration = 10;
    stats.haste_rating = 250;
}

Lightweave::Lightweave()
{
    id = LIGHTWEAVE;
    name = "Lightweave";
    duration = 15;
    stats.spell_power = 295;
}

Speed::Speed()
{
    id = SPEED;
    name = "Speed";
    duration = 15;
    stats.haste_rating = 500;
}

WildMagic::WildMagic()
{
    id = WILD_MAGIC;
    name = "Wild Magic";
    duration = 15;
    stats.spell_power = 200;
    stats.crit = critRatingToChance(200);
}

ManaSurge::ManaSurge()
{
    id = MANA_SURGE;
    name = "Mana Surge";
    duration = 15;
    stats.spell_power = 225;
}

ManaSurges::ManaSurges()
{
    id = MANA_SURGES;
    name = "Mana Surges";
    duration = 15;
    stats.spell_power = 225;
}

Praxis::Praxis()
{
    id = PRAXIS;
    name = "Praxis";
    duration = 15;
    stats.spell_power = 350;
}

QuadCore::QuadCore()
{
    id = QUAD_CORE;
    name = "Quad Core";
    duration = 30;
}

PushingTheLimit::PushingTheLimit()
{
    id = PUSHING_THE_LIMIT;
    name = "Pushing the Limit";
    duration = 5;
}

AshenBand::AshenBand()
{
    id = ASHEN_BAND;
    name = "Frostforged Sage";
    duration = 10;
    stats.spell_power = 285;
}

TwilightSerpent::TwilightSerpent()
{
    id = TWILIGHT_SERPENT;
    name = "Twilight Serpent";
    duration = 20;
    stats.spell_power = 292;
}

InfinitePower::InfinitePower()
{
    id = INFINITE_POWER;
    name = "Infinite Power";
    duration = 20;
    stats.spell_power = 202;
}

TomeArcanePhenomena::TomeArcanePhenomena()
{
    id = TOME_ARCANE_PHENOMENA;
    name = "Tome of Arcane Phenomena";
    duration = 20;
    stats.haste_rating = 256;
}

ArgentValor::ArgentValor()
{
    id = ARGENT_VALOR;
    name = "Argent Valor";
    duration = 20;
    stats.spell_power = 281;
}

MarkWarPrisoner::MarkWarPrisoner()
{
    id = MARK_WAR_PRISONER;
    name = "Mark of the War Prisoner";
    duration = 20;
    stats.spell_power = 346;
}

ForgeEmber::ForgeEmber()
{
    id = FORGE_EMBER;
    name = "Forge Ember";
    duration = 10;
    stats.spell_power = 512;
}

NowIsTheTime::NowIsTheTime()
{
    id = NOW_IS_THE_TIME;
    name = "Now is the Time!";
    duration = 10;
    stats.spell_power = 590;
}

EmbraceSpider::EmbraceSpider()
{
    id = EMBRACE_SPIDER;
    name = "Embrace of the Spider";
    duration = 10;
    stats.haste_rating = 505;
}

DyingCurse::DyingCurse()
{
    id = DYING_CURSE;
    name = "Dying Curse";
    duration = 10;
    stats.spell_power = 765;
}

IllustrationDragonSoul::IllustrationDragonSoul()
{
    id = ILLUSTRATION_DRAGON_SOUL;
    name = "Illustration of the Dragon Soul";
    duration = 10;
    stats.spell_power = 20;
    max_stacks = 10;
}

AlacrityElements::AlacrityElements()
{
    id = ALACRITY_ELEMENTS;
    name = "Alacrity of the Elements";
    duration = 10;
    stats.haste_rating = 552;
}

EyeBroodmother::EyeBroodmother()
{
    id = EYE_BROODMOTHER;
    name = "Eye of the Broodmother";
    duration = 10;
    max_stacks = 5;
    stats.spell_power = 26;
}

MemoriesLove::MemoriesLove()
{
    id = MEMORIES_LOVE;
    name = "Memories of Love";
    duration = 15;
    stats.mp5 = 220;
}

ScaleFates::ScaleFates()
{
    id = SCALE_FATES;
    name = "Scale of Fates";
    duration = 20;
    stats.haste_rating = 457;
}

FlareHeavens::FlareHeavens()
{
    id = FLARE_HEAVENS;
    name = "Flame of the Heavens";
    duration = 10;
    stats.spell_power = 959;
}

ShowFaith::ShowFaith()
{
    id = SHOW_FAITH;
    name = "Show of Faith";
    duration = 15;
    stats.mp5 = 272;
}

LivingFlame::LivingFlame()
{
    id = LIVING_FLAME;
    name = "Living Flame";
    duration = 20;
    stats.spell_power = 534;
}

PandorasPlea::PandorasPlea()
{
    id = PANDORAS_PLEA;
    name = "Pandora's Plea";
    duration = 10;
    stats.spell_power = 794;
}

VolatilePowerHc::VolatilePowerHc()
{
    id = VOLATILE_POWER_HC;
    name = "Volatile Power";
    duration = 20;
}

VolatilePowerNm::VolatilePowerNm()
{
    id = VOLATILE_POWER_NM;
    name = "Volatile Power";
    duration = 20;
}

VolatilityHc::VolatilityHc()
{
    id = VOLATILITY_HC;
    name = "Volatility";
    duration = 20;
    max_stacks = 8;
    stats.haste_rating = 64;
}

VolatilityNm::VolatilityNm()
{
    id = VOLATILITY_NM;
    name = "Volatility";
    duration = 20;
    max_stacks = 8;
    stats.haste_rating = 57;
}

EnergizedHc::EnergizedHc()
{
    id = ENERGIZED_HC;
    name = "Energized";
    duration = 10;
    max_stacks = 8;
    stats.mp5 = 18;
}

EnergizedNm::EnergizedNm()
{
    id = ENERGIZED_NM;
    name = "Energized";
    duration = 10;
    max_stacks = 8;
    stats.mp5 = 16;
}

ReignUnlivingHc::ReignUnlivingHc()
{
    id = REIGN_UNLIVING_HC;
    name = "Shard of Flame";
    duration = 10000;
    max_stacks = 3;
}

ReignUnlivingNm::ReignUnlivingNm()
{
    id = REIGN_UNLIVING_NM;
    name = "Mote of Flame";
    duration = 10000;
    max_stacks = 3;
}

MuradinsSpyglassHc::MuradinsSpyglassHc()
{
    id = MURADINS_SPYGLASS_HC;
    name = "Cultivated Power";
    duration = 10;
    max_stacks = 10;
    stats.spell_power = 20;
}

MuradinsSpyglassNm::MuradinsSpyglassNm()
{
    id = MURADINS_SPYGLASS_NM;
    name = "Cultivated Power";
    duration = 10;
    max_stacks = 10;
    stats.spell_power = 18;
}

DislodgedObjectHc::DislodgedObjectHc()
{
    id = DISLODGED_OBJECT_HC;
    name = "Surge of Power";
    duration = 20;
    max_stacks = 10;
    stack_refresh = false;
    stats.spell_power = 121;
}

DislodgedObjectNm::DislodgedObjectNm()
{
    id = DISLODGED_OBJECT_NM;
    name = "Surge of Power";
    duration = 20;
    max_stacks = 10;
    stack_refresh = false;
    stats.spell_power = 105;
}

NamelessLichHc::NamelessLichHc()
{
    id = NAMELESS_LICH_HC;
    name = "Siphoned Power";
    duration = 20;
    stats.spell_power = 1207;
}

NamelessLichNm::NamelessLichNm()
{
    id = NAMELESS_LICH_NM;
    name = "Siphoned Power";
    duration = 20;
    stats.spell_power = 1074;
}

HodirStormPower::HodirStormPower()
{
    id = HODIR_STORM_POWER;
    name = "Storm Power";
    duration = 30;
}

HodirStarlight::HodirStarlight()
{
    id = HODIR_STARLIGHT;
    name = "Starlight";
    duration = 50;
}

IronCouncilShieldOfRunes::IronCouncilShieldOfRunes()
{
    id = IRON_COUNCIL_SHIELD_OF_RUNES;
    name = "Shield of Runes";
    duration = 15;
}

FlameCap::FlameCap()
{
    id = FLAME_CAP;
    name = "Flame Cap";
    duration = 60;
}

SkullGuldan::SkullGuldan()
{
    id = SKULL_GULDAN;
    name = "Fel Infusion";
    duration = 20;
    stats.haste_rating = 175;
}

ShrunkenHead::ShrunkenHead()
{
    id = SHRUNKEN_HEAD;
    name = "Mojo Madness";
    duration = 20;
    stats.spell_power = 211;
}

NaaruSliver::NaaruSliver()
{
    id = NAARU_SLIVER;
    name = "Power Circle";
    duration = 15;
    stats.spell_power = 210;
}

AshtongueTalisman::AshtongueTalisman()
{
    id = ASHTONGUE_TALISMAN;
    name = "Insight of the Ashtongue";
    duration = 5;
    stats.haste_rating = 145;
}

MindQuickening::MindQuickening()
{
    id = MQG;
    name = "MQG";
    duration = 20;
    stats.haste_rating = 330;
}

ArcaneMadness::ArcaneMadness()
{
    id = ARCANE_MADNESS;
    name = "Arcane Madness";
    duration = 6;
    stats.spell_power = 70;
}

}