// nlm_config_generator.cpp - Implementation of ConfigGenerator

#include "core/ConfigGenerator.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {
    // ConfigGenerator implementation is in the header (inline)
    // This file exists for explicit linking and to avoid duplicate symbols
    
    // Additional utility functions
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
}