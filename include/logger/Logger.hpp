#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>

namespace opencmd {
    class Logger {
    public:
        enum class Level {
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            CRITICAL
        };

        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }

        void setLevel(Level severity_){
            severity = static_cast<unsigned int>(severity_);
        }
        
        void log(const std::string&, Level);

    private:
        Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;    
        unsigned int severity = static_cast<unsigned int>(Level::DEBUG);
    };

}