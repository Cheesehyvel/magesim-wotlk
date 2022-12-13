#include "config.h"
#include "minijson_reader.hpp"
#include "wow_data.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <math.h>

using namespace minijson::handlers;
using minijson::value;

namespace
{
bool is_downgrade(const wow_data::Item& a, const wow_data::Item& b)
{
#if 1
    return false;
#else
    // TODO: this might be worth implementing for p2 to reduce total permutations?
    if (a.sockets || b.sockets)
        return false;

    if (a.intellect > b.intellect &&
        a.spirit > b.spirit &&
        a.sp > b.sp &&
        a.hit > b.hit)
        return true;

    return false;
#endif
}

GemColor GetGem_color(char color)
{
    switch (color)
    {
    case 'a':
        return GemColor::Any;
    case 'r':
        return GemColor::Red;
    case 'b':
        return GemColor::Blue;
    case 'y':
        return GemColor::Yellow;
    case 'p':
        return GemColor::Purple;
    case 'g':
        return GemColor::Green;
    case 'o':
        return GemColor::Orange;
    case 'm':
        return GemColor::Meta;
    default:
        break;
    }
    throw std::runtime_error("Unrecognized gem color");
}
}

BulkSimPlan initialize_from_json(const std::string& path, Config& config, Talents& talents, Glyphs& glyphs, size_t threads)
{
    std::ifstream in(path);

    if (!in.is_open())
        throw std::runtime_error("Failed to open json");

    std::vector<int> items;
    std::vector<int> gems;
    std::vector<int> enchants;

    minijson::istream_context ctx(in);

    // TODO: In the future, many of these fields could be made optional,
    // and their absence would trigger exploration of all possible values
    static const minijson::dispatcher dispatcher
    {
#define Required(x) handler(#x, [](Config&c, value v) {v.to(c.##x);})
#define Optional(x) optional_handler(#x, [](Config&c, value v) {v.to(c.##x);})

        // BEGIN STATIC CONFIG OPTIONS

        // required fields
        Required(duration),
        Required(duration_variance),
        Required(avg_spell_dmg),
        Required(targets),
        Required(target_resistance),
        Required(target_level),
        Required(spell_travel_time),
        Required(reaction_time),
        Required(regem),

        Required(debuff_spell_crit),
        Required(debuff_spell_dmg),
        Required(debuff_spell_hit),
        Required(debuff_crit),
        Required(buff_spell_crit),
        Required(buff_spell_haste),
        Required(buff_haste),
        Required(buff_dmg),
        Required(totem_of_wrath),
        Required(flametongue),
        Required(demonic_pact),
        Required(demonic_pact_bonus),
        Required(mana_replenishment),
        Required(judgement_of_wisdom),
        Required(mage_armor),
        Required(molten_armor),
        Required(amplify_magic),
        Required(prof_engineer),

        Required(pre_mirror_image),
        Required(pre_water_elemental),
        Required(pre_incanters_absorption),
        Required(pre_mana_incanters_absorption),

        handler("potion", [](Config& c, value v) {int val = POTION_NONE; v.to(val); c.potion = static_cast<Potion>(val); }),
        handler("pre_potion", [](Config& c, value v) {int val = POTION_NONE; v.to(val); c.pre_potion = static_cast<Potion>(val); }),
        handler("conjured", [](Config& c, value v) {int val = CONJURED_NONE; v.to(val); c.conjured = static_cast<Conjured>(val); }),

        Required(evo_ticks),

        handler("rotation", [](Config& c, value v)
            {
                if (v.type() != minijson::value_type::String)
                    throw std::runtime_error("rotation must be string");

                const std::string str{ v.raw() };

                if (str == "st_frostfire")
                    c.rotation = ROTATION_ST_FROSTFIRE;
                else if (str == "st_ab_am")
                    c.rotation = ROTATION_ST_AB_AM;
                else if (str == "st_ab_am_barrage")
                    c.rotation = ROTATION_ST_AB_AM_BARRAGE;
                else if (str == "st_fire")
                    c.rotation = ROTATION_ST_FIRE;
                else if (str == "st_fire_sc")
                    c.rotation = ROTATION_ST_FIRE_SC;
                else if (str == "st_frost")
                    c.rotation = ROTATION_ST_FROST;
                else if (str == "aoe_ae")
                    c.rotation = ROTATION_AOE_AE;
                else if (str == "aoe_blizz")
                    c.rotation = ROTATION_AOE_BLIZZ;
                else if (str == "aoe_blizz_fs")
                    c.rotation = ROTATION_AOE_BLIZZ_FS;
                else
                    throw std::runtime_error("unrecognized rotation string");
            }),

        // optional fields
        Optional(rng_seed),

        // Single target AB -> AM rotation options
        Optional(rot_ab3_mana),
        Optional(rot_ab_no_mb_mana),
        Optional(rot_abs_ap),
        Optional(rot_mb_below_ab),
        Optional(rot_mb_mana),

        // Single target frost rotation options
        Optional(rot_ice_lance),
        Optional(rot_brain_freeze_hold),

        // Single target fire rotation options
        Optional(maintain_imp_scorch),
        Optional(ignite_munching),
        Optional(hot_streak_cqs),
        Optional(hot_streak_cqs_time),

        // END STATIC CONFIG OPTIONS

        // BEGIN BULK PLAN OPTIONS
        handler("talents", [&](Config&, value v)
            {
                talents = talents_from_url(std::string(v.raw()));
            }),

        handler("items", [&](Config&, value v, auto& context)
            {
                parse_array(context, [&](value v) { items.push_back(v.as<int>()); });
            }),

        handler("gems", [&](Config&, value v, auto& context)
            {
                parse_array(context, [&](value v) { gems.push_back(v.as<int>()); });
            }),

        handler("enchants", [&](Config&, value v, auto& context)
            {
                parse_array(context, [&](value v) { enchants.push_back(v.as<int>()); });
            }),

        // END BULK PLAN OPTIONS


        // TODO: Not sure if we actually want this in production?
        ignore_any_handler {},

#undef Optional
#undef Required
    };

    dispatcher.run(ctx, config);

    std::array<std::vector<int>, BulkSimPlan::slots> plan_items;
    std::array<std::vector<int>, BulkSimPlan::enchant_slots> plan_enchants;
    std::vector<int> plan_gems;

    // place items into the correct slot
    for (auto const& i : items)
    {
        auto found = false;

        for (auto const& i2 : wow_data::items)
        {
            if (i != i2.id)
                continue;

            if (i2.slot >= BulkSimPlan::slots)
                throw std::runtime_error("Bad item slot");

            plan_items[i2.slot].push_back(i);
            found = true;
            break;
        }

        if (!found)
            std::cerr << "No data for item " << i << ".  Ignoring." << std::endl;
    }

    for (auto const& g : gems)
    {
        auto found = false;
        for (auto const& g2 : wow_data::gems)
        {
            if (g != g2.id)
                continue;

            plan_gems.push_back(g);
            found = true;
            break;
        }
        
        if (!found)
            std::cerr << "No data for gem " << g << ".  Ignoring." << std::endl;
    }

    for (auto const& e : enchants)
    {
        auto found = false;

        for (auto const& e2 : wow_data::enchants)
        {
            if (e != e2.id)
                continue;

            if (e2.slot >= BulkSimPlan::enchant_slots)
                throw std::runtime_error("Bad enchant slot");
             
            plan_enchants[e2.slot].push_back(e);
            found = true;
            break;
        }

        if (!found)
            std::cerr << "No data for enchant " << e << ".  Ignoring." << std::endl;
    }

    if (!config.regem)
        plan_gems.clear();

    if (config.rotation == ROTATION_ST_FIRE)
    {
        config.timings.emplace_back("conjured", 0, 0, 0);
        config.timings.emplace_back("combustion", 0, 0, 0);
        config.timings.emplace_back("trinket1", 0, 0, 0);
        config.timings.emplace_back("hyperspeed_accelerators", 0, 0, 0);
        config.timings.emplace_back("bloodlust", 2, 0, 0);
        config.timings.emplace_back("hyperspeed_accelerators", 60, 0, 0);
        config.timings.emplace_back("potion", 60, 0, 0);
        config.timings.emplace_back("combustion", 120, 0, 0);
        config.timings.emplace_back("trinket1", 120, 0, 0);
    }

    return { threads, plan_items, plan_enchants, plan_gems };
}

