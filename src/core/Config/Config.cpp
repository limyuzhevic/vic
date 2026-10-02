#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastError;
    size_t maxStringLength;
    std::set<std::string> validKeys;
    bool strictValidation;  // Enable strict validation of configuration
    size_t maxVectorSize;   // Maximum allowed size for vector values
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->maxStringLength = 1024;
    pImpl->strictValidation = true;
    pImpl->maxVectorSize = 1024;
    
    pImpl->validKeys = {
        "random_seed",
        "simulation_timestep", 
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_time_constant",
        "development_synaptogenesis_rate",
        "development_pruning_rate",
        "dopamine_baseline",
        "reward_discount_factor",
        "environment_name",
        "environment_width",
        "environment_height",
        "log_level",
        "log_to_file",
        "log_filename",
        "max_simulation_steps",
        "simulation_time_limit",
        "visualization_enabled",
        "visualization_update_rate",
        // Advanced options
        "advanced.enable_structural_plasticity",
        "advanced.enable_reward_modulation",
        "advanced.enable_development",
        "advanced.enable_curiosity",
        "advanced.enable_prediction",
        "advanced.enable_memory",
        "advanced.optimization_level",
        "advanced.parallel_processing",
        "advanced.simd_optimization",
        "advanced.use_memory_pools",
        "advanced.use_event_queue",
        "advanced.checkpoint_frequency",
        "advanced.enable_replay",
        "advanced.replay_ratio",
        "advanced.development_stage_transition",
        "advanced.critical_period_end",
        "advanced.plasticity_decay_rate",
        "advanced.reward_prediction_error",
        "advanced.curiosity_threshold",
        "advanced.novelty_detection_threshold",
        "advanced.noise_level",
        "advanced.learning_rate",
        "advanced.momentum",
        "advanced.weight_initialization",
        "advanced.connection_distribution",
        "advanced.synapse_type_distribution",
        "advanced.stdp_window_size",
        "advanced.stdp_learning_rate",
        "advanced.hebbian_learning_rate",
        "advanced.plasticity_modulation"
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check file extension to determine parsing method
    std::string ext = "";
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    // Try JSON parsing first for .json and .cfg files
    if (ext == "json" || ext == "cfg" || ext == "yaml" || ext == "yml") {
        return parseFromJSON(file);
    }
    
    // Fall back to simple key=value format for other files
    return parseFromKeyValue(file);
}

