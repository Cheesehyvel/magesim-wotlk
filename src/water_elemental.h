using namespace std;

namespace unit
{

    const int WATER_ELEMENTAL = 37994;

    class WaterElemental : public Unit
    {

    public:

        WaterElemental(shared_ptr<Config> _config) : Unit(_config)
        {
            base_mana = 1082;
            name = "Water Elemental";
            id = WATER_ELEMENTAL;
            duration = 45;
        }

        void setStats(Stats _stats)
        {
            stats = _stats;
            stats.haste_rating = 0;
            stats.haste = 0;
            stats.spell_power*= 0.33;
            stats.intellect = 369 + stats.intellect * 0.3;
        }

        double maxMana()
        {
            return base_mana + stats.intellect*4.95;
        }

        shared_ptr<action::Action> nextAction(shared_ptr<State> state)
        {
            shared_ptr<action::Action> action = NULL;

            action = gcdAction(state->t);
            if (action)
                return action;

            action = spellAction(make_shared<spell::Waterbolt>());

            return action;
        }

    };

}