BulkSimPlan::BulkSimPlan(
    size_t _threads,
    const std::array<std::vector<int>, slots>& _items,
    const std::array<std::vector<int>, enchant_slots>& _enchants,
    const std::vector<int>& _gems
) :
    threads(_threads), thread_locks(new std::mutex[_threads]), threads_work_queue(_threads),
    items(_items), enchants(_enchants), gems(_gems), total_permutations(0), done(false)
{
    auto static constexpr item_count = sizeof(wow_data::items) / sizeof(wow_data::items[0]);
    auto static constexpr gem_count = sizeof(wow_data::gems) / sizeof(wow_data::gems[0]);

    item_cache.clear();
    gem_cache.clear();

    for (auto i = 0u; i < items.size(); ++i)
        for (auto const& id : items[i])
        {
            bool found = false;
            for (auto j = 0u; j < item_count; ++j)
                if (wow_data::items[j].id == id)
                {
                    item_cache.emplace_back(wow_data::items[j]);
                    found = true;
                    break;
                }

            if (!found)
                throw std::runtime_error("Could not find item while populating cache");
        }

    std::sort(item_cache.begin(), item_cache.end(),
        [](const wow_data::Item& a, const  wow_data::Item& b)
        {
            return a.id < b.id;
        });

    std::vector<int> for_gem_cache;

    // no gems specified?  populate the cache with the automatic ones we will use
    if (_gems.empty())
        for_gem_cache = { wow_data::ids::META_CHAOTIC_SKYFLARE, 40026, 39998, 40051 };
    else
        for_gem_cache = _gems;

    for (auto const& id : for_gem_cache)
    {
        bool found = false;
        for (auto j = 0u; j < gem_count; ++j)
            if (wow_data::gems[j].id == id)
            {
                gem_cache.emplace_back(wow_data::gems[j]);
                found = true;
                break;
            }

        if (!found)
            throw std::runtime_error("Could not find gem while populating cache");
    }

    std::sort(gem_cache.begin(), gem_cache.end(),
        [](const wow_data::Gem& a, const wow_data::Gem& b)
        {
            return a.id < b.id;
        });
}

