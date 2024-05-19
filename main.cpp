#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>

#include "club.hpp"
#include "util.hpp"

class parseError : public std::runtime_error {
public:
    explicit parseError(const std::string &what_arg)
        : std::runtime_error(what_arg){};
};

bool isClientNameAppropriate(const std::string & name) {
    for (auto c : name) {
        if (!(c >= 'a' && c <= 'z')
            && !(c >= '0' && c <= '9')
            && !(c == '_')
            && !(c == '-')) return false;
    }
    return true;
}

int parseEventString(Club & club, const std::string & eventString) {
    std::istringstream iss(eventString);

    int hour, minute, eventID, tableNum;
    char timeSeparator;
    std::string clientName;

    if (!(iss >> hour >> timeSeparator >> minute >> eventID >> clientName)
        || (timeSeparator != ':')
        || (!(hour >= 0 && hour <= 24 && minute >= 0 && minute <= 59))
        || (eventID == 2 && !(iss >> tableNum))
        || !isClientNameAppropriate(clientName)
        || !iss.eof()) {
        throw parseError(eventString);
    }

    club.logInEvent(eventString);
    int time = timeInMinutes(hour, minute);
    switch(eventID) {
        case 1: 
            club.inClientEnters(time, clientName);
            break;
        case 2:
            club.inClientSitsToTable(time, tableNum, clientName);
            break;
        case 3:
            club.inClientWaits(time, clientName);
            break;
        case 4:
            club.inClientLeaves(time, clientName);
            break;
        default:
            throw parseError(eventString);
    }
    return 0;
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Error: incorrect input format; this program has a single filename as an argument\n";
    }
    std::ifstream file(argv[1]);

    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << argv[1] << '\n';
        return 1;
    }

    int tableCount;
    int openHour, openMinute, closeHour, closeMinute;
    char timeSeparator1, timeSeparator2, lineSeparator;
    int hourlyRate;
    try {
        std::string line1, line2, line3;
        getline(file, line1); // first line with table count
        getline(file, line2); // second line with business hours
        getline(file, line3); // third line with hourly rate

        std::istringstream iss(line1 + " | " + line2 + " | " + line3); 
        // using '|' crutches as I had trouble setting up custom_locale 
        // to limit allowed separator symbols for istringstream

        if (!(iss >> tableCount >> lineSeparator) || lineSeparator != '|') throw parseError(line1);
        if (!(iss >> openHour >> timeSeparator1 >> openMinute 
            >> closeHour >> timeSeparator2 >> closeMinute >> lineSeparator)
            || (!(openHour >= 0 && openHour <= 24 && openMinute >= 0 && openMinute <= 59))
            || (!(closeHour >= 0 && closeHour <= 24 && closeMinute >= 0 && closeMinute <= 59))
            || lineSeparator != '|'
            || timeSeparator1 != ':'
            || timeSeparator2 != ':') {
            throw parseError(line2);
        }
        if (!(iss >> hourlyRate) || !iss.eof()) throw parseError(line3);

        Club club(
            timeInMinutes(openHour, openMinute),
            timeInMinutes(closeHour, closeMinute),
            hourlyRate,
            tableCount
        );

        std::string line;
        while (getline(file, line)) {
            parseEventString(club, line);
        }

        file.close();

        club.closeAllTablesAndPrintLogs();
    } catch (parseError & pe) {
        // cout << "Error while parsing!\n";
        std::cout << pe.what() << '\n';
    }
}