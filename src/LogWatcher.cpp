#include "../include/LogWatcher.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef __linux__
#include <sys/inotify.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

void watchLogFile(const std::string& filename, const std::function<void(const std::string&)>& onLine) {
    std::ifstream logFile(filename);
    logFile.seekg(0, std::ios::end);
#ifdef __linux__
    const std::size_t slash = filename.find_last_of('/');
    const std::string directory = slash == std::string::npos ? "." : filename.substr(0, slash);
    const std::string basename = slash == std::string::npos ? filename : filename.substr(slash + 1);
    const int fd = inotify_init1(0);
    if (fd < 0) throw std::runtime_error("Unable to initialize inotify watcher.");
    if (inotify_add_watch(fd, directory.c_str(), IN_MODIFY | IN_CLOSE_WRITE) < 0) throw std::runtime_error("Unable to watch log directory.");
    std::cout << "[WATCHER] Tailing " << filename << std::endl;
    char buffer[4096];
    while (true) {
        const int length = read(fd, buffer, sizeof(buffer));
        for (int offset = 0; offset < length;) {
            auto* event = reinterpret_cast<inotify_event*>(buffer + offset);
            if (event->len && basename == event->name && (event->mask & (IN_MODIFY | IN_CLOSE_WRITE))) {
                std::string line;
                while (std::getline(logFile, line)) if (!line.empty()) onLine(line);
                logFile.clear();
            }
            offset += sizeof(inotify_event) + event->len;
        }
    }
#else
    std::cout << "[WATCHER] inotify is only available on Linux; polling " << filename << std::endl;
    while (true) {
        std::string line;
        while (std::getline(logFile, line)) if (!line.empty()) onLine(line);
        logFile.clear();
        Sleep(1000);
    }
#endif
}