const wow_data::Item& BulkSimPlan::GetItem(int id) const
{
    for (auto const& item : item_cache)
    {
        if (item.id == id)
            return item;
        else if (item.id > id)
            break;
    }

    throw std::runtime_error("Item ID not found");
}

const wow_data::Gem& BulkSimPlan::GetGem(int id) const
{
    for (auto const& gem : gem_cache)
    {
        if (gem.id == id)
            return gem;
        else if (gem.id > id)
            break;
    }

    throw std::runtime_error("Gem ID not found");
}

size_t BulkSimPlan::Permutations(bool include_gems) const
{
    if (total_permutations > 0)
        return total_permutations;

    auto const gem_count = gems.size();
    size_t result = 1;

    for (auto i = 0; i < slots; ++i)
    {
        if (items[i].empty())
            continue;

        if (include_gems)
        {
            auto max_sockets = 0;
            for (auto const& item_id : items[i])
            {
                auto const& item = GetItem(item_id);

                int sockets = 0;
                for (auto p = item.sockets; p && *p; ++p)
                    ++sockets;

                if (sockets > max_sockets)
                    max_sockets = sockets;
            }

            for (auto j = 0; j < max_sockets; ++j)
                result *= gem_count;
        }

        result *= items[i].size();

    }

    return result;
}

