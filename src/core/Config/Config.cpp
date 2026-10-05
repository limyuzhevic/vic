#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>  # Include JSON parser

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first (preferred format)
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse JSON with nlohmann library
        auto json = nlohmann::json::parse(file);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert JSON to config entries
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigSource source = ConfigSource::File;
            
            // Try to determine value type and set accordingly
            try {
                // Try different types in order of likelihood
                if (it->is_boolean()) {
                    set(key, it->get<bool>(), source);
                } else if (it->is_number_integer()) {
                    set(key, it->get<int64_t>(), source);
                } else if (it->is_number_float()) {
                    set(key, it->get<double>(), source);
                } else if (it->is_string()) {
                    set(key, it->get<std::string>(), source);
                } else if (it->is_array()) {
                    // Check array element type
                    auto arr = it->get<std::vector<nlohmann::json>>();
                    if (!arr.empty() && arr[0].is_number_integer()) {
                        std::vector<int> intVec;
                        intVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            intVec.push_back(item.get<int>());
                        }
                        set(key, intVec, source);
                    } else if (!arr.empty() && arr[0].is_number_float()) {
                        std::vector<double> doubleVec;
                        doubleVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            doubleVec.push_back(item.get<double>());
                        }
                        set(key, doubleVec, source);
                    } else if (!arr.empty() && arr[0].is_string()) {
                        std::vector<std::string> stringVec;
                        stringVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            stringVec.push_back(item.get<std::string>());
                        }
                        set(key, stringVec, source);
                    }
                }
            } catch (const std::exception& e) {
                // If conversion fails, log and continue with string representation
                NLM_LOG_ERROR("Failed to convert config value '" + key + "' from JSON: " + e.what());
                set(key, it->dump(), source);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        // Fallback to simple key=value format if JSON parsing fails
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
            return false;
        }
        
        // Clear existing entries
        pImpl->entries.clear();
        
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
                
                // Try to infer type from value string
                bool isNumber = true;
                float floatVal = 0.0f;
                bool isBool = false;
                
                // Check if it's a number
                try {
                    size_t pos2;
                    floatVal = std::stof(value, &pos2);
                    if (pos2 == value.size()) {
                        isNumber = true;
                    }
                } catch (...) {
                    isNumber = false;
                }
                
                // Check if it's a boolean
                if (!isNumber) {
                    if (value == "true" || value == "false") {
                        isBool = true;
                    }
                }
                
                if (isBool) {
                    set(key, value == "true", ConfigSource::File);
                } else if (isNumber) {
                    // Check if it's an integer or float
                    try {
                        size_t pos2;
                        int intVal = std::stoi(value, &pos2);
                        if (pos2 == value.size()) {
                            set(key, intVal, ConfigSource::File);
                        } else {
                            set(key, floatVal, ConfigSource::File);
                        }
                    } catch (...) {
                        // Keep as string if conversion fails
                        set(key, value, ConfigSource::File);
                    }
                } else {
                    set(key, value, ConfigSource::File);
                }
            }
        }
        
        if (!pImpl->entries.empty()) {
            NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to parse configuration file: " + filepath);
            return false;
        }
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple key=value format for now, but properly serialize values
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Properly serialize different value types
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
    
    return true;
}

// Configuration validation (new feature)
std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Basic validation rules
        if (entry.key.empty()) {
            errors.push_back("Empty configuration key");
            continue;
        }
        
        // Check type-specific validation
        std::visit([&errors, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                if (arg < -1000000 || arg > 1000000) {
                    errors.push_back("Configuration value for key '" + entry.key + "' is out of range: " + std::to_string(arg));
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::isnan(arg) || std::isinf(arg)) {
                    errors.push_back("Configuration value for key '" + entry.key + "' is not a valid number");
                } else if (arg < -1000000.0 || arg > 1000000.0) {
                    errors.push_back("Configuration value for key '" + entry.key + "' is out of range: " + std::to_string(arg));
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                // No validation needed for booleans
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (arg.empty()) {
                    errors.push_back("Configuration string value for key '" + entry.key + "' is empty");
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                if (arg.size() > 10000) {
                    errors.push_back("Configuration vector for key '" + entry.key + "' is too large: " + std::to_string(arg.size()) + " elements");
                }
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (arg[i] < -1000000 || arg[i] > 1000000) {
                        errors.push_back("Configuration element " + std::to_string(i) + " for key '" + entry.key + "' is out of range: " + std::to_string(arg[i]));
                        break;
                    }
                }
            }
        }, entry.value);
    }
    
    return errors;
}

