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
    int lineNumber = 0;
    while (std::getline(file, line)) {
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
            
            // Validate key format
            if (key.empty() || !isValidKey(key)) {
                NLM_LOG_WARNING("Invalid configuration key at line " + std::to_string(lineNumber) + ": '" + key + "'");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid configuration format at line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}

bool Config::validate() const {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        bool valid = validateValue(entry.key, entry.value);
        if (!valid) {
            NLM_LOG_ERROR("Invalid configuration value: " + entry.key + " = " + getStringValue(entry.value));
            allValid = false;
        }
    }
    
    return allValid;
}

bool Config::isValidKey(const std::string& key) const {
    // Valid keys should be alphanumeric with underscores or hyphens
    return !key.empty() && std::all_of(key.begin(), key.end(),
        [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Type-specific validation
    bool valid = true;
    
    // Check for known configuration keys and validate them
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t neurons = std::get<int64_t>(value);
            if (neurons < 10 || neurons > 1000000) {
                NLM_LOG_ERROR("Invalid neuron count: " + std::to_string(neurons) +
                             ". Must be between 10 and 1000000.");
                valid = false;
            }
        }
    }
    else if (key == "simulation_steps") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t steps = std::get<int64_t>(value);
            if (steps < 1 || steps > 10000000) {
                NLM_LOG_ERROR("Invalid simulation steps: " + std::to_string(steps) +
                             ". Must be between 1 and 10000000.");
                valid = false;
            }
        }
    }
    else if (key == "simulation_timestep") {
        if (std::holds_alternative<double>(value)) {
            double timestep = std::get<double>(value);
            if (timestep <= 0.0 || timestep > 1.0) {
                NLM_LOG_ERROR("Invalid timestep: " + std::to_string(timestep) +
                             ". Must be between 0.0001 and 1.0 seconds.");
                valid = false;
            }
        }
    }
    else if (key == "region_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t regions = std::get<int64_t>(value);
            if (regions < 1 || regions > 100) {
                NLM_LOG_ERROR("Invalid region count: " + std::to_string(regions) +
                             ". Must be between 1 and 100.");
                valid = false;
            }
        }
    }
    else if (key == "connection_probability") {
        if (std::holds_alternative<float>(value)) {
            float prob = std::get<float>(value);
            if (prob < 0.0f || prob > 1.0f) {
                NLM_LOG_ERROR("Invalid connection probability: " + std::to_string(prob) +
                             ". Must be between 0.0 and 1.0.");
                valid = false;
            }
        }
    }
    else if (key == "random_seed") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t seed = std::get<int64_t>(value);
            if (seed < 0 || seed > UINT32_MAX) {
                NLM_LOG_ERROR("Invalid random seed: " + std::to_string(seed) +
                             ". Must be between 0 and 4294967295.");
                valid = false;
            }
        }
    }
    else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight") {
        if (std::holds_alternative<float>(value)) {
            float weight = std::get<float>(value);
            if (weight < 0.0f || weight > 1.0f) {
                NLM_LOG_ERROR("Invalid STDP weight for " + key + ": " + std::to_string(weight) +
                             ". Must be between 0.0 and 1.0.");
                valid = false;
            }
        }
    }
    else if (key == "synaptogenesis_rate" || key == "pruning_rate") {
        if (std::holds_alternative<float>(value)) {
            float rate = std::get<float>(value);
            if (rate < 0.0f || rate > 0.1f) {
                NLM_LOG_ERROR("Invalid plasticity rate for " + key + ": " + std::to_string(rate) +
                             ". Must be between 0.0 and 0.1.");
                valid = false;
            }
        }
    }
    
    return valid;
}

std::string Config::getStringValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
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