void BulkSimPlan::RemoveDowngrades()
{
    // TODO
#if 0
    for (auto i = 0; i < slots; ++i)
    {
        auto& slot = items[i];
        std::vector<int> indices_to_remove;

        for (auto item1 = 0; item1 < slot.size(); ++item1)
            for (auto item2 = item1 + 1; item2 < slot.size(); ++item2)
            {
                auto const& i1 = GetItem(slot[item1]);
                auto const& i2 = GetItem(slot[item2]);

                if (is_downgrade(i1, i2))
                    indices_to_remove.push_back(item2);
                else if (is_downgrade(i2, i1))
                    indices_to_remove.push_back(item1);
            }

        // this is not right
        for (auto const& idx : indices_to_remove)
            slot.erase(slot.begin() + idx);
    }
#endif
}

void BulkSimPlan::Producer()
{
    GearSet gear{ 0 };

    unassigned_work_queue.clear();
    unassigned_work_queue.reserve(threads * max_size_per_thread);

    PopulateWorkQueue(gear, 0, 0);

    // count all pending work, assigned and unassigned, to balance
    // incoming assignments as evenly as possible

    total_permutations = unassigned_work_queue.size();

    for (auto i = 0u; i < threads; ++i)
    {
        std::lock_guard<std::mutex> lock(thread_locks[i]);
        total_permutations += threads_work_queue.size();
    }

    auto const per_thread = total_permutations / threads;

    // insert however many items are necessary to get each thread to 
    // 'per_thread' work items
    for (auto i = 0u; i < threads; ++i)
    {
        std::lock_guard<std::mutex> lock(thread_locks[i]);

        auto const this_thread_size = threads_work_queue.size();

        // does it already have enough?  leave it alone
        if (this_thread_size >= per_thread)
            continue;

        auto const count = per_thread - this_thread_size;
        auto const offset = i < threads - 1 ? unassigned_work_queue.size() - count : 0;

        threads_work_queue[i].insert(threads_work_queue[i].end(),
            std::make_move_iterator(unassigned_work_queue.begin() + offset),
            std::make_move_iterator(unassigned_work_queue.end()));

        unassigned_work_queue.resize(offset);
    }

    done = true;
}

std::vector<GearSet> BulkSimPlan::Consumer(int thread_id)
{
    std::lock_guard<std::mutex> guard(thread_locks[thread_id]);
    return std::move(threads_work_queue[thread_id]);
}

void BulkSimPlan::PopulateWorkQueue(GearSet& gear, int item_slot, int gem_slot)
{
    auto const this_item_slot = item_slot;
    auto const next_item_slot = item_slot + 1;

    if (item_slot == slots)
    {
        if (gear.IsValid(*this))
        {
            // if we are not regemming, apply standard gemming
            if (gems.empty())
                ApplyStandardGemming(gear);

            unassigned_work_queue.push_back(gear);
            SleepProducer();
        }
        return;
    }

    // second finger slot?
    if (item_slot == 14)
        item_slot = 13;
    // trinket slot?
    else if (item_slot == 15 || item_slot == 16)
        item_slot = 14;
    // off-hand?  check if main-hand is 2h, and if so skip this
    else if (item_slot == 1 && GetItem(gear.items[0]).two_hand)
    {
        gear.items[item_slot] = 0;
        PopulateWorkQueue(gear, next_item_slot, GearSet::max_gems);
        return;
    }
    
    // are we replacing gems?
    if (!gems.empty())
    {
        // finished gem slots? next item slot
        if (gem_slot == GearSet::max_gems)
        {
            PopulateWorkQueue(gear, next_item_slot, 0);
            return;
        }
        // working on gems?  next gem
        else if (gem_slot > 0)
        {
            auto const& item = GetItem(gear.items[this_item_slot]);
            for (auto p = item.sockets + gem_slot; p && *p; ++p)
            {
                auto const gem_index = static_cast<int>(p - item.sockets);
                auto const socket_color = *p;

                // just try every gem in every socket, deferring validation for later
                for (auto const& g : gems)
                {
                    auto const socket_is_meta = socket_color == 'm';
                    auto const gem_is_meta = GetGem(g).color == 'm';

                    // meta gems only in meta slots
                    if (socket_is_meta != gem_is_meta)
                        continue;

                    gear.gems[this_item_slot][gem_index] = g;
                    PopulateWorkQueue(gear, this_item_slot, gem_index + 1);
                }
            }

            // after gems are done, move on to next slot
            PopulateWorkQueue(gear, next_item_slot, 0);
            return;
        }
        // initialize gem slots for this item to zero
        else if (gem_slot == 0)
            gear.gems[this_item_slot] = { 0 };
    }

    for (auto i = 0u; i < items[item_slot].size(); ++i)
    {
        auto const& item = GetItem(items[item_slot][i]);

        gear.items[this_item_slot] = items[item_slot][i];

        // no sockets or not regemming?  just add the item and continue
        if (!item.sockets || gems.empty())
            PopulateWorkQueue(gear, next_item_slot, 0);
        else
        {
            // otherwise, try all possible gems
            for (auto p = item.sockets; p && *p; ++p)
            {
                auto const gem_index = static_cast<int>(p - item.sockets);
                auto const socket_color = *p;

                // meta gem requirements validation is deferred
                for (auto const& g : gems)
                {
                    auto const socket_is_meta = socket_color == 'm';
                    auto const gem_is_meta = GetGem(g).color == 'm';

                    // meta gems only in meta slots
                    if (socket_is_meta != gem_is_meta)
                        continue;

                    gear.gems[this_item_slot][gem_index] = g;
                    PopulateWorkQueue(gear, this_item_slot, gem_index + 1);
                }
            }
        }
    }
}

