#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <iostream>
#include <stdexcept>

std::string timeFormatted(int time);

class Table {
    bool busy = false;
    int timeWhenOpened = 0;
    int totalUsageTime = 0;
    int totalNominalUsageHours = 0;

    public:
        void open(int currentTime) {
            if (busy) return;
            busy = true;
            timeWhenOpened = currentTime;
        }

        void close(int currentTime) {
            if (!busy) return;
            int timeUsed = currentTime - timeWhenOpened;
            busy = false;
            timeWhenOpened = 0;
            totalUsageTime += timeUsed;
            totalNominalUsageHours +=  timeUsed / 60 + (timeUsed % 60 ? 1 : 0);
        }

        bool isBusy() {
            return busy;
        }

        int getTotalUsageTime() {
            return totalUsageTime;
        }
        int getTotalNominalUsageHours() {
            return totalNominalUsageHours;
        }
};

class Club {
    std::vector<Table> tables;
    std::vector<std::string> logBook;

    int hourlyRate;
    int openTime;
    int closeTime;

    int tableCount;
    int realQueueSize;

    // map<int, string> clientNames;
    std::set<std::string> waitingClientSet; // to check whether client is inside
    std::queue<std::string> waitingClientQueue; // to choose client to lead to an empty table
    std::map<std::string, int> clientTable; // to get a number of table client is occupying

    void logEvent(int time, int eventID, const std::string &message);

    // Below are 4 utility functions used for adding/removing client from waiting or busy client categories
    void utilAddClientToWaiting(const std::string & name);

    void utilRemoveClientFromWaiting(const std::string & name);

    void utilAssignClientToTable(int time, int tableNum, const std::string & name);

    void utilRemoveClientFromTables(int time, const std::string & name);

    // Out events - events that are fired as a responce to incoming events

    // fires at the end of workday or when queue is full and another client is trying to enter
    void outClientLeaves(int time, const std::string &name); 

    // fires when a table becomes empty
    void outFirstClientFromQueueSits(int time, int tableNum);

    // fires when an error of some kind is encountered
    void outError(int time, const std::string & message); 
public:
    Club(int openTime_, int closeTime_, int hourlyRate_, int tableCount_) : 
        openTime(openTime_), closeTime(closeTime_), hourlyRate(hourlyRate_), tableCount(tableCount_) {
        tables.resize(tableCount);
        realQueueSize = 0;
    }
    // Incoming event-handling functions
    void inClientEnters(int time, const std::string & name);

    void inClientSitsToTable(int time, int tableNum, const std::string & name);

    void inClientWaits(int time, const std::string & name);

    void inClientLeaves(int time, const std::string & name);

    // simple method to log input entries after they pass parser checks
    void logInEvent(const std::string & s);

    // method used after fully parsing input file to print full log of day events & income&time used per table
    void closeAllTablesAndPrintLogs();
};
