#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <string>
#include <iostream>

using namespace std;

struct LogEntry{

    long long timestamp;
    string ipAddress;
    string action;
    int severity;
    int threatScore = 0;
    bool blacklisted = false;

    void printLog() const {
        cout << "[" << timestamp << "] "
             << "IPADRESS: " << ipAddress << " "
             << "ACTION: " << action << " "
             << "SEVERITY: " << severity << " "
             << "THREAT SCORE: " << threatScore << endl;
    }
};

#endif