void BulkSimPlan::SleepProducer()
{
    // if the queue is getting too large, sleep to allow the workers
    // to consume data so we do not run out of memory
    if (unassigned_work_queue.size() < threads * max_size_per_thread)
        return;

    // for each thread, take an equal share of the queue, and move it to that queue.
    // note that we handle the final thread as a special case, to take whatever is
    // leftover

    auto const move_per_thread = unassigned_work_queue.size() / threads;

    for (auto i = 0u; i < threads; ++i)
    {
        std::lock_guard<std::mutex> lock(thread_locks[i]);

        auto const offset = i < threads - 1 ? unassigned_work_queue.size() - move_per_thread : 0;

        threads_work_queue[i].insert(threads_work_queue[i].end(),
            std::make_move_iterator(unassigned_work_queue.begin() + offset),
            std::make_move_iterator(unassigned_work_queue.end()));

        unassigned_work_queue.resize(offset);
    }

    // now that each thread has had some work delegated to it, sleep until at least one
    // of those threads has consumed everything

    bool wake_up = false;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        for (auto i = 0u; i < threads; ++i)
        {
            std::lock_guard<std::mutex> lock(thread_locks[i]);

            if (threads_work_queue[i].empty())
            {
                wake_up = true;
                break;
            }
        }
    } while (!wake_up);
}

void BulkSimPlan::ApplyStandardGemming(GearSet& gear)
{
    for (auto i = 0u; i < gear.items.size(); ++i)
    {
        if (i == 1 && !gear.items[i])
            continue;

        auto const& item = GetItem(gear.items[i]);

        for (auto p = item.sockets; p && *p; ++p)
        {
            auto const socket_color = *p;
            auto const socket_index = static_cast<int>(p - item.sockets);

            if (socket_color == 'm')
                gear.gems[i][socket_index] = wow_data::ids::META_CHAOTIC_SKYFLARE;
            else if (socket_color == 'b')
                gear.gems[i][socket_index] = 40026; // purified twilight opal
            else if (socket_color == 'r')
                gear.gems[i][socket_index] = 39998; // runed scarlet ruby
            else if (socket_color == 'y')
                gear.gems[i][socket_index] = 40051; // reckless monarch topaz (todo: unless we need hit?)
        }
    }
}

bool BulkSimPlan::Done() const
{
    return done;
}

