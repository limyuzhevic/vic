#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <cmath>
#include <ctime>
#include <map>
#include <set>
#include <functional>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first (Phase 2 improvement)
    // Check if file exists and try to parse as JSON
    
    // For now, keep simple parser as fallback
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect JSON format by reading first few characters
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Simple heuristic: if line starts with { and ends with }, assume JSON
    if (!firstLine.empty() && firstLine.front() == '{' && firstLine.back() == '}') {
        NLM_LOG_INFO("Attempting to parse " + filepath + " as JSON");
        // TODO: Implement JSON parsing (Phase 2)
        // For now, fall back to simple parser
        NLM_LOG_WARNING("JSON parsing not implemented yet, falling back to simple format");
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs (Phase 1 format)
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
            
            // Try to parse value as different types
            set(key, value, ConfigSource::File);
        } else if (!line.empty() && line[0] == '[' && line.back() == ']') {
            // Simple array format: key = [1, 2, 3]
            size_t colonPos = line.find('=');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string arrayStr = trim(line.substr(colonPos + 1));
                
                // Parse array based on context
                // TODO: Implement proper array parsing
                set(key, arrayStr, ConfigSource::File);
            }
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}
bool Config::loadFromEnvironment(const std::string& prefix) {
    bool loaded = false;
    
    // Get environment variables by prefix
    // Note: This would typically use platform-specific APIs
    // For portability, we'll use a simplified approach
    
    // Common NLM environment variable prefixes
    const std::vector<std::string> prefixes = {
        "NLM_",
        "NLM_CONFIG_",
        "NEURAL_"
    };
    
    for (const auto& envPrefix : prefixes) {
        // In a real implementation, this would iterate over environment variables
        // For now, we'll just return false to indicate no environment loading
        // TODO: Implement proper environment variable loading
    }
    
    NLM_LOG_INFO("Environment configuration loading (Phase 2+)");
    return loaded;
}

bool Config::loadFromJSON(const std::string& filepath) {
    NLM_LOG_INFO("JSON configuration loading (Phase 2)");
    
    // TODO: Implement proper JSON parsing
    // This would use a JSON library like nlohmann::json
    
    NLM_LOG_WARNING("JSON parsing not implemented yet");
    return false;
}

bool Config::loadFromYAML(const std::string& filepath) {
    NLM_LOG_INFO("YAML configuration loading (Phase 3)");
    
    // TODO: Implement proper YAML parsing
    // This would use a YAML library like YAML-CPP
    
    NLM_LOG_WARNING("YAML parsing not implemented yet");
    return false;
}

bool Config::validate() const {
    NLM_LOG_INFO("Configuration validation (Phase 2)");
    
    // TODO: Implement proper validation
    // Check for required keys, type checking, value ranges, etc.
    
    NLM_LOG_WARNING("Configuration validation not implemented yet");
    return true;
}

std::vector<std::string> Config::getGroups() const {
    // TODO: Implement group management
    return {};
}

void Config::addGroup(const std::string& group, const std::string& description) {
    // TODO: Implement group management
    NLM_LOG_INFO("Group management (Phase 3): " + group);
}

void Config::moveToGroup(const std::string& key, const std::string& group) {
    // TODO: Implement group management
    NLM_LOG_INFO("Move to group (Phase 3): " + key + " -> " + group);
}

template<typename T>
T Config::getValidated(const std::string& key, const T& defaultValue) const {
    // TODO: Implement validation
    return getOr(key, defaultValue);
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // TODO: Implement validation
    set(key, value, source);
    return true;
}

bool Config::executeCommand(const std::string& command) {
    // TODO: Implement configuration commands
    NLM_LOG_INFO("Configuration commands (Phase 2): " + command);
    return false;
}

std::vector<std::string> Config::getAvailableCommands() const {
    return {
        "validate",
        "save",
        "load",
        "backup",
        "restore",
        "export",
        "import",
        "diff",
        "merge"
    };
}

bool Config::saveTemplate(const std::string& name, const std::string& description) {
    // TODO: Implement template saving
    NLM_LOG_INFO("Template saving (Phase 3): " + name);
    return true;
}

bool Config::loadTemplate(const std::string& name) {
    // TODO: Implement template loading
    NLM_LOG_INFO("Template loading (Phase 3): " + name);
    return true;
}

std::vector<std::string> Config::getAvailableTemplates() const {
    // TODO: Implement template management
    return {};
}

std::string Config::analyze() const {
    std::ostringstream oss;
    oss << "Configuration Analysis:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    oss << "  Configuration source distribution:\n";
    
    std::map<ConfigSource, size_t> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    for (const auto& pair : sourceCounts) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << " entries\n";
    }
    
    oss << "\nEntry keys:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  - " << entry.key << " (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::map<std::string, double> Config::getStatistics() const {
    std::map<std::string, double> stats;
    
    stats["entry_count"] = static_cast<double>(pImpl->entries.size());
    stats["group_count"] = 0.0; // TODO: Implement group counting
    
    if (pImpl->entries.size() > 0) {
        double totalStringLength = 0.0;
        size_t stringCount = 0;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    totalStringLength += arg.size();
                    stringCount++;
                }
            }, entry.value);
        }
        
        if (stringCount > 0) {
            stats["average_string_length"] = totalStringLength / stringCount;
        }
    }
    
    return stats;
}

