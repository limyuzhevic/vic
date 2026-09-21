#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>

using namespace nlm;

// Implementation of is_vector helper
template<typename T>
struct is_vector_helper {
    template<typename U>
    static auto test(U&& u) -> decltype(std::declval<U>().begin(), std::declval<U>().end(), std::true_type{});
    
    template<typename U>
    static auto test(...) -> std::false_type;
    
    static constexpr bool value = std::is_same_v<decltype(test<T>(std::declval<T>())), std::true_type>;
};

template<typename T>
constexpr bool is_vector(const ConfigValue& value) {
    return is_vector_helper<T>::value;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Override loadFromFile to support both .cfg and .json files
bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine parser
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    // Use JSON parser for .json files, legacy parser for .cfg
    if (ext == ".json" || ext == ".jsonc") {
        return parseJsonFromFile(filepath);
    } else {
        // Legacy key=value format for .cfg and other files
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

// Override saveToFile to support both .cfg and .json files
bool Config::saveToFile(const std::string& filepath) const {
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    if (ext == ".json" || ext == ".jsonc") {
        return saveToJsonFileImpl(filepath, *this);
    } else {
        return saveToLegacyFileImpl(filepath, *this);
    }
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    return saveToJsonFileImpl(filepath, *this);
}

bool Config::saveToLegacyFile(const std::string& filepath) const {
    return saveToLegacyFileImpl(filepath, *this);
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

std::string Config::getFormattedSummary() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration ===\n\n";
    
    // Group configuration by source
    std::map<ConfigSource, std::vector<const ConfigEntry*>> grouped;
    for (const auto& entry : pImpl->entries) {
        grouped[entry.source].push_back(&entry);
    }
    
    for (const auto& [source, entries] : grouped) {
        std::string sourceName;
        switch (source) {
            case ConfigSource::Default: sourceName = "Default"; break;
            case ConfigSource::File: sourceName = "File"; break;
            case ConfigSource::CommandLine: sourceName = "Command Line"; break;
            case ConfigSource::Runtime: sourceName = "Runtime"; break;
        }
        
        oss << "[" << sourceName << "]\n";
        for (const auto* entry : entries) {
            oss << "  " << entry->key << " = ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    oss << std::fixed << std::setprecision(6) << arg;
                } else {
                    oss << arg;
                }
            }, entry->value);
            oss << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::toJsonString() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (is_vector<T>::value) {
                oss << "[";
                const auto& vec = arg;
                for (size_t j = 0; j < vec.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << vec[j];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

std::map<std::string, std::string> Config::getKeyValueMap() const {
    std::map<std::string, std::string> kvMap;
    
    for (const auto& entry : pImpl->entries) {
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg;
                valueStr = oss.str();
            } else if constexpr (is_vector<T>::value) {
                std::ostringstream oss;
                const auto& vec = arg;
                oss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
                valueStr = oss.str();
            } else {
                std::ostringstream oss;
                oss << arg;
                valueStr = oss.str();
            }
        }, entry.value);
        
        kvMap[entry.key] = valueStr;
    }
    
    return kvMap;
}

std::string Config::exportForCommandLine() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        // Generate command-line argument
        std::string arg;
        std::visit([&arg, &entry](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes in string values
                std::string escaped = val;
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                arg = "--" + entry.key + "=\"" + escaped + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                arg = "--" + entry.key + "=" + (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                arg = "--" + entry.key + "=" + std::to_string(val);
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << val;
                arg = "--" + entry.key + "=" + oss.str();
            } else {
                arg = "--" + entry.key + "=" + std::to_string(val);
            }
        }, entry.value);
        
        oss << arg << " ";
    }
    
    return oss.str();
}

bool Config::isModifiedFromFile(const std::string& filepath) const {
    // Load file configuration
    Config fileConfig;
    if (!fileConfig.loadFromFile(filepath)) {
        return true; // File doesn't exist or can't be read
    }
    
    // Compare entries
    if (pImpl->entries.size() != fileConfig.pImpl->entries.size()) {
        return true;
    }
    
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& fileEntry : fileConfig.pImpl->entries) {
            if (entry.key == fileEntry.key) {
                // Compare values (simplified - would need proper variant comparison)
                found = true;
                break;
            }
        }
        if (!found) {
            return true;
        }
    }
    
    return false;
}

void Config::applyCommandLineConfig(Config& config, int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Try to convert to appropriate type
                if (key == "random_seed" || key == "neuron_count" || key == "region_count" || 
                    key == "max_simulation_steps") {
                    try {
                        int64_t intVal = std::stoll(value);
                        config.set(key, intVal, ConfigSource::CommandLine);
                    } catch (...) {
                        config.set(key, value, ConfigSource::CommandLine);
                    }
                } else if (key == "connection_probability" || key == "stdp_ltp_weight" || 
                           key == "stdp_ltd_weight" || key == "stdp_time_constant" ||
                           key == "development_synaptogenesis_rate" || key == "development_pruning_rate" ||
                           key == "dopamine_baseline" || key == "reward_discount_factor" ||
                           key == "environment_width" || key == "environment_height" ||
                           key == "max_simulation_steps" || key == "visualization_update_rate") {
                    try {
                        double doubleVal = std::stod(value);
                        config.set(key, doubleVal, ConfigSource::CommandLine);
                    } catch (...) {
                        config.set(key, value, ConfigSource::CommandLine);
                    }
                } else if (key == "log_to_file" || key == "visualization_enabled") {
                    bool boolVal = (value == "true" || value == "1" || value == "yes");
                    config.set(key, boolVal, ConfigSource::CommandLine);
                } else {
                    config.set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            config.set(key, value, ConfigSource::CommandLine);
        }
    }
}

