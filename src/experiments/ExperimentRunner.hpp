#pragma once

#include "ExperimentEnhanced.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <unordered_map>

namespace nlm {

// Export formats for experiment results
enum class ExportFormat {
    TXT,
    CSV,
    JSON,
    YAML,
    BIN
};

// Experiment result containing all data and metadata
struct ExperimentResult {
    std::string experimentName;
    ExperimentType experimentType;
    double executionTime; // seconds
    std::string timestamp;
    std::unique_ptr<Experiment> experiment;
    std::map<std::string, std::string> metadata;
    bool success;
    std::string errorMessage;
    
    // Export results
    bool exportToFile(const std::string& filepath, ExportFormat format) const;
};

// Unified experiment runner with CLI interface
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Run a single experiment
    ExperimentResult runExperiment(const std::string& experimentType, 
                                   const std::string& experimentName = "",
                                   const std::map<std::string, ExperimentParameter>& config = {});
    
    // Run experiment from command line arguments
    ExperimentResult runFromArgs(int argc, char** argv);
    
    // Get available experiment types
    std::vector<ExperimentType> getAvailableExperiments() const;
    
    // Get experiment description
    const std::string& getExperimentDescription(const std::string& experimentType) const;
    
    // Get default configuration for experiment type
    std::map<std::string, ExperimentParameter>> getDefaultConfig(const std::string& experimentType) const;
    
    // Parse command line arguments
    struct ParsedArgs {
        std::string experimentType;
        std::string experimentName;
        std::map<std::string, ExperimentParameter> config;
        bool help = false;
        bool verbose = false;
        std::string outputFormat;
        std::string outputFile;
    };
    
    ParsedArgs parseArgs(int argc, char** argv) const;
    
    // Print help information
    void printHelp() const;
    void printExperimentInfo(const std::string& experimentType) const;
    
    // Configuration validation
    bool validateConfig(const std::string& experimentType, const std::map<std::string, ExperimentParameter>& config) const;
    
    // Experiment parameter parsing utilities
    static ExperimentParameter parseParameter(const std::string& value);
    static std::string parameterToString(const ExperimentParameter& param);
    
    // Experiment parameter schema definitions
    struct ParameterSchema {
        std::string name;
        std::string type; // "int", "double", "string", "bool"
        std::string description;
        ExperimentParameter defaultValue;
        bool required = false;
        std::vector<ExperimentParameter> allowedValues;
    };
    
    // Get parameter schema for experiment type
    std::vector<ParameterSchema> getParameterSchema(const std::string& experimentType) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global experiment runner instance
ExperimentRunner& getExperimentRunner();

} // namespace nlm
