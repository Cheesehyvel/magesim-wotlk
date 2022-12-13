#include "talents.h"

#include <string>
#include <stdexcept>

Talents talents_from_url(const std::string& url)
{
    auto const slash = url.rfind('/');

    if (slash == url.npos)
        throw std::runtime_error("Failed to parse url");

    auto const first_hyphen = url.find('-', slash);

    if (first_hyphen == url.npos)
        throw std::runtime_error("Failed to parse url");

    Talents result{ 0 };

    // arcane
    for (auto i = slash + 1; i < first_hyphen; ++i)
    {
        auto const qty = url[i] - '0';

        if (!qty)
            continue;

        auto const slot = i - slash - 1;

        switch (slot)
        {
        case 1:
            result.arcane_focus = qty;
            break;
        case 5:
            result.clearcast = qty;
            break;
        case 6:
            result.magic_attunement = qty;
            break;
        case 7:
            result.spell_impact = qty;
            break;
        case 8:
            result.student_of_the_mind = qty;
            break;
        case 9:
            result.focus_magic = qty;
            break;
        case 10:
            result.arcane_shielding = qty;
            break;
        case 12:
            result.arcane_meditation = qty;
            break;
        case 13:
            result.torment_the_weak = qty;
            break;
        case 15:
            result.presence_of_mind = qty;
            break;
        case 16:
            result.arcane_mind = qty;
            break;
        case 18:
            result.arcane_instability = qty;
            break;
        case 19:
            result.arcane_potency = qty;
            break;
        case 20:
            result.arcane_empowerment = qty;
            break;
        case 21:
            result.arcane_power = qty;
            break;
        case 22:
            result.incanters_absorption = qty;
            break;
        case 23:
            result.arcane_flows = qty;
            break;
        case 24:
            result.mind_mastery = qty;
            break;
        case 26:
            result.missile_barrage = qty;
            break;
        case 27:
            result.netherwind_presence = qty;
            break;
        case 28:
            result.spell_power = qty;
            break;
        case 29:
            result.arcane_barrage = qty;
            break;
        default:
            break;
        }
    }

    auto const second_hyphen = url.find('-', first_hyphen + 1);
    auto const underscore = url.find('_', first_hyphen);

    if (second_hyphen == url.npos && underscore == url.npos)
        throw std::runtime_error("Failed to parse url");

    auto const fire_end_pos = second_hyphen == url.npos ? underscore : second_hyphen;

    // fire
    for (auto i = first_hyphen + 1; i < fire_end_pos; ++i)
    {
        auto const qty = url[i] - '0';

        if (!qty)
            continue;

        auto const slot = i - first_hyphen - 1;

        switch (slot)
        {
        case 0:
            result.imp_fire_blast = qty;
            break;
        case 1:
            result.incineration = qty;
            break;
        case 2:
            result.imp_fireball = qty;
            break;
        case 3:
            result.ignite = qty;
            break;
        case 5:
            result.world_in_flames = qty;
            break;
        case 8:
            result.pyroblast = qty;
            break;
        case 10:
            result.imp_scorch = qty;
            break;
        case 12:
            result.master_of_elements = qty;
            break;
        case 13:
            result.playing_with_fire = qty;
            break;
        case 14:
            result.critical_mass = qty;
            break;
        case 15:
            result.blast_wave = qty;
            break;
        case 17:
            result.fire_power = qty;
            break;
        case 18:
            result.pyromaniac = qty;
            break;
        case 19:
            result.combustion = qty;
            break;
        case 20:
            result.molten_fury = qty;
            break;
        case 22:
            result.empowered_fire = qty;
            break;
        case 23:
            result.firestarter = qty;
            break;
        case 24:
            result.dragons_breath = qty;
            break;
        case 25:
            result.hot_streak = qty;
            break;
        case 26:
            result.burnout = qty;
            break;
        case 27:
            result.living_bomb = qty;
            break;
        default:
            break;
        }
    }
    
    if (second_hyphen != url.npos)
    {
        // frost
        for (auto i = second_hyphen + 1; i < underscore; ++i)
        {
            auto const qty = url[i] - '0';

            if (!qty)
                continue;

            auto const slot = i - second_hyphen - 1;

            switch (slot)
            {
            case 1:
                result.imp_frostbolt = qty;
                break;
            case 2:
                result.ice_floes = qty;
                break;
            case 3:
                result.ice_shards = qty;
                break;
            case 5:
                result.precision = qty;
                break;
            case 7:
                result.piercing_ice = qty;
                break;
            case 8:
                result.icy_veins = qty;
                break;
            case 11:
                result.frost_channeling = qty;
                break;
            case 12:
                result.shatter = qty;
                break;
            case 13:
                result.cold_snap = qty;
                break;
            case 14:
                result.imp_cone_of_cold = qty;
                break;
            case 16:
                result.cold_as_ice = qty;
                break;
            case 17:
                result.winters_chill = qty;
                break;
            case 20:
                result.arctic_winds = qty;
                break;
            case 21:
                result.empowered_frostbolt = qty;
                break;
            case 22:
                result.fingers_of_frost = qty;
                break;
            case 23:
                result.brain_freeze = qty;
                break;
            case 24:
                result.water_elemental = qty;
                break;
            case 25:
                result.enduring_winter = qty;
                break;
            case 26:
                result.chilled_to_the_bone = qty;
                break;
            case 27:
                result.deep_freeze = qty;
                break;
            }
        }
    }
    return result;
}