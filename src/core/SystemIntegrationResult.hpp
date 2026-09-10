#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

// System integration verification result structure
struct SystemIntegrationResult {
    bool overallStatus = false;
    bool memorySystemStatus = false;
    bool cognitiveSystemStatus = false;
    bool neuromodulationSystemStatus = false;
    bool performanceInfrastructureStatus = false;
    bool sleepSystemStatus = false;
    bool developmentSystemStatus = false;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, double> metrics;
    
    SystemIntegrationResult() = default;
    
    // Convert to string summary
    std::string summary() const {
        std::string result = "System Integration Verification Result:\n";
        result += "Overall Status: " + std::string(overallStatus ? "PASS" : "FAIL") + "\n";
        result += "Memory System: " + std::string(memorySystemStatus ? "PASS" : "FAIL") + "\n";
        result += "Cognitive System: " + std::string(cognitiveSystemStatus ? "PASS" : "FAIL") + "\n";
        result += "Neuromodulation System: " + std::string(neuromodulationSystemStatus ? "PASS" : "FAIL") + "\n";
        result += "Performance Infrastructure: " + std::string(performanceInfrastructureStatus ? "PASS" : "FAIL") + "\n";
        result += "Sleep System: " + std::string(sleepSystemStatus ? "PASS" : "FAIL") + "\n";
        result += "Development System: " + std::string(developmentSystemStatus ? "PASS" : "FAIL") + "\n";
        
        if (!errors.empty()) {
            result += "\nErrors:\n";
            for (const auto& error : errors) {
                result += "  - " + error + "\n";
            }
        }
        
        if (!warnings.empty()) {
            result += "\nWarnings:\n";
            for (const auto& warning : warnings) {
                result += "  - " + warning + "\n";
            }
        }
        
        if (!metrics.empty()) {
            result += "\nMetrics:\n";
            for (const auto& metric : metrics) {
                result += "  - " + metric.first + ": " + std::to_string(metric.second) + "\n";
            }
        }
        
        return result;
    }
};

} // namespace nlm
