#include "../include/SentinelVault.h"
#include "../include/LogWatcher.h"
#include "../include/VaultHeader.h"
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace {
#pragma pack(push, 1)
struct VaultRecord { long long timestamp; int severity; int threatScore; int blacklisted; char ip[64]; char action[64]; };
#pragma pack(pop)
}

SentinelVault::SentinelVault() : blacklist(nullptr) { cout << "[SYSTEM] SentinelVault initialized and secured." << endl; }

int SentinelVault::calculateThreatScore(const LogEntry& entry) const {
    int frequency = 0;
    for (const auto& log : logs) if (log.ipAddress == entry.ipAddress) ++frequency;
    time_t rawTime = static_cast<time_t>(entry.timestamp);
    tm* utc = gmtime(&rawTime);
    const int hourBonus = utc && utc->tm_hour >= 2 && utc->tm_hour <= 5 ? 25 : 0;
    int typeWeight = 5;
    if (entry.action.find("UNAUTHORIZED") != string::npos || entry.action.find("BRUTE") != string::npos) typeWeight = 45;
    else if (entry.action.find("FAILED") != string::npos || entry.action.find("SCAN") != string::npos) typeWeight = 30;
    else if (entry.action.find("LOGIN") != string::npos) typeWeight = 15;
    return min(100, frequency * 10 + hourBonus + typeWeight + entry.severity * 3);
}

bool SentinelVault::ingestLogLine(const string& line) {
    try {
        stringstream ss(line); string temp; LogEntry entry{};
        getline(ss, temp, ','); entry.timestamp = stoll(temp);
        getline(ss, entry.ipAddress, ','); getline(ss, entry.action, ',');
        getline(ss, temp, ','); entry.severity = stoi(temp);
        entry.threatScore = calculateThreatScore(entry);
        logs.push_back(entry);
        return true;
    } catch (const exception&) { cerr << "Skipping malformed log line: " << line << endl; return false; }
}

void SentinelVault::addToBlacklist(string ip) {
    int score = 0;
    for (auto& log : logs) if (log.ipAddress == ip) { score = max(score, calculateThreatScore(log)); log.blacklisted = true; log.threatScore = max(log.threatScore, score); }
    blacklist = insert(blacklist, ip, score);
    cout << "[+] Surveillance active for IP: " << ip << endl;
}

void SentinelVault::loadLogsFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) { cerr << "Error opening file: " << filename << endl; return; }
    string line; while (getline(file, line)) ingestLogLine(line);
    cout << logs.size() << " records are ingested into the secure vault." << endl;
}

void SentinelVault::watchLogs(const string& filename, const string& vaultFilename) {
    watchLogFile(filename, [this, &vaultFilename](const string& line) {
        if (ingestLogLine(line)) { saveToBinaryVault(vaultFilename); cout << "[WATCHER] Ingested new event." << endl; }
    });
}

void SentinelVault::performSort() {
    if (logs.empty()) { cerr << "No logs to sort." << endl; return; }
    mergeSort(logs, 0, logs.size() - 1); cout << "Logs have been sorted by timestamp." << endl;
}

void SentinelVault::runSecurityScan() {
    cout << "Running security scan on the logs..." << endl;
    for (auto& log : logs) {
        if (isblacklisted(blacklist, log.ipAddress)) { log.blacklisted = true; log.threatScore = max(log.threatScore, getThreatScore(blacklist, log.ipAddress)); cout << "ALERT: Suspicious activity detected from IP: " << log.ipAddress << endl; }
        log.printLog();
    }
    cout << "---------------------------------------------------------------------------" << endl;
}

void SentinelVault::saveToBinaryVault(string filename) {
    FILE* primary = fopen(filename.c_str(), "wb"); FILE* mirror = fopen((filename + ".bak").c_str(), "wb");
    if (!primary || !mirror) throw runtime_error("RAID Failure: Could not access storage for mirroring.");
    VaultHeader header{}; memcpy(header.magic, "SVLT001", 8); header.version = 1; header.recordCount = static_cast<unsigned int>(logs.size()); header.lastSyncTime = static_cast<long long>(time(nullptr));
    fwrite(&header, sizeof(header), 1, primary); fwrite(&header, sizeof(header), 1, mirror);
    for (const auto& log : logs) {
        VaultRecord record{}; record.timestamp = log.timestamp; record.severity = log.severity; record.threatScore = log.threatScore; record.blacklisted = log.blacklisted ? 1 : 0;
        strncpy(record.ip, log.ipAddress.c_str(), sizeof(record.ip) - 1); strncpy(record.action, log.action.c_str(), sizeof(record.action) - 1);
        fwrite(&record, sizeof(record), 1, primary); fwrite(&record, sizeof(record), 1, mirror);
    }
    fclose(primary); fclose(mirror); cout << "[RAID 1] Vault locked and mirrored to " << filename << ".bak" << endl;
}

void SentinelVault::loadFromBinaryVault(string filename) {
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) { cout << "[!] PRIMARY VAULT CORRUPTED. Attempting RAID Recovery..." << endl; file = fopen((filename + ".bak").c_str(), "rb"); if (!file) throw runtime_error("CRITICAL SYSTEM FAILURE: All vaults lost."); }
    VaultHeader header{};
    if (fread(&header, sizeof(header), 1, file) != 1 || strncmp(header.magic, "SVLT001", 7) != 0) { fclose(file); throw runtime_error("Invalid Sentinel-Vault binary format."); }
    VaultRecord record{};
    while (fread(&record, sizeof(record), 1, file)) { LogEntry temp{}; temp.timestamp = record.timestamp; temp.severity = record.severity; temp.threatScore = record.threatScore; temp.blacklisted = record.blacklisted != 0; temp.ipAddress = record.ip; temp.action = record.action; logs.push_back(temp); }
    fclose(file);
}
