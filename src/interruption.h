class Interruption
{
public:
    Interruption(){}
    bool silence = false;
    bool affects_all = false;
    double t;
    double duration;
};