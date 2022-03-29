using namespace std;

namespace unit
{

    const int MIRROR_IMAGE = 31216;

    class MirrorImage : public Unit
    {

    public:

        MirrorImage(shared_ptr<Config> _config) : Unit(_config)
        {
            base_mana = 3000; // Unknown
            name = "Mirror Image";
            id = MIRROR_IMAGE;
            duration = 30;
            get_raid_buffs = false;
            unique = false;
        }

        void setStats(Stats _stats)
        {
            stats = _stats;
            stats.haste_rating = 0;
            stats.haste = 0;
            stats.spell_power*= 0.33;
        }

        std::list<shared_ptr<action::Action>> onCastSuccessProc(shared_ptr<State> state, shared_ptr<spell::Spell> spell)
        {
            std::list<shared_ptr<action::Action>> actions = Unit::onCastSuccessProc(state, spell);

            // Cooldowns
            if (spell->id == spell::MIRROR_FIRE_BLAST)
                actions.push_back(cooldownAction(make_shared<cooldown::FireBlast>()));

            return actions;
        }

        shared_ptr<action::Action> nextAction(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            if (!hasCooldown(cooldown::FIRE_BLAST) && random<int>(0, 2) == 0)
                action = spellAction(make_shared<spell::MirrorFireBlast>());
            else
                action = spellAction(make_shared<spell::MirrorFrostbolt>());

            return action;
        }

    };

}