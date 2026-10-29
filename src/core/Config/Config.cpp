// Config.cpp - Enhanced Configuration Management System
// Provides robust configuration management with JSON/YAML support, validation, and metadata

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

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
        return false;
    }
    
    // Try to detect file format by extension
    std::string extension = "." + filepath.substr(filepath.find_last_of('.') + 1);
    toLower(extension);
    
    if (extension == ".json") {
        return loadFromJSON(file);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAML(file);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFormat(file);
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
                success = false; // Invalid format
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write the value based on its type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << val[i];
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << val[i];
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    file << "\"" << val[i] << "\"";
                    if (i < val.size() - 1) file << ", ";
                }
                file << "]";
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
    
    if (it->metadata && !validateValueAgainstMetadata(key, it->value)) {
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

// Get metadata for a configuration key
std::optional<ConfigMetadata> Config::getMetadata(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->metadata;
    }
    return std::nullopt;
}

// Validate configuration values based on metadata
std::vector<std::string> Config::validateConfiguration() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.metadata) {
            if (!validateValueAgainstMetadata(entry.key, entry.value)) {
                errors.push_back("Invalid value for configuration key '" + entry.key + "': " + 
                               std::to_string(static_cast<int>(entry.source)));
            }
        }
    }
    
    return errors;
}

// Get configuration category
std::vector<std::string> Config::getCategories() const {
    std::vector<std::string> categories;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.metadata && 
            std::find(categories.begin(), categories.end(), entry.metadata->category) == categories.end()) {
            categories.push_back(entry.metadata->category);
        }
    }
    
    return categories;
}

// Get configuration keys by category
std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.metadata && entry.metadata->category == category) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

