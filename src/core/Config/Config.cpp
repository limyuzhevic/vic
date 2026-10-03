#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// Parameter group definition (local copy for implementation)
struct ParameterGroup {
    std::string name;
    std::string description;
    std::vector<std::string> parameter_keys;
    bool readonly;
    
    ParameterGroup() : name(), description(), readonly(false) {}
    ParameterGroup(const std::string& n, const std::string& desc, bool ro = false)
        : name(n), description(desc), readonly(ro) {}
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ParameterGroup> groups;
    std::unordered_map<std::string, std::function<bool(const std::string&, const ConfigValue&)>> validators;
    std::unordered_map<std::string, std::string> macros;
    std::unordered_map<std::string, std::vector<ConfigEntry>> profiles;
    std::string optimization_mode;
    Config* inheritance_parent;
    bool inheritance_enabled;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    std::unordered_map<std::string, std::vector<std::string>> reverse_dependencies;
    std::string current_profile;
    
    Impl() : inheritance_parent(nullptr), inheritance_enabled(false), current_profile("") {
        optimization_mode = "balanced";
    }
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
    
    // Try to parse as JSON first
    try {
        nlohmann::json json_data;
        file >> json_data;
        
        // Clear existing entries
        clear();
        
        // Parse JSON data
        if (json_data.is_object()) {
            for (auto it = json_data.begin(); it != json_data.end(); ++it) {
                std::string key = it.key();
                std::string description = "Configuration parameter: " + key;
                
                // Parse value based on JSON type
                const auto& value = it.value();
                if (value.is_string()) {
                    set(key, value.get<std::string>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), ConfigSource::File);
                } else if (value.is_number_integer()) {
                    set(key, value.get<int64_t>(), ConfigSource::File);
                } else if (value.is_number_float()) {
                    set(key, value.get<double>(), ConfigSource::File);
                } else if (value.is_array()) {
                    // For arrays, convert to vector of strings for now
                    std::vector<std::string> array_values;
                    for (const auto& item : value) {
                        if (item.is_string()) {
                            array_values.push_back(item.get<std::string>());
                        } else {
                            array_values.push_back(item.dump());
                        }
                    }
                    set(key, array_values, ConfigSource::File);
                }
                
                // Add description if available
                auto entry_it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (entry_it != pImpl->entries.end()) {
                    entry_it->description = description;
                }
            }
        }
        
        return true;
    } catch (const std::exception&) {
        // JSON parsing failed, fall back to simple key=value format
    }
    
    // Fallback to simple key=value format
    file.clear();
    file.seekg(0);
    
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
            std::string value_str = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value_str.size() >= 2 && 
                ((value_str.front() == '"' && value_str.back() == '"') ||
                 (value_str.front() == '\'' && value_str.back() == '\''))) {
                value_str = value_str.substr(1, value_str.size() - 2);
            }
            
            // Try to convert to appropriate type
            set(key, value_str, ConfigSource::File);
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
        
        // Serialize the actual value based on its type
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
        
        file << "\n";
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

}

// Advanced Configuration Features Implementation

std::vector<std::string> Config::getKeysByGroup(const std::string& group) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.group == group) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getGroups() const {
    std::vector<std::string> groups;
    for (const auto& pair : pImpl->groups) {
        groups.push_back(pair.first);
    }
    return groups;
}

bool Config::validate() const {
    // Check if all dependencies are resolved
    if (!validateDependencies()) {
        return false;
    }
    
    // Check if all parameters pass their validators
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key)) {
            return false;
        }
    }
    
    return true;
}

bool Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return true; // Key doesn't exist, no validation needed
    }
    
    // Check if there's a validator for this key
    auto validator_it = pImpl->validators.find(key);
    if (validator_it != pImpl->validators.end()) {
        return validator_it->second(key, it->value);
    }
    
    return true; // No validator, validation passes
}

void Config::addValidator(const std::string& key, ValidationFunction validator) {
    pImpl->validators[key] = validator;
}

void Config::clearValidators() {
    pImpl->validators.clear();
}

void Config::addGroup(const std::string& name, const std::string& description, bool readonly) {
    ParameterGroup group;
    group.name = name;
    group.description = description;
    group.readonly = readonly;
    pImpl->groups[name] = group;
}

void Config::addParameterToGroup(const std::string& group, const std::string& key) {
    auto group_it = pImpl->groups.find(group);
    if (group_it != pImpl->groups.end()) {
        // Find the entry and add it to the group
        auto entry_it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (entry_it != pImpl->entries.end()) {
            entry_it->group = group;
            group_it->second.parameter_keys.push_back(key);
        }
    }
}

void Config::removeGroup(const std::string& name) {
    auto group_it = pImpl->groups.find(name);
    if (group_it != pImpl->groups.end()) {
        // Remove parameters from this group
        for (const auto& key : group_it->second.parameter_keys) {
            auto entry_it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (entry_it != pImpl->entries.end()) {
                entry_it->group.clear();
            }
        }
        pImpl->groups.erase(group_it);
    }
}

bool Config::hasGroup(const std::string& name) const {
    return pImpl->groups.find(name) != pImpl->groups.end();
}

void Config::setInheritanceParent(Config* parent) {
    pImpl->inheritance_parent = parent;
    pImpl->inheritance_enabled = (parent != nullptr);
}

Config* Config::getInheritanceParent() const {
    return pImpl->inheritance_parent;
}

void Config::addMacro(const std::string& name, const std::string& expansion) {
    pImpl->macros[name] = expansion;
}

void Config::removeMacro(const std::string& name) {
    pImpl->macros.erase(name);
}

