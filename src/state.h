using namespace std;

class State
{

public:
    double t;
    unsigned long long dmg;
    double duration;
    double ignite_dmg;
    double ignite_t;

    map<spell::ID, SpellStats> spells;
    map<debuff::ID, shared_ptr<debuff::Debuff>> debuffs;
    list<shared_ptr<unit::Unit>> units;

    shared_ptr<Config> config;

    State(shared_ptr<Config> _config)
    {
        config = _config;
        reset();
    }

    void reset()
    {
        t = 0;
        dmg = 0;
        duration = config->duration;
        duration+= -config->duration_variance + random<double>(0, config->duration_variance*2);

        ignite_dmg = 0;
        ignite_t = -20;

        spells.clear();
        debuffs.clear();
        units.clear();
    }

    bool inCombat()
    {
        return t >= 0;
    }

    double dps()
    {
        if (t == 0)
            return 0;

        return dmg / t;
    }

    int debuffStacks(debuff::ID id)
    {
        if (hasDebuff(id))
            return debuffs[id]->stacks;
        return 0;
    }

    bool hasDebuff(debuff::ID id)
    {
        return debuffs.find(id) != debuffs.end();
    }

    int addDebuff(shared_ptr<debuff::Debuff> debuff)
    {
        if (hasDebuff(debuff->id))
            return debuffs[debuff->id]->addStack();
        else
            debuffs[debuff->id] = debuff;

        return 1;
    }

    void removeDebuff(debuff::ID id)
    {
        debuffs.erase(id);
    }

    bool hasUnit(int id)
    {
        for (auto itr = units.begin(); itr != units.end(); itr++) {
            if ((*itr)->id == id)
                return true;
        }

        return false;
    }

    void addUnit(shared_ptr<unit::Unit> unit)
    {
        units.push_back(unit);
    }

    void removeUnit(shared_ptr<unit::Unit> unit)
    {
        for (auto itr = units.begin(); itr != units.end(); itr++) {
            if ((*itr) == unit) {
                units.erase(itr);
                break;
            }
        }
    }

    void removeUnit(int id)
    {
        for (auto itr = units.begin(); itr != units.end(); itr++) {
            if ((*itr)->id == id) {
                units.erase(itr);
                break;
            }
        }
    }

    double timeRemain()
    {
        return duration - t;
    }

};
