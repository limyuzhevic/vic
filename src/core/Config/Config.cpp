#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, fall back to simple key=value format
    std::vector<ConfigEntry> entries;
    
    // Try JSON parsing
    if (parseJSONFile(filepath, entries)) {
        pImpl->entries = std::move(entries);
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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

bool Config::parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    // Check if file exists and has .json extension or content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON (simplified - in production use a proper JSON library)
    std::string line;
    std::string content;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    // Simple JSON parsing - look for key-value pairs in format "key": value
    // This is a simplified implementation - real JSON parsing would need a proper parser
    size_t pos = 0;
    while ((pos = content.find('"', pos)) != std::string::npos) {
        size_t keyEnd = content.find('"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace and colon
        while (pos < content.size() && (std::isspace(content[pos]) || content[pos] == ':')) pos++;
        if (pos >= content.size()) break;
        
        // Find the value (simplified - would need proper JSON parsing)
        size_t valuePos = content.find('"', pos);
        if (valuePos != std::string::npos) {
            // String value
            size_t valueEnd = content.find('"', valuePos + 1);
            if (valueEnd != std::string::npos) {
                std::string value = content.substr(valuePos + 1, valueEnd - valuePos - 1);
                entries.emplace_back(key, ConfigValue(value), ConfigSource::File);
            }
        } else {
            // Try numeric value
            size_t nextComma = content.find(',', pos);
            size_t nextBracket = content.find('}', pos);
            size_t nextBrace = content.find('}', pos);
            
            size_t endPos = std::min({nextComma, nextBracket, nextBrace, content.size()});
            std::string valueStr = content.substr(pos, endPos - pos);
            
            // Try to parse as int
            try {
                int intValue = std::stoi(valueStr);
                entries.emplace_back(key, ConfigValue(intValue), ConfigSource::File);
            } catch (...) {
                try {
                    double doubleValue = std::stod(valueStr);
                    entries.emplace_back(key, ConfigValue(doubleValue), ConfigSource::File);
                } catch (...) {
                    // Keep as string if not numeric
                    entries.emplace_back(key, ConfigValue(valueStr), ConfigSource::File);
                }
            }
        }
        
        pos = endPos + 1;
    }
    
    return !entries.empty();
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
    
    // Write JSON format for better compatibility
    file << "{" << std::endl;
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": " << valueToString(entry.value);
        if (i < pImpl->entries.size() - 1) {
            file << "," << std::endl;
        } else {
            file << std::endl;
        }
    }
    file << "}" << std::endl;
    
    return true;
}

std::string Config::valueToString(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else {
            return std::to_string(arg);
        }
    }, value);
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
    if (!validateKey(key)) {
        return; // Silently ignore invalid keys
    }
    
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

