using namespace std;

class State
{

public:
    double t;
    int dmg;
    int innervates;
    double duration;
    double evocated_at = -1;

    map<spell::ID, SpellStats> spells;
    map<debuff::ID, shared_ptr<debuff::Debuff>> debuffs;

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
        innervates = config->innervate;
        duration = config->duration;
        duration+= -config->duration_variance + random<double>(0, config->duration_variance*2);

        evocated_at = -1;
        spells.clear();
        debuffs.clear();
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

    double timeRemain()
    {
        return duration - t;
    }

};