// Get configuration summary with metadata
std::string Config::summaryWithMetadata() const {
    std::ostringstream oss;
    oss << "Configuration with Metadata (" << pImpl->entries.size() << " entries):\n";
    
    // Group by category
    std::map<std::string, std::vector<ConfigEntry>> byCategory;
    for (const auto& entry : pImpl->entries) {
        std::string category = entry.metadata ? entry.metadata->category : "General";
        byCategory[category].push_back(entry);
    }
    
    for (const auto& category_pair : byCategory) {
        oss << "\n=== " << category_pair.first << " ===\n";
        for (const auto& entry : category_pair.second) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")";
            if (entry.metadata) {
                oss << " - " << entry.metadata->description;
                if (!entry.metadata->unit.empty()) oss << " [" << entry.metadata->unit << "]";
                if (!entry.metadata->validation.empty()) oss << " (" << entry.metadata->validation << ")";
            } else {
                oss << " - No metadata";
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

// Validate value against metadata
bool Config::validateValueAgainstMetadata(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end() || !it->metadata) {
        return true; // No validation if no metadata
    }
    
    const auto& metadata = it->metadata.value();
    
    // Basic type validation
    bool type_valid = false;
    if (metadata.type == "int") {
        type_valid = std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (metadata.type == "double") {
        type_valid = std::holds_alternative<double>(value);
    } else if (metadata.type == "bool") {
        type_valid = std::holds_alternative<bool>(value);
    } else if (metadata.type == "string") {
        type_valid = std::holds_alternative<std::string>(value);
    } else if (metadata.type == "vector<int>") {
        type_valid = std::holds_alternative<std::vector<int>>(value);
    } else if (metadata.type == "vector<double>") {
        type_valid = std::holds_alternative<std::vector<double>>(value);
    } else if (metadata.type == "vector<string>") {
        type_valid = std::holds_alternative<std::vector<std::string>>(value);
    }
    
    if (!type_valid) {
        return false;
    }
    
    // Range validation
    if (!metadata.validation.empty()) {
        if (metadata.validation.find("range") != std::string::npos) {
            // Extract range from validation string (e.g., "range(0,100)")
            size_t open_paren = metadata.validation.find('(');
            size_t close_paren = metadata.validation.find(')');
            if (open_paren != std::string::npos && close_paren != std::string::npos) {
                std::string range_str = metadata.validation.substr(open_paren + 1, close_paren - open_paren - 1);
                // Simple range parsing - extract min and max
                size_t comma = range_str.find(',');
                if (comma != std::string::npos) {
                    try {
                        double min_val = std::stod(range_str.substr(0, comma));
                        double max_val = std::stod(range_str.substr(comma + 1));
                        
                        double current_val = 0.0;
                        if (std::holds_alternative<double>(value)) {
                            current_val = std::get<double>(value);
                        } else if (std::holds_alternative<int>(value)) {
                            current_val = static_cast<double>(std::get<int>(value));
                        } else if (std::holds_alternative<int64_t>(value)) {
                            current_val = static_cast<double>(std::get<int64_t>(value));
                        }
                        
                        if (current_val < min_val || current_val > max_val) {
                            return false;
                        }
                    } catch (...) {
                        // If parsing fails, skip range validation
                    }
                }
            }
        }
    }
    
    return true;
}

// Simple format: key = value (backward compatibility)
bool Config::loadFromSimpleFormat(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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

// Basic JSON parser (supports simple JSON format)
bool Config::loadFromJSON(std::ifstream& file) {
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Remove all whitespace for simpler parsing
        std::string compact_content;
        for (char c : content) {
            if (!std::isspace(c)) {
                compact_content += c;
            }
        }
        
        // Find JSON object braces
        size_t brace_start = compact_content.find('{');
        if (brace_start == std::string::npos) {
            return false;
        }
        
        size_t brace_end = compact_content.rfind('}');
        if (brace_end == std::string::npos || brace_end < brace_start) {
            return false;
        }
        
        std::string json_content = compact_content.substr(brace_start + 1, brace_end - brace_start - 1);
        
        // Parse key-value pairs
        size_t pos = 0;
        while (pos < json_content.length()) {
            // Find key (enclosed in quotes)
            size_t key_start = json_content.find('"', pos);
            if (key_start == std::string::npos) break;
            
            size_t key_end = json_content.find('"', key_start + 1);
            if (key_end == std::string::npos) break;
            
            std::string key = json_content.substr(key_start + 1, key_end - key_start - 1);
            
            // Find colon separator
            size_t colon_pos = json_content.find(':', key_end);
            if (colon_pos == std::string::npos) break;
            
            size_t value_start = json_content.find_first_not_of(', ', colon_pos + 1);
            if (value_start == std::string::npos) break;
            
            // Parse value based on first character
            ConfigValue value;
            std::string remaining = json_content.substr(value_start);
            
            if (remaining[0] == '"') { // String
                size_t quote_end = remaining.find('"', 1);
                if (quote_end != std::string::npos) {
                    value = remaining.substr(1, quote_end - 1);
                    pos = value_start + quote_end + 1;
                } else {
                    break;
                }
            } else if (remaining.substr(0, 4) == "true" || remaining.substr(0, 5) == "false") { // Boolean
                bool bool_val = (remaining.substr(0, 4) == "true");
                value = bool_val;
                size_t end_pos = bool_val ? 4 : 5;
                while (end_pos < remaining.length() && (remaining[end_pos] == ',' || remaining[end_pos] == '}')) end_pos++;
                pos = value_start + end_pos;
            } else { // Number (int or double)
                size_t num_end = value_start;
                while (num_end < remaining.length() && (std::isdigit(remaining[num_end]) || remaining[num_end] == '.' || remaining[num_end] == '-')) {
                    num_end++;
                }
                
                std::string num_str = remaining.substr(value_start, num_end - value_start);
                try {
                    if (num_str.find('.') != std::string::npos) {
                        value = std::stod(num_str);
                    } else {
                        value = std::stoll(num_str);
                    }
                } catch (...) {
                    break;
                }
                pos = value_start + num_end;
            }
            
            set(key, value, ConfigSource::File);
            
            // Skip comma separator
            while (pos < json_content.length() && (json_content[pos] == ',' || json_content[pos] == '}')) pos++;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Basic YAML parser (simple key: value format)
bool Config::loadFromYAML(std::ifstream& file) {
    try {
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Handle simple key: value format (YAML)
            size_t colon_pos = line.find(':');
            if (colon_pos == std::string::npos) {
                continue;
            }
            
            std::string key = trim(line.substr(0, colon_pos));
            std::string value = trim(line.substr(colon_pos + 1));
            
            // Remove YAML quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (...) {
        return false;
    }
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
