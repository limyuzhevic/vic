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
        
        // Format value based on type
        if (std::holds_alternative<int>(entry.value)) {
            file << entry.key << " = " << std::get<int>(entry.value) << "\n";
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << entry.key << " = " << std::get<int64_t>(entry.value) << "\n";
        } else if (std::holds_alternative<double>(entry.value)) {
            file << entry.key << " = " << std::get<double>(entry.value) << "\n";
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << entry.key << " = " << (std::get<bool>(entry.value) ? "true" : "false") << "\n";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"\n";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            file << entry.key << " = ";
            const auto& vec = std::get<std::vector<int>>(entry.value);
            for (size_t i = 0; i < vec.size(); ++i) {
                file << vec[i];
                if (i < vec.size() - 1) file << ",";
            }
            file << "\n";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            file << entry.key << " = ";
            const auto& vec = std::get<std::vector<double>>(entry.value);
            for (size_t i = 0; i < vec.size(); ++i) {
                file << vec[i];
                if (i < vec.size() - 1) file << ",";
            }
            file << "\n";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            file << entry.key << " = ";
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            for (size_t i = 0; i < vec.size(); ++i) {
                file << "\"" << vec[i] << "\"";
                if (i < vec.size() - 1) file << ",";
            }
            file << "\n";
        }
    }
    
    return true;
}

// Set value with proper type handling and validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Remove any existing entry with the same key
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Create new entry
    std::string description = getDefaultDescription(key);
    pImpl->entries.emplace_back(key, value, source, description);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to parse the string value into appropriate type
    // For now, store as string and let getOr() handle conversion
    set(key, value, source);
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

// Get value with proper type handling
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Get value with default
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value.has_value() ? *value : defaultValue;
}

// Check if key exists
bool Config::has(const std::string& key) const {
    return get<void>(key).has_value();
}

// Get all keys
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Clear all entries
void Config::clear() {
    pImpl->entries.clear();
}

// Get configuration summary
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << ": ";
        
        if (std::holds_alternative<int>(entry.value)) {
            oss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            oss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            oss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            oss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            oss << "\"" << std::get<std::string>(entry.value) << "\"";
        }
        
        oss << " (" << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
}

// Default descriptions for common configuration keys
std::string Config::getDefaultDescription(const std::string& key) const {
    // Return appropriate description based on key
    if (key == "neuron_count") return "Number of neurons in the brain";
    if (key == "region_count") return "Number of neural regions";
    if (key == "connection_probability") return "Probability of connection between neurons";
    if (key == "simulation_timestep") return "Time step for simulation (seconds)";
    if (key == "random_seed") return "Random seed for reproducible results";
    if (key == "v_thresh") return "Neuron firing threshold (mV)";
    if (key == "v_rest") return "Neuron resting potential (mV)";
    if (key == "v_reset") return "Neuron reset potential after spike (mV)";
    if (key == "tau_mem") return "Membrane time constant (ms)";
    if (key == "tau_ref") return "Refractory period (ms)";
    if (key == "stdp_ltp_weight") return "STDP long-term potentiation weight";
    if (key == "stdp_ltd_weight") return "STDP long-term depression weight";
    if (key == "stdp_tau") return "STDP time constant (ms)";
    if (key == "synaptogenesis_rate") return "Rate of new synapse formation";
    if (key == "pruning_rate") return "Rate of synapse pruning";
    if (key == "curiosity.enable") return "Enable curiosity-driven exploration";
    if (key == "novelty.enable") return "Enable novelty detection";
    if (key == "enable_development") return "Enable developmental processes";
    if (key == "enable_checkpointing") return "Enable checkpoint save/load";
    
    return "Configuration parameter: " + key;
}

// Trim whitespace from string
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// Convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace nlm


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
