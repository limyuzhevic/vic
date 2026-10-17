#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <cmath>
#include <limits>
#include <regex>
#include <unordered_map>

// Simple JSON parsing implementation
// Note: For production use, consider integrating with a proper JSON library like nlohmann::json

namespace nlm {

namespace nlm {

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigTypeValidator> validators;
    
    Impl() : entries(), validators() {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    if (JsonParser::parseJsonFile(filepath, *pImpl)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        
        // Validate after loading
        std::string validationError;
        if (!ConfigValidator::validateConfig(*pImpl, validationError)) {
            NLM_LOG_WARNING("Configuration validation failed: " + validationError);
        }
        return true;
    }
    
    // If JSON parsing failed, try simple key=value format as fallback
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
        file << entry.key << " = ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << '"' << arg << '"';
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
                    file << '"' << arg[i] << '"';
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

ValidationResult Config::validate() const {
    std::string error;
    if (!ConfigValidator::validateConfig(*pImpl, error)) {
        return ValidationResult(false, error);
    }
    return ValidationResult();
}

ValidationResult Config::validateKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ValidationResult(false, "Key not found: " + key);
    }
    
    std::string error;
    if (!ConfigValidator::validateEntry(*it, error)) {
        return ValidationResult(false, error);
    }
    return ValidationResult();
}

std::vector<std::string> Config::getAvailableConfigFiles(const std::string& directory) {
    std::vector<std::string> files;
    std::error_code ec;
    
    if (!std::filesystem::exists(directory, ec)) {
        return files;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory, ec)) {
        if (entry.is_regular_file(ec)) {
            std::string filename = entry.path().filename().string();
            if (filename.length() >= 4) {
                std::string ext = filename.substr(filename.length() - 4);
                if (ext == ".cfg" || ext == ".json" || ext == ".yaml" || ext == ".yml") {
                    files.push_back(filename);
                }
            }
        }
    }
    
    return files;
}

bool Config::mergeConfigFiles(const std::vector<std::string>& files, std::string& outputFile) {
    if (files.empty()) {
        return false;
    }
    
    // Create a temporary Config and load all files
    Config merged;
    for (const auto& filename : files) {
        if (!merged.loadFromFile(filename)) {
            return false;
        }
    }
    
    // Save to output file
    if (outputFile.empty()) {
        outputFile = "merged_config.cfg";
    }
    
    return merged.saveToFile(outputFile);
}

std::string Config::exportToJson() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << entry.key << "\": ";
        
        // Format value based on type
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Escape JSON special characters
                std::string escaped = arg;
                replaceJsonSpecialChars(escaped);
                oss << "\"" << escaped << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::setprecision(6) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    std::string escaped = arg[i];
                    replaceJsonSpecialChars(escaped);
                    oss << "\"" << escaped << "\"";
                }
                oss << "]";
            }
        }, entry.value);
    }
    
    oss << "\n}";
    return oss.str();
}

bool Config::importFromJson(const std::string& json) {
    // Clear existing config
    clear();
    
    // Parse JSON
    if (!JsonParser::parseJsonObject(json, *pImpl)) {
        return false;
    }
    
    return true;
}

void Config::registerValidator(const std::string& key, const ConfigTypeValidator& validator) {
    pImpl->validators[key] = validator;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        return it->second(value);
    }
    
    // Default validation
    std::string error;
    return ConfigValidator::validateEntry(ConfigEntry(key, value, ConfigSource::Runtime), error);
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