#ifndef LOGWATCHER_H
#define LOGWATCHER_H

#include <functional>
#include <string>

void watchLogFile(const std::string& filename, const std::function<void(const std::string&)>& onLine);

#endif
