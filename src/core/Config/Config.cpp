#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format based on extension and content
    bool isJSON = false;
    bool isYAML = false;
    
    // Check file extension
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    // Convert to lowercase for comparison
    std::string extLower = ext;
    std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
    
    isJSON = (extLower == ".json");
    isYAML = (extLower == ".yaml" || extLower == ".yml");
    
    // For unknown extensions, try to detect from content
    if (!isJSON && !isYAML) {
        // Peek at first line to determine format
        std::ifstream peekFile(filepath);
        if (peekFile.is_open()) {
            std::string firstLine;
            std::getline(peekFile, firstLine);
            peekFile.close();
            
            // Check for JSON characteristics
            if (firstLine.find('{') != std::string::npos || 
                (firstLine.find('"') != std::string::npos && firstLine.find(':') != std::string::npos)) {
                isJSON = true;
            }
            // Check for YAML characteristics
            else if (firstLine.find('- ') != std::string::npos || 
                     firstLine.find(": ") != std::string::npos) {
                isYAML = true;
            }
            // Default to simple format
        }
    }
    
    // Try JSON format first if supported
    if (isJSON) {
        NLM_LOG_INFO("Attempting to load JSON configuration from: " + filepath);
        // TODO: Implement JSON parser (using nlohmann/json or similar)
        // For now, fall back to simple format
        NLM_LOG_WARNING("JSON parsing not implemented, falling back to simple format");
    }
    
    // Try YAML format if supported  
    if (!isJSON && isYAML) {
        NLM_LOG_INFO("Attempting to load YAML configuration from: " + filepath);
        // TODO: Implement YAML parser (using YAML-cpp or similar)
        // For now, fall back to simple format
        NLM_LOG_WARNING("YAML parsing not implemented, falling back to simple format");
    }
    
    // Fall back to simple key=value format (original implementation)
    NLM_LOG_INFO("Loading configuration using simple key=value format: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid line " + std::to_string(lineNum) + " in config file (no '='): " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing line " + std::to_string(lineNum) + " in config file: " + e.what());
            NLM_LOG_ERROR("Line: " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries from " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            std::string key = arg.substr(2, arg.find('=') - 2);
            std::string value = arg.substr(arg.find('=') + 1);
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing command line argument --" + key + "=" + value + ": " + e.what());
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing command line argument -" + key + " " + value + ": " + e.what());
                return false;
            }
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
        // Write comment with description if available
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write the key=value pair with actual value
        file << entry.key << " = ";
        
        // Use appropriate formatting based on value type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << "\", \"";
                    file << arg[i];
                }
                file << "\"]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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
    // Validate configuration key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Validate configuration value based on key
    validateConfigKey(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, getDefaultDescription(key));
    }
}

