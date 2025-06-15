#pragma once

#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>

class Auditable {
protected:
    std::ofstream logFile;
    
    void logAction(const std::string& action) {
        if (!logFile.is_open()) {
            return;
        }
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        logFile << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                << " - " << action << std::endl;
    }

public:
    Auditable(const std::string& logFileName) {
        logFile.open(logFileName, std::ios::app);
    }
    
    virtual ~Auditable() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
}; 