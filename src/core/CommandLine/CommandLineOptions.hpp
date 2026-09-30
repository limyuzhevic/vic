// Advanced command-line option definitions for NLM
// Groups options by functionality: performance, debugging, experiments, validation, output, neural, memory, checkpoint

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <unordered_set>
#include "core/CommandLine/CommandLineParser.hpp"

namespace nlm {

// Command-line option categories
enum class OptionCategory {
    Performance,
    Debugging,
    Experiment,
    Validation,
    Output,
    Neural,
    Memory,
    Checkpoint,
    General
};

// Option metadata for help generation and validation
struct OptionMetadata {
    std::string longName;
    std::string shortName;
    std::string description;
    std::string type;
    OptionCategory category;
    bool required;
    std::string defaultValue;
    std::string helpText;
    std::function<bool(const std::string&)> validator;
    std::function<void(const std::string&, Config&)> applier;
    
    OptionMetadata(const std::string& ln = "", const std::string& sn = "", 
                   const std::string& desc = "", const std::string& t = "string",
                   OptionCategory cat = OptionCategory::General, bool req = false,
                   const std::string& def = "", const std::string& help = "",
                   const std::function<bool(const std::string&)> &val = nullptr,
                   const std::function<void(const std::string&, Config&)> &appl = nullptr)
        : longName(ln), shortName(sn), description(desc), type(t), category(cat),
          required(req), defaultValue(def), helpText(help), validator(val), applier(appl) {}
};

// Advanced command-line options configuration
class CommandLineOptions {
public:
    CommandLineOptions();
    ~CommandLineOptions();
    
    // Register performance options
    void registerPerformanceOptions();
    
    // Register debugging options
    void registerDebuggingOptions();
    
    // Register experiment options
    void registerExperimentOptions();
    
    // Register validation options
    void registerValidationOptions();
    
    // Register output options
    void registerOutputOptions();
    
    // Register neural network options
    void registerNeuralOptions();
    
    // Register memory management options
    void registerMemoryOptions();
    
    // Register checkpoint options
    void registerCheckpointOptions();
    
    // Get option metadata by long name
    const OptionMetadata* getOption(const std::string& longName) const;
    
    // Get option metadata by short name
    const OptionMetadata* getOptionByShort(const std::string& shortName) const;
    
    // Get all options in a specific category
    std::vector<const OptionMetadata*> getOptionsInCategory(OptionCategory category) const;
    
    // Get all option long names
    std::vector<std::string> getAllLongNames() const;
    
    // Get all option short names
    std::vector<std::string> getAllShortNames() const;
    
    // Display help organized by category
    void displayHelpByCategory() const;
    
    // Get categories with available options
    std::vector<OptionCategory> getActiveCategories() const;
    
    // Initialize all options
    void initializeAllOptions();
    
    // Get number of registered options
    size_t size() const { return options.size(); }
    
private:
    std::map<std::string, OptionMetadata> options; // Keyed by long name
    std::unordered_set<OptionCategory> activeCategories;
    
    // Performance option callbacks
    static bool validatePerformanceMode(const std::string& value);
    static void applyPerformanceMode(const std::string& value, Config& config);
    static bool validateThreads(const std::string& value);
    static void applyThreads(const std::string& value, Config& config);
    static bool validateMemoryPoolSize(const std::string& value);
    static void applyMemoryPoolSize(const std::string& value, Config& config);
    
    // Debug option callbacks
    static bool validateDebugLevel(const std::string& value);
    static void applyDebugLevel(const std::string& value, Config& config);
    static void applyDebugNeurons(const std::string& value, Config& config);
    static void applyDebugSynapses(const std::string& value, Config& config);
    static void applyDebugPlasticity(const std::string& value, Config& config);
    static void applyDebugValidation(const std::string& value, Config& config);
    
    // Experiment option callbacks
    static bool validateExperimentMode(const std::string& value);
    static void applyExperimentMode(const std::string& value, Config& config);
    static bool validateObservationInterval(const std::string& value);
    static void applyObservationInterval(const std::string& value, Config& config);
    static bool validateRewardMode(const std::string& value);
    static void applyRewardMode(const std::string& value, Config& config);
    
    // Validation option callbacks
    static bool validateValidationThreshold(const std::string& value);
    static void applyValidationThreshold(const std::string& value, Config& config);
    
    // Output option callbacks
    static bool validateOutputFormat(const std::string& value);
    static void applyOutputFormat(const std::string& value, Config& config);
    static bool validateOutputInterval(const std::string& value);
    static void applyOutputInterval(const std::string& value, Config& config);
    
    // Neural option callbacks
    static bool validateNeuronType(const std::string& value);
    static void applyNeuronType(const std::string& value, Config& config);
    static bool validateSynapseType(const std::string& value);
    static void applySynapseType(const std::string& value, Config& config);
    static bool validateConnectivityDensity(const std::string& value);
    static void applyConnectivityDensity(const std::string& value, Config& config);
    static bool validateDelayRange(const std::string& value);
    static void applyDelayRange(const std::string& value, Config& config);
    static bool validateWeightRange(const std::string& value);
    static void applyWeightRange(const std::string& value, Config& config);
    
    // Memory option callbacks
    static bool validateMemoryOptimization(const std::string& value);
    static void applyMemoryOptimization(const std::string& value, Config& config);
    static bool validateMemoryPoolSize(const std::string& value);
    static void applyMemoryPoolSizeConfig(const std::string& value, Config& config);
    static bool validateMmapThreshold(const std::string& value);
    static void applyMmapThreshold(const std::string& value, Config& config);
    
    // Checkpoint option callbacks
    static bool validateCheckpointInterval(const std::string& value);
    static void applyCheckpointInterval(const std::string& value, Config& config);
    static bool validateCheckpointFormat(const std::string& value);
    static void applyCheckpointFormat(const std::string& value, Config& config);
    static bool validateCheckpointCompression(const std::string& value);
    static void applyCheckpointCompression(const std::string& value, Config& config);
    
    // General option callbacks
    static void applyOutputPrefix(const std::string& value, Config& config);
    static void applyOutputIntervalConfig(const std::string& value, Config& config);
};

} // namespace nlm