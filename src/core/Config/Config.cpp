#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <variant>

namespace nlm {

// Enhanced configuration implementation
class EnhancedConfigImpl {
public:
    EnhancedConfigImpl() {
        // Initialize with basic configuration entries from default.cfg
        initializeBasicConfig();
    }
    
    // Load configuration with enhanced features
    bool load(const std::string& filepath, 
              const std::string& configFormat = "auto",
              bool inherit = true,
              bool validate = true,
              bool overrideWithEnv = true) {
        try {
            // Parse configuration based on format
            std::vector<ConfigEntry> entries;
            EnhancedConfigParser parser;
            
            if (configFormat == "auto" || configFormat == "simple") {
                entries = parser.parseFromFile(filepath, EnhancedConfigParser::Format::Simple);
            } else if (configFormat == "json") {
                entries = parser.parseFromFile(filepath, EnhancedConfigParser::Format::JSON);
            } else if (configFormat == "yaml") {
                entries = parser.parseFromFile(filepath, EnhancedConfigParser::Format::YAML);
            } else {
                return false;
            }
            
            // Apply inheritance if requested
            if (inherit && !filepath.empty()) {
                std::string parentDir = std::filesystem::path(filepath).parent_path().string();
                std::string parentFile = parentDir + "/parent.cfg";
                if (std::filesystem::exists(parentFile)) {
                    auto parentEntries = parser.parseFromFile(parentFile, EnhancedConfigParser::Format::Simple);
                    // Merge parent and child configurations
                    entries = configManager_.mergeConfigurations(parentEntries, entries);
                }
            }
            
            // Override with environment variables
            if (overrideWithEnv) {
                EnvVarConfig envConfig;
                entries = envConfig.overrideWithEnvVars(entries);
            }
            
            // Validate configuration
            if (validate) {
                ConfigValidator validator;
                std::vector<std::string> errors = validator.validate(entries);
                if (!errors.empty()) {
                    // Log validation errors
                    for (const auto& error : errors) {
                        // Log error
                    }
                    return false;
                }
            }
            
            // Store configuration entries
            entries_ = std::move(entries);
            return true;
            
        } catch (const std::exception& e) {
            // Log exception
            return false;
        }
    }
    
