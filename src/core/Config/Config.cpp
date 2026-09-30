#ifdef _WIN32
    #include <windows.h>
#endif

#include <nlohmann/json.hpp>
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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    try {
        // Try to parse as JSON first
        nlohmann::json j;
        file >> j;
        
        clear();  // Clear existing entries
        
        // Parse JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = json_helpers::from_json(it.value());
                set(key, value, ConfigSource::File);
            }
            NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
            return true;
        }
        
        // If not JSON, try legacy key=value format
        file.clear();
        file.seekg(0);
        return loadFromLegacyFormat(filepath);
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_WARNING("JSON parse error in config file " + filepath + ": " + std::string(e.what()));
        // Try legacy format
        file.clear();
        file.seekg(0);
        return loadFromLegacyFormat(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromLegacyFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool success = true;
    
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
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            ConfigValue value;
            if (json_helpers::config_value_from_string(valueStr, value)) {
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING(std::string("Invalid value format in config file ") + 
                              filepath + std::string(" line ") + std::to_string(lineNumber) +
                              std::string(": ") + valueStr);
                success = false;
            }
        } else {
            NLM_LOG_WARNING(std::string("Invalid format in config file ") + 
                          filepath + std::string(" line ") + std::to_string(lineNumber) +
                          std::string(": missing '='"));
            success = false;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully loaded legacy configuration from: " + filepath);
    }
    return success;
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
        NLM_LOG_ERROR("Could not open config file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json j;
        
        for (const auto& entry : pImpl->entries) {
            j[entry.key] = json_helpers::to_json(entry.value);
        }
        
        file << std::setw(2) << j << std::endl;
        NLM_LOG_INFO("Successfully saved JSON configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error saving config file " + filepath + ": " + e.what()));
        return false;
    }
}

// JSON serialization helpers implementation
namespace json_helpers {
    nlohmann::json to_json(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return arg;
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return arg;
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type for JSON serialization");
            }
        }, value);
    }
    
    ConfigValue from_json(const nlohmann::json& j) {
        if (j.is_number_integer()) {
            return j.get<int64_t>();
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_array()) {
            // Try to infer array type
            if (!j.empty()) {
                if (j[0].is_number_integer()) {
                    std::vector<int64_t> vec = j.get<std::vector<int64_t>>();
                    return vec;
                } else if (j[0].is_number_float()) {
                    std::vector<double> vec = j.get<std::vector<double>>();
                    return vec;
                } else if (j[0].is_string()) {
                    std::vector<std::string> vec = j.get<std::vector<std::string>>();
                    return vec;
                }
            }
            return std::vector<int64_t>();
        }
        return std::string(); // Default empty string
    }
    
    std::string config_value_to_string(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double to remove trailing zeros
                std::ostringstream oss;
                oss << arg;
                std::string result = oss.str();
                // Remove trailing .0 for integers
                if (result.find('.') != std::string::npos && result.back() == '0') {
                    result.erase(result.find_last_not_of('0') + 1);
                    if (result.back() == '.') {
                        result.pop_back();
                    }
                }
                return result;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
                return oss.str();
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type for string conversion");
            }
        }, value);
    }
    
    bool config_value_from_string(const std::string& str, ConfigValue& value) {
        // Try to infer type from string
        if (str.empty()) {
            value = std::string();
            return true;
        }
        
        // Check if it's a boolean
        if (str == "true" || str == "false") {
            value = (str == "true");
            return true;
        }
        
        // Check if it's a number
        bool isInt = true;
        bool isFloat = true;
        for (char c : str) {
            if (c == '-' || c == '+' || c == '.') continue;
            if (!std::isdigit(c)) {
                isInt = false;
                isFloat = false;
                break;
            }
        }
        
        if (isInt) {
            try {
                int64_t intVal = std::stoll(str);
                value = intVal;
                return true;
            } catch (...) {
                return false;
            }
        }
        
        if (isFloat) {
            try {
                double floatVal = std::stod(str);
                value = floatVal;
                return true;
            } catch (...) {
                return false;
            }
        }
        
        // Check if it's a vector (starts with [ and ends with ])
        if (str.front() == '[' && str.back() == ']') {
            std::string content = str.substr(1, str.size() - 2);
            if (content.empty()) {
                value = std::vector<int64_t>();
                return true;
            }
            
            // Try to parse as integer vector
            std::vector<int64_t> intVec;
            std::stringstream ss(content);
            std::string item;
            bool allInts = true;
            
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (item.empty()) continue;
                if (item.front() == '\"' && item.back() == '\"') {
                    item = item.substr(1, item.size() - 2);
                }
                try {
                    int64_t val = std::stoll(item);
                    intVec.push_back(val);
                } catch (...) {
                    allInts = false;
                    break;
                }
            }
            
            if (allInts && !intVec.empty()) {
                value = intVec;
                return true;
            }
            
            // Try to parse as string vector
            std::vector<std::string> strVec;
            ss.clear();
            ss.str(content);
            bool allStrings = true;
            
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (item.empty()) continue;
                if (item.front() == '\"' && item.back() == '\"') {
                    item = item.substr(1, item.size() - 2);
                }
                strVec.push_back(item);
            }
            
            if (!strVec.empty()) {
                value = strVec;
                return true;
            }
        }
        
        // Default to string
        value = str;
        return true;
    }
}

// Keep the rest of the original Config methods
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
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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
// These would be implemented in a separate .cpp file

} // namespace nlm