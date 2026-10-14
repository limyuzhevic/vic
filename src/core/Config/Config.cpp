#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), validationRules(), validationErrors() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
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

// === USER-FRIENDLY ENHANCEMENTS ===

bool Config::applyProfile(ConfigProfile profile) {
    clear();
    applyProfileDefaults(profile);
    return true;
}

std::string Config::profileToString(ConfigProfile profile) {
    static const std::map<ConfigProfile, std::string> profileNames = {
        {ConfigProfile::Default, "Default"},
        {ConfigProfile::Research, "Research"},
        {ConfigProfile::Production, "Production"},
        {ConfigProfile::Demo, "Demo"},
        {ConfigProfile::Development, "Development"},
        {ConfigProfile::Performance, "Performance"},
        {ConfigProfile::MemoryIntensive, "MemoryIntensive"}
    };
    
    auto it = profileNames.find(profile);
    return it != profileNames.end() ? it->second : "Unknown";
}

bool Config::validate(const std::vector<ValidationRule>& rules) const {
    validationErrors.clear();
    
    for (const auto& rule : rules) {
        if (!has(rule.key)) {
            validationErrors.push_back("Missing required key: " + rule.key);
            continue;
        }
        
        auto value = get(rule.key);
        if (!value.has_value()) {
            validationErrors.push_back("Invalid value type for key: " + rule.key);
            continue;
        }
        
        if (!validateValue(value.value(), rule)) {
            validationErrors.push_back(rule.errorMessage);
        }
    }
    
    return validationErrors.empty();
}

void Config::addValidationRule(const ValidationRule& rule) {
    validationRules.push_back(rule);
}

void Config::removeValidationRule(const std::string& key) {
    validationRules.erase(
        std::remove_if(validationRules.begin(), validationRules.end(),
            [&key](const ValidationRule& rule) { return rule.key == key; }),
        validationRules.end()
    );
}

std::vector<std::string> Config::getValidationErrors() const {
    return validationErrors;
}

bool Config::isValid() const {
    return validate(validationRules);
}

// === ADVANCED FEATURES ===

bool Config::saveWithBackup(const std::string& filepath, bool createBackup) {
    if (createBackup) {
        std::string backupPath = filepath + ".backup";
        saveToFile(backupPath);
    }
    return saveToFile(filepath);
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Create a copy with Runtime source to override previous values
        set(entry.key, entry.value, ConfigSource::Runtime);
    }
}

Config Config::getDifference(const Config& other) const {
    Config diff;
    for (const auto& entry : pImpl->entries) {
        if (!other.has(entry.key)) {
            diff.set(entry.key, entry.value, entry.source);
        }
    }
    return diff;
}

// === CONVENIENCE METHODS ===

void Config::setBrainConfig(size_t neuronCount, size_t regionCount, 
                           double connectionProbability, double timestep) {
    set("brain.neuron_count", static_cast<int64_t>(neuronCount));
    set("brain.region_count", static_cast<int64_t>(regionCount));
    set("brain.connection_probability", connectionProbability);
    set("simulation_timestep", timestep);
}

void Config::setPlasticityConfig(bool enableSTDP, bool enableHebbian, 
                                bool enableStructural, double learningRate) {
    set("plasticity.stdp.enable", enableSTDP);
    set("plasticity.hebbian.enable", enableHebbian);
    set("plasticity.structural.enable", enableStructural);
    set("plasticity.stdp.learning_rate", learningRate);
}

void Config::setNeuromodulationConfig(bool enableDopamine, bool enableCuriosity,
                                     bool enableNovelty, double dopamineScale) {
    set("neuromod.dopamine.enable", enableDopamine);
    set("neuromod.curiosity.enable", enableCuriosity);
    set("neuromod.novelty.enable", enableNovelty);
    set("neuromod.dopamine.scale", dopamineScale);
}

void Config::setDevelopmentConfig(bool enableDevelopment, size_t developmentalStages) {
    set("development.enabled", enableDevelopment);
    set("development.stages", static_cast<int64_t>(developmentalStages));
}

void Config::setMemoryConfig(bool enableWorkingMemory, bool enableEpisodicMemory,
                            bool enableAssociativeMemory, size_t memoryCapacity) {
    set("memory.working.enable", enableWorkingMemory);
    set("memory.episodic.enable", enableEpisodicMemory);
    set("memory.associative.enable", enableAssociativeMemory);
    set("memory.capacity", static_cast<int64_t>(memoryCapacity));
}

void Config::setPredictionConfig(bool enablePrediction, size_t predictionHorizon) {
    set("prediction.enabled", enablePrediction);
    set("prediction.horizon", static_cast<int64_t>(predictionHorizon));
}

void Config::setCognitionConfig(bool enablePlanner, bool enableConceptFormation,
                               bool enableAttention) {
    set("cognition.planner.enable", enablePlanner);
    set("cognition.concept.enable", enableConceptFormation);
    set("cognition.attention.enable", enableAttention);
}

// === UTILITY METHODS ===

std::unordered_map<std::string, ConfigValue> Config::getDomainConfig(const std::string& domain) const {
    std::unordered_map<std::string, ConfigValue> domainConfig;
    std::string prefix = domain + ".";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            domainConfig[entry.key.substr(prefix.length())] = entry.value;
        }
    }
    
    return domainConfig;
}

