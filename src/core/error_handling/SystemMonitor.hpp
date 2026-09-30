// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Cross-system error correlation and monitoring
 */
class SystemMonitor {
public:
    struct SystemState {
        std::string component;
        bool healthy;
        double healthScore;
        std::vector<std::string> activeErrors;
        std::map<std::string, std::string> metrics;
        Timestamp timestamp;
    };
    
    static void updateSystemState(const std::string& component, bool healthy, 
                                 const std::vector<std::string>& errors = {});
    static SystemState getSystemState(const std::string& component);
    static std::vector<SystemState> getAllSystemStates();
    static void correlateErrors(const std::string& error1, const std::string& error2);
    static std::vector<std::string> getCorrelatedErrors(const std::string& error);
    static void generateHealthReport();
};

} // namespace nlm
