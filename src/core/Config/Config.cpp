#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include "core/Logger/Logger.hpp"

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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            // Handle different value formats based on key patterns
            if (key.find("vector") != std::string::npos || 
                key.find("list") != std::string::npos) {
                // Try to parse as vector (e.g., "[1, 2, 3]" or "1,2,3")
                if (!value.empty() && value.front() == '[' && value.back() == ']') {
                    std::string inner = value.substr(1, value.size() - 2);
                    std::vector<int> intVec;
                    std::vector<double> doubleVec;
                    bool isIntVector = true;
                    
                    std::istringstream iss(inner);
                    std::string token;
                    while (std::getline(iss, token, ',')) {
                        token = trim(token);
                        if (token.empty()) continue;
                        
                        try {
                            int intVal = std::stoi(token);
                            intVec.push_back(intVal);
                        } catch (...) {
                            try {
                                double doubleVal = std::stod(token);
                                doubleVec.push_back(doubleVal);
                                isIntVector = false;
                            } catch (...) {
                                NLM_LOG_WARN("Failed to parse vector value for key '" + key + "': '" + token + "' at line " + std::to_string(lineNumber));
                                break;
                            }
                        }
                    }
                    
                    if (!intVec.empty()) {
                        if (isIntVector) {
                            set(key, intVec, ConfigSource::File);
                        } else {
                            set(key, doubleVec, ConfigSource::File);
                        }
                    }
                } else {
                    // Try to parse as comma-separated values
                    std::vector<std::string> stringVec;
                    std::istringstream iss(value);
                    std::string token;
                    while (std::getline(iss, token, ',')) {
                        token = trim(token);
                        if (!token.empty()) {
                            stringVec.push_back(token);
                        }
                    }
                    if (!stringVec.empty()) {
                        set(key, stringVec, ConfigSource::File);
                    } else {
                        set(key, value, ConfigSource::File);
                    }
                }
            } else if (key.find("int") != std::string::npos) {
                // Try to parse as integer
                try {
                    int intVal = std::stoi(value);
                    set(key, intVal, ConfigSource::File);
                } catch (...) {
                    try {
                        double doubleVal = std::stod(value);
                        set(key, doubleVal, ConfigSource::File);
                    } catch (...) {
                        NLM_LOG_WARN("Failed to parse integer value for key '" + key + "': '" + value + "' at line " + std::to_string(lineNumber));
                        set(key, value, ConfigSource::File);
                    }
                }
            } else if (key.find("bool") != std::string::npos) {
                // Try to parse as boolean
                bool boolVal = (value == "true" || value == "1" || value == "yes");
                set(key, boolVal, ConfigSource::File);
            } else if (key.find("float") != std::string::npos || 
                       key.find("double") != std::string::npos ||
                       key.find("real") != std::string::npos) {
                // Try to parse as double
                try {
                    double doubleVal = std::stod(value);
                    set(key, doubleVal, ConfigSource::File);
                } catch (...) {
                    NLM_LOG_WARN("Failed to parse numeric value for key '" + key + "': '" + value + "' at line " + std::to_string(lineNumber));
                    set(key, value, ConfigSource::File);
                }
            } else {
                // Try to parse as numeric first, fall back to string
                try {
                    int intVal = std::stoi(value);
                    set(key, intVal, ConfigSource::File);
                } catch (...) {
                    try {
                        double doubleVal = std::stod(value);
                        set(key, doubleVal, ConfigSource::File);
                    } catch (...) {
                        set(key, value, ConfigSource::File);
                    }
                }
            }
        } else {
            NLM_LOG_WARN("Invalid config line (no '='): " + line + " at line " + std::to_string(lineNumber));
        }
    }
    
    file.close();
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_DEBUG("Loading configuration from " + std::to_string(argc) + " arguments");
    
    // Simple argument parsing - could be enhanced with proper argument parsing library
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Loaded from args: --" + key + "=" + value);
            } else {
                NLM_LOG_WARN("Invalid argument format (missing '='): " + arg);
                NLM_LOG_INFO("Use format: --key=value");
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_DEBUG("Loaded from args: -" + key + " " + value);
        } else if (arg[0] == '-') {
            NLM_LOG_WARN("Invalid argument format (missing value): " + arg);
            NLM_LOG_INFO("Use format: -key value or --key=value");
        } else if (arg == "--help" || arg == "-h") {
            // Print help message
            NLM_LOG_INFO("NLM Configuration Help:");
            NLM_LOG_INFO("  --help, -h: Show this help message");
            NLM_LOG_INFO("  --config=file: Load configuration from file");
            NLM_LOG_INFO("  --key=value: Set configuration key to value");
            NLM_LOG_INFO("  -k value: Set configuration key to value (short form)");
            NLM_LOG_INFO("\nCommon configuration keys:");
            NLM_LOG_INFO("    --random_seed=42: Set random seed");
            NLM_LOG_INFO("    --neuron_count=500: Set number of neurons");
            NLM_LOG_INFO("    --connection_probability=0.15: Set connection probability");
            NLM_LOG_INFO("    --stdp_ltp_weight=0.02: Set STDP LTP weight");
            NLM_LOG_INFO("    --log_level=DEBUG: Set logging level (DEBUG, INFO, WARN, ERROR");
            NLM_LOG_INFO("    --visualization_enabled=true: Enable visualization");
            return false; // Exit after showing help
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write the actual value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
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
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file.flush();
    if (!file.good()) {
        NLM_LOG_ERROR("Error writing to file: " + filepath);
        return false;
    }
    
    return true;
}

// Type name for error reporting
template<typename T>
static std::string getTypeName() {
    if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else if constexpr (std::is_same_v<T, bool>) return "bool";
    else if constexpr (std::is_same_v<T, std::string>) return "string";
    else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
    else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
    else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
    else return "unknown";
}

std::string getTypeName(const ConfigValue& value) {
    std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "unknown";
    }, value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_WARN("Using default value for key '" + key + "': " + 
                    std::to_string(defaultValue) + " (key not found)");
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added config key: " + key);
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
