#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <mutex>
#include <chrono>
#include <unordered_set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<Constraint>> constraints;
    std::unordered_map<std::string, std::string> groups;
    std::unordered_map<std::string, bool> groupEnabled;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    std::unordered_map<std::string, bool> isSecret;
    std::unordered_map<std::string, std::string> validationErrors;
    std::unordered_map<std::string, size_t> fileModificationTimes;
    mutable std::mutex mutex;
    ConfigStats stats;
    
    Impl() {
        stats.reset();
        groupEnabled["global"] = true; // Default group
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if file has been modified
    auto currentTime = std::filesystem::last_write_time(filepath);
    auto currentModTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime.time_since_epoch()).count();
    
    // Try to parse as JSON first
    bool loaded = loadFromJSON(filepath, validate);
    if (!loaded) {
        // Try to parse as YAML
        loaded = loadFromYAML(filepath, validate);
        if (!loaded) {
            // Fall back to simple key=value format
            loaded = loadFromSimpleFormat(filepath, validate);
        }
    }
    
    if (loaded) {
        pImpl->fileModificationTimes[filepath] = currentModTime;
    }
    
    return loaded;
}

bool Config::loadFromJSON(const std::string& filepath, bool validate) {
    // For now, use simple format
    return loadFromSimpleFormat(filepath, validate);
}

bool Config::loadFromYAML(const std::string& filepath, bool validate) {
    // For now, use simple format
    return loadFromSimpleFormat(filepath, validate);
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine, "Command line argument", "command_line", validate);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine, "Command line argument", "command_line", validate);
        }
    }
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix, bool validate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // For now, just a placeholder
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool saveSecrets) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Determine file format based on extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return saveToJSON(filepath, saveSecrets);
    } else if (filepath.size() >= 5 && (filepath.substr(filepath.size() - 5) == ".yaml" || 
                                        filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml")) {
        // YAML saving (placeholder)
        return saveToYAML(filepath, saveSecrets);
    } else {
        // Simple format
        return saveToSimpleFormat(filepath, saveSecrets);
    }
}

bool Config::saveToJSON(const std::string& filepath, bool saveSecrets) const {
    // For now, use simple format
    return saveToSimpleFormat(filepath, saveSecrets);
}

bool Config::saveToYAML(const std::string& filepath, bool saveSecrets) const {
    // For now, use simple format
    return saveToSimpleFormat(filepath, saveSecrets);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, const std::string& group, bool validate) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    
    // Check type compatibility
    if (validate) {
        auto typeInfo = TypeInfo::getTypeInfo(value);
        if (!typeInfo.validator || !typeInfo.validator(value)) {
            pImpl->validationErrors[normalizedKey] = "Invalid type for key: " + normalizedKey;
            return;
        }
    }
    
    // Apply constraints if any
    if (validate) {
        ValidationResult validationResult = validateValue(normalizedKey, value);
        if (!validationResult.valid) {
            pImpl->validationErrors[normalizedKey] = validationResult.errorMessage;
            return;
        }
    }
    
    // Check dependencies
    if (validate && hasDependency(normalizedKey)) {
        pImpl->validationErrors[normalizedKey] = "Key has unresolved dependencies";
        return;
    }
    
    // Set the value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description;
        it->group = group;
        it->validationError.clear();
        pImpl->validationErrors.erase(normalizedKey);
    } else {
        ConfigEntry newEntry(normalizedKey, value, source, description, group);
        newEntry.validationError.clear();
        pImpl->entries.push_back(newEntry);
        
        // Update statistics
        pImpl->stats.totalEntries++;
        if (pImpl->validationErrors.find(normalizedKey) == pImpl->validationErrors.end()) {
            pImpl->stats.validEntries++;
        } else {
            pImpl->stats.invalidEntries++;
        }
    }
    
    // Update source statistics
    pImpl->stats.sourceCounts[std::to_string(static_cast<int>(source))]++;
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    ValidationResult result;
    
    // Check if key has constraints
    auto constraintIt = pImpl->constraints.find(key);
    if (constraintIt == pImpl->constraints.end()) {
        return result; // No constraints, validation passes
    }
    
    return ConfigHelpers::validateAgainstConstraints(key, value, constraintIt->second);
}

