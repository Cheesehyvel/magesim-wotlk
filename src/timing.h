class Timing
{
public:
    Timing(){}
    std::string name;
    double t;
    double wait_t = 0;
    bool used = false;
    int wait_for_buff = 0;
};