void Config::validateConfigKey(const std::string& key, const ConfigValue& value) {
    // Common validation rules for known configuration keys
    if (key == "random_seed") {
        try {
            int64_t seed = std::get<int64_t>(value);
            if (seed < 0) {
                throw std::invalid_argument("random_seed must be non-negative");
            }
        } catch (const std::bad_variant_access&) {
            // If it's a string, try to parse
            try {
                size_t pos;
                int64_t seed = std::stoll(key);
                if (seed < 0) {
                    throw std::invalid_argument("random_seed must be non-negative");
                }
            } catch (...) {
                throw std::invalid_argument("random_seed must be a valid integer");
            }
        }
    }
    
    else if (key == "neuron_count" || key == "region_count") {
        try {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0) {
                throw std::invalid_argument(key + " must be positive");
            }
            if (count > 1000000) {
                throw std::invalid_argument(key + " cannot exceed 1,000,000");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument(key + " must be a valid integer");
        }
    }
    
    else if (key == "connection_probability" || key == "plasticity_learning_rate" || 
             key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || 
             key == "stdp_tau" || key == "synaptogenesis_rate" || 
             key == "pruning_rate" || key == "reward_discount_factor" || 
             key == "dopamine_baseline" || key == "simulation_timestep" ||
             key == "visualization_update_rate") {
        try {
            float floatVal = std::get<float>(value);
            if (floatVal < 0.0f) {
                throw std::invalid_argument(key + " must be non-negative");
            }
        } catch (const std::bad_variant_access&) {
            try {
                double doubleVal = std::get<double>(value);
                if (doubleVal < 0.0) {
                    throw std::invalid_argument(key + " must be non-negative");
                }
            } catch (const std::bad_variant_access&) {
                throw std::invalid_argument(key + " must be a valid number");
            }
        }
    }
    
    else if (key == "log_level") {
        try {
            std::string level = std::get<std::string>(value);
            std::transform(level.begin(), level.end(), level.begin(), ::toupper);
            if (level != "DEBUG" && level != "INFO" && level != "WARNING" && level != "ERROR") {
                throw std::invalid_argument("log_level must be one of: DEBUG, INFO, WARNING, ERROR");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument("log_level must be a string");
        }
    }
    
    else if (key == "checkpoint_dir" || key == "log_filename") {
        try {
            std::string path = std::get<std::string>(value);
            // Basic path validation - check for empty path
            if (path.empty()) {
                throw std::invalid_argument(key + " cannot be empty");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument(key + " must be a string");
        }
    }
}

std::string Config::getDefaultDescription(const std::string& key) {
    static const std::unordered_map<std::string, std::string> descriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Global plasticity learning rate"},
        {"stdp_ltp_weight", "STDP LTP weight"},
        {"stdp_ltd_weight", "STDP LTD weight"},
        {"stdp_tau", "STDP time constant"},
        {"synaptogenesis_rate", "Structural synaptogenesis rate"},
        {"pruning_rate", "Structural pruning rate"},
        {"dopamine_baseline", "Dopamine baseline value"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Log level (DEBUG, INFO, WARNING, ERROR)"},
        {"log_to_file", "Whether to log to file"},
        {"log_filename", "Log filename"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"simulation_time_limit", "Simulation time limit in seconds"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate"},
        {"checkpoint_dir", "Checkpoint directory path"},
        {"replay_interval", "Replay interval"},
        {"consolidation_interval", "Memory consolidation interval"},
    };
    
    auto it = descriptions.find(key);
    return it != descriptions.end() ? it->second : "No description available";
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void Config::validateConfigKey(const std::string& key, const ConfigValue& value) {
    // Validate configuration key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Common validation rules for known configuration keys
    if (key == "random_seed") {
        try {
            int64_t seed = std::get<int64_t>(value);
            if (seed < 0) {
                throw std::invalid_argument("random_seed must be non-negative");
            }
        } catch (const std::bad_variant_access&) {
            // If it's a string, try to parse
            try {
                size_t pos;
                int64_t seed = std::stoll(key);
                if (seed < 0) {
                    throw std::invalid_argument("random_seed must be non-negative");
                }
            } catch (...) {
                throw std::invalid_argument("random_seed must be a valid integer");
            }
        }
    }
    
    else if (key == "neuron_count" || key == "region_count") {
        try {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0) {
                throw std::invalid_argument(key + " must be positive");
            }
            if (count > 1000000) {
                throw std::invalid_argument(key + " cannot exceed 1,000,000");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument(key + " must be a valid integer");
        }
    }
    
    else if (key == "connection_probability" || key == "plasticity_learning_rate" || 
             key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || 
             key == "stdp_tau" || key == "synaptogenesis_rate" || 
             key == "pruning_rate" || key == "reward_discount_factor" || 
             key == "dopamine_baseline" || key == "simulation_timestep" ||
             key == "visualization_update_rate") {
        try {
            float floatVal = std::get<float>(value);
            if (floatVal < 0.0f) {
                throw std::invalid_argument(key + " must be non-negative");
            }
        } catch (const std::bad_variant_access&) {
            try {
                double doubleVal = std::get<double>(value);
                if (doubleVal < 0.0) {
                    throw std::invalid_argument(key + " must be non-negative");
                }
            } catch (const std::bad_variant_access&) {
                throw std::invalid_argument(key + " must be a valid number");
            }
        }
    }
    
    else if (key == "log_level") {
        try {
            std::string level = std::get<std::string>(value);
            std::transform(level.begin(), level.end(), level.begin(), ::toupper);
            if (level != "DEBUG" && level != "INFO" && level != "WARNING" && level != "ERROR") {
                throw std::invalid_argument("log_level must be one of: DEBUG, INFO, WARNING, ERROR");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument("log_level must be a string");
        }
    }
    
    else if (key == "checkpoint_dir" || key == "log_filename") {
        try {
            std::string path = std::get<std::string>(value);
            // Basic path validation - check for empty path
            if (path.empty()) {
                throw std::invalid_argument(key + " cannot be empty");
            }
        } catch (const std::bad_variant_access&) {
            throw std::invalid_argument(key + " must be a string");
        }
    }
}

std::string Config::getDefaultDescription(const std::string& key) {
    static const std::unordered_map<std::string, std::string> descriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Global plasticity learning rate"},
        {"stdp_ltp_weight", "STDP LTP weight"},
        {"stdp_ltd_weight", "STDP LTD weight"},
        {"stdp_tau", "STDP time constant"},
        {"synaptogenesis_rate", "Structural synaptogenesis rate"},
        {"pruning_rate", "Structural pruning rate"},
        {"dopamine_baseline", "Dopamine baseline value"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Log level (DEBUG, INFO, WARNING, ERROR)"},
        {"log_to_file", "Whether to log to file"},
        {"log_filename", "Log filename"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"simulation_time_limit", "Simulation time limit in seconds"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate"},
        {"checkpoint_dir", "Checkpoint directory path"},
        {"replay_interval", "Replay interval"},
        {"consolidation_interval", "Memory consolidation interval"},
    };
    
    auto it = descriptions.find(key);
    return it != descriptions.end() ? it->second : "No description available";
}

// Explicit template instantiations
