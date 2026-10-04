// Include Config.hpp
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Config::Impl method implementations

bool Config::Impl::has(const std::vector<ConfigEntry>& entries, const std::string& key) {
    return std::any_of(entries.begin(), entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::string Config::Impl::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::Impl::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void Config::remove(std::vector<ConfigEntry>::iterator begin, std::vector<ConfigEntry>::iterator end,
    const std::function<bool(const ConfigEntry&)>& pred) {
    entries.erase(std::remove_if(begin, end, pred), end);
}

// Load from file implementation
bool Config::loadFromFileImpl(const std::string& filepath) {
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

// Clear command-line arguments
bool Config::clearCommandLineArgs() {
    for (auto it = pImpl->entries.begin(); it != pImpl->entries.end(); ) {
        if (it->source == ConfigSource::CommandLine) {
            it = pImpl->entries.erase(it);
        } else {
            ++it;
        }
    }
    return true;
}

// Helper function for remove
void Config::remove(std::vector<ConfigEntry>::iterator begin, std::vector<ConfigEntry>::iterator end,
    const std::function<bool(const ConfigEntry&)>& pred) {
    pImpl->entries.erase(std::remove_if(begin, end, pred), end);
}

} // namespace nlm

// Config.cpp - Template instantiations
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Helper for template member functions
Config::Impl::Impl() : totalChanges(0), validationFailures(0) {
    // Initialize with default validation rules
    initializeDefaultSchema();
}

void Config::Impl::initializeDefaultSchema() {
    schema.name = "NLM Configuration";
    schema.version = "1.0";
    schema.description = "Default NLM configuration schema";
    
    // Add common validation rules
    ValidationRule seedRule;
    seedRule.key = "random_seed";
    seedRule.type = "int";
    seedRule.validation = "range";
    seedRule.min = "0";
    seedRule.max = "2147483647";
    seedRule.defaultValue = "42";
    seedRule.required = false;
    seedRule.description = "Random seed for reproducible simulations";
    schema.rules.push_back(seedRule);
    
    ValidationRule timestepRule;
    timestepRule.key = "simulation_timestep";
    timestepRule.type = "double";
    timestepRule.validation = "range";
    timestepRule.min = "0.0001";
    timestepRule.max = "1.0";
    timestepRule.defaultValue = "0.001";
    timestepRule.required = true;
    timestepRule.description = "Simulation time step in seconds";
    schema.rules.push_back(timestepRule);
    
    ValidationRule neuronCountRule;
    neuronCountRule.key = "neuron_count";
    neuronCountRule.type = "int";
    neuronCountRule.validation = "range";
    neuronCountRule.min = "1";
    neuronCountRule.max = "1000000";
    neuronCountRule.defaultValue = "1000";
    neuronCountRule.required = true;
    neuronCountRule.description = "Number of neurons in the simulation";
    schema.rules.push_back(neuronCountRule);
    
    ValidationRule connectionProbRule;
    connectionProbRule.key = "connection_probability";
    connectionProbRule.type = "double";
    connectionProbRule.validation = "range";
    connectionProbRule.min = "0.0";
    connectionProbRule.max = "1.0";
    connectionProbRule.defaultValue = "0.1";
    connectionProbRule.required = true;
    connectionProbRule.description = "Probability of connections between neurons";
    schema.rules.push_back(connectionProbRule);
    
    ValidationRule regionCountRule;
    regionCountRule.key = "region_count";
    regionCountRule.type = "int";
    regionCountRule.validation = "range";
    regionCountRule.min = "1";
    regionCountRule.max = "100";
    regionCountRule.defaultValue = "1";
    regionCountRule.required = true;
    regionCountRule.description = "Number of neural regions";
    schema.rules.push_back(regionCountRule);
    
    ValidationRule stdpLtpRule;
    stdpLtpRule.key = "stdp_ltp_weight";
    stdpLtpRule.type = "double";
    stdpLtpRule.validation = "range";
    stdpLtpRule.min = "0.0";
    stdpLtpRule.max = "1.0";
    stdpLtpRule.defaultValue = "0.02";
    stdpLtpRule.required = true;
    stdpLtpRule.description = "STDP Long-Term Potentiation weight factor";
    schema.rules.push_back(stdpLtpRule);
    
    ValidationRule stdpLtdRule;
    stdpLtdRule.key = "stdp_ltd_weight";
    stdpLtdRule.type = "double";
    stdpLtdRule.validation = "range";
    stdpLtdRule.min = "0.0";
    stdpLtdRule.max = "1.0";
    stdpLtdRule.defaultValue = "0.015";
    stdpLtdRule.required = true;
    stdpLtdRule.description = "STDP Long-Term Depression weight factor";
    schema.rules.push_back(stdpLtdRule);
    
    ValidationRule stdpTauRule;
    stdpTauRule.key = "stdp_tau";
    stdpTauRule.type = "double";
    stdpTauRule.validation = "range";
    stdpTauRule.min = "0.1";
    stdpTauRule.max = "1000.0";
    stdpTauRule.defaultValue = "20.0";
    stdpTauRule.required = true;
    stdpTauRule.description = "STDP time constant (ms)";
    schema.rules.push_back(stdpTauRule);
    
    ValidationRule synaptogenesisRule;
    synaptogenesisRule.key = "synaptogenesis_rate";
    synaptogenesisRule.type = "double";
    synaptogenesisRule.validation = "range";
    synaptogenesisRule.min = "0.0";
    synaptogenesisRule.max = "1.0";
    synaptogenesisRule.defaultValue = "0.0001";
    synaptogenesisRule.required = true;
    synaptogenesisRule.description = "Rate of new synapse formation";
    schema.rules.push_back(synaptogenesisRule);
    
    ValidationRule pruningRule;
    pruningRule.key = "pruning_rate";
    pruningRule.type = "double";
    pruningRule.validation = "range";
    pruningRule.min = "0.0";
    pruningRule.max = "1.0";
    pruningRule.defaultValue = "0.00001";
    pruningRule.required = true;
    pruningRule.description = "Rate of synapse removal";
    schema.rules.push_back(pruningRule);
    
    ValidationRule replayIntervalRule;
    replayIntervalRule.key = "replay_interval";
    replayIntervalRule.type = "int";
    replayIntervalRule.validation = "range";
    replayIntervalRule.min = "1";
    replayIntervalRule.max = "100000";
    replayIntervalRule.defaultValue = "100";
    replayIntervalRule.required = false;
    replayIntervalRule.description = "Steps between memory replay events";
    schema.rules.push_back(replayIntervalRule);
    
    ValidationRule consolidationIntervalRule;
    consolidationIntervalRule.key = "consolidation_interval";
    consolidationIntervalRule.type = "int";
    consolidationIntervalRule.validation = "range";
    consolidationIntervalRule.min = "1";
    consolidationIntervalRule.max = "1000000";
    consolidationIntervalRule.defaultValue = "1000";
    consolidationIntervalRule.required = false;
    consolidationIntervalRule.description = "Steps between memory consolidation events";
    schema.rules.push_back(consolidationIntervalRule);
    
    ValidationRule checkpointDirRule;
    checkpointDirRule.key = "checkpoint_dir";
    checkpointDirRule.type = "string";
    checkpointDirRule.validation = "pattern";
    checkpointDirRule.pattern = ".*";
    checkpointDirRule.defaultValue = "./checkpoints";
    checkpointDirRule.required = false;
    checkpointDirRule.description = "Directory for checkpoint files";
    schema.rules.push_back(checkpointDirRule);
    
    ValidationRule compressionLevelRule;
    compressionLevelRule.key = "compression_level";
    compressionLevelRule.type = "int";
    compressionLevelRule.validation = "range";
    compressionLevelRule.min = "0";
    compressionLevelRule.max = "9";
    compressionLevelRule.defaultValue = "5";
    compressionLevelRule.required = false;
    compressionLevelRule.description = "ZIP compression level (0-9)";
    schema.rules.push_back(compressionLevelRule);
}

bool Config::Impl::validateEntry(const ConfigEntry& entry, const ValidationRule& rule, ValidationResult& result) const {
    try {
        if (rule.type == "int") {
            if (!std::holds_alternative<int>(entry.value)) {
                result.addError("Configuration key '" + entry.key + "' expects int type, got " + getTypeName(entry.value));
                return false;
            }
            
            int value = std::get<int>(entry.value);
            if (rule.validation == "range") {
                try {
                    int minVal = std::stoi(rule.min);
                    int maxVal = std::stoi(rule.max);
                    if (value < minVal || value > maxVal) {
                        result.addError("Configuration key '" + entry.key + "' value " + std::to_string(value) + " outside range [" + rule.min + ", " + rule.max + "]");
                        return false;
                    }
                } catch (const std::exception&) {
                    result.addError("Invalid range specification for key '" + entry.key + "'");
                    return false;
                }
            }
        } else if (rule.type == "double") {
            if (!std::holds_alternative<double>(entry.value)) {
                result.addError("Configuration key '" + entry.key + "' expects double type, got " + getTypeName(entry.value));
                return false;
            }
            
            double value = std::get<double>(entry.value);
            if (rule.validation == "range") {
                try {
                    double minVal = std::stod(rule.min);
                    double maxVal = std::stod(rule.max);
                    if (value < minVal || value > maxVal) {
                        result.addError("Configuration key '" + entry.key + "' value " + std::to_string(value) + " outside range [" + rule.min + ", " + rule.max + "]");
                        return false;
                    }
                } catch (const std::exception&) {
                    result.addError("Invalid range specification for key '" + entry.key + "'");
                    return false;
                }
            }
        } else if (rule.type == "bool") {
            if (!std::holds_alternative<bool>(entry.value)) {
                result.addError("Configuration key '" + entry.key + "' expects bool type, got " + getTypeName(entry.value));
                return false;
            }
        } else if (rule.type == "string") {
            if (!std::holds_alternative<std::string>(entry.value)) {
                result.addError("Configuration key '" + entry.key + "' expects string type, got " + getTypeName(entry.value));
                return false;
            }
            
            if (rule.validation == "pattern") {
                std::string value = std::get<std::string>(entry.value);
                std::regex pattern(rule.pattern);
                if (!std::regex_match(value, pattern)) {
                    result.addError("Configuration key '" + entry.key + "' value '" + value + "' does not match pattern '" + rule.pattern + "'");
                    return false;
                }
            }
        }
        
        // Check enum values if specified
        if (!rule.enumValues.empty()) {
            if (rule.type == "int") {
                int value = std::get<int>(entry.value);
                if (rule.enumValues.find(std::to_string(value)) == rule.enumValues.end()) {
                    result.addError("Configuration key '" + entry.key + "' value " + std::to_string(value) + " not in allowed enum values");
                    return false;
                }
            } else if (rule.type == "string") {
                std::string value = std::get<std::string>(entry.value);
                if (rule.enumValues.find(value) == rule.enumValues.end()) {
                    result.addError("Configuration key '" + entry.key + "' value '" + value + "' not in allowed enum values");
                    return false;
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        result.addError("Exception during validation of key '" + entry.key + "': " + e.what());
        return false;
    }
}

std::string Config::Impl::getTypeName(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

void Config::Impl::addValidationRule(const ValidationRule& rule) {
    // Check if rule already exists
    for (auto it = schema.rules.begin(); it != schema.rules.end(); ++it) {
        if (it->key == rule.key) {
            *it = rule;
            return;
        }
    }
    schema.rules.push_back(rule);
}

// Config::Impl helper method implementations
bool Config::Impl::has(const std::vector<ConfigEntry>& entries, const std::string& key) {
    return std::any_of(entries.begin(), entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::string Config::Impl::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::Impl::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void Config::Impl::remove(std::vector<ConfigEntry>::iterator begin, std::vector<ConfigEntry>::iterator end,
    const std::function<bool(const ConfigEntry&)>& pred) {
    entries.erase(std::remove_if(begin, end, pred), end);
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {

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