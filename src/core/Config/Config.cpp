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
    // Phase 2: Implement proper JSON/YAML parser
    // Support multiple formats: .cfg (simple), .json, .yaml, .yml
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Determine file format from extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    if (ext == ".json") {
        success = loadFromJSON(file);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAML(file);
    } else {
        // Default to simple key=value format
        success = loadFromSimpleFormat(file);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to parse config file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromJSON(std::istream& stream) {
    // TODO: Implement JSON parser
    // For now, fall back to simple format
    stream.clear();
    stream.seekg(0, std::ios::beg);
    return loadFromSimpleFormat(stream);
}

bool Config::loadFromYAML(std::istream& stream) {
    // TODO: Implement YAML parser
    // For now, fall back to simple format
    stream.clear();
    stream.seekg(0, std::ios::beg);
    return loadFromSimpleFormat(stream);
}

bool Config::loadFromSimpleFormat(std::istream& stream) {
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        
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
            
            // Handle quoted values
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on content
            setParsedValue(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}

void Config::setParsedValue(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Auto-detect value type and set appropriately
    if (valueStr.empty()) {
        set(key, std::string(), source);
        return;
    }
    
    // Try to parse as boolean
    if (valueStr == "true" || valueStr == "false") {
        set(key, valueStr == "true", source);
        return;
    }
    
    // Try to parse as integer (decimal)
    try {
        size_t pos;
        int intVal = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, intVal, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as integer64
    try {
        size_t pos;
        int64_t int64Val = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, int64Val, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, doubleVal, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as vector of integers
    if (valueStr.find(',') != std::string::npos) {
        std::vector<int> intVec;
        std::stringstream ss(valueStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try {
                intVec.push_back(std::stoi(item));
            } catch (...) {
                intVec.clear();
                break;
            }
        }
        if (!intVec.empty()) {
            set(key, intVec, source);
            return;
        }
    }
    
    // Try to parse as vector of doubles
    if (valueStr.find(',') != std::string::npos && 
        (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos)) {
        std::vector<double> doubleVec;
        std::stringstream ss(valueStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try {
                doubleVec.push_back(std::stod(item));
            } catch (...) {
                doubleVec.clear();
                break;
            }
        }
        if (!doubleVec.empty()) {
            set(key, doubleVec, source);
            return;
        }
    }
    
    // Default to string
    set(key, valueStr, source);
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File (generated)" << std::endl;
    file << "# Format: key = value (type auto-detected)" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
        file << entry.key << " = " << valueToString(entry.value) << std::endl;
        file << std::endl;
    }
    
    return true;
}

void Config::setParsedValue(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Auto-detect value type and set appropriately
    if (valueStr.empty()) {
        set(key, std::string(), source);
        return;
    }
    
    // Try to parse as boolean
    if (valueStr == "true" || valueStr == "false") {
        set(key, valueStr == "true", source);
        return;
    }
    
    // Try to parse as integer (decimal)
    try {
        size_t pos;
        int intVal = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, intVal, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as integer64
    try {
        size_t pos;
        int64_t int64Val = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, int64Val, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, doubleVal, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as vector of integers
    if (valueStr.find(',') != std::string::npos) {
        std::vector<int> intVec;
        std::stringstream ss(valueStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try {
                intVec.push_back(std::stoi(item));
            } catch (...) {
                intVec.clear();
                break;
            }
        }
        if (!intVec.empty()) {
            set(key, intVec, source);
            return;
        }
    }
    
    // Try to parse as vector of doubles
    if (valueStr.find(',') != std::string::npos && 
        (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos)) {
        std::vector<double> doubleVec;
        std::stringstream ss(valueStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try {
                doubleVec.push_back(std::stod(item));
            } catch (...) {
                doubleVec.clear();
                break;
            }
        }
        if (!doubleVec.empty()) {
            set(key, doubleVec, source);
            return;
        }
    }
    
    // Default to string
    set(key, valueStr, source);
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

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i]; if (i < arg.size() - 1) oss << ",";
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i]; if (i < arg.size() - 1) oss << ",";
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                oss << "\"" << arg[i] << "\""; if (i < arg.size() - 1) oss << ",";
            } oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}
