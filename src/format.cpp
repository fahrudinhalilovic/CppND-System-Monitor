#include "format.h"

#include <string>
#include <sstream>
#include <iomanip>

std::string Format::ElapsedTime(long seconds) {
    constexpr auto secondsInMin = 60u;
    constexpr auto secondsInHour = 60u * secondsInMin;
    const auto h = seconds / secondsInHour;
    const auto min = (seconds % secondsInHour) / secondsInMin;
    const auto sec = seconds % 60;

    std::stringstream sstream;
    sstream << std::setw(2) << std::setfill('0') << h << ":" << std::setw(2) << std::setfill('0') << min
            << ":" << std::setw(2) << std::setfill('0') << sec;

    return sstream.str();
}