bool Config::migrate(const std::string& fromVersion, const std::string& toVersion) {
    NLM_LOG_INFO("Configuration migration (Phase 3): " + fromVersion + " -> " + toVersion);
    
    // TODO: Implement configuration migration
    return true;
}

std::string Config::getVersion() const {
    return "1.0.0"; // Placeholder
}

void Config::setVersion(const std::string& version) {
    NLM_LOG_INFO("Setting configuration version: " + version);
    // TODO: Store version information
}

bool Config::backup(const std::string& filepath) const {
    return saveToFile(filepath);
}

bool Config::restore(const std::string& filepath) {
    clear();
    return loadFromFile(filepath);
}

std::vector<std::string> Config::search(const std::string& pattern) const {
    std::vector<std::string> results;
    
    // Simple pattern matching - in production, would use more sophisticated search
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(pattern) != std::string::npos) {
            results.push_back(entry.key);
        }
    }
    
    return results;
}

bool Config::exportToJSON(const std::string& filepath, bool pretty) const {
    NLM_LOG_INFO("JSON export (Phase 2): " + filepath);
    
    // TODO: Implement JSON export
    return false;
}

bool Config::exportToINI(const std::string& filepath) const {
    NLM_LOG_INFO("INI export (Phase 2): " + filepath);
    
    // TODO: Implement INI export
    return false;
}

void Config::recordChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    // TODO: Implement change tracking
    NLM_LOG_INFO("Change tracking (Phase 3): " + key);
}

std::vector<std::pair<std::string, ConfigValue>> Config::getHistory() const {
    // TODO: Implement change history
    return {};
}

void Config::clearHistory() {
    NLM_LOG_INFO("Clear change history (Phase 3)");
}

void Config::optimize() {
    NLM_LOG_INFO("Configuration optimization (Phase 3)");
    
    // TODO: Implement optimization
}

double Config::getMemoryUsage() const {
    double memoryKB = static_cast<double>(pImpl->entries.size() * 100); // Rough estimate
    return memoryKB;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getGroupCount() const {
    return getGroups().size();
}

double Config::getAverageValueLength() const {
    size_t totalLength = 0;
    size_t count = 0;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                totalLength += arg.size();
                count++;
            }
        }, entry.value);
    }
    
    return count > 0 ? static_cast<double>(totalLength) / count : 0.0;
}

void Config::removeUnused() {
    // TODO: Implement cleanup of unused entries
    NLM_LOG_INFO("Configuration cleanup (Phase 3)");
}

void Config::removeOldEntries(ConfigSource source) {
    // TODO: Implement cleanup by source
    NLM_LOG_INFO("Remove old entries (Phase 3): source " + std::to_string(static_cast<int>(source)));
}

void Config::batchSet(const std::map<std::string, ConfigValue>& settings) {
    for (const auto& pair : settings) {
        set(pair.first, pair.second);
    }
}

void Config::batchRemove(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        remove(key);
    }
}

void Config::merge(const Config& other, bool overwrite) {
    for (const auto& entry : other.pImpl->entries) {
        if (overwrite || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

std::vector<Config::DiffEntry> Config::computeDiff(const Config& other) const {
    std::vector<DiffEntry> diff;
    
    // Check for entries in this config that are not in other
    for (const auto& entry : pImpl->entries) {
        if (!other.has(entry.key)) {
            DiffEntry diffEntry;
            diffEntry.key = entry.key;
            diffEntry.oldValue = ConfigValue(); // Empty for new
            diffEntry.newValue = entry.value;
            diffEntry.source = entry.source;
            diffEntry.timestamp = "now";
            diff.push_back(diffEntry);
        }
    }
    
    // Check for entries in other that are not in this
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key)) {
            DiffEntry diffEntry;
            diffEntry.key = entry.key;
            diffEntry.oldValue = entry.value;
            diffEntry.newValue = ConfigValue(); // Empty for deleted
            diffEntry.source = entry.source;
            diffEntry.timestamp = "now";
            diff.push_back(diffEntry);
        }
    }
    
    return diff;
}

void Config::addValidationRule(const ValidationRule& rule) {
    // TODO: Implement validation rules
    NLM_LOG_INFO("Add validation rule (Phase 3): " + rule.key);
}

void Config::removeValidationRule(const std::string& key) {
    // TODO: Implement validation rules
    NLM_LOG_INFO("Remove validation rule (Phase 3): " + key);
}

bool Config::hasValidationRule(const std::string& key) const {
    // TODO: Implement validation rules
    return false;
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

    // Advanced configuration template instantiations
    template int Config::getValidated<int>(const std::string&, const int&) const;
    template double Config::getValidated<double>(const std::string&, const double&) const;
    template std::string Config::getValidated<std::string>(const std::string&, const std::string&) const;

    } // namespace nlm