bool GearSet::IsValid(const BulkSimPlan& plan) const
{
    // two of the same ring?
    // reject this condition because the exact same gear set should be produced,
    // only with the rings reversed.  this just prevents duplication.
    // TODO: whitelist non-unique rings, if any
    if (items[13] >= items[14])
        return false;

    // two of the same trinket?
    // same de-duplication strategy for trinkets
    // TODO: whitelist non-unique rings, if any
    if (items[15] >= items[16])
        return false;

    auto const regemming = !plan.gems.empty();

    std::array<int, GemColor::Count> meta_gem_requirements;
    std::array<int, GemColor::Count> colors;

    for (auto i = 0; i < GemColor::Count; ++i)
        colors[i] = meta_gem_requirements[i] = 0;

    for (auto i = 0; i < BulkSimPlan::slots; ++i)
    {
        if (!items[i])
        {
            if (i == 1)
                continue;
            else
                return false;
        }

        if (!regemming)
            continue;

        auto const& item = plan.GetItem(items[i]);

        // for all gems on this item
        for (auto g = item.sockets; g && *g; ++g)
        {
            auto const socket_color = *g;
            auto const gem_index = static_cast<int>(g - item.sockets);
            auto const gem_id = gems[i][gem_index];

            // for some reason no gem was socketed?  reject
            if (!gem_id)
                return false;

            auto const& gem = plan.GetGem(gems[i][gem_index]);

            ++colors[GetGem_color(gem.color)];

            // meta gem?  record requirements
            if (gem.color == 'm')
            {
                for (auto r = gem.req; r && *r; ++r)
                    ++meta_gem_requirements[GetGem_color(*r)];
            }
        }
    }

    if (regemming)
    {
        for (auto i = 0u; i < GemColor::Count; ++i)
        {
            if (i == GemColor::Meta || !meta_gem_requirements[i])
                continue;

            // meta gem requires this color.  check for matching
            switch (i)
            {
            case GemColor::Blue:
                if (meta_gem_requirements[i] > (
                    colors[GemColor::Blue] +
                    colors[GemColor::Purple] +
                    colors[GemColor::Green]))
                    return false;
                break;
            case GemColor::Yellow:
                if (meta_gem_requirements[i] > (
                    colors[GemColor::Yellow] +
                    colors[GemColor::Green] +
                    colors[GemColor::Orange]))
                    return false;
                break;
            case GemColor::Red:
                if (meta_gem_requirements[i] > (
                    colors[GemColor::Red] +
                    colors[GemColor::Purple] +
                    colors[GemColor::Orange]))
                    return false;
                break;
            default:
                throw std::runtime_error("Unsupported meta gem requirements");
            }
        }
    }

    return true;
}

