#pragma once

#include <string>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>
#include "performance/PerformanceMonitor.hpp"
#include "cli/CLIParser.hpp"

namespace nlm {

// CLI options structure (forward declaration to avoid circular dependency)
struct CLIOptions;

// Output formatter class
class OutputFormatter {
public:
    OutputFormatter();
    virtual ~OutputFormatter() = default;
    
    // Format for console output
    std::string formatConsole(const std::string& content, const CLIOptions& options);
    
    // Format for JSON output
    std::string formatJSON(const std::string& content, const CLIOptions& options);
    
    // Format for YAML output
    std::string formatYAML(const std::string& content, const CLIOptions& options);
    
    // Format for human-readable output
    std::string formatHumanReadable(const std::string& content, const CLIOptions& options);
    
    // Format for progress indicators
    std::string formatProgress(double progress, const std::string& message = "");
    
    // Format for performance metrics
    std::string formatMetrics(const PerformanceMonitor& monitor);
    
    // Set output format
    void setOutputFormat(const std::string& format) { this->outputFormat = format; }
    
private:
    std::string outputFormat;
};

} // namespace nlm
