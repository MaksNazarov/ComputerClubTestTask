#include "util.hpp"

int timeInMinutes(int hour, int minute) {
    return hour * 60 + minute;
}

std::string timeFormatted(int time) {
    int hour = time / 60;
    int minute = time % 60;
    return ((hour < 10) ? "0" : "") + std::to_string(hour) + ":" +
            ((minute < 10) ? "0" : "") + std::to_string(minute);
}