void Config::setDomainConfig(const std::string& domain, const std::unordered_map<std::string, ConfigValue>& values) {
    std::string prefix = domain + ".";
    
    for (const auto& pair : values) {
        set(prefix + pair.first, pair.second);
    }
}

std::string Config::getConfigStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Total entries: " << pImpl->entries.size() << std::endl;
    oss << "  Entries by source:" << std::endl;
    
    std::map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    for (const auto& pair : sourceCount) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    oss << "  Validation rules: " << validationRules.size() << std::endl;
    oss << "  Validation errors: " << validationErrors.size() << std::endl;
    
    return oss.str();
}

bool Config::resetToProfile(ConfigProfile profile) {
    return applyProfile(profile);
}

bool Config::exportToJSON(const std::string& filepath) const {
    // Placeholder implementation
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": " << "\"PLACEHOLDER_VALUE\"";
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
    return true;
}

bool Config::exportToYAML(const std::string& filepath) const {
    // Placeholder implementation
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Configuration exported from NLM\n";
    for (const auto& entry : pImpl->entries) {
        file << entry.key << ": " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

bool Config::importFromJSON(const std::string& filepath) {
    // Placeholder implementation
    return loadFromFile(filepath);
}

bool Config::importFromYAML(const std::string& filepath) {
    // Placeholder implementation
    return loadFromFile(filepath);
}

// Private helper methods

bool Config::validateValue(const ConfigValue& value, const ValidationRule& rule) const {
    return rule.validator(value);
}

void Config::applyProfileDefaults(ConfigProfile profile) {
    switch (profile) {
        case ConfigProfile::Default:
            applyProfileDefaultsDefault();
            break;
        case ConfigProfile::Research:
            applyProfileDefaultsResearch();
            break;
        case ConfigProfile::Production:
            applyProfileDefaultsProduction();
            break;
        case ConfigProfile::Demo:
            applyProfileDefaultsDemo();
            break;
        case ConfigProfile::Development:
            applyProfileDefaultsDevelopment();
            break;
        case ConfigProfile::Performance:
            applyProfileDefaultsPerformance();
            break;
        case ConfigProfile::MemoryIntensive:
            applyProfileDefaultsMemoryIntensive();
            break;
    }
}

void Config::applyProfileDefaultsDefault() {
    setBrainConfig(1000, 1, 0.1, 0.001);
    setPlasticityConfig(true, true, true, 0.001);
    setNeuromodulationConfig(true, true, true, 1.0);
    setDevelopmentConfig(true, 5);
    setMemoryConfig(true, true, true, 1000);
    setPredictionConfig(true, 10);
    setCognitionConfig(true, true, true);
}

void Config::applyProfileDefaultsResearch() {
    setBrainConfig(2000, 2, 0.15, 0.001);
    setPlasticityConfig(true, true, true, 0.002);
    setNeuromodulationConfig(true, true, true, 1.5);
    setDevelopmentConfig(true, 8);
    setMemoryConfig(true, true, true, 2000);
    setPredictionConfig(true, 20);
    setCognitionConfig(true, true, true);
}

void Config::applyProfileDefaultsProduction() {
    setBrainConfig(500, 1, 0.05, 0.001);
    setPlasticityConfig(true, true, false, 0.001);
    setNeuromodulationConfig(true, false, true, 1.0);
    setDevelopmentConfig(false, 3);
    setMemoryConfig(true, true, false, 500);
    setPredictionConfig(true, 5);
    setCognitionConfig(true, false, true);
}

void Config::applyProfileDefaultsDemo() {
    setBrainConfig(100, 1, 0.2, 0.01);
    setPlasticityConfig(false, false, false, 0.0);
    setNeuromodulationConfig(false, false, false, 0.0);
    setDevelopmentConfig(false, 1);
    setMemoryConfig(false, false, false, 100);
    setPredictionConfig(false, 1);
    setCognitionConfig(false, false, false);
}

void Config::applyProfileDefaultsDevelopment() {
    setBrainConfig(1500, 2, 0.12, 0.001);
    setPlasticityConfig(true, true, true, 0.003);
    setNeuromodulationConfig(true, true, true, 1.2);
    setDevelopmentConfig(true, 10);
    setMemoryConfig(true, true, true, 1500);
    setPredictionConfig(true, 15);
    setCognitionConfig(true, true, true);
}

void Config::applyProfileDefaultsPerformance() {
    setBrainConfig(300, 1, 0.03, 0.001);
    setPlasticityConfig(true, false, false, 0.001);
    setNeuromodulationConfig(true, false, true, 1.0);
    setDevelopmentConfig(false, 2);
    setMemoryConfig(true, false, false, 300);
    setPredictionConfig(true, 3);
    setCognitionConfig(true, false, true);
}

void Config::applyProfileDefaultsMemoryIntensive() {
    setBrainConfig(5000, 3, 0.25, 0.001);
    setPlasticityConfig(true, true, true, 0.005);
    setNeuromodulationConfig(true, true, true, 2.0);
    setDevelopmentConfig(true, 15);
    setMemoryConfig(true, true, true, 5000);
    setPredictionConfig(true, 50);
    setCognitionConfig(true, true, true);
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