// Helper implementations

std::string Config::parseJsonValue(const std::string& line, size_t& pos) {
    // Skip whitespace after colon
    while (pos < line.size() && std::isspace(line[pos])) {
        pos++;
    }
    
    if (pos >= line.size()) {
        return "";
    }
    
    char quote = line[pos];
    if (quote == '"' || quote == '\'') {
        // String value
        pos++; // Skip opening quote
        size_t end = line.find(quote, pos);
        if (end == std::string::npos) {
            return line.substr(pos);
        }
        std::string result = line.substr(pos, end - pos);
        pos = end + 1; // Skip closing quote
        return result;
    } else if (quote == '[') {
        // Array - not fully implemented yet
        // Find matching ]
        int depth = 1;
        size_t start = pos++;
        while (pos < line.size() && depth > 0) {
            if (line[pos] == '[') depth++;
            else if (line[pos] == ']') depth--;
            pos++;
        }
        return line.substr(start, pos - start - 1);
    } else {
        // Number or boolean
        size_t end = pos;
        while (end < line.size() && !std::isspace(line[end]) && line[end] != ',' && line[end] != ']') {
            end++;
        }
        std::string result = line.substr(pos, end - pos);
        pos = end;
        return result;
    }
}

bool Config::parseJsonFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Remove comments
    size_t commentPos = content.find("//");
    while (commentPos != std::string::npos) {
        size_t newline = content.find('\n', commentPos);
        if (newline == std::string::npos) {
            content.erase(commentPos);
            break;
        }
        content.erase(commentPos, newline - commentPos);
        commentPos = content.find("//");
    }
    
    return mergeJsonObjects(content, *this);
}

bool Config::saveToJsonFileImpl(const std::string& filepath, const Config& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << config.toJsonString();
    return true;
}

bool Config::saveToLegacyFileImpl(const std::string& filepath, const Config& config) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM neural learning machine\n\n";
    
    for (const auto& entry : config.pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << std::fixed << std::setprecision(6) << arg << "\n";
            } else if constexpr (is_vector<T>::value) {
                file << entry.key << " = [";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
    }
    
    return true;
}

bool Config::mergeJsonObjects(const std::string& jsonContent, Config& config) {
    // Simple JSON parsing - assumes flat structure
    size_t objStart = jsonContent.find('{');
    if (objStart == std::string::npos) {
        return false;
    }
    
    size_t objEnd = jsonContent.rfind('}');
    if (objEnd == std::string::npos) {
        return false;
    }
    
    std::string objContent = jsonContent.substr(objStart + 1, objEnd - objStart - 1);
    
    // Parse key-value pairs
    size_t pos = 0;
    while (pos < objContent.size()) {
        // Find key
        size_t keyStart = objContent.find('"', pos);
        if (keyStart == std::string::npos) {
            break;
        }
        size_t keyEnd = objContent.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = objContent.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = objContent.find(':', keyEnd);
        if (pos == std::string::npos) {
            break;
        }
        
        // Parse value
        std::string valueStr = parseJsonValue(objContent, ++pos);
        
        // Convert value string to appropriate type based on key
        if (key == "random_seed" || key == "neuron_count" || key == "region_count" || 
            key == "connection_probability" || key == "stdp_ltp_weight" || 
            key == "stdp_ltd_weight" || key == "stdp_time_constant" ||
            key == "development_synaptogenesis_rate" || key == "development_pruning_rate" ||
            key == "dopamine_baseline" || key == "reward_discount_factor" ||
            key == "environment_width" || key == "environment_height" ||
            key == "max_simulation_steps" || key == "visualization_update_rate") {
            try {
                double numValue = std::stod(valueStr);
                if (key == "random_seed" || key == "neuron_count" || key == "region_count" || key == "max_simulation_steps") {
                    config.set(key, static_cast<int64_t>(numValue), ConfigSource::File);
                } else {
                    config.set(key, numValue, ConfigSource::File);
                }
            } catch (...) {
                config.set(key, valueStr, ConfigSource::File);
            }
        } else if (key == "log_to_file" || key == "visualization_enabled") {
            bool boolValue = (valueStr == "true" || valueStr == "1" || valueStr == "yes");
            config.set(key, boolValue, ConfigSource::File);
        } else {
            config.set(key, valueStr, ConfigSource::File);
        }
        
        // Move to next key
        pos = keyEnd + 1;
        while (pos < objContent.size() && (objContent[pos] == ' ' || objContent[pos] == '\t' || objContent[pos] == '\n' || objContent[pos] == ',')) {
            pos++;
        }
    }
    
    return true;
}

bool Config::parseKeyValue(const std::string& line, Config& config) {
    // Parse legacy key=value format
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
        
        config.set(key, value, ConfigSource::File);
        return true;
    }
    return false;
}

bool Config::parseNestedJson(const std::string& jsonContent, size_t startPos, Config& config) {
    // Placeholder for nested JSON parsing
    return true;
}

std::string Config::extractValueString(const std::string& jsonContent, size_t& pos) {
    // Extract value string from JSON
    std::string result;
    while (pos < jsonContent.size() && jsonContent[pos] != ',' && jsonContent[pos] != '}' && jsonContent[pos] != ']') {
        if (jsonContent[pos] == '"') {
            // String value
            pos++;
            size_t end = jsonContent.find('"', pos);
            if (end == std::string::npos) {
                break;
            }
            result += jsonContent.substr(pos, end - pos);
            pos = end + 1;
        } else {
            result += jsonContent[pos];
            pos++;
        }
    }
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