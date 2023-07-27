#pragma once

#include <string>

namespace debuff
{
    enum ID : int
    {
        IMPROVED_SCORCH = 22959,
        WINTERS_CHILL = 12579,
        HODIR_SINGED = 65280,
        ICEHOWL_STAGGERED_DAZE = 66758,
    };

    // Prolonged ownership by Event, State and Unit. TODO: Do Event and Unit really need the same copy (i.e. pointer)?
    struct Debuff
    {
        ID id;
        std::string name = "";
        double duration = 0;
        int stacks = 1;
        int max_stacks = 1;
        bool hidden = false;

        int addStack()
        {
            if (stacks < max_stacks)
                ++stacks;
            else
                return 0;

            return stacks;
        }

    };

    class ImprovedScorch : public Debuff
    {
    public:
        ImprovedScorch()
        {
            id = IMPROVED_SCORCH;
            name = "Improved Scorch";
            duration = 30;
        }
    };

    class WintersChill : public Debuff
    {
    public:
        WintersChill()
        {
            id = WINTERS_CHILL;
            name = "Winter's Chill";
            duration = 15;
            max_stacks = 5;
        }
    };

    class HodirSinged : public Debuff
    {
    public:
        HodirSinged()
        {
            id = HODIR_SINGED;
            name = "Singed";
            duration = 3000;
        }
    };

    class IcehowlStaggeredDaze : public Debuff
    {
    public:
        IcehowlStaggeredDaze()
        {
            id = ICEHOWL_STAGGERED_DAZE;
            name = "Staggered Daze";
            duration = 15;
        }
    };

}