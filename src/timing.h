#pragma once

#include <string>

struct Timing
{
    std::string name = "";
    double t = 0;
    double wait_t = 0;
    int wait_for_buff = 0;

    Timing() {}
    Timing(const std::string& _name, double _t, double _wait_t, int _wait_for_buff) :
        name(_name), t(_t), wait_t(_wait_t), wait_for_buff(_wait_for_buff) {}
};