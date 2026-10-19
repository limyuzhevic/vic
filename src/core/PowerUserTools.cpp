// nlm_power_user.cpp - Implementation of PowerUserTools

#include "core/PowerUserTools.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {
    // PowerUserTools implementation is in the header (inline)
    // This file exists for explicit linking and to avoid duplicate symbols
    
    // Additional utility functions
    
    std::string generateJobId() {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << "job_" << now_time_t << "_" 
           << std::chrono::duration_cast<std::chrono::microseconds>(
               now.time_since_epoch()).count() % 1000000;
        
        return ss.str();
    }
}