void Config::addConstraint(const std::string& key, Constraint constraint) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    constraint.key = normalizeKey(key);
    pImpl->constraints[key].push_back(constraint);
}

void Config::removeConstraint(const std::string& key, Constraint::Type type) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    key = normalizeKey(key);
    
    auto it = pImpl->constraints.find(key);
    if (it != pImpl->constraints.end()) {
        it->second.erase(
            std::remove_if(it->second.begin(), it->second.end(),
                [type](const Constraint& c) { return c.type == type; }),
            it->second.end()
        );
        if (it->second.empty()) {
            pImpl->constraints.erase(it);
        }
    }
}

void Config::addGroup(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedName = normalizeKey(name);
    pImpl->groups[normalizedName] = description;
    pImpl->groupEnabled[normalizedName] = true;
}

void Config::removeGroup(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedName = normalizeKey(name);
    pImpl->groups.erase(normalizedName);
    pImpl->groupEnabled.erase(normalizedName);
    
    // Remove entries in this group
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedName](const ConfigEntry& e) { return e.group == normalizedName; }),
        pImpl->entries.end()
    );
}

void Config::setGroupEnabled(const std::string& name, bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedName = normalizeKey(name);
    pImpl->groupEnabled[normalizedName] = enabled;
}

bool Config::isGroupEnabled(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedName = normalizeKey(name);
    auto it = pImpl->groupEnabled.find(normalizedName);
    return it != pImpl->groupEnabled.end() ? it->second : true;
}

ValidationResult Config::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ValidationResult overallResult;
    size_t validCount = 0;
    size_t invalidCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        if (isGroupEnabled(entry.group)) {
            ValidationResult entryResult = validateKey(entry.key);
            if (entryResult.valid) {
                validCount++;
            } else {
                invalidCount++;
                if (!overallResult.errorMessage.empty()) {
                    overallResult.errorMessage += "\n";
                }
                overallResult.errorMessage += entryResult.errorMessage;
            }
        }
    }
    
    pImpl->stats.validEntries = validCount;
    pImpl->stats.invalidEntries = invalidCount;
    
    if (invalidCount > 0) {
        overallResult.valid = false;
    }
    
    return overallResult;
}

ValidationResult Config::validateKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    
    // Find the entry
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (entryIt == pImpl->entries.end()) {
        return ValidationResult::error("Key not found: " + normalizedKey);
    }
    
    // Validate constraints
    ValidationResult result;
    auto constraintIt = pImpl->constraints.find(normalizedKey);
    if (constraintIt != pImpl->constraints.end()) {
        result = ConfigHelpers::validateAgainstConstraints(normalizedKey, entryIt->value, constraintIt->second);
    }
    
    return result;
}

bool Config::hasValidConfiguration() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->validationErrors.empty();
}

void Config::merge(const Config& other, bool overwrite) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& entry : other.pImpl->entries) {
        auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (existingIt != pImpl->entries.end()) {
            if (overwrite) {
                *existingIt = entry;
            }
        } else {
            pImpl->entries.push_back(entry);
            pImpl->stats.totalEntries++;
        }
    }
}

void Config::addDependency(const std::string& key, const std::string& dependsOn) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    std::string normalizedDependsOn = normalizeKey(dependsOn);
    
    pImpl->dependencies[normalizedKey].push_back(normalizedDependsOn);
}

void Config::removeDependency(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    pImpl->dependencies.erase(normalizedKey);
}

bool Config::hasDependency(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    auto it = pImpl->dependencies.find(normalizedKey);
    
    if (it == pImpl->dependencies.end()) {
        return false;
    }
    
    // Check if all dependencies are satisfied
    for (const auto& dep : it->second) {
        if (!has(dep)) {
            return true; // Has unsatisfied dependency
        }
    }
    
    return false;
}

std::vector<std::string> Config::getDependencies(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    auto it = pImpl->dependencies.find(normalizedKey);
    
    if (it == pImpl->dependencies.end()) {
        return {};
    }
    
    return it->second;
}

std::vector<std::string> Config::getGroups() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> groups;
    for (const auto& group : pImpl->groups) {
        groups.push_back(group.first);
    }
    return groups;
}

