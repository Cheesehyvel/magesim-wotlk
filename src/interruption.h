#pragma once

struct Interruption
{
    bool silence = false;
    bool affects_all = false;
    double t = 0;
    double duration = 0;
};