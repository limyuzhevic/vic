#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check if it looks like JSON
    bool isJSON = false;
    if (!content.empty() && content.find('{') != std::string::npos) {
        // Simple JSON detection - check for typical JSON patterns
        isJSON = true;
        for (char c : content) {
            if (!std::isspace(c) && c != '{' && c != '}' && c != '[' && c != ']' &&
                c != '"' && c != ':' && c != ',' && !std::isdigit(c) && c != '.' && c != '-' && c != 't' && c != 'f' && c != 'n') {
                isJSON = false;
                break;
            }
        }
    }
    
    if (isJSON) {
        // Try to parse as JSON
        try {
            nlohmann::json jsonData = nlohmann::json::parse(content);
            
            // Apply JSON data to configuration
            for (auto& [key, value] : jsonData.items()) {
                if (value.is_number_integer()) {
                    set(key, value.get<int64_t>(), ConfigSource::File);
                } else if (value.is_number_float()) {
                    set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), ConfigSource::File);
                } else if (value.is_string()) {
                    set(key, value.get<std::string>(), ConfigSource::File);
                } else if (value.is_array()) {
                    // Handle different array types
                    if (!value.empty()) {
                        if (value[0].is_number_integer()) {
                            std::vector<int64_t> vec;
                            vec.reserve(value.size());
                            for (const auto& item : value) {
                                vec.push_back(item.get<int64_t>());
                            }
                            set(key, vec, ConfigSource::File);
                        } else if (value[0].is_number_float()) {
                            std::vector<double> vec;
                            vec.reserve(value.size());
                            for (const auto& item : value) {
                                vec.push_back(item.get<double>());
                            }
                            set(key, vec, ConfigSource::File);
                        } else if (value[0].is_string()) {
                            std::vector<std::string> vec;
                            vec.reserve(value.size());
                            for (const auto& item : value) {
                                vec.push_back(item.get<std::string>());
                            }
                            set(key, vec, ConfigSource::File);
                        }
                    }
                }
            }
            
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
            
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_WARNING("JSON parse error in config file " + filepath + ": " + e.what() + ". Falling back to simple format.");
            // Fall back to simple key=value format
            return loadFromFileSimple(filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + e.what() + ". Falling back to simple format.");
            return loadFromFileSimple(filepath);
        }
    }
    
    // Parse as simple key=value format
    return loadFromFileSimple(filepath);
}

bool Config::loadFromFileSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for reading: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNum;
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
            
            if (!parseSimpleKeyValue(line, key, value)) {
                NLM_LOG_WARNING("Could not parse config value for key '" + key + "' on line " + std::to_string(lineNum));
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Skipping invalid config line (no '=') on line " + std::to_string(lineNum) + ": " + line);
            success = false;
        }
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
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        // Write JSON formatted config
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&](const auto& value) {
                jsonData[entry.key] = value;
            }, entry.value);
        }
        
        file << jsonData.dump(4); // Pretty print with 4-space indentation
        
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save configuration to file " + filepath + ": " + e.what());
        return false;
    }
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
        std::visit([&oss](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) oss << ", ";
                    oss << value[i];
                }
                oss << "]";
            } else {
                oss << value;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::validate() const {
    bool isValid = true;
    
    // Check for critical required keys
    const std::vector<std::string> criticalKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    for (const auto& key : criticalKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing critical configuration key: " + key);
            isValid = false;
        }
    }
    
    // Validate specific value ranges
    if (has("random_seed")) {
        auto seed = get<int64_t>("random_seed");
        if (seed && seed < 0) {
            NLM_LOG_WARNING("random_seed should be non-negative");
            isValid = false;
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep && timestep <= 0.0) {
            NLM_LOG_WARNING("simulation_timestep should be positive");
            isValid = false;
        }
    }
    
    if (has("neuron_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        if (neuronCount && neuronCount <= 0) {
            NLM_LOG_WARNING("neuron_count should be positive");
            isValid = false;
        }
    }
    
    if (has("connection_probability")) {
        auto prob = get<double>("connection_probability");
        if (prob && (prob < 0.0 || prob > 1.0)) {
            NLM_LOG_WARNING("connection_probability should be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    return isValid;
}

bool Config::parseSimpleKeyValue(const std::string& line, std::string& key, const std::string& valueStr) const {
    ConfigValue value;
    if (parseSimpleValue(valueStr, value)) {
        set(key, value, ConfigSource::File);
        return true;
    }
    
    // If parsing failed, try as string
    set(key, valueStr, ConfigSource::File);
    return true; // Accept as string even if specific type parsing failed
}

bool Config::parseSimpleValue(const std::string& valueStr, ConfigValue& outValue) const {
    // Try to parse as boolean
    if (valueStr == "true") {
        outValue = true;
        return true;
    } else if (valueStr == "false") {
        outValue = false;
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t posEnd;
        int64_t i = std::stoll(valueStr, &posEnd);
        if (posEnd == valueStr.size()) {
            outValue = i;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as floating point
    try {
        size_t posEnd;
        double d = std::stod(valueStr, &posEnd);
        if (posEnd == valueStr.size()) {
            outValue = d;
            return true;
        }
    } catch (...) {}
    
    // Return as string
    outValue = valueStr;
    return true;
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    try {
        return std::get<T>(it->value);
    } catch (...) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

// Specializations for vector types
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string& key) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string& key) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string& key) const;

template std::vector<int> Config::getOr<std::vector<int>>(const std::string& key, const std::vector<int>& defaultValue) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string& key, const std::vector<double>& defaultValue) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string& key, const std::vector<std::string>& defaultValue) const;

} // namespace nlm