std::vector<std::string> Config::getKeysInGroup(const std::string& group) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedGroup = normalizeKey(group);
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.group == normalizedGroup && isGroupEnabled(entry.group)) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<ConfigEntry> Config::getEntriesInGroup(const std::string& group) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedGroup = normalizeKey(group);
    std::vector<ConfigEntry> entries;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.group == normalizedGroup && isGroupEnabled(entry.group)) {
            entries.push_back(entry);
        }
    }
    
    return entries;
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    
    // Remove from entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; }),
        pImpl->entries.end()
    );
    
    // Remove constraints
    pImpl->constraints.erase(normalizedKey);
    
    // Remove dependencies
    pImpl->dependencies.erase(normalizedKey);
    
    // Remove from validation errors
    pImpl->validationErrors.erase(normalizedKey);
    
    // Update statistics
    pImpl->stats.totalEntries--;
}

void Config::removeGroup(const std::string& group) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedGroup = normalizeKey(group);
    
    // Remove entries in this group
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedGroup](const ConfigEntry& e) { return e.group == normalizedGroup; }),
        pImpl->entries.end()
    );
    
    // Remove from groups
    pImpl->groups.erase(normalizedGroup);
    pImpl->groupEnabled.erase(normalizedGroup);
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (isGroupEnabled(entry.group)) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
    pImpl->constraints.clear();
    pImpl->groups.clear();
    pImpl->groupEnabled.clear();
    pImpl->dependencies.clear();
    pImpl->validationErrors.clear();
    pImpl->stats.reset();
}

ConfigStats Config::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return summary(true);
}

std::string Config::summary(bool verbose) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "  Valid: " << pImpl->stats.validEntries << "\n";
    oss << "  Invalid: " << pImpl->stats.invalidEntries << "\n";
    
    if (verbose) {
        oss << "\nGroups:\n";
        for (const auto& group : pImpl->groups) {
            bool enabled = isGroupEnabled(group.first);
            oss << "  " << (enabled ? "✓" : "✗") << " " << group.first << " - " << group.second << "\n";
        }
        
        oss << "\nSource Distribution:\n";
        for (const auto& sourceCount : pImpl->stats.sourceCounts) {
            oss << "  " << sourceCount.first << ": " << sourceCount.second << "\n";
        }
    }
    
    return oss.str();
}

std::string Config::getConfigurationReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== NLM Configuration Report ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Valid entries: " << pImpl->stats.validEntries << "\n";
    oss << "Invalid entries: " << pImpl->stats.invalidEntries << "\n";
    oss << "Memory usage: " << ConfigHelpers::formatMemorySize(pImpl->stats.memoryUsage) << "\n\n";
    
    // Invalid entries report
    if (!pImpl->validationErrors.empty()) {
        oss << "Invalid Entries:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  " << error.first << ": " << error.second << "\n";
        }
        oss << "\n";
    }
    
    // Group report
    oss << "Configuration Groups:\n";
    for (const auto& group : pImpl->groups) {
        bool enabled = isGroupEnabled(group.first);
        size_t groupCount = 0;
        for (const auto& entry : pImpl->entries) {
            if (entry.group == group.first && enabled) {
                groupCount++;
            }
        }
        oss << "  " << (enabled ? "✓" : "✗") << " " << group.first << ": " << group.second 
            << " (" << groupCount << " entries)\n";
    }
    
    return oss.str();
}

void Config::markAsSecret(const std::string& key, bool isSecret) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    pImpl->isSecret[normalizedKey] = isSecret;
}

bool Config::isSecret(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string normalizedKey = normalizeKey(key);
    auto it = pImpl->isSecret.find(normalizedKey);
    return it != pImpl->isSecret.end() ? it->second : false;
}

void Config::clearValidationErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->validationErrors.clear();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> errors;
    for (const auto& error : pImpl->validationErrors) {
        errors.push_back(error.first + ": " + error.second);
    }
    return errors;
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

std::string Config::normalizeKey(const std::string& key) {
    std::string normalized = key;
    // Replace underscores with hyphens for consistency
    std::replace(normalized.begin(), normalized.end(), '_', '-');
    // Convert to lowercase
    normalized = toLower(normalized);
    // Remove leading/trailing whitespace
    normalized = trim(normalized);
    return normalized;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<float>> Config::get<std::vector<float>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<float> Config::getOr<std::vector<float>>(const std::string&, const std::vector<float>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;
template std::vector<bool> Config::getOr<std::vector<bool>>(const std::string&, const std::vector<bool>&) const;

} // namespace nlm
