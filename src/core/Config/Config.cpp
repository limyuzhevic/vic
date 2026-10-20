#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <map>
// nlohmann/json library for JSON parsing
// Include the single header version of nlohmann/json
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format and parse accordingly
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check file extension for format detection
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    // Convert to lowercase for case-insensitive comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json" || extension == "yaml" || extension == "yml") {
        return loadFromJSON(filepath, ConfigSource::File);
    } else {
        // Default to legacy key=value format
        return loadFromLegacyFormat(filepath, ConfigSource::File);
    }
}

bool Config::loadFromJSON(const std::string& filepath, ConfigSource source) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
            return false;
        }
        
        // Check if file is empty
        if (file.peek() == std::ifstream::traits_type::eof()) {
            NLM_LOG_WARNING("JSON config file is empty: " + filepath);
            return false;
        }
        
        // Parse JSON file
        json j;
        try {
            file >> j;
        } catch (const json::parse_error& e) {
            NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + e.what());
            return false;
        }
        
        // Validate JSON structure (basic validation)
        if (!j.is_object()) {
            NLM_LOG_ERROR("JSON config file must contain an object, got " + j.type_name() + " in " + filepath);
            return false;
        }
        
        // Convert JSON to Config entries
        int successCount = 0;
        int errorCount = 0;
        
        for (auto& [key, value] : j.items()) {
            // Convert JSON value to ConfigValue based on type
            ConfigValue configValue;
            bool conversionSuccess = true;
            
            try {
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number()) {
                    configValue = value.get<double>();
                } else if (value.is_array()) {
                    // For now, convert array to string representation
                    // TODO: Implement proper array support
                    configValue = value.dump();
                } else if (value.is_null()) {
                    // Skip null values with warning
                    NLM_LOG_WARNING("Null value encountered for key '" + key + "' in " + filepath);
                    continue;
                } else {
                    // For other types (object, etc.), convert to string
                    configValue = value.dump();
                }
                
                // Try to set the value
                set(key, configValue, source);
                successCount++;
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to convert JSON value for key '" + key + "' in " + filepath + ": " + e.what());
                errorCount++;
                conversionSuccess = false;
            }
        }
        
        if (errorCount > 0) {
            NLM_LOG_WARNING("JSON config file " + filepath + " had " + std::to_string(errorCount) + " conversion errors, " + std::to_string(successCount) + " succeeded");
        }
        
        return successCount > 0;  // Consider it successful if at least some values were parsed
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error loading JSON config " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromLegacyFormat(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open legacy config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    int successCount = 0;
    int errorCount = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line (no '=' found) in " + filepath + ":" + std::to_string(lineNumber) + " - " + line);
            errorCount++;
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
        
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key encountered in " + filepath + ":" + std::to_string(lineNumber));
            errorCount++;
            continue;
        }
        
        try {
            set(key, value, source);
            successCount++;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse config line in " + filepath + ":" + std::to_string(lineNumber) + " - " + e.what());
            errorCount++;
        }
    }
    
    if (errorCount > 0) {
        NLM_LOG_WARNING("Legacy config file " + filepath + " had " + std::to_string(errorCount) + " errors on " + std::to_string(lineNumber) + " lines, " + std::to_string(successCount) + " succeeded");
    }
    
    return successCount > 0;  // Consider it successful if at least some values were parsed
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Save the actual value based on its type
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
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