std::string Config::expandMacro(const std::string& macro_name) const {
    auto macro_it = pImpl->macros.find(macro_name);
    if (macro_it != pImpl->macros.end()) {
        return macro_it->second;
    }
    return "";
}

void Config::saveProfile(const std::string& profile_name) const {
    std::vector<ConfigEntry> profile_entries;
    
    // Include all current entries
    for (const auto& entry : pImpl->entries) {
        profile_entries.push_back(entry);
    }
    
    pImpl->profiles[profile_name] = profile_entries;
}

bool Config::loadProfile(const std::string& profile_name) {
    auto profile_it = pImpl->profiles.find(profile_name);
    if (profile_it == pImpl->profiles.end()) {
        return false;
    }
    
    // Clear current entries and load profile entries
    clear();
    pImpl->current_profile = profile_name;
    
    for (const auto& entry : profile_it->second) {
        ConfigEntry new_entry = entry;
        new_entry.source = ConfigSource::Runtime; // Profile entries get Runtime source
        pImpl->entries.push_back(new_entry);
    }
    
    return true;
}

std::vector<std::string> Config::getAvailableProfiles() const {
    std::vector<std::string> profiles;
    for (const auto& pair : pImpl->profiles) {
        profiles.push_back(pair.first);
    }
    return profiles;
}

bool Config::optimize(const std::string& optimization_mode) {
    setOptimizationMode(optimization_mode);
    
    // Perform optimization based on mode
    if (optimization_mode == "memory") {
        // Optimize for memory usage
        for (auto& entry : pImpl->entries) {
            // Convert larger types to smaller ones if possible
            if (std::holds_alternative<int64_t>(entry.value) && 
                std::get<int64_t>(entry.value) >= -1000 && 
                std::get<int64_t>(entry.value) <= 1000) {
                // Could convert to int, but not necessary for optimization
            }
        }
    } else if (optimization_mode == "performance") {
        // Optimize for performance
        for (auto& entry : pImpl->entries) {
            // Ensure frequently accessed parameters are in cache-friendly locations
        }
    } else if (optimization_mode == "balanced") {
        // Balanced optimization
        // Perform basic optimizations
    }
    
    return validate();
}

void Config::setOptimizationMode(const std::string& mode) {
    pImpl->optimization_mode = mode;
}

void Config::addDependency(const std::string& key, const std::string& depends_on) {
    pImpl->dependencies[key].push_back(depends_on);
    pImpl->reverse_dependencies[depends_on].push_back(key);
}

void Config::removeDependency(const std::string& key) {
    auto dep_it = pImpl->dependencies.find(key);
    if (dep_it != pImpl->dependencies.end()) {
        // Remove reverse dependencies
        for (const auto& dep : dep_it->second) {
            auto rev_it = pImpl->reverse_dependencies.find(dep);
            if (rev_it != pImpl->reverse_dependencies.end()) {
                auto& deps = rev_it->second;
                deps.erase(std::remove(deps.begin(), deps.end(), key), deps.end());
            }
        }
        pImpl->dependencies.erase(dep_it);
    }
}

bool Config::hasDependency(const std::string& key) const {
    return pImpl->dependencies.find(key) != pImpl->dependencies.end();
}

std::vector<std::string> Config::getDependencies(const std::string& key) const {
    auto it = pImpl->dependencies.find(key);
    if (it != pImpl->dependencies.end()) {
        return it->second;
    }
    return std::vector<std::string>();
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    
    stats.total_parameters = pImpl->entries.size();
    stats.file_loaded_parameters = 0;
    stats.command_line_parameters = 0;
    stats.runtime_parameters = 0;
    stats.validation_score = 1.0;
    stats.parameters_by_group.clear();
    
    // Count parameters by source
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File:
                stats.file_loaded_parameters++;
                break;
            case ConfigSource::CommandLine:
                stats.command_line_parameters++;
                break;
            case ConfigSource::Runtime:
                stats.runtime_parameters++;
                break;
            default:
                break;
        }
        
        // Count by group
        if (!entry.group.empty()) {
            stats.parameters_by_group[entry.group]++;
        }
    }
    
    // Calculate validation score
    stats.validation_score = validate() ? 1.0 : 0.0;
    
    return stats;
}

bool Config::isSameAs(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto other_it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (other_it == other.pImpl->entries.end()) {
            return false;
        }
        
        if (other_it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

double Config::similarityScore(const Config& other) const {
    if (pImpl->entries.empty() && other.pImpl->entries.empty()) {
        return 1.0;
    }
    
    size_t max_size = std::max(pImpl->entries.size(), other.pImpl->entries.size());
    if (max_size == 0) {
        return 1.0;
    }
    
    size_t similar_count = 0;
    
    for (const auto& entry : pImpl->entries) {
        auto other_it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (other_it != other.pImpl->entries.end()) {
            if (other_it->value == entry.value) {
                similar_count++;
            }
        }
    }
    
    return static_cast<double>(similar_count) / max_size;
}

// Private helper methods

void Config::resolveDependencies() const {
    // Implementation for resolving parameter dependencies
    // This would ensure that dependent parameters are available
}

bool Config::validateDependencies() const {
    // Implementation for validating parameter dependencies
    // This would check if all required dependencies are satisfied
    return true;
}

std::string Config::resolveMacroExpansions(const std::string& value) const {
    // Implementation for resolving macro expansions in values
    // This would replace macro references with their actual values
    return value;
}

bool Config::applyProfile(const std::string& profile_name) {
    return loadProfile(profile_name);
}

bool Config::applyOptimization(const std::string& mode) {
    return optimize(mode);
}

// Template instantiations remain the same
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
