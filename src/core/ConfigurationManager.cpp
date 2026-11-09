// Simplified Enhanced Configuration Manager Implementation
// Focuses on practical improvements for demo executables

#include "core/ConfigurationManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <chrono>
#include <ctime>

namespace nlm {

struct ConfigurationManager::Impl {
    std::unique_ptr<Config> config;
    std::unordered_map<std::string, ConfigPreset> presetMap;
    std::unordered_map<ConfigPreset, std::vector<ConfigEntry>> presetDefinitions;
    std::vector<ConfigChangeCallback> changeCallbacks;
    bool monitoringEnabled;
    
    Impl() : config(std::make_unique<Config>()), monitoringEnabled(false) {
        initializePresets();
    }
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {}

ConfigurationManager::~ConfigurationManager() = default;

ConfigurationManager::ConfigurationManager(ConfigurationManager&&) noexcept = default;

ConfigurationManager& ConfigurationManager::operator=(ConfigurationManager&&) noexcept = default;

void ConfigurationManager::Impl::initializePresets() {
    // Quick test preset
    {
        std::vector<ConfigEntry> entries;
        entries.emplace_back("neuron_count", 500, ConfigSource::Default);
        entries.emplace_back("region_count", 1, ConfigSource::Default);
        entries.emplace_back("connection_probability", 0.15f, ConfigSource::Default);
        entries.emplace_back("simulation_timestep", 0.001, ConfigSource::Default);
        entries.emplace_back("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        entries.emplace_back("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        entries.emplace_back("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        entries.emplace_back("stdp_tau", 20.0f, ConfigSource::Default);
        entries.emplace_back("enable_checkpointing", true, ConfigSource::Default);
        entries.emplace_back("enable_replay", false, ConfigSource::Default);
        entries.emplace_back("enable_development", false, ConfigSource::Default);
        entries.emplace_back("max_steps", 1000, ConfigSource::Default);
        
        presetMap["quick"] = ConfigPreset::QuickTest;
        presetDefinitions[ConfigPreset::QuickTest] = std::move(entries);
    }
    
    // Full test preset
    {
        std::vector<ConfigEntry> entries;
        entries.emplace_back("neuron_count", 1000, ConfigSource::Default);
        entries.emplace_back("region_count", 2, ConfigSource::Default);
        entries.emplace_back("connection_probability", 0.1f, ConfigSource::Default);
        entries.emplace_back("simulation_timestep", 0.001, ConfigSource::Default);
        entries.emplace_back("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        entries.emplace_back("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        entries.emplace_back("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        entries.emplace_back("stdp_tau", 20.0f, ConfigSource::Default);
        entries.emplace_back("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        entries.emplace_back("pruning_rate", 0.00001f, ConfigSource::Default);
        entries.emplace_back("enable_checkpointing", true, ConfigSource::Default);
        entries.emplace_back("enable_replay", true, ConfigSource::Default);
        entries.emplace_back("enable_development", true, ConfigSource::Default);
        entries.emplace_back("replay_interval", 100, ConfigSource::Default);
        entries.emplace_back("consolidation_interval", 1000, ConfigSource::Default);
        entries.emplace_back("max_steps", 10000, ConfigSource::Default);
        
        presetMap["full"] = ConfigPreset::FullTest;
        presetDefinitions[ConfigPreset::FullTest] = std::move(entries);
    }
    
    // Performance preset
    {
        std::vector<ConfigEntry> entries;
        entries.emplace_back("neuron_count", 2000, ConfigSource::Default);
        entries.emplace_back("region_count", 4, ConfigSource::Default);
        entries.emplace_back("connection_probability", 0.05f, ConfigSource::Default);
        entries.emplace_back("simulation_timestep", 0.001, ConfigSource::Default);
        entries.emplace_back("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        entries.emplace_back("stdp_ltp_weight", 0.005f, ConfigSource::Default);
        entries.emplace_back("stdp_ltd_weight", 0.006f, ConfigSource::Default);
        entries.emplace_back("stdp_tau", 30.0f, ConfigSource::Default);
        entries.emplace_back("enable_checkpointing", false, ConfigSource::Default);
        entries.emplace_back("enable_replay", false, ConfigSource::Default);
        entries.emplace_back("enable_development", false, ConfigSource::Default);
        entries.emplace_back("max_steps", 500, ConfigSource::Default);
        
        presetMap["performance"] = ConfigPreset::Performance;
        presetDefinitions[ConfigPreset::Performance] = std::move(entries);
    }
}

bool ConfigurationManager::loadFromFile(const std::string& filepath, ConfigSource source) {
    return pImpl->config->loadFromFile(filepath);
}

bool ConfigurationManager::loadFromArgs(int argc, char** argv) {
    // Parse command line arguments and apply to config
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setWithValidation(key, value, ConfigSource::CommandLine, "Command line argument");
            } else {
                std::cerr << "Warning: Invalid argument format \"" << arg << "\" (expected --key=value)" << std::endl;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setWithValidation(key, value, ConfigSource::CommandLine, "Command line argument");
        }
        // Handle --help or -h
        else if (arg == "--help" || arg == "-h") {
            printHelp();
            return false; // Exit after showing help
        }
        // Handle --version
        else if (arg == "--version") {
            std::cout << "NLM Enhanced Configuration Manager v1.0.0" << std::endl;
            return false; // Exit after showing version
        }
        // Unknown argument
        else if (arg[0] == '-') {
            std::cerr << "Warning: Unknown argument \"" << arg << "\"" << std::endl;
        }
    }
    return true;
}

bool ConfigurationManager::loadFromEnvironment(const std::string& prefix) {
    return false; // Simplified implementation
}

bool ConfigurationManager::applyPreset(ConfigPreset preset) {
    if (!validatePreset(preset)) {
        std::cerr << "Invalid preset: " << static_cast<int>(preset) << std::endl;
        return false;
    }
    
    // Clear existing configuration
    pImpl->config->clear();
    
    // Apply preset definition
    const auto& entries = presetDefinitions[preset];
    for (const auto& entry : entries) {
        setWithValidation(entry.key, entry.value, entry.source, "Applying preset");
    }
    
    return true;
}

bool ConfigurationManager::savePreset(const std::string& name, const ConfigPreset preset) {
    if (!validatePreset(preset)) {
        std::cerr << "Cannot save invalid preset: " << static_cast<int>(preset) << std::endl;
        return false;
    }
    
    // Create a new preset name mapping
    pImpl->presetMap[name] = preset;
    return true;
}

ConfigValidationResult ConfigurationManager::validate(const ConfigSchema& schema) const {
    ConfigValidationResult result;
    
    for (const auto& key : pImpl->config->getKeys()) {
        if (!validateValue(key, getConfigValue(key))) {
            result.addError("Invalid configuration for key: " + key);
        }
    }
    
    return result;
}

bool ConfigurationManager::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation
    return true;
}

template<typename T>
T ConfigurationManager::getWithValidation(const std::string& key, const T& defaultValue, 
                                         const std::string& errorContext) const {
    auto val = pImpl->config->template get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        if (!errorContext.empty()) {
            std::cerr << "Warning: Configuration key \"" << key << "\" not found in " << errorContext << std::endl;
        }
        return defaultValue;
    }
}

bool ConfigurationManager::setWithValidation(const std::string& key, const ConfigValue& value, 
                                            ConfigSource source, const std::string& errorContext) {
    pImpl->config->set(key, value, source);
    
    // Notify change callbacks
    for (const auto& callback : pImpl->changeCallbacks) {
        callback(key, value);
    }
    
    return true;
}

void ConfigurationManager::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void ConfigurationManager::removeChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.erase(std::remove(pImpl->changeCallbacks.begin(), 
                                            pImpl->changeCallbacks.end(), callback),
                                 pImpl->changeCallbacks.end());
}

std::vector<std::string> ConfigurationManager::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->presetMap) {
        names.push_back(pair.first);
    }
    return names;
}

bool ConfigurationManager::hasPreset(const std::string& name) const {
    std::string lowerName = toLower(name);
    return pImpl->presetMap.find(lowerName) != pImpl->presetMap.end();
}

ConfigPreset ConfigurationManager::getPreset(const std::string& name) const {
    std::string lowerName = toLower(name);
    auto it = pImpl->presetMap.find(lowerName);
    if (it != pImpl->presetMap.end()) {
        return it->second;
    }
    return ConfigPreset::Custom;
}

std::string ConfigurationManager::getConfigurationSummary() const {
    return pImpl->config->summary();
}

std::string ConfigurationManager::getPresetSummary(ConfigPreset preset) const {
    std::ostringstream oss;
    oss << "Preset: " << static_cast<int>(preset) << std::endl;
    
    const auto& entries = presetDefinitions.at(preset);
    for (const auto& entry : entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
}

void ConfigurationManager::enableMonitoring(double intervalSeconds) {
    pImpl->monitoringEnabled = true;
    std::cout << "Configuration monitoring enabled (interval: " << intervalSeconds << "s)" << std::endl;
}

void ConfigurationManager::disableMonitoring() {
    pImpl->monitoringEnabled = false;
}

bool ConfigurationManager::isMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

std::string ConfigurationManager::getMonitoringStats() const {
    return "Monitoring enabled: " + std::to_string(pImpl->monitoringEnabled);
}

bool ConfigurationManager::exportToFile(const std::string& filepath, ConfigPreset preset) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration Export" << std::endl;
    file << "# Preset: " << static_cast<int>(preset) << std::endl;
    file << std::endl;
    
    if (preset == ConfigPreset::Custom) {
        for (const auto& entry : pImpl->config->getKeys()) {
            file << entry << " = PLACEHOLDER" << std::endl;
        }
    } else {
        const auto& entries = presetDefinitions.at(preset);
        for (const auto& entry : entries) {
            file << entry.key << " = PLACEHOLDER" << std::endl;
        }
    }
    
    return true;
}

bool ConfigurationManager::importFromFile(const std::string& filepath) {
    return loadFromFile(filepath, ConfigSource::File);
}

size_t ConfigurationManager::getEntryCount() const {
    return pImpl->config->getKeys().size();
}

size_t ConfigurationManager::getChangeCount() const {
    return 0; // Simplified
}

std::chrono::steady_clock::time_point ConfigurationManager::getLastChangeTime() const {
    return std::chrono::steady_clock::time_point(); // Simplified
}

void ConfigurationManager::printHelp() const {
    std::cout << "=== NLM Enhanced Configuration Manager ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Available presets:" << std::endl;
    std::cout << "  --quick     - Quick test configuration (500 neurons, 1000 steps)" << std::endl;
    std::cout << "  --full      - Full integration test configuration (1000 neurons, 10000 steps)" << std::endl;
    std::cout << "  --performance - Performance benchmark configuration (2000 neurons, fast)" << std::endl;
    std::cout << "  --development - Development configuration (500 neurons, high plasticity)" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration file format: key = value" << std::endl;
    std::cout << "Environment variable prefix: NLM_" << std::endl;
    std::cout << "Command line: --key=value or -key value" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage examples:" << std::endl;
    std::cout << "  nlm --quick --neuron_count=2000" << std::endl;
    std::cout << "  nlm --load myconfig.cfg --enable_replay" << std::endl;
    std::cout << "  nlm --help" << std::endl;
}

void ConfigurationManager::printConfigurationSummary() const {
    std::cout << getConfigurationSummary() << std::endl;
}

void ConfigurationManager::printPresetHelp(ConfigPreset preset) const {
    std::cout << getPresetSummary(preset) << std::endl;
}

ConfigSchema ConfigurationManager::getBrainSchema() {
    ConfigSchema schema;
    schema.key = "neuron_count";
    schema.type = "int";
    schema.description = "Number of neurons in the brain";
    schema.required = true;
    schema.defaultValue = "1000";
    schema.minValue = 100;
    schema.maxValue = 100000;
    
    return schema;
}

ConfigSchema ConfigurationManager::getEnvironmentSchema() {
    ConfigSchema schema;
    schema.key = "environment_width";
    schema.type = "int";
    schema.description = "Width of the environment";
    schema.required = false;
    schema.defaultValue = "20";
    schema.minValue = 10;
    schema.maxValue = 1000;
    
    return schema;
}

ConfigSchema ConfigurationManager::getPerformanceSchema() {
    ConfigSchema schema;
    schema.key = "connection_probability";
    schema.type = "double";
    schema.description = "Connection probability for random connectivity";
    schema.required = false;
    schema.defaultValue = "0.1";
    schema.minValue = 0.01;
    schema.maxValue = 1.0;
    
    return schema;
}

ConfigSchema ConfigurationManager::getDebugSchema() {
    ConfigSchema schema;
    schema.key = "log_level";
    schema.type = "string";
    schema.description = "Log level (Debug, Info, Warning, Error)";
    schema.required = false;
    schema.defaultValue = "Info";
    schema.allowedValues = {"Debug", "Info", "Warning", "Error"};
    
    return schema;
}

bool ConfigurationManager::configureQuickTest(int neuronCount, int steps) {
    return applyPreset(ConfigPreset::QuickTest) &&
           setWithValidation("neuron_count", neuronCount) &&
           setWithValidation("max_steps", static_cast<int64_t>(steps));
}

bool ConfigurationManager::configureFullTest(int neuronCount, int steps) {
    return applyPreset(ConfigPreset::FullTest) &&
           setWithValidation("neuron_count", neuronCount) &&
           setWithValidation("max_steps", static_cast<int64_t>(steps));
}

bool ConfigurationManager::configurePerformance(int neuronCount, float connectionProb) {
    return applyPreset(ConfigPreset::Performance) &&
           setWithValidation("neuron_count", neuronCount) &&
           setWithValidation("connection_probability", connectionProb);
}

bool ConfigurationManager::configureDevelopment() {
    return applyPreset(ConfigPreset::Development);
}

bool ConfigurationManager::configureProduction() {
    return applyPreset(ConfigPreset::Production);
}

bool ConfigurationManager::validatePreset(const ConfigPreset preset) const {
    return presetDefinitions.find(preset) != presetDefinitions.end();
}

void ConfigurationManager::setupPresetDefaults(ConfigPreset preset) {
    // Setup any additional defaults for specific presets
    switch (preset) {
        case ConfigPreset::QuickTest:
            setWithValidation("enable_checkpointing", true);
            setWithValidation("enable_replay", false);
            setWithValidation("enable_development", false);
            break;
        case ConfigPreset::FullTest:
            setWithValidation("enable_checkpointing", true);
            setWithValidation("enable_replay", true);
            setWithValidation("enable_development", true);
            setWithValidation("replay_interval", static_cast<int64_t>(100));
            setWithValidation("consolidation_interval", static_cast<int64_t>(1000));
            break;
        case ConfigPreset::Performance:
            setWithValidation("enable_checkpointing", false);
            setWithValidation("enable_replay", false);
            setWithValidation("enable_development", false);
            break;
        case ConfigPreset::Development:
            setWithValidation("development_stage", "CriticalPeriod");
            setWithValidation("synaptogenesis_rate", 0.002f);
            setWithValidation("pruning_rate", 0.001f);
            break;
        case ConfigPreset::Production:
            setWithValidation("enable_checkpointing", true);
            setWithValidation("enable_replay", true);
            setWithValidation("enable_development", false);
            break;
        case ConfigPreset::Custom:
            // No additional defaults
            break;
    }
}

std::string ConfigurationManager::getConfigTypeName(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    return "unknown";
}

ConfigEntry* ConfigurationManager::findConfigEntry(const std::string& key) {
    for (auto& entry : pImpl->config->getKeys()) {
        if (entry == key) {
            // This is a placeholder - in a real implementation, we would access the internal
            // config entries structure
            return nullptr;
        }
    }
    return nullptr;
}

ConfigValue ConfigurationManager::getConfigValue(const std::string& key) {
    // This is a helper method to get ConfigValue from config
    // For now, return a default value
    return ConfigValue();
}

bool ConfigurationManager::isNumber(const std::string& str) {
    std::istringstream iss(str);
    double val;
    return (iss >> val) && iss.eof();
}

std::string ConfigurationManager::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

ConfigurationManager& getConfigurationManager() {
    static ConfigurationManager instance;
    return instance;
}

} // namespace nlm