bool Config::parseFromKeyValue(std::ifstream& file) {
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

bool Config::parseFromJSON(std::ifstream& file) {
    try {
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonContent = buffer.str();
        
        // Simple JSON parsing for common patterns
        // Remove whitespace
        jsonContent.erase(remove_if(jsonContent.begin(), jsonContent.end(), ::isspace), jsonContent.end());
        
        // Check if it's a JSON object { ... }
        if (jsonContent.empty() || jsonContent[0] != '{') {
            return parseFromKeyValue(std::ifstream(file));
        }
        
        // Simple JSON object parsing (for configurations)
        size_t pos = 1;  // Skip '{'
        while (pos < jsonContent.length()) {
            // Find next key (string in quotes)
            size_t keyStart = jsonContent.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = jsonContent.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonContent.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 3;  // Skip ", "
            
            // Find value
            size_t valueEnd = findJSONValueEnd(jsonContent, pos);
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = jsonContent.substr(pos, valueEnd - pos);
            set(key, valueStr, ConfigSource::File);
            
            pos = valueEnd + 1;  // Skip comma or }
            if (pos < jsonContent.length() && jsonContent[pos] == ',') {
                pos++;  // Skip comma
                while (pos < jsonContent.length() && 
                      (jsonContent[pos] == ' ' || jsonContent[pos] == '\t')) {
                    pos++;  // Skip whitespace
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // If JSON parsing fails, fall back to key=value format
        return parseFromKeyValue(std::ifstream(file));
    }
}

// Helper function to find the end of a JSON value
static size_t findJSONValueEnd(const std::string& json, size_t start) {
    if (start >= json.length()) return std::string::npos;
    
    char c = json[start];
    
    if (c == '"') {
        // String value
        size_t end = json.find('"', start + 1);
        if (end == std::string::npos) return std::string::npos;
        return end;
    } else if (c == 't' || c == 'f') {
        // Boolean value
        if (json.substr(start, 4) == "true" || json.substr(start, 5) == "false") {
            return start + (json.substr(start, 4) == "true" ? 4 : 5);
        }
    } else if (c == 'n') {
        // Null value
        if (json.substr(start, 4) == "null") {
            return start + 4;
        }
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number value
        size_t end = start;
        while (end < json.length() && (json[end] == '.' || 
                                      (json[end] >= '0' && json[end] <= '9') ||
                                      json[end] == 'e' || json[end] == 'E' ||
                                      json[end] == '+' || json[end] == '-')) {
            end++;
        }
        return end;
    }
    
    return std::string::npos;
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
    if (start >= json.length()) return std::string::npos;
    
    char c = json[start];
    
    if (c == '"') {
        // String value
        size_t end = json.find('"', start + 1);
        if (end == std::string::npos) return std::string::npos;
        return end;
    } else if (c == 't' || c == 'f') {
        // Boolean value
        if (json.substr(start, 4) == "true" || json.substr(start, 5) == "false") {
            return start + (json.substr(start, 4) == "true" ? 4 : 5);
        }
    } else if (c == 'n') {
        // Null value
        if (json.substr(start, 4) == "null") {
            return start + 4;
        }
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number value
        size_t end = start;
        while (end < json.length() && (json[end] == '.' || 
                                      (json[end] >= '0' && json[end] <= '9') ||
                                      json[end] == 'e' || json[end] == 'E' ||
                                      json[end] == '+' || json[end] == '-')) {
            end++;
        }
        return end;
    }
    
    return std::string::npos;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clearError();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(arg.substr(2, pos - 2));
                std::string value = trim(arg.substr(pos + 1));
                set(key, value, ConfigSource::CommandLine);
            } else {
                setError("Invalid command line argument format: " + arg);
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle help flag
        else if (arg == "--help" || arg == "-help" || arg == "--h" || arg == "-h") {
            printHelp();
            return true; // Help was displayed
        }
        // Handle version flag
        else if (arg == "--version") {
            printVersion();
            return true; // Version was displayed
        }
        // Handle invalid arguments
        else {
            setError("Unknown command line argument: " + arg);
            return false;
        }
    }
    
    return true;
}

void Config::printHelp() const {
    std::cout << "NLM Configuration Help" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Command line argument format:" << std::endl;
    std::cout << "  --key=value" << std::endl;
    std::cout << "  -key value" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Common configuration options:" << std::endl;
    std::cout << "  --random_seed=<int>        Random seed for reproducible experiments" << std::endl;
    std::cout << "  --simulation_timestep=<float> Simulation timestep in seconds" << std::endl;
    std::cout << "  --neuron_count=<int>       Total number of neurons in the brain" << std::endl;
    std::cout << "  --region_count=<int>       Number of brain regions" << std::endl;
    std::cout << "  --connection_probability=<float> Connection probability for random connectivity" << std::endl;
    std::cout << "  --log_level=<level>        Log level (trace, debug, info, warn, error, critical)" << std::endl;
    std::cout << "  --log_to_file=<bool>       Enable/disable logging to file" << std::endl;
    std::cout << "  --max_simulation_steps=<int> Maximum number of simulation steps" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Special flags:" << std::endl;
    std::cout << "  --help, -h                 Display this help message" << std::endl;
    std::cout << "  --version                  Display version information" << std::endl;
    std::cout << std::endl;
}

void Config::printVersion() const {
    std::cout << "NLM (Neural Learning Machine) version 0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "An experimental artificial developmental brain." << std::endl;
    std::cout << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Use JSON format for better readability and interoperability
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        // Add comment with source
        if (!entry.description.empty()) {
            file << "  // " << entry.description << "\n";
        }
        
        // Write key-value pair
        file << "  \"" << entry.key << "\": ";
        
        // Convert ConfigValue to JSON representation
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || 
                                 std::is_same_v<T, int> ||
                                 std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";  // In a real implementation, would serialize vectors
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << val[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";  // In a real implementation, would serialize vectors
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << val[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";  // In a real implementation, would serialize vectors
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << val[j] << "\"";
                }
                file << "]";
            } else {
                file << "null";  // Unknown type
            }
        }, entry.value);
        
        // Add comma for all but last entry
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    
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

// Helper function to check if a string starts with another string
static bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

// Validation methods implementation
bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        setError("Key cannot be empty");
        return false;
    }
    
    if (key.length() > pImpl->maxStringLength) {
        setError("Key too long: " + key);
        return false;
    }
    
    // Check for valid characters (alphanumeric, underscores, dots, dashes)
    if (!std::all_of(key.begin(), key.end(), 
                    [](char c) { return std::isalnum(c) || c == '_' || c == '.' || c == '-'; })) {
        setError("Invalid characters in key: " + key);
        return false;
    }
    
    return true;
}

std::string Config::getLastError() const {
    return pImpl->lastError;
}

bool Config::validateConfiguration() const {
    clearError();
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            return false;
        }
        
        if (!validateValue(entry.key, entry.value)) {
            return false;
        }
    }
    
    return true;
}