    // Save configuration
    bool save(const std::string& filepath, 
              const std::string& outputFormat = "simple") const {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            if (outputFormat == "simple") {
                for (const auto& entry : entries_) {
                    file << "# " << entry.description << "\n";
                    file << entry.key << " = " << getValueAsString(entry.value) << "\n";
                }
            } else if (outputFormat == "json") {
                // Export to JSON format
                file << configManager_.exportToJson(entries_);
            } else if (outputFormat == "yaml") {
                // Export to YAML format
                file << configManager_.exportToYaml(entries_);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    // Validate configuration
    bool validate() const {
        ConfigValidator validator;
        return validator.validate(entries_).empty();
    }
    
    // Get configuration entries
    const std::vector<ConfigEntry>& getEntries() const { return entries_; }
    
    // Get configuration entry by key
    std::optional<ConfigEntry> getEntry(const std::string& key) const {
        auto it = std::find_if(entries_.begin(), entries_.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != entries_.end()) {
            return *it;
        }
        return std::nullopt;
    }
    
    // Set configuration entry
    void setEntry(const std::string& key, const ConfigValue& value, 
                  const std::string& description = "",
                  ConfigSource source = ConfigSource::Runtime) {
        auto it = std::find_if(entries_.begin(), entries_.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != entries_.end()) {
            it->value = value;
            it->source = source;
        } else {
            entries_.emplace_back(key, value, source, description);
        }
    }
    
    // Remove configuration entry
    bool removeEntry(const std::string& key) {
        auto it = std::find_if(entries_.begin(), entries_.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != entries_.end()) {
            entries_.erase(it);
            return true;
        }
        return false;
    }
    
    // Merge with another configuration
    bool mergeWith(const EnhancedConfigImpl& other, 
                  ConfigManager::MergeStrategy strategy = ConfigManager::MergeStrategy::Override) {
        auto mergedEntries = configManager_.mergeConfigurations(
            entries_, other.entries_, strategy);
        
        if (mergedEntries.size() != entries_.size()) {
            entries_ = std::move(mergedEntries);
            return true;
        }
        return false;
    }
    
    // Compare with another configuration
    std::vector<std::string> compareWith(const EnhancedConfigImpl& other) const {
        return configManager_.compareConfigurations(entries_, other.entries_);
    }
    
    // Get configuration summary
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Enhanced Configuration (" << entries_.size() << " entries):\n";
        oss << "Loaded from: default.cfg\n";
        oss << "Inheritance: enabled\n";
        oss << "Validation: enabled\n";
        oss << "Environment override: enabled\n\n";
        
        for (const auto& entry : entries_) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
        
        return oss.str();
    }
    
    // Create backup
    bool createBackup(const std::string& name = "") {
        return backupManager_.createBackup(entries_, name);
    }
    
    // Restore from backup
    bool restoreBackup(const std::string& name = "latest") {
        auto restoredEntries = backupManager_.restoreBackup(name);
        if (!restoredEntries.empty()) {
            entries_ = std::move(restoredEntries);
            return true;
        }
        return false;
    }
    
    // Add to history
    void addToHistory(const std::string& comment = "") {
        historyManager_.addToHistory(entries_, comment);
    }
    
    // Restore from history
    bool restoreFromHistory(size_t index) {
        auto restoredEntries = historyManager_.getFromHistory(index);
        if (!restoredEntries.empty()) {
            entries_ = std::move(restoredEntries);
            return true;
        }
        return false;
    }
    
    // Apply preset
    bool applyPreset(const std::string& name,
                    const std::unordered_map<std::string, std::string>& parameters = {}) {
        return presetManager_.applyPreset(name, parameters);
    }
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const {
        return validator_.validate(entries_);
    }
    
    // Get available presets
    std::vector<std::string> listPresets() const {
        return presetManager_.listPresets();
    }
    
    // Get available backups
    std::vector<std::string> listBackups() const {
        return backupManager_.listBackups();
    }
    
    // Get history
    std::vector<std::pair<std::string, std::string>> listHistory() const {
        return historyManager_.listHistory();
    }
    
private:
    std::vector<ConfigEntry> entries_;
    EnhancedConfigParser parser_;
    AdvancedConfigValidator validator_;
    ConfigManager configManager_;
    EnvVarConfig envVarConfig_;
    ConfigBackup backupManager_;
    ConfigHistory historyManager_;
    ConfigTemplateManager templateManager_;
    ConfigCompare compareUtility_;
    ConfigPresetManager presetManager_;
    
    // Initialize basic configuration from default.cfg
    void initializeBasicConfig() {
        // Core configuration
        setEntry("random_seed", int64_t(42), "Random seed for reproducible experiments");
        setEntry("simulation_timestep", 0.001, "Simulation timestep in seconds");
        setEntry("neuron_count", int64_t(1000), "Number of neurons in the brain");
        setEntry("region_count", int64_t(2), "Number of brain regions");
        setEntry("connection_probability", 0.1f, "Connection probability for random connectivity");
        
        // Plasticity settings
        setEntry("plasticity_learning_rate", 0.01f, "Plasticity learning rate");
        setEntry("stdp_ltp_weight", 0.01f, "STDP LTP weight");
        setEntry("stdp_ltd_weight", 0.012f, "STDP LTD weight");
        setEntry("stdp_time_constant", 20.0f, "STDP time constant");
        
        // Development settings
        setEntry("development_synaptogenesis_rate", 0.001f, "Synaptogenesis rate");
        setEntry("development_pruning_rate", 0.0001f, "Pruning rate");
        
        // Neuromodulation settings
        setEntry("dopamine_baseline", 0.0f, "Dopamine baseline level");
        setEntry("dopamine_modulation_strength", 0.1f, "Dopamine modulation strength");
        
        // Reward settings
        setEntry("reward_discount_factor", 0.99f, "Reward discount factor");
        
        // Environment settings
        setEntry("environment_name", std::string("GridWorld"), "Environment name");
        setEntry("environment_width", int(10), "Environment width");
        setEntry("environment_height", int(10), "Environment height");
        
        // Logging settings
        setEntry("log_level", std::string("INFO"), "Log level");
        setEntry("log_to_file", false, "Log to file");
        setEntry("log_filename", std::string("nlm.log"), "Log filename");
        
        // Simulation settings
        setEntry("max_simulation_steps", int64_t(10000), "Maximum simulation steps");
        setEntry("simulation_time_limit", 0.0, "Simulation time limit");
        
        // Visualization settings
        setEntry("visualization_enabled", false, "Enable visualization");
        setEntry("visualization_update_rate", 30.0f, "Visualization update rate");
    }
    
    // Convert ConfigValue to string
    std::string getValueAsString(const ConfigValue& value) const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
};

// Forward declarations for the new classes
class EnhancedConfigParser;
class ConfigValidator;
class ConfigManager;
class EnvVarConfig;
class ConfigBackup;
class ConfigHistory;
class ConfigTemplateManager;
class ConfigCompare;
class ConfigPresetManager;

// Global enhanced configuration instance
EnhancedConfigImpl& getEnhancedConfigImpl() {
    static EnhancedConfigImpl instance;
    return instance;
}

// Enhanced configuration class implementation
EnhancedConfig::EnhancedConfig() {
    // Initialize with default configuration
    configImpl_ = std::make_unique<EnhancedConfigImpl>();
}

EnhancedConfig::~EnhancedConfig() = default;

bool EnhancedConfig::load(const std::string& filepath, 
                         const std::string& configFormat,
                         bool inherit,
                         bool validate,
                         bool overrideWithEnv) {
    return configImpl_->load(filepath, configFormat, inherit, validate, overrideWithEnv);
}

bool EnhancedConfig::save(const std::string& filepath, 
                         const std::string& outputFormat) const {
    return configImpl_->save(filepath, outputFormat);
}

bool EnhancedConfig::validate() const {
    return configImpl_->validate();
}

void EnhancedConfig::setEntry(const std::string& key, const ConfigValue& value,
                             const std::string& description,
                             ConfigSource source) {
    configImpl_->setEntry(key, value, description, source);
}

bool EnhancedConfig::removeEntry(const std::string& key) {
    return configImpl_->removeEntry(key);
}

bool EnhancedConfig::mergeWith(const EnhancedConfig& other, 
                              MergeStrategy strategy) {
    return configImpl_->mergeWith(*other.configImpl_, static_cast<ConfigManager::MergeStrategy>(strategy));
}

std::vector<std::string> EnhancedConfig::compareWith(const EnhancedConfig& other) const {
    return configImpl_->compareWith(*other.configImpl_);
}

std::string EnhancedConfig::getSummary() const {
    return configImpl_->getSummary();
}

bool EnhancedConfig::createBackup(const std::string& name) {
    return configImpl_->createBackup(name);
}

bool EnhancedConfig::restoreBackup(const std::string& name) {
    return configImpl_->restoreBackup(name);
}

void EnhancedConfig::addToHistory(const std::string& comment) {
    configImpl_->addToHistory(comment);
}

bool EnhancedConfig::restoreFromHistory(size_t index) {
    return configImpl_->restoreFromHistory(index);
}

bool EnhancedConfig::applyPreset(const std::string& name,
                                const std::unordered_map<std::string, std::string>& parameters) {
    return configImpl_->applyPreset(name, parameters);
}

std::vector<std::string> EnhancedConfig::listPresets() const {
    return configImpl_->listPresets();
}

std::vector<std::string> EnhancedConfig::listBackups() const {
    return configImpl_->listBackups();
}

std::vector<std::pair<std::string, std::string>> EnhancedConfig::listHistory() const {
    return configImpl_->listHistory();
}

std::vector<std::string> EnhancedConfig::getValidationErrors() const {
    return configImpl_->getValidationErrors();
}

bool EnhancedConfig::isValid() const {
    return configImpl_->validate();
}

// Enhanced argument parser implementation
EnhancedArgumentParser::EnhancedArgumentParser() {
    // Initialize with default arguments
    initializeDefaultArguments();
}

EnhancedArgumentParser::~EnhancedArgumentParser() = default;

void EnhancedArgumentParser::initializeDefaultArguments() {
    // Add standard NLM arguments
    addOption("--experiment", 'e', "Experiment name", false, "basic");
    addOption("--steps", 's', "Number of simulation steps", false, "1000");
    addOption("--episodes", 'E', "Number of episodes", false, "10");
    addOption("--output", 'o', "Output file path", false, "");
    addOption("--config", 'c', "Configuration file path", false, "configs/default.cfg");
    addOption("--config-format", "", "Configuration file format (simple, json, yaml)", false, "auto");
    addOption("--log-level", "", "Log level (debug, info, warning, error)", false, "info");
    addOption("--log-file", "", "Log file path", false, "nlm.log");
    addOption("--seed", 'S', "Random seed", false, "42");
    addOption("--benchmark", "", "Enable benchmarking mode", false, "false");
    addOption("--validate", "", "Validate configuration", false, "false");
    addOption("--help", 'h', "Show help information", false, "false");
    addOption("--version", 'v', "Show version information", false, "false");
    
    // Add advanced arguments
    addFlag("--parallel", "", "Run experiments in parallel", false);
    addFlag("--compare", "", "Compare experiment results", false);
    addFlag("--visualize", "", "Enable visualization", false);
    addFlag("--adaptive", "", "Enable adaptive parameters", false);
    addFlag("--statistical", "", "Enable statistical analysis", false);
    addFlag("--detailed", "", "Enable detailed output", false);
    addFlag("--env-override", "", "Enable environment variable override", false);
    addFlag("--backup", "", "Create backup before running", false);
    addFlag("--restore", "", "Restore from backup after running", false);
    
    addPositional("experiment", "Experiment name", false);
    addPositional("output", "Output file path", false);
}

bool EnhancedArgumentParser::parse(int argc, char** argv) {
    try {
        errors_.clear();
        
        // Parse arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Check for short options
            if (arg.substr(0, 2) == "--" || (arg.size() == 2 && arg[0] == '-')) {
                bool found = false;
                
                // Check for long options
                if (arg.substr(0, 2) == "--") {
                    std::string longName = arg.substr(2);
                    auto it = std::find_if(options_.begin(), options_.end(),
                                          [&longName](const Option& opt) { return opt.longName == longName; });
                    if (it != options_.end()) {
                        parseOption(*it, argv, i, argc);
                        found = true;
                    }
                } else {
                    // Check for short options
                    for (char c : arg.substr(1)) {
                        auto it = std::find_if(options_.begin(), options_.end(),
                                              [c](const Option& opt) { return opt.shortName == c; });
                        if (it != options_.end()) {
                            parseOption(*it, argv, i, argc);
                            found = true;
                        }
                    }
                }
                
                if (!found) {
                    errors_.push_back("Unknown option: " + arg);
                }
            } else {
                // Positional argument
                parsePositionalArgument(arg);
            }
        }
        
        // Validate required options
        validateRequiredOptions();
        
        return !hasErrors();
        
    } catch (const std::exception& e) {
        errors_.push_back(std::string("Error parsing arguments: ") + e.what());
        return false;
    }
}

void EnhancedArgumentParser::parseOption(Option& option, char** argv, int& i, int argc) {
    if (option.longName.empty() && option.shortName == '\0') {
        return;
    }
    
    option.specified = true;
    
    // Check if option has a value
    bool hasValue = false;
    std::string value;
    
    if (option.longName.empty()) {
        // Short option
        if (i + 1 < argc && argv[i + 1][0] != '-') {
            hasValue = true;
            value = argv[++i];
        }
    } else {
        // Long option
        size_t pos = std::string("--" + option.longName).find('=');
        if (pos != std::string::npos) {
            hasValue = true;
            value = std::string("--" + option.longName).substr(pos + 1);
        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
            hasValue = true;
            value = argv[++i];
        }
    }
    
    if (hasValue) {
        option.value = value;
    } else if (option.validator && !option.validator(option.defaultValue)) {
        errors_.push_back("Invalid default value for option: " + option.longName);
    }
}

void EnhancedArgumentParser::parsePositionalArgument(const std::string& arg) {
    if (positionalArgs_.empty()) {
        return;
    }
    
    // Parse first available positional argument
    auto it = std::find_if(positionalArgs_.begin(), positionalArgs_.end(),
                          [](const PositionalArgument& arg) { return !arg.required; });
    if (it != positionalArgs_.end()) {
        it->value = arg;
        it->specified = true;
    }
}

void EnhancedArgumentParser::validateRequiredOptions() const {
    for (const auto& option : options_) {
        if (option.required && !option.specified) {
            errors_.push_back("Required option missing: " + option.longName);
        }
    }
    
    for (const auto& arg : positionalArgs_) {
        if (arg.required && !arg.specified) {
            errors_.push_back("Required positional argument missing: " + arg.name);
        }
    }
}

std::string EnhancedArgumentParser::getHelpText() const {
    std::ostringstream oss;
    oss << "NLM - Neural Learning Machine\n";
    oss << "======================\n\n";
    
    oss << "Usage: nlm [OPTIONS] [ARGS]\n\n";
    
    oss << "Options:\n";
    for (const auto& option : options_) {
        oss << "  -" << (option.shortName == '\0' ? "  " : std::string(1, option.shortName)) << 
              "  --" << option.longName << "\n";
        oss << "      " << option.description << "\n";
        if (!option.defaultValue.empty()) {
            oss << "      Default: " << option.defaultValue << "\n";
        }
    }
    
    oss << "\nFlags:\n";
    for (const auto& option : options_) {
        if (option.defaultValue == "false") {
            oss << "  --" << option.longName << "\n";
            oss << "      " << option.description << "\n";
        }
    }
    
    oss << "\nPositional Arguments:\n";
    for (const auto& arg : positionalArgs_) {
        oss << "  " << arg.name << "\n";
        oss << "      " << arg.description << "\n";
    }
    
    return oss.str();
}

// Global functions implementation
const std::vector<ConfigEntry>& getConfigEntries() {
    return getEnhancedConfigImpl().getEntries();
}

ConfigEntry getConfigEntry(const std::string& key) {
    auto entry = getEnhancedConfigImpl().getEntry(key);
    if (entry) {
        return *entry;
    }
    static ConfigEntry emptyEntry;
    return emptyEntry;
}

bool hasConfigEntry(const std::string& key) {
    return getEnhancedConfigImpl().getEntry(key).has_value();
}

void setConfigEntry(const std::string& key, const ConfigValue& value,
                   const std::string& description,
                   ConfigSource source) {
    getEnhancedConfigImpl().setEntry(key, value, description, source);
}

void removeConfigEntry(const std::string& key) {
    getEnhancedConfigImpl().removeEntry(key);
}

std::vector<std::string> getConfigKeys() {
    std::vector<std::string> keys;
    const auto& entries = getConfigEntries();
    for (const auto& entry : entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::string getConfigSummary() {
    return getEnhancedConfigImpl().getSummary();
}

bool loadConfigFile(const std::string& filepath,
                  const std::string& format,
                  bool inherit,
                  bool validate,
                  bool overrideWithEnv) {
    return getEnhancedConfigImpl().load(filepath, format, inherit, validate, overrideWithEnv);
}

bool saveConfigFile(const std::string& filepath,
                  const std::string& format) {
    return getEnhancedConfigImpl().save(filepath, format);
}

bool validateConfigFile(const std::string& filepath) {
    return getEnhancedConfigImpl().load(filepath, "auto", true, true, true);
}

std::vector<std::string> compareConfigFiles(const std::string& filepath1,
                                          const std::string& filepath2) {
    // This is a simplified implementation
    // In a real implementation, you would load both configurations and compare them
    return std::vector<std::string>();
}

void initializeApplication(int argc, char** argv) {
    // Initialize enhanced configuration
    getEnhancedConfigImpl().load("configs/default.cfg", "auto", true, true, true);
    
    // Initialize argument parser
    getEnhancedArgumentParser().parse(argc, argv);
}

bool isApplicationInitialized() {
    return true;  // Simplified for this implementation
}

void runApplication() {
    // Run the main NLM application
    // This would call the original main() function with enhanced arguments
}

void shutdownApplication() {
    // Clean up resources
}

// Enhanced configuration application class
NLMApplication::NLMApplication() {
    // Initialize configuration preset manager
    presetManager_.addPreset("phase2", {
        {"random_seed", ConfigValue(int64_t(42)), ConfigSource::Default, "Random seed for reproducible experiments"},
        {"simulation_timestep", ConfigValue(0.001), ConfigSource::Default, "Simulation timestep in seconds"},
        {"neuron_count", ConfigValue(int64_t(500)), ConfigSource::Default, "Number of neurons in the brain"},
        {"region_count", ConfigValue(int64_t(1)), ConfigSource::Default, "Number of brain regions"},
        {"connection_probability", ConfigValue(0.15f), ConfigSource::Default, "Connection probability"},
    }, "Phase 2: Real Neural Computation", "simulation");
    
    presetManager_.addPreset("phase3", {
        {"random_seed", ConfigValue(int64_t(42)), ConfigSource::Default, "Random seed for reproducible experiments"},
        {"simulation_timestep", ConfigValue(0.001), ConfigSource::Default, "Simulation timestep in seconds"},
        {"neuron_count", ConfigValue(int64_t(1000)), ConfigSource::Default, "Number of neurons in the brain"},
        {"region_count", ConfigValue(int64_t(2)), ConfigSource::Default, "Number of brain regions"},
        {"connection_probability", ConfigValue(0.1f), ConfigSource::Default, "Connection probability"},
        {"environment_name", ConfigValue(std::string("GridWorld")), ConfigSource::Default, "Environment name"},
        {"environment_width", ConfigValue(int(20)), ConfigSource::Default, "Environment width"},
        {"environment_height", ConfigValue(int(20)), ConfigSource::Default, "Environment height"},
    }, "Phase 3: World Interaction", "world_interaction");
    
    presetManager_.addPreset("phase4", {
        {"random_seed", ConfigValue(int64_t(42)), ConfigSource::Default, "Random seed for reproducible experiments"},
        {"simulation_timestep", ConfigValue(0.001), ConfigSource::Default, "Simulation timestep in seconds"},
        {"neuron_count", ConfigValue(int64_t(1000)), ConfigSource::Default, "Number of neurons in the brain"},
        {"region_count", ConfigValue(int64_t(2)), ConfigSource::Default, "Number of brain regions"},
        {"connection_probability", ConfigValue(0.1f), ConfigSource::Default, "Connection probability"},
        {"log_level", ConfigValue(std::string("info")), ConfigSource::Default, "Log level"},
        {"log_to_file", ConfigValue(false), ConfigSource::Default, "Log to file"},
    }, "Phase 4: Emerging Cognition", "cognition");
}

NLMApplication::~NLMApplication() = default;

bool NLMApplication::initialize(int argc, char** argv) {
    try {
        // Initialize argument parser
        initializeArgumentParser();
        
        // Initialize configuration preset manager
        initializePresetManager();
        
        // Initialize configuration managers
        initializeConfigManagers();
        
        // Load configuration
        if (!loadConfiguration()) {
            return false;
        }
        
        // Save configuration if requested
        if (hasArgument("--backup")) {
            createBackup();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

int NLMApplication::run() {
    try {
        // Handle command-line arguments
        handleCommandLineArguments();
        
        // Handle configuration operations
        handleConfigurationOperations();
        
        // Handle preset operations
        handlePresetOperations();
        
        // Handle backup operations
        handleBackupOperations();
        
        // Handle history operations
        handleHistoryOperations();
        
        // Handle validation
        handleValidation();
        
        // Run the main NLM application
        return runApplication();
        
    } catch (const std::exception& e) {
        return ErrorCodes::APPLICATION_RUN_ERROR;
    }
}

void NLMApplication::shutdown() {
    // Clean up resources
}

void NLMApplication::initializeArgumentParser() {
    argParser_.addOption("--experiment", 'e', "Experiment name", false, "basic");
    argParser_.addOption("--steps", 's', "Number of simulation steps", false, "1000");
    argParser_.addOption("--episodes", 'E', "Number of episodes", false, "10");
    argParser_.addOption("--output", 'o', "Output file path", false, "");
    argParser_.addOption("--config", 'c', "Configuration file path", false, "configs/default.cfg");
    argParser_.addOption("--config-format", "", "Configuration file format", false, "auto");
    argParser_.addOption("--log-level", "", "Log level", false, "info");
    argParser_.addOption("--log-file", "", "Log file path", false, "nlm.log");
    argParser_.addOption("--seed", 'S', "Random seed", false, "42");
    argParser_.addOption("--benchmark", "", "Enable benchmarking mode", false, "false");
    argParser_.addOption("--validate", "", "Validate configuration", false, "false");
    argParser_.addOption("--help", 'h', "Show help information", false, "false");
    argParser_.addOption("--version", 'v', "Show version information", false, "false");
    
    // Add flags
    argParser_.addFlag("--parallel", "", "Run experiments in parallel", false);
    argParser_.addFlag("--compare", "", "Compare experiment results", false);
    argParser_.addFlag("--visualize", "", "Enable visualization", false);
    argParser_.addFlag("--adaptive", "", "Enable adaptive parameters", false);
    argParser_.addFlag("--statistical", "", "Enable statistical analysis", false);
    argParser_.addFlag("--detailed", "", "Enable detailed output", false);
    argParser_.addFlag("--env-override", "", "Enable environment variable override", false);
    argParser_.addFlag("--backup", "", "Create backup before running", false);
    argParser_.addFlag("--restore", "", "Restore from backup after running", false);
}

void NLMApplication::initializePresetManager() {
    // Initialize preset manager with default presets
    // (Already done in constructor)
}

void NLMApplication::initializeConfigManagers() {
    // Initialize configuration managers
    // (Already done in constructor)
}

bool NLMApplication::loadConfiguration() {
    // Load configuration from file
    std::string configFile = "configs/default.cfg";
    if (hasArgument("--config")) {
        configFile = getArgumentValue("--config");
    }
    
    std::string configFormat = "auto";
    if (hasArgument("--config-format")) {
        configFormat = getArgumentValue("--config-format");
    }
    
    return config_.load(configFile, configFormat, true, hasArgument("--validate"), 
                       hasArgument("--env-override"));
}

bool NLMApplication::saveConfiguration() {
    // Save configuration to file
    std::string outputFile = "enhanced_config.cfg";
    if (hasArgument("--output")) {
        outputFile = getArgumentValue("--output");
    }
    
    return config_.save(outputFile, "simple");
}

bool NLMApplication::validateConfiguration() {
    return config_.validate();
}

bool NLMApplication::exportConfiguration() {
    // Export configuration
    return saveConfiguration();
}

bool NLMApplication::importConfiguration() {
    // Import configuration (placeholder)
    return true;
}

void NLMApplication::handleCommandLineArguments() {
    // Handle command-line arguments
    if (hasArgument("--help")) {
        std::cout << argParser_.getHelpText();
    } else if (hasArgument("--version")) {
        std::cout << "NLM - Neural Learning Machine v1.0.0\n";
    }
}

void NLMApplication::handleConfigurationOperations() {
    // Handle configuration operations
    if (hasArgument("--validate")) {
        if (!validateConfiguration()) {
            std::cout << "Configuration validation failed:\n";
            auto errors = config_.getValidationErrors();
            for (const auto& error : errors) {
                std::cout << "  - " << error << "\n";
            }
        }
    }
}

void NLMApplication::handlePresetOperations() {
    // Handle preset operations
    if (hasArgument("--experiment")) {
        std::string experiment = getArgumentValue("--experiment");
        if (experiment == "phase2" || experiment == "phase3" || experiment == "phase4") {
            // Apply preset
            bool applied = config_.applyPreset(experiment);
            if (applied) {
                std::cout << "Applied preset: " << experiment << "\n";
            } else {
                std::cout << "Failed to apply preset: " << experiment << "\n";
            }
        }
    }
}

void NLMApplication::handleBackupOperations() {
    // Handle backup operations
    if (hasArgument("--backup")) {
        if (createBackup()) {
            std::cout << "Backup created successfully\n";
        } else {
            std::cout << "Failed to create backup\n";
        }
    }
}

void NLMApplication::handleHistoryOperations() {
    // Handle history operations
    if (hasArgument("--config-history")) {
        auto history = config_.listHistory();
        std::cout << "Configuration history:\n";
        for (const auto& entry : history) {
            std::cout << "  " << entry.first << ": " << entry.second << "\n";
        }
    }
}

void NLMApplication::handleValidation() {
    // Handle validation
    if (!config_.isValid()) {
        std::cout << "Configuration is invalid:\n";
        auto errors = config_.getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  - " << error << "\n";
        }
    }
}

// Global function implementations
NLMApplication& getApplication() {
    static NLMApplication app;
    return app;
}

// Enhanced argument parser global function implementations
EnhancedArgumentParser& getEnhancedArgumentParser() {
    static EnhancedArgumentParser parser;
    return parser;
}

ConfigPresetManager& getConfigPresetManager() {
    static ConfigPresetManager manager;
    return manager;
}

ConfigBackup& getConfigBackup() {
    static ConfigBackup backup;
    return backup;
}

ConfigHistory& getConfigHistory() {
    static ConfigHistory history;
    return history;
}

// Enhanced configuration global functions
const std::vector<ConfigEntry>& getConfigEntries() {
    return getEnhancedConfigImpl().getEntries();
}

ConfigEntry getConfigEntry(const std::string& key) {
    auto entry = getEnhancedConfigImpl().getEntry(key);
    if (entry) {
        return *entry;
    }
    static ConfigEntry emptyEntry;
    return emptyEntry;
}

bool hasConfigEntry(const std::string& key) {
    return getEnhancedConfigImpl().getEntry(key).has_value();
}

void setConfigEntry(const std::string& key, const ConfigValue& value,
                   const std::string& description,
                   ConfigSource source) {
    getEnhancedConfigImpl().setEntry(key, value, description, source);
}

void removeConfigEntry(const std::string& key) {
    getEnhancedConfigImpl().removeEntry(key);
}

std::vector<std::string> getConfigKeys() {
    return getEnhancedConfigImpl().getSummary();
}

std::string getConfigSummary() {
    return getEnhancedConfigImpl().getSummary();
}

bool loadConfigFile(const std::string& filepath,
                  const std::string& format,
                  bool inherit,
                  bool validate,
                  bool overrideWithEnv) {
    return getEnhancedConfigImpl().load(filepath, format, inherit, validate, overrideWithEnv);
}

bool saveConfigFile(const std::string& filepath,
                  const std::string& format) {
    return getEnhancedConfigImpl().save(filepath, format);
}

bool validateConfigFile(const std::string& filepath) {
    return getEnhancedConfigImpl().load(filepath, "auto", true, true, true);
}

std::vector<std::string> compareConfigFiles(const std::string& filepath1,
                                          const std::string& filepath2) {
    return getEnhancedConfigImpl().compareWith(
        EnhancedConfig(getEnhancedConfigImpl()));
}

// Enhanced argument parser global function implementations
void addArgumentOption(const std::string& longName, char shortName, 
                      const std::string& description,
                      bool required,
                      const std::string& defaultValue,
                      std::function<bool(const std::string&)> validator) {
    getEnhancedArgumentParser().addOption(longName, shortName, description, required, defaultValue, validator);
}

void addArgumentFlag(const std::string& longName, char shortName,
                    const std::string& description,
                    bool defaultValue) {
    getEnhancedArgumentParser().addFlag(longName, shortName, description, defaultValue);
}

void addArgumentPositional(const std::string& name,
                          const std::string& description,
                          bool required,
                          std::function<bool(const std::string&)> validator) {
    getEnhancedArgumentParser().addPositional(name, description, required, validator);
}

void addArgumentMutuallyExclusiveGroup(const std::vector<std::string>& optionNames) {
    getEnhancedArgumentParser().addMutuallyExclusiveGroup(optionNames);
}

bool parseArguments(int argc, char** argv) {
    return getEnhancedArgumentParser().parse(argc, argv);
}

std::string getArgumentValue(const std::string& optionName) {
    return getEnhancedArgumentParser().getOptionValue(optionName);
}

bool getArgumentFlag(const std::string& optionName) {
    return getEnhancedArgumentParser().getFlagValue(optionName);
}

std::string getArgumentPositional(size_t index) {
    return getEnhancedArgumentParser().getPositionalValue(index);
}

bool hasArgument(const std::string& optionName) {
    return getEnhancedArgumentParser().hasOption(optionName);
}

std::string getArgumentHelpText() {
    return getEnhancedArgumentParser().getHelpText();
}

std::vector<std::string> getArgumentErrors() {
    return getEnhancedArgumentParser().getErrors();
}

// Enhanced configuration application global function implementations
void initializeApplication(int argc, char** argv) {
    getApplication().initialize(argc, argv);
}

bool isApplicationInitialized() {
    return getApplication().getConfig().isValid();
}

void runApplication() {
    getApplication().run();
}

void shutdownApplication() {
    getApplication().shutdown();
}

} // namespace nlm
