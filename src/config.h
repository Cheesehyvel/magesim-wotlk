#pragma once

#include "common.h"
#include "timing.h"
#include "interruption.h"
#include "stats.h"
#include "talents.h"
#include "wow_data.h"

#include <string>
#include <array>
#include <vector>
#include <mutex>
#include <memory>

struct Config
{
    Race race = RACE_GNOME;
    int duration = 200;
    double duration_variance = 1.5;
    int rng_seed = 0;
    bool avg_spell_dmg = false;
    bool additional_data = false;
    int targets = 1;
    int target_resistance = 0;
    int target_level = 83;
    int spell_travel_time = 500;
    int reaction_time = 300;
    bool regem = false;

    // Buffs / debuffs
    bool debuff_spell_crit = false;
    bool debuff_spell_dmg = false;
    bool debuff_spell_hit = false;
    bool debuff_crit = false;
    bool buff_spell_crit = false;
    bool buff_spell_haste = false;
    bool buff_haste = false;
    bool buff_dmg = false;
    bool totem_of_wrath = false;
    bool flametongue = false;
    bool demonic_pact = false;
    double demonic_pact_bonus = 0;
    bool mana_replenishment = false;
    bool judgement_of_wisdom = false;
    bool mage_armor = true;
    bool molten_armor = false;
    bool amplify_magic = false;
    bool prof_engineer = false;

    bool pre_mirror_image = false;
    bool pre_water_elemental = false;
    bool pre_incanters_absorption = false;
    bool pre_mana_incanters_absorption = false;

    Potion potion = POTION_NONE;
    Potion pre_potion = POTION_NONE;
    Conjured conjured = CONJURED_NONE;

    // Rotation
    Rotation rotation = ROTATION_ST_AB_AM;
    double rot_ab3_mana = 0;
    double rot_ab_no_mb_mana = 100;
    int rot_abs_ap = 0;
    int rot_mb_below_ab = 0;
    double rot_mb_mana = 0;
    bool rot_ice_lance = false;
    double rot_brain_freeze_hold = 15;
    bool maintain_imp_scorch = false;
    bool ignite_munching = false;
    bool hot_streak_cqs = false;
    double hot_streak_cqs_time = 100;
    int evo_ticks = 0;

    std::vector<Timing> timings;
    std::vector<Interruption> interruptions;
};

enum GemColor
{
    Any = 0,
    Red,
    Blue,
    Yellow,
    Purple,
    Green,
    Orange,
    Meta,
    Count
};

struct GearSet;

struct BulkSimPlan
{
    static constexpr int slots = 17;
    static constexpr int enchant_slots = 10;

    const std::array<std::vector<int>, slots> items;
    const std::array<std::vector<int>, enchant_slots> enchants;
    const std::vector<int> gems;

    // TODO: allow bulk sim across multiple glyphs, talents?

    // this is an over-estimation and does not account for invalid builds
    size_t Permutations(bool include_gems) const;

    // Removes items from consideration which are strictly worse than other
    // chosen items
    void RemoveDowngrades();

    // To be run in its own thread, produces work for consumers
    void Producer();

    // Each thread calls this to get a batch of work
    std::vector<GearSet> Consumer(int thread_id);

    BulkSimPlan() = delete;
    BulkSimPlan(const BulkSimPlan&) = delete;

    BulkSimPlan(
        size_t threads,
        const std::array<std::vector<int>, slots>& _items,
        const std::array<std::vector<int>, enchant_slots>& _enchants,   // todo, or drop
        const std::vector<int>& _gems
    );

    const wow_data::Item& GetItem(int id) const;
    const wow_data::Gem& GetGem(int id) const;

private:
    // maximum queue size per thread
    static constexpr size_t max_size_per_thread = 400000;

    const size_t threads;

    std::vector<wow_data::Item> item_cache;
    std::vector<wow_data::Gem> gem_cache;

    std::vector<GearSet> unassigned_work_queue;
    std::vector<std::vector<GearSet>> threads_work_queue;
    mutable std::unique_ptr<std::mutex[]> thread_locks;

    bool done;
    size_t total_permutations;

    void PopulateWorkQueue(GearSet& gear, int item_slot, int gem_slot);

    void SleepProducer();

    void ApplyStandardGemming(GearSet& gear);

public:
    bool Done() const;
};

struct GearSet
{
    // no gear should ever get more than four gems?
    static constexpr size_t max_gems = 4;

    using GemSlots = std::array<int, max_gems>;

    std::array<int, BulkSimPlan::slots> items;
    std::array<GemSlots, BulkSimPlan::slots> gems;

    bool IsValid(const BulkSimPlan& plan) const;

    Stats BuildStats(const Config& config, const Talents& talents, const Glyphs& glyphs, const BulkSimPlan& plan) const;
};

// All runs involve a bulk sim plan.  The original use case of simulating a single set of equipment
// is the trivial case of the bulk sim plan, with only one option in each slot.
BulkSimPlan initialize_from_json(const std::string& path, Config& config, Talents& talents, Glyphs& glyphs, size_t threads);
