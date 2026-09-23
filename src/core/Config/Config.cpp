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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

bool Config::validate() const {
    // Check for essential configuration requirements
    bool hasNeuronCount = has("brain.neuron_count") && 
                         get<int64_t>("brain.neuron_count").has_value() &&
                         get<int64_t>("brain.neuron_count").value() > 0;
    
    bool hasRegionCount = has("brain.region_count") && 
                         get<int64_t>("brain.region_count").has_value() &&
                         get<int64_t>("brain.region_count").value() > 0;
    
    bool hasConnectionProbability = has("brain.connection_probability") &&
                                   get<double>("brain.connection_probability").has_value() &&
                                   get<double>("brain.connection_probability").value() >= 0.0 &&
                                   get<double>("brain.connection_probability").value() <= 1.0;
    
    // Check consistency
    bool consistent = true;
    if (has("brain.neuron_count") && has("brain.region_count")) {
        int64_t neuronCount = get<int64_t>("brain.neuron_count").value();
        int64_t regionCount = get<int64_t>("brain.region_count").value();
        if (neuronCount < regionCount) {
            consistent = false;
        }
    }
    
    return hasNeuronCount && hasRegionCount && hasConnectionProbability && consistent;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!has("brain.neuron_count") || 
        !get<int64_t>("brain.neuron_count").has_value() ||
        get<int64_t>("brain.neuron_count").value() <= 0) {
        errors.push_back("brain.neuron_count must be a positive integer");
    }
    
    if (!has("brain.region_count") ||
        !get<int64_t>("brain.region_count").has_value() ||
        get<int64_t>("brain.region_count").value() <= 0) {
        errors.push_back("brain.region_count must be a positive integer");
    }
    
    if (has("brain.neuron_count") && has("brain.region_count")) {
        int64_t neuronCount = get<int64_t>("brain.neuron_count").value();
        int64_t regionCount = get<int64_t>("brain.region_count").value();
        if (neuronCount < regionCount) {
            errors.push_back("brain.neuron_count must be >= brain.region_count");
        }
    }
    
    if (has("brain.connection_probability")) {
        auto probOpt = get<double>("brain.connection_probability");
        if (!probOpt.has_value()) {
            errors.push_back("brain.connection_probability must be a number");
        } else if (probOpt.value() < 0.0 || probOpt.value() > 1.0) {
            errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
        }
    }
    
    return errors;
}

std::map<std::string, std::string> Config::getConfigGroups() const {
    std::map<std::string, std::string> groups;
    
    // Group settings by section
    for (const auto& entry : pImpl->entries) {
        size_t dotPos = entry.key.find('.');
        if (dotPos != std::string::npos) {
            std::string group = entry.key.substr(0, dotPos);
            groups[group] = (groups.find(group) != groups.end()) ? 
                          groups[group] + ", " + entry.key : 
                          entry.key;
        } else {
            groups["general"] = (groups.find("general") != groups.end()) ?
                             groups["general"] + ", " + entry.key :
                             entry.key;
        }
    }
    
    return groups;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

bool Config::loadFromJSON(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // This is a placeholder for future implementation
    NLM_LOG_WARN("JSON config loading not yet implemented, using simple format");
    return loadFromFile(filepath);
}

bool Config::saveToJSON(const std::string& filepath) const {
    // TODO PHASE 2: Implement proper JSON/YAML serializer
    // This is a placeholder for future implementation
    NLM_LOG_WARN("JSON config saving not yet implemented");
    return saveToFile(filepath);
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end() || otherIt->value != entry.value) {
            return false;
        }
    }
    
    return true;
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