void Config::setError(const std::string& error) {
    pImpl->lastError = error;
}

void Config::clearError() {
    pImpl->lastError.clear();
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Check if key is in valid keys list (if list is not empty)
    if (!pImpl->validKeys.empty()) {
        bool isValidKey = false;
        for (const auto& validKey : pImpl->validKeys) {
            if (key == validKey) {
                isValidKey = true;
                break;
            }
        }
        if (!isValidKey) {
            setError("Invalid configuration key: " + key);
            return false;
        }
    }
    
    // Validate specific values based on key
    if (key == "random_seed") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            setError("random_seed must be an integer");
            return false;
        }
    } else if (key == "simulation_timestep") {
        if (!std::holds_alternative<double>(value)) {
            setError("simulation_timestep must be a double");
            return false;
        }
        double timestep = std::get<double>(value);
        if (timestep <= 0.0) {
            setError("simulation_timestep must be positive");
            return false;
        }
    } else if (key == "neuron_count") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            setError("neuron_count must be an integer");
            return false;
        }
        int64_t count = std::holds_alternative<int64_t>(value) ? 
            std::get<int64_t>(value) : std::get<int>(value);
        if (count <= 0) {
            setError("neuron_count must be positive");
            return false;
        }
    } else if (key == "region_count") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            setError("region_count must be an integer");
            return false;
        }
        int64_t count = std::holds_alternative<int64_t>(value) ? 
            std::get<int64_t>(value) : std::get<int>(value);
        if (count <= 0) {
            setError("region_count must be positive");
            return false;
        }
    } else if (key == "connection_probability") {
        if (!std::holds_alternative<double>(value)) {
            setError("connection_probability must be a double");
            return false;
        }
        double prob = std::get<double>(value);
        if (prob < 0.0 || prob > 1.0) {
            setError("connection_probability must be between 0.0 and 1.0");
            return false;
        }
    } else if (key == "log_level") {
        if (!std::holds_alternative<std::string>(value)) {
            setError("log_level must be a string");
            return false;
        }
        std::string level = std::get<std::string>(value);
        static const std::set<std::string> validLevels = {"trace", "debug", "info", "warn", "error", "critical"};
        if (validLevels.find(toLower(level)) == validLevels.end()) {
            setError("Invalid log_level: " + level);
            return false;
        }
    } else if (key == "log_to_file") {
        if (!std::holds_alternative<bool>(value)) {
            setError("log_to_file must be a boolean");
            return false;
        }
    } else if (key == "max_simulation_steps") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            setError("max_simulation_steps must be an integer");
            return false;
        }
        int64_t steps = std::holds_alternative<int64_t>(value) ? 
            std::get<int64_t>(value) : std::get<int>(value);
        if (steps <= 0) {
            setError("max_simulation_steps must be positive");
            return false;
        }
    } else if (key == "visualization_update_rate") {
        if (!std::holds_alternative<double>(value)) {
            setError("visualization_update_rate must be a double");
            return false;
        }
        double rate = std::get<double>(value);
        if (rate <= 0.0) {
            setError("visualization_update_rate must be positive");
            return false;
        }
    } else if (startsWith(key, "advanced.")) {
        // Validate advanced configuration options
        std::string advancedKey = key.substr(8);  // Remove "advanced." prefix
        
        if (advancedKey == "enable_structural_plasticity" ||
            advancedKey == "enable_reward_modulation" ||
            advancedKey == "enable_development" ||
            advancedKey == "enable_curiosity" ||
            advancedKey == "enable_prediction" ||
            advancedKey == "enable_memory" ||
            advancedKey == "enable_replay") {
            if (!std::holds_alternative<bool>(value)) {
                setError(advancedKey + " must be a boolean");
                return false;
            }
        } else if (advancedKey == "optimization_level") {
            if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
                setError("optimization_level must be an integer");
                return false;
            }
            int64_t level = std::holds_alternative<int64_t>(value) ? 
                std::get<int64_t>(value) : std::get<int>(value);
            if (level < 0 || level > 3) {
                setError("optimization_level must be between 0 and 3");
                return false;
            }
        } else if (advancedKey == "parallel_processing" ||
                   advancedKey == "simd_optimization" ||
                   advancedKey == "use_memory_pools" ||
                   advancedKey == "use_event_queue" ||
                   advancedKey == "enable_replay") {
            if (!std::holds_alternative<bool>(value)) {
                setError(advancedKey + " must be a boolean");
                return false;
            }
        } else if (advancedKey == "checkpoint_frequency") {
            if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
                setError("checkpoint_frequency must be an integer");
                return false;
            }
            int64_t freq = std::holds_alternative<int64_t>(value) ? 
                std::get<int64_t>(value) : std::get<int>(value);
            if (freq <= 0) {
                setError("checkpoint_frequency must be positive");
                return false;
            }
        } else if (advancedKey == "replay_ratio") {
            if (!std::holds_alternative<double>(value)) {
                setError("replay_ratio must be a double");
                return false;
            }
            double ratio = std::get<double>(value);
            if (ratio < 0.0 || ratio > 1.0) {
                setError("replay_ratio must be between 0.0 and 1.0");
                return false;
            }
        } else if (advancedKey == "critical_period_end") {
            if (!std::holds_alternative<double>(value)) {
                setError("critical_period_end must be a double");
                return false;
            }
            double end = std::get<double>(value);
            if (end < 0.0 || end > 1.0) {
                setError("critical_period_end must be between 0.0 and 1.0");
                return false;
            }
        } else if (advancedKey == "plasticity_decay_rate") {
            if (!std::holds_alternative<double>(value)) {
                setError("plasticity_decay_rate must be a double");
                return false;
            }
            double rate = std::get<double>(value);
            if (rate < 0.0 || rate > 1.0) {
                setError("plasticity_decay_rate must be between 0.0 and 1.0");
                return false;
            }
        } else if (advancedKey == "curiosity_threshold") {
            if (!std::holds_alternative<double>(value)) {
                setError("curiosity_threshold must be a double");
                return false;
            }
            double threshold = std::get<double>(value);
            if (threshold < 0.0 || threshold > 1.0) {
                setError("curiosity_threshold must be between 0.0 and 1.0");
                return false;
            }
        } else if (advancedKey == "novelty_detection_threshold") {
            if (!std::holds_alternative<double>(value)) {
                setError("novelty_detection_threshold must be a double");
                return false;
            }
            double threshold = std::get<double>(value);
            if (threshold < 0.0 || threshold > 1.0) {
                setError("novelty_detection_threshold must be between 0.0 and 1.0");
                return false;
            }
        } else if (advancedKey == "noise_level") {
            if (!std::holds_alternative<double>(value)) {
                setError("noise_level must be a double");
                return false;
            }
            double noise = std::get<double>(value);
            if (noise < 0.0 || noise > 1.0) {
                setError("noise_level must be between 0.0 and 1.0");
                return false;
            }
        } else if (advancedKey == "learning_rate") {
            if (!std::holds_alternative<double>(value)) {
                setError("learning_rate must be a double");
                return false;
            }
            double rate = std::get<double>(value);
            if (rate <= 0.0) {
                setError("learning_rate must be positive");
                return false;
            }
        } else if (advancedKey == "momentum") {
            if (!std::holds_alternative<double>(value)) {
                setError("momentum must be a double");
                return false;
            }
            double momentum = std::get<double>(value);
            if (momentum < 0.0 || momentum > 1.0) {
                setError("momentum must be between 0.0 and 1.0");
                return false;
            }
        } else if (advancedKey == "stdp_window_size") {
            if (!std::holds_alternative<double>(value)) {
                setError("stdp_window_size must be a double");
                return false;
            }
            double window = std::get<double>(value);
            if (window <= 0.0) {
                setError("stdp_window_size must be positive");
                return false;
            }
        } else if (advancedKey == "development_stage_transition") {
            if (!std::holds_alternative<double>(value)) {
                setError("development_stage_transition must be a double");
                return false;
            }
            double transition = std::get<double>(value);
            if (transition < 0.0 || transition > 1.0) {
                setError("development_stage_transition must be between 0.0 and 1.0");
                return false;
            }
        }
    
    return true;
}
    if (start >= json.length()) return std::string::npos;
    
    char c = json[start];
    
    if (c == '"') {
        // String value
        size_t end = json.find('"', start + 1);
        if (end == std::string::npos) return std::string::npos;
        return end;
    } else if (c == 't' || c == 'f') {
        // Boolean value
        if (json.substr(start, 4) == "true" || json.substr(start, 5) == "false") {
            return start + (json.substr(start, 4) == "true" ? 4 : 5);
        }
    } else if (c == 'n') {
        // Null value
        if (json.substr(start, 4) == "null") {
            return start + 4;
        }
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number value
        size_t end = start;
        while (end < json.length() && (json[end] == '.' || 
                                      (json[end] >= '0' && json[end] <= '9') ||
                                      json[end] == 'e' || json[end] == 'E' ||
                                      json[end] == '+' || json[end] == '-')) {
            end++;
        }
        return end;
    }
    
    return std::string::npos;
}

// Explicit template instantiations
// ...

} // namespace nlm
