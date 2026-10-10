#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

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

    bool Config::validate() const {
    std::vector<std::string> errors, warnings;
    
    // Check for common validation issues
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors.push_back("Configuration key is empty");
        }
        
        if (entry.key.find('.') == std::string::npos) {
            // Check if this looks like a key with sections (e.g., "brain.neuron_count")
            // Not a hard error, but could be a warning
            bool hasUnderscore = entry.key.find('_') != std::string::npos;
            if (hasUnderscore) {
                warnings.push_back("Configuration key contains underscores instead of dots: " + entry.key);
            }
        }
    }
    
    // Check for required configuration sections
    bool hasBrainSection = false;
    bool hasNeuralSection = false;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find("brain.") == 0) {
            hasBrainSection = true;
        }
        if (entry.key.find("neural.") == 0) {
            hasNeuralSection = true;
        }
    }
    
    if (!hasBrainSection) {
        warnings.push_back("Configuration missing 'brain' section");
    }
    
    if (!hasNeuralSection) {
        warnings.push_back("Configuration missing 'neural' section");
    }
    
    // Check for basic brain parameters
    if (!has("brain.neuron_count") && !has("neuron_count")) {
        warnings.push_back("Configuration missing neuron count");
    }
    
    if (!has("brain.synapse_density") && !has("synapse_density")) {
        warnings.push_back("Configuration missing synapse density");
    }
    
    // Check for plasticity configuration
    bool hasPlasticityConfig = false;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find("plasticity.") == 0) {
            hasPlasticityConfig = true;
            break;
        }
    }
    
    if (!hasPlasticityConfig) {
        warnings.push_back("Configuration missing plasticity settings");
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return validate() ? std::vector<std::string>() : std::vector<std::string>{
        "Basic configuration validation failed"
    };
}

std::vector<std::string> Config::getValidationWarnings() const {
    return validate() ? std::vector<std::string>() : std::vector<std::string>{
        "Warning: Basic configuration validation needed"
    };
}

std::string Config::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Report ===\n";
    oss << "Generated at: " << std::chrono::system_clock::now() << "\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n\n";
    
    oss << "Configuration Summary:\n";
    oss << summary() << "\n";
    
    oss << "Validation Status:\n";
    bool isValid = validate();
    oss << "  Valid: " << (isValid ? "YES" : "NO") << "\n";
    
    auto errors = getValidationErrors();
    auto warnings = getValidationWarnings();
    
    oss << "  Errors: " << errors.size() << "\n";
    for (const auto& error : errors) {
        oss << "    - " << error << "\n";
    }
    
    oss << "  Warnings: " << warnings.size() << "\n";
    for (const auto& warning : warnings) {
        oss << "    - " << warning << "\n";
    }
    
    oss << "\n=== End Report ===\n";
    return oss.str();
}

std::vector<std::string> Config::getOptimizationSuggestions() const {
    std::vector<std::string> suggestions;
    
    // Check for optimization opportunities
    if (has("brain.neuron_count") && getOr<int>("brain.neuron_count", 0) > 10000) {
        suggestions.push_back("Large neuron count detected. Consider reducing to improve performance.");
    }
    
    if (has("brain.synapse_density") && getOr<double>("brain.synapse_density", 0.0) > 0.3) {
        suggestions.push_back("High synapse density detected. Consider reducing to improve efficiency.");
    }
    
    bool hasLearningEnabled = has("plasticity.stdp.enable") && getOr<bool>("plasticity.stdp.enable", false);
    bool hasHebbianEnabled = has("plasticity.hebbian.enable") && getOr<bool>("plasticity.hebbian.enable", false);
    bool hasStructuralEnabled = has("plasticity.structural.enable") && getOr<bool>("plasticity.structural.enable", false);
    
    if (hasLearningEnabled && hasHebbianEnabled && hasStructuralEnabled) {
        suggestions.push_back("Multiple plasticity rules enabled. Consider deactivating some for better performance.");
    }
    
    bool hasDevelopmentEnabled = has("development.enable") && getOr<bool>("development.enable", false);
    bool hasCuriosityEnabled = has("neuromod.dopamine.scale", 0.0) > 1.0;
    
    if (hasDevelopmentEnabled && hasCuriosityEnabled) {
        suggestions.push_back("Development and high curiosity levels detected. Consider reducing for stability.");
    }
    
    return suggestions;
}

std::vector<std::string> Config::compareWithBaseline(const std::string& baselineConfig) const {
    std::vector<std::string> differences;
    
    // This is a placeholder implementation - in practice, you would load and compare with actual baseline
    differences.push_back("Baseline comparison: loading baseline configuration...");
    differences.push_back("Baseline comparison: comparing neuron counts...");
    differences.push_back("Baseline comparison: comparing synapse densities...");
    
    return differences;
}

std::vector<std::string> Config::diff(const Config& other) const {
    std::vector<std::string> differences;
    
    // Check for keys in this config that are not in other
    for (const auto& entry : pImpl->entries) {
        if (!other.has(entry.key)) {
            differences.push_back("Added key: " + entry.key);
        }
    }
    
    // Check for keys in other that are not in this config
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key)) {
            differences.push_back("Removed key: " + entry.key);
        }
    }
    
    // Check for value differences
    for (const auto& entry : pImpl->entries) {
        if (other.has(entry.key)) {
            auto otherVal = other.get<std::string>(entry.key);
            if (otherVal && otherVal.value() != std::get<std::string>(entry.value)) {
                differences.push_back("Changed value for key: " + entry.key + 
                                    " (this: " + std::get<std::string>(entry.value) + 
                                    ", other: " + otherVal.value() + ")");
            }
        }
    }
    
    return differences;
}

bool Config::optimize(const std::string& optimizationType) {
    // Simple optimization logic
    if (optimizationType == "memory") {
        // Reduce parameter values for memory efficiency
        set("brain.synapse_density", std::max(0.01, getOr<double>("brain.synapse_density", 0.1) / 2.0));
        set("plasticity.stdp.learning_rate", std::max(0.0001, getOr<double>("plasticity.stdp.learning_rate", 0.001) / 10.0));
        return true;
    } else if (optimizationType == "performance") {
        // Increase parameter values for better performance
        set("brain.synapse_density", std::min(0.5, getOr<double>("brain.synapse_density", 0.1) * 2.0));
        set("plasticity.stdp.learning_rate", std::min(0.01, getOr<double>("plasticity.stdp.learning_rate", 0.001) * 10.0));
        return true;
    } else if (optimizationType == "balanced") {
        // Balanced optimization
        set("brain.synapse_density", 0.1);
        set("plasticity.stdp.learning_rate", 0.001);
        return true;
    }
    
    return false;
}


} // namespace nlm