bool Config::validateKey(const std::string& key) {
    // Validate key format - should be alphanumeric with underscores
    if (key.empty()) return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    // Check for reserved keys
    static const std::set<std::string> reservedKeys = {
        "help", "version", "verbose", "quiet", "config"
    };
    
    if (reservedKeys.find(toLower(key)) != reservedKeys.end()) {
        return false;
    }
    
    return true;
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    // Validate required parameters
    if (!has("neuron_count")) {
        pImpl->validationErrors.push_back("Missing required parameter: neuron_count");
    } else if (!validateNeuronCount(getOr<int64_t>("neuron_count", 0))) {
        pImpl->validationErrors.push_back("Invalid neuron_count value");
    }
    
    if (!has("region_count")) {
        pImpl->validationErrors.push_back("Missing required parameter: region_count");
    } else if (!validateRegionCount(getOr<int64_t>("region_count", 0))) {
        pImpl->validationErrors.push_back("Invalid region_count value");
    }
    
    if (has("connection_probability")) {
        if (!validateConnectionProbability(getOr<float>("connection_probability", 0.0f))) {
            pImpl->validationErrors.push_back("Invalid connection_probability value");
        }
    }
    
    if (has("simulation_timestep")) {
        if (!validateTimeStep(getOr<double>("simulation_timestep", 0.0))) {
            pImpl->validationErrors.push_back("Invalid simulation_timestep value");
        }
    }
    
    if (has("stdp_ltp_weight") && has("stdp_ltd_weight") && has("stdp_tau")) {
        if (!validateSTDPParameters(
            getOr<float>("stdp_ltp_weight", 0.0f),
            getOr<float>("stdp_ltd_weight", 0.0f),
            getOr<float>("stdp_tau", 0.0f)
        )) {
            pImpl->validationErrors.push_back("Invalid STDP parameters");
        }
    }
    
    // Validate consistency between parameters
    if (has("neuron_count") && has("region_count")) {
        int64_t neurons = getOr<int64_t>("neuron_count", 0);
        int64_t regions = getOr<int64_t>("region_count", 0);
        if (neurons <= 0) {
            pImpl->validationErrors.push_back("neuron_count must be positive");
        }
        if (regions <= 0) {
            pImpl->validationErrors.push_back("region_count must be positive");
        }
        if (neurons < regions) {
            pImpl->validationErrors.push_back("neuron_count must be >= region_count");
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    if (pImpl->validationErrors.empty()) {
        validate(); // Update errors if not already done
    }
    return pImpl->validationErrors;
}

bool Config::validateNeuronCount(int64_t count) {
    return count > 0 && count <= 100000; // Reasonable upper bound
}

bool Config::validateRegionCount(int64_t regions) {
    return regions > 0 && regions <= 1000; // Reasonable upper bound
}

bool Config::validateConnectionProbability(float prob) {
    return prob >= 0.0f && prob <= 1.0f;
}

bool Config::validateTimeStep(double timestep) {
    return timestep > 0.0 && timestep <= 1.0; // Timestep in seconds
}

bool Config::validateSTDPParameters(float ltp, float ltd, float tau) {
    return ltp >= 0.0f && ltd >= 0.0f && tau > 0.0f && ltp != ltd;
}

bool Config::validatePlasticityRate(float rate) {
    return rate >= 0.0f && rate <= 1.0f;
}

void Config::setDefaults() {
    // Clear existing config
    clear();
    
    // Set Phase 2 default values
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
}

const std::unordered_map<std::string, std::string> Config::getConfigSchema() {
    static const std::unordered_map<std::string, std::string> schema = {
        {"random_seed", "int64_t"},
        {"simulation_timestep", "double"},
        {"neuron_count", "int64_t"},
        {"region_count", "int64_t"},
        {"connection_probability", "float"},
        {"plasticity_learning_rate", "float"},
        {"stdp_ltp_weight", "float"},
        {"stdp_ltd_weight", "float"},
        {"stdp_time_constant", "float"},
        {"development_synaptogenesis_rate", "float"},
        {"development_pruning_rate", "float"},
        {"dopamine_baseline", "float"},
        {"reward_discount_factor", "float"},
        {"environment_name", "string"},
        {"environment_width", "int64_t"},
        {"environment_height", "int64_t"},
        {"log_level", "string"},
        {"log_to_file", "bool"},
        {"log_filename", "string"},
        {"max_simulation_steps", "int64_t"},
        {"simulation_time_limit", "double"},
        {"visualization_enabled", "bool"},
        {"visualization_update_rate", "float"}
    };
    return schema;
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

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Internal helper function to convert argument value string to ConfigValue type
static ConfigValue stringToConfigValue(const std::string& valueStr) {
    // Try to parse as boolean
    if (valueStr == "true" || valueStr == "1" || valueStr == "yes" || valueStr == "on") {
        return true;
    }
    if (valueStr == "false" || valueStr == "0" || valueStr == "no" || valueStr == "off") {
        return false;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        long longVal = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            return longVal;
        }
    } catch (...) {}
    
    // Try to parse as unsigned integer
    try {
        size_t pos;
        unsigned long ulongVal = std::stoul(valueStr, &pos);
        if (pos == valueStr.size()) {
            return static_cast<int64_t>(ulongVal);
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string if all parsing fails
    return valueStr;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Original simple argument parsing
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

bool Config::loadFromArgsEnhanced(int argc, char** argv, const std::unordered_map<std::string, std::string>& validKeys) {
    // Clear any existing command line arguments
    clear();
    setDefaults();  // Start with defaults
    
    // Map of valid keys to their types for validation
    std::unordered_map<std::string, std::string> normalizedKeys;
    for (const auto& pair : validKeys) {
        normalizedKeys[normalizeKey(pair.first)] = pair.first;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                std::string normalizedKey = normalizeKey(key);
                if (normalizedKeys.find(normalizedKey) != normalizedKeys.end()) {
                    // Validate the value before setting
                    if (validateValue(key, value)) {
                        set(key, value, ConfigSource::CommandLine);
                    } else {
                        std::cerr << "Warning: Invalid value '" << value << "' for key '--" << key << "'" << std::endl;
                    }
                } else {
                    std::cerr << "Warning: Unknown key '--" << key << "'" << std::endl;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            std::string normalizedKey = normalizeKey(key);
            if (normalizedKeys.find(normalizedKey) != normalizedKeys.end()) {
                if (validateValue(key, value)) {
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    std::cerr << "Warning: Invalid value '" << value << "' for key '-" << key << "'" << std::endl;
                }
            } else {
                std::cerr << "Warning: Unknown key '-" << key << "'" << std::endl;
            }
        }
        // Handle --help and --version
        else if (arg == "--help") {
            printHelp("nlm");
            return true; // Exit after showing help
        }
        else if (arg == "--version") {
            printVersion();
            return true; // Exit after showing version
        }
        else if (arg == "--config-template") {
            printConfigurationTemplate();
            return true; // Exit after showing template
        }
    }
    
    return true;
}

void Config::printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << "  --config-template         Show configuration template" << std::endl;
    std::cout << "  --config=<file>           Load configuration from file" << std::endl;
    std::cout << "  -c <file>                Short form for --config" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Keys:" << std::endl;
    
    const auto& schema = getConfigSchema();
    for (const auto& pair : schema) {
        std::cout << "  --" << pair.first << "=<value>      " << pair.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --neuron_count=1000 --simulation_timestep=0.01" << std::endl;
    std::cout << "  " << programName << " -c configs/myconfig.cfg -stdp_ltp_weight=0.03" << std::endl;
    std::cout << std::endl;
}

void Config::printVersion() {
    std::cout << "NLM Phase 2 Real Neural Computation" << std::endl;
    std::cout << "Version: 0.2.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neuron dynamics" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
}

void Config::printConfigurationTemplate(const std::string& filepath) {
    std::cout << "Configuration Template:" << std::endl;
    std::cout << "Copy this to " << filepath << " and customize as needed:" << std::endl;
    std::cout << std::endl;
    
    const auto& schema = getConfigSchema();
    for (const auto& pair : schema) {
        std::cout << pair.first << " = " << pair.second << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Data types:" << std::endl;
    std::cout << "  int64_t, double, float, bool, string" << std::endl;
    std::cout << std::endl;
    std::cout << "Example values:" << std::endl;
    std::cout << "  random_seed = 42" << std::endl;
    std::cout << "  neuron_count = 500" << std::endl;
    std::cout << "  simulation_timestep = 0.001" << std::endl;
    std::cout << "  connection_probability = 0.15" << std::endl;
    std::cout << "  stdp_ltp_weight = 0.02" << std::endl;
    std::cout << "  stdp_ltd_weight = 0.015" << std::endl;
    std::cout << "  stdp_tau = 20.0" << std::endl;
}

std::string Config::normalizeKey(const std::string& key) {
    std::string normalized = toLower(key);
    // Remove common prefixes
    if (normalized.substr(0, 2) == "--") {
        normalized = normalized.substr(2);
    } else if (normalized.substr(0, 1) == "-") {
        normalized = normalized.substr(1);
    }
    return normalized;
}

bool Config::validateValue(const std::string& key, const std::string& value) {
    // Basic validation - check for empty values
    if (value.empty()) {
        return false;
    }
    
    // Key-specific validation
    if (key == "neuron_count" || key == "region_count") {
        try {
            int64_t val = std::stoll(value);
            return validateNeuronCount(val);
        } catch (...) {
            return false;
        }
    }
    
    if (key == "connection_probability" || key == "plasticity_learning_rate" || 
        key == "stdp_ltp_weight" || key == "stdp_ltd_weight" ||
        key == "synaptogenesis_rate" || key == "pruning_rate" ||
        key == "dopamine_baseline" || key == "reward_discount_factor") {
        try {
            float val = std::stof(value);
            return validatePlasticityRate(val);
        } catch (...) {
            return false;
        }
    }
    
    if (key == "simulation_timestep" || key == "simulation_time_limit" ||
        key == "stdp_tau") {
        try {
            double val = std::stod(value);
            return validateTimeStep(val);
        } catch (...) {
            return false;
        }
    }
    
    if (key == "environment_width" || key == "environment_height") {
        try {
            int64_t val = std::stoll(value);
            return val > 0 && val <= 1000;
        } catch (...) {
            return false;
        }
    }
    
    if (key == "log_level") {
        return value == "FATAL" || value == "ERROR" || value == "WARNING" ||
               value == "INFO" || value == "DEBUG" || value == "TRACE" ||
               value == "silent" || value == "fatal" || value == "error" ||
               value == "warning" || value == "notice" || value == "info" ||
               value == "debug" || value == "trace";
    }
    
    if (key == "visualization_enabled" || key == "log_to_file") {
        return value == "true" || value == "false" || value == "1" || value == "0";
    }
    
    // For string values, just check they're not empty
    return true;
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    
    for (const auto& entry : pImpl->entries) {
        stats.totalEntries++;
        stats.entriesByKey[entry.key]++;
        
        switch (entry.source) {
            case ConfigSource::File:
                stats.fileEntries++;
                break;
            case ConfigSource::CommandLine:
                stats.commandLineEntries++;
                break;
            case ConfigSource::Runtime:
                stats.runtimeEntries++;
                break;
            case ConfigSource::Default:
                // Count as file for stats purposes
                stats.fileEntries++;
                break;
        }
    }
    
    return stats;
}

} // namespace nlm
