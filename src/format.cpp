#include <string>
#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    auto tmp_minutes = (seconds / 60) % 60;
    string minutes = (tmp_minutes < 10) ? "0" + std::to_string(tmp_minutes) : std::to_string(tmp_minutes);
    string hours = std::to_string((seconds / 60) / 60);
    string secs = (seconds % 60 < 10) ? "0" + std::to_string(seconds % 60) : std::to_string(seconds % 60);
    string result = hours + ":" + minutes + ":" + secs;
    return result;
}