// Load from JSON string
bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert JSON to config entries
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigSource source = ConfigSource::Runtime;  // From JSON string
            
            // Try to determine value type and set accordingly
            try {
                // Try different types in order of likelihood
                if (it->is_boolean()) {
                    set(key, it->get<bool>(), source);
                } else if (it->is_number_integer()) {
                    set(key, it->get<int64_t>(), source);
                } else if (it->is_number_float()) {
                    set(key, it->get<double>(), source);
                } else if (it->is_string()) {
                    set(key, it->get<std::string>(), source);
                } else if (it->is_array()) {
                    // Check array element type
                    auto arr = it->get<std::vector<nlohmann::json>>();
                    if (!arr.empty() && arr[0].is_number_integer()) {
                        std::vector<int> intVec;
                        intVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            intVec.push_back(item.get<int>());
                        }
                        set(key, intVec, source);
                    } else if (!arr.empty() && arr[0].is_number_float()) {
                        std::vector<double> doubleVec;
                        doubleVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            doubleVec.push_back(item.get<double>());
                        }
                        set(key, doubleVec, source);
                    } else if (!arr.empty() && arr[0].is_string()) {
                        std::vector<std::string> stringVec;
                        stringVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            stringVec.push_back(item.get<std::string>());
                        }
                        set(key, stringVec, source);
                    }
                }
            } catch (const std::exception& e) {
                // If conversion fails, log and continue with string representation
                NLM_LOG_ERROR("Failed to convert config value '" + key + "' from JSON: " + e.what());
                set(key, it->dump(), source);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON string");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration string: " + e.what());
        return false;
    }
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
    
    // Simple key=value format for now, but properly serialize values
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Properly serialize different value types
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
    
    return true;
}

// Configuration validation (new feature)
std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Basic validation rules
        if (entry.key.empty()) {
            errors.push_back("Empty configuration key");
            continue;
        }
        
        // Check type-specific validation
        std::visit([&errors, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                if (arg < -1000000 || arg > 1000000) {
                    errors.push_back("Configuration value for key '" + entry.key + "' is out of range: " + std::to_string(arg));
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::isnan(arg) || std::isinf(arg)) {
                    errors.push_back("Configuration value for key '" + entry.key + "' is not a valid number");
                } else if (arg < -1000000.0 || arg > 1000000.0) {
                    errors.push_back("Configuration value for key '" + entry.key + "' is out of range: " + std::to_string(arg));
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                // No validation needed for booleans
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (arg.empty()) {
                    errors.push_back("Configuration string value for key '" + entry.key + "' is empty");
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                if (arg.size() > 10000) {
                    errors.push_back("Configuration vector for key '" + entry.key + "' is too large: " + std::to_string(arg.size()) + " elements");
                }
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (arg[i] < -1000000 || arg[i] > 1000000) {
                        errors.push_back("Configuration element " + std::to_string(i) + " for key '" + entry.key + "' is out of range: " + std::to_string(arg[i]));
                        break;
                    }
                }
            }
        }, entry.value);
    }
    
    return errors;
}

// Load from JSON string
bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert JSON to config entries
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigSource source = ConfigSource::Runtime;  // From JSON string
            
            // Try to determine value type and set accordingly
            try {
                // Try different types in order of likelihood
                if (it->is_boolean()) {
                    set(key, it->get<bool>(), source);
                } else if (it->is_number_integer()) {
                    set(key, it->get<int64_t>(), source);
                } else if (it->is_number_float()) {
                    set(key, it->get<double>(), source);
                } else if (it->is_string()) {
                    set(key, it->get<std::string>(), source);
                } else if (it->is_array()) {
                    // Check array element type
                    auto arr = it->get<std::vector<nlohmann::json>>();
                    if (!arr.empty() && arr[0].is_number_integer()) {
                        std::vector<int> intVec;
                        intVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            intVec.push_back(item.get<int>());
                        }
                        set(key, intVec, source);
                    } else if (!arr.empty() && arr[0].is_number_float()) {
                        std::vector<double> doubleVec;
                        doubleVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            doubleVec.push_back(item.get<double>());
                        }
                        set(key, doubleVec, source);
                    } else if (!arr.empty() && arr[0].is_string()) {
                        std::vector<std::string> stringVec;
                        stringVec.reserve(arr.size());
                        for (const auto& item : arr) {
                            stringVec.push_back(item.get<std::string>());
                        }
                        set(key, stringVec, source);
                    }
                }
            } catch (const std::exception& e) {
                // If conversion fails, log and continue with string representation
                NLM_LOG_ERROR("Failed to convert config value '" + key + "' from JSON: " + e.what());
                set(key, it->dump(), source);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON string");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration string: " + e.what());
        return false;
    }
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
