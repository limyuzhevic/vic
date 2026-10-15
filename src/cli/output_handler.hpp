// Output handler for CLI
// Handles formatting and exporting results, status, and statistics

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"

namespace nlm {

// Output format options
enum class OutputFormat {
    PlainText,
    CSV,
    JSON,
    YAML
};

// Output handler for formatting and exporting results
class OutputHandler {
public:
    OutputHandler();
    ~OutputHandler();
    
    // Set output format
    void setFormat(OutputFormat format) { format_ = format; }
    
    // Print brain status
    void printBrainStatus(std::shared_ptr<Brain> brain) const;
    
    // Print agent status
    void printAgentStatus(std::shared_ptr<AgentBrain> agentBrain) const;
    
    // Print world status
    void printWorldStatus(std::shared_ptr<SimpleWorld> world) const;
    
    // Print simulation statistics
    void printSimulationStats(const SimulationStats& stats) const;
    
    // Export brain status to file
    bool exportBrainStatus(std::shared_ptr<Brain> brain, const std::string& filepath) const;
    
    // Export agent status to file
    bool exportAgentStatus(std::shared_ptr<AgentBrain> agentBrain, const std::string& filepath) const;
    
    // Export world status to file
    bool exportWorldStatus(std::shared_ptr<SimpleWorld> world, const std::string& filepath) const;
    
    // Export simulation statistics to file
    bool exportSimulationStats(const SimulationStats& stats, const std::string& filepath) const;
    
    // Print simulation progress
    void printProgress(size_t currentStep, size_t totalSteps, 
                      const SimulationStats& stats) const;
    
    // Print error message
    void printError(const std::string& error) const;
    
    // Print success message
    void printSuccess(const std::string& message) const;
    
    // Print warning message
    void printWarning(const std::string& message) const;
    
    // Print info message
    void printInfo(const std::string& message) const;
    
private:
    OutputFormat format_;
    
    // Format as plain text
    std::string formatAsText(const std::string& content) const;
    
    // Format as CSV
    std::string formatAsCSV(const std::vector<std::vector<std::string>>& data) const;
    
    // Format as JSON
    std::string formatAsJSON(const std::string& content) const;
    
    // Format as YAML
    std::string formatAsYAML(const std::string& content) const;
    
    // Escape string for different formats
    std::string escapeForJSON(const std::string& str) const;
    std::string escapeForYAML(const std::string& str) const;
    
    // Get timestamp for output
    std::string getTimestamp() const;
};

} // namespace nlm