Stats GearSet::BuildStats(const Config& config, const Talents& talents, const Glyphs &glyphs, const BulkSimPlan& plan) const
{
    Stats result;

    // base stats for undead
    result.intellect = 179;
    result.spirit = 179;
    result.crit = 0.91;

    switch (config.race)
    {
    case RACE_TROLL:
        result.intellect = 177;
        result.spirit = 175;
        break;
    case RACE_BLOOD_ELF:
        result.intellect = 184;
        result.spirit = 172;
        break;
    case RACE_DRAENEI:
        result.intellect = 181;
        result.spirit = 176;
        break;
    case RACE_GNOME:
        result.intellect = 184;
        result.spirit = 174;
        break;
    case RACE_HUMAN:
        result.intellect = 181;
        result.spirit = 174;
        break;
    default:
        break;
    }

    double crit_rating = 0;

    // Arcane Intellect
    result.intellect += 60;

    // TODO: allow overriding of these assumptions.  for now they use front-end defaults.
    
    // divine spirit
    result.spirit += 80;

    // imp motw
    result.intellect += 52;
    result.spirit += 52;

    // flask
    result.spell_power += 125;

    // food
    result.spell_power += 46;

    // focus magic
    result.crit += 3;

    // blessing of wisdom
    result.mp5 += 110;

    // END hard-coded front-end defaults

    result.trinket1 = static_cast<Trinket>(items[15]);
    result.trinket2 = static_cast<Trinket>(items[16]);

    // tier 3, 5, 6, 7, 8, 9, 10, udc, cudc
    int set_counts[9] = { 0 };

    // todo: ashen band check

    for (auto i = 0u; i < items.size(); ++i)
    {
        // no offhand
        if (i == 1 && !items[i])
            continue;

        auto const& item = plan.GetItem(items[i]);

        result.intellect += item.intellect;
        result.spirit += item.spirit;
        crit_rating += item.crit;
        result.hit += item.hit;
        result.haste_rating += item.haste;
        result.spell_power += item.sp;

        // todo: enchants
        switch (item.slot)
        {
        case 0:
            // mighty spellpower
            result.spell_power += 63;
            break;
        case 3:
            // arcanum of burning mysteries
            result.spell_power += 30;
            crit_rating += 20;
            break;
        case 5:
            // greater inscription of the storm
            result.spell_power += 24;
            crit_rating += 15;
            break;
        case 6:
            // lightweave embroidery
            result.lightweave_embroidery = true;
            break;
        case 7:
            // +10 stats to chest
            result.intellect += 10;
            result.spirit += 10;
            break;
        case 8:
            // superior spellpower (wrists)
            result.spell_power += 30;
            break;
        case 9:
            // hyperspeed accelerators
            result.hyperspeed_accelerators = true;
            break;
        case 11:
            // brilliant spellthread
            result.spell_power += 50;
            result.spirit += 20;
            break;
        case 12:
            // nitro boosts
            crit_rating += 24;
            break;
        default:
            break;
        }

        switch (item.itemset)
        {
        case wow_data::ids::T3_SET:
            ++set_counts[0];
            break;
        case wow_data::ids::T5_SET:
            ++set_counts[1];
            break;
        case wow_data::ids::T6_SET:
            ++set_counts[2];
            break;
        case wow_data::ids::T7_SET:
            ++set_counts[3];
            break;
        case wow_data::ids::T8_SET:
            ++set_counts[4];
            break;
        case wow_data::ids::T9_SET:
            ++set_counts[5];
            break;
        case wow_data::ids::T10_SET:
            ++set_counts[6];
            break;
        case wow_data::ids::UDC_SET:
            ++set_counts[7];
            break;
        case wow_data::ids::CUDC_SET:
            ++set_counts[8];
            break;
        default:
            break;
        }

        if (!item.sockets)
            continue;

        // check if requirements are met to apply gem bonus and apply gems themselves
        auto const& slot_gems = gems[i];

        bool apply_bonus = true;
        for (auto j = 0; j < slot_gems.size(); ++j)
        {
            if (!slot_gems[j])
                break;

            auto const& gem = plan.GetGem(slot_gems[j]);

            if (gem.color == 'm')
                result.meta_gem = static_cast<MetaGem>(slot_gems[j]);

            result.intellect += gem.intellect;
            result.spirit += gem.spirit;
            result.hit += gem.hit;
            crit_rating += gem.crit;
            result.haste_rating += gem.haste;
            result.mp5 += gem.mp5;
            result.spell_power += gem.sp;

            auto const socket_color = *(item.sockets + j);

            // belt buckle?  no matching
            if (item.slot == 10 && !socket_color)
                continue;

            if (!(socket_color == 'b' && (gem.color == 'b' || gem.color == 'p' || gem.color == 'g')) &&
                !(socket_color == 'y' && (gem.color == 'y' || gem.color == 'g' || gem.color == 'o')) &&
                !(socket_color == 'r' && (gem.color == 'r' || gem.color == 'p' || gem.color == 'o')) &&
                !(socket_color == 'm' && gem.color == 'm'))
            {
                apply_bonus = false;
                break;
            }
        }

        if (apply_bonus)
        {
            result.spell_power += item.bonus.sp;
            result.hit += item.bonus.hit;
            crit_rating += item.bonus.crit;
            result.haste_rating += item.bonus.haste;
            result.intellect += item.bonus.intellect;
            result.spirit += item.bonus.spirit;
        }
    }

    // tier 3, 5, 6, 7, 8, 9, 10, udc, cudc
    if (set_counts[0] >= 2)
        result.t3_2set = true;
    if (set_counts[1] >= 2)
        result.t5_2set = true;
    if (set_counts[1] >= 4)
        result.t5_4set = true;
    if (set_counts[2] >= 2)
        result.t6_2set = true;
    if (set_counts[2] >= 4)
        result.t6_4set = true;
    if (set_counts[3] >= 2)
        result.t7_2set = true;
    if (set_counts[3] >= 4)
        result.t7_4set = true;
    if (set_counts[4] >= 2)
        result.t8_2set = true;
    if (set_counts[4] >= 4)
        result.t8_4set = true;
    if (set_counts[5] >= 2)
        result.t9_2set = true;
    if (set_counts[5] >= 4)
        result.t9_4set = true;
    if (set_counts[6] >= 2)
        result.t10_2set = true;
    if (set_counts[6] >= 4)
        result.t10_4set = true;
    if (set_counts[7] >= 2)
        result.udc_2set = true;
    if (set_counts[7] >= 4)
        result.udc_4set = true;
    if (set_counts[8] >= 3)
        result.cudc_3set = true;

    // talents

    // Student of the Mind
    if (talents.student_of_the_mind == 1)
        result.spirit *= 1.04;
    else if (talents.student_of_the_mind == 2)
        result.spirit *= 1.07;
    else if (talents.student_of_the_mind == 3)
        result.spirit *= 1.1;

    // Arcane Mind
    if (talents.arcane_mind > 0)
        result.intellect *= 1.0 + talents.arcane_mind * 0.03;

    // Gnome racial
    if (config.race == RACE_GNOME)
        result.intellect *= 1.05;
    // Human racial
    else if (config.race == RACE_HUMAN)
        result.spirit *= 1.03;

    // Blessing of Kings
    result.intellect *= 1.1;
    result.spirit *= 1.1;

    // Buff spell power
    if (config.demonic_pact || config.totem_of_wrath || config.flametongue)
    {
        double x = 0;
        if (config.totem_of_wrath)
            x = 280;
        else if (config.flametongue)
            x = 144;
        if (config.demonic_pact && config.demonic_pact_bonus > x)
            x = config.demonic_pact_bonus;
        result.spell_power += x;
    }

    // Buff spell haste (e.g. wrath of air)
    if (config.buff_spell_haste)
        result.haste += 5;

    // Buff haste
    if (config.buff_haste)
        result.haste += 3;

    // Buff spell crit
    if (config.buff_spell_crit)
        result.crit += 5;

    // Debuff spell crit
    if (config.debuff_spell_crit)
        result.crit += 5;

    // Debuff spell hit
    if (config.debuff_spell_hit)
        result.hit += 3 * 26.23;

    // Debuff crit
    if (config.debuff_crit)
        result.crit += 3;

    // Molten Armor
    if (config.molten_armor)
    {
        double multiplier = 0.35;
        if (glyphs.molten_armor)
            multiplier += 0.2;
        if (result.t9_2set)
            multiplier += 0.15;
        crit_rating += result.spirit * multiplier;
    }

    // Ember Skyflare?
    if (result.meta_gem == wow_data::ids::META_EMBER_SKYFLARE)
        result.intellect *= 1.02;

    // Mind mastery, deliberately applied later, after intellect bonuses
    if (talents.mind_mastery > 0)
        result.spell_power += round(result.intellect + talents.mind_mastery * 0.03);

    result.intellect = round(result.intellect);
    result.spirit = round(result.spirit);

    // Heroic Presence
    if (config.race == RACE_DRAENEI || config.race == RACE_GNOME || config.race == RACE_HUMAN)
        result.hit += 26.23;

    result.crit += talents.arcane_instability;
    result.crit += talents.pyromaniac;

    if (talents.precision > 0)
        result.hit += 26.23 * talents.precision;

    result.crit += result.intellect / 166.6667;
    result.crit += crit_rating / 45.91;

    result.hit /= 26.23;

    return result;
}