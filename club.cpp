#include <map>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "util.hpp"
#include "club.hpp"

void Club::logEvent(int time, int eventID, std::string message) {
    logBook.emplace_back(timeFormatted(time) + " " + std::to_string(eventID) + " " +
                       message);
}

void Club::utilAddClientToWaiting(const std::string &name) {
    waitingClientQueue.push(name);
    realQueueSize++;
    waitingClientSet.insert(name);
}

void Club::utilRemoveClientFromWaiting(const std::string &name) {
    if (waitingClientSet.contains(name)) {
        waitingClientSet.erase(name);
        realQueueSize--;
    }
}

void Club::utilAssignClientToTable(int time, int tableNum, const std::string &name) {
    clientTable.insert({name, tableNum});
    tables[clientTable[name] - 1].open(time);
}

void Club::utilRemoveClientFromTables(int time, const std::string &name) {
    if (clientTable.contains(name)) {
        tables[clientTable[name] - 1].close(time);
        clientTable.erase(name);
    }
}

void Club::outClientLeaves(int time, std::string name) {
    utilRemoveClientFromWaiting(name);
    logEvent(time, 11, name);
}

void Club::outFirstClientFromQueueSits(int time, int tableNum) {
    if (!waitingClientSet.empty()) {
    std::string name = waitingClientQueue.front();
    while (!waitingClientSet.contains(name)) {
        // this part removes from queue people that left the club while waiting
        // for a table
        // - better performance than trying to remove them from the middle immediately;
        // could cause problems if too many people are leaving without using a table
        // - but at this point the business has different problems to worry about
        waitingClientQueue.pop();
        name = waitingClientQueue.front();
    }
    waitingClientQueue.pop();
    utilRemoveClientFromWaiting(name);
    utilAssignClientToTable(time, tableNum, name);

    logEvent(time, 12, name + " " + std::to_string(tableNum));
  }
}

void Club::outError(int time, std::string message) { logEvent(time, 13, message); }

void Club::inClientEnters(int time, const std::string name) {
    if (waitingClientSet.contains(name)) {
        outError(time, "YouShallNotPass");
        return;
    }
    if (time < openTime) {
        outError(time, "NotOpenYet");
        return;
    }
    utilAddClientToWaiting(name);
}

void Club::inClientSitsToTable(int time, int tableNum, const std::string name) {
    if (!waitingClientSet.contains(name)) {
        outError(time, "ClientUnknown");
        return;
    }
    if (tables[tableNum - 1].isBusy()) {
        outError(time, "PlaceIsBusy");
        return;
    }
    utilRemoveClientFromWaiting(name);
    utilAssignClientToTable(time, tableNum, name);
}

void Club::inClientWaits(int time, const std::string name) {
    if (clientTable.size() < tableCount) {
        outError(time, "ICanWaitNoLonger!");
        return;
    }
    if (realQueueSize > tableCount)
    outClientLeaves(time, name);
}

void Club::inClientLeaves(int time, const std::string name) {
    if (!waitingClientSet.contains(name) && !clientTable.contains(name)) {
        outError(time, "ClientUnknown");
        return;
    }
    utilRemoveClientFromWaiting(name);
    int freeTableNum = (clientTable.contains(name)) ? clientTable[name] : 0; // starting number is 1
    utilRemoveClientFromTables(time, name);
    if (freeTableNum) outFirstClientFromQueueSits(time, freeTableNum);
}

void Club::logInEvent(const std::string &s) {
    logBook.push_back(s);
}

void Club::closeAllTablesAndPrintLogs() {
    std::cout << timeFormatted(openTime) << '\n';
    for (auto & clientWithTable : clientTable) outClientLeaves(closeTime, clientWithTable.first);
    // Note that the map itself is not cleared; 
    // this is of little importance as this method as well could have been a custom destructor

    for (auto &logEntry : logBook) {
        std::cout << logEntry << '\n';
    }

    std::cout << timeFormatted(closeTime) << '\n';
    int tableNum = 1;
    for (auto &table : tables) {
        table.close(closeTime);
        std::cout << tableNum++ << " " << hourlyRate * table.getTotalNominalUsageHours()
            << " " << timeFormatted(table.getTotalUsageTime()) << '\n';
    }
}
