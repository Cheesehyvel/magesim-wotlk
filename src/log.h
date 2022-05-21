using namespace std;

enum LogType : int
{
    LOG_NONE,
    LOG_CAST_START,
    LOG_CAST_SUCCESS,
    LOG_SPELL_IMPACT,
    LOG_MANA,
    LOG_BUFF,
    LOG_DOT,
    LOG_DEBUG,
    LOG_WAIT,
    LOG_UNIT,
};

struct LogEntry
{
    LogType type;
    shared_ptr<unit::Unit> unit;
    string text;
    double t;
    double mana;
    double mana_percent;
    int dmg;
};