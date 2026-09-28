#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<SchemaEntry> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Auto-detect format based on file extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    return loadFromFile(filepath, ext);
}

bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }

    // Clear existing entries
    clear();

    try {
        if (format == ".json" || format == ".JSON") {
            // Implement JSON parsing
            // For now, use simple parsing
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '/' || line[0] == '#') {
                    continue;
                }

                // Simple JSON object parsing (limited support)
                if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
                    // Very basic JSON parsing - TODO: Use proper JSON library
                    size_t colon = line.find(':');
                    if (colon != std::string::npos) {
                        std::string key = trim(line.substr(1, colon - 1));
                        std::string value = trim(line.substr(colon + 1));
                        // Remove trailing comma and closing brace
                        if (!value.empty() && value.back() == ',') {
                            value.pop_back();
                        }
                        if (!value.empty() && value.back() == '}') {
                            value.pop_back();
                        }
                        set(stringToConfigValue(key), ConfigSource::File);
                    }
                } else {
                    // Parse key=value format
                    size_t pos = line.find('=');
                    if (pos != std::string::npos) {
                        std::string key = trim(line.substr(0, pos));
                        std::string value = trim(line.substr(pos + 1));
                        set(key, value, ConfigSource::File);
                    }
                }
            }
        } else {
            // Parse simple key=value format (original implementation)
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '/' || line[0] == '#') {
                    continue;
                }

                // Parse key=value pairs
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
        }

        NLM_LOG_INFO("Loaded " + std::to_string(getEntryCount()) + " configuration entries from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else if (arg[0] == '-') {
            NLM_LOG_WARNING("Invalid command line argument: " + arg);
            success = false;
        }
    }
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }

    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::time(nullptr) << "\n\n";

    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
    }

    NLM_LOG_INFO("Saved " + std::to_string(getEntryCount()) + " configuration entries to: " + filepath);
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
        NLM_LOG_WARNING("Config key " + key + " has type mismatch for requested type");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (pImpl->schema.empty() || !hasSchema(key)) {
        // No schema, allow setting
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });

        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    } else {
        // Validate against schema
        ValidationResult validation = validate();
        if (!validation.isValid) {
            NLM_LOG_WARNING("Validation failed: " + validation.errorMessage);
        }
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });

        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
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
    oss << "Configuration (" << getEntryCount() << " entries):\n";
    
    // Group by source
    std::map<ConfigSource, std::vector<const ConfigEntry*>> grouped;
    for (const auto& entry : pImpl->entries) {
        grouped[entry.source].push_back(&entry);
    }
    
    for (const auto& [source, entries] : grouped) {
        oss << "  " << static_cast<int>(source) << ":\n";
        for (const auto* entry : entries) {
            oss << "    " << entry->key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry->value);
            oss << "]\n";
        }
    }
    
    return oss.str();
}

void Config::addSchema(const SchemaEntry& schema) {
    pImpl->schema.push_back(schema);
}

Config::ValidationResult Config::validate() const {
    for (const auto& schemaEntry : pImpl->schema) {
        // Check if key exists
        if (schemaEntry.required && !has(schemaEntry.key)) {
            return ValidationResult(false, "Required key missing: " + schemaEntry.key);
        }
        
        // Validate type if key exists
        if (has(schemaEntry.key)) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&schemaEntry](const ConfigEntry& e) { return e.key == schemaEntry.key; });
            
            if (it != pImpl->entries.end()) {
                // Check if the actual type matches schema type
                if (!std::holds_alternative<std::remove_cvref_t<decltype(schemaEntry.type)>>(*it, 
                    std::get<0>(schemaEntry.type) && std::get<0>(it->value)) {
                    return ValidationResult(false, "Type mismatch for key: " + schemaEntry.key);
                }
            }
        }
    }
    
    return ValidationResult(true);
}

bool Config::hasSchema(const std::string& key) const {
    return std::any_of(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const SchemaEntry& e) { return e.key == key; });
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [source](const ConfigEntry& e) { return e.source == source; });
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
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

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    // Try parsing as boolean first (case-insensitive)
    std::string lower = toLower(str);
    if (lower == "true") {
        return true;
    } else if (lower == "false") {
        return false;
    }
    
    // Try parsing as integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try parsing as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string
    return str;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
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
