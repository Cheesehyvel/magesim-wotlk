#pragma once

#include <string>

struct Timing
{
    std::string name = "";
    double t = 0;
    double wait_t = 0;
    int wait_for_buff = 0;
    bool used = false;

    Timing() {}
    void use() { used = true; }
};