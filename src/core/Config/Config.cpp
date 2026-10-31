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
        
        // Handle --key=value format (enhanced)
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format (enhanced)
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle boolean flags (--verbose)
        else if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(2);
            set(key, "1", ConfigSource::CommandLine);
        }
        // Handle short boolean flags (-v)
        else if (arg.size() == 2 && arg[0] == '-') {
            std::string key = arg.substr(1);
            set(key, "1", ConfigSource::CommandLine);
        }
    }
    return true;
}

// Load from environment variables
void Config::loadFromEnv(const std::string& prefix) {
    for (const auto& env : std::filesystem::environment()) {
        if (env.first.find(prefix) == 0) {
            std::string key = env.first.substr(prefix.size());
            std::string value = env.second;
            set(key, value, ConfigSource::File);
        }
    }
}

// Merge configuration from another config
void Config::mergeFrom(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

// Override configuration values from another config
void Config::overrideFrom(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, ConfigSource::CommandLine);
    }
}

// Type-safe getters with automatic conversion
int Config::getInt(const std::string& key, int defaultValue) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto val = get<double>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::vector<int> Config::getIntList(const std::string& key) const {
    auto val = get<std::vector<int>>(key);
    return val.has_value() ? val.value() : std::vector<int>();
}

std::vector<double> Config::getDoubleList(const std::string& key) const {
    auto val = get<std::vector<double>>(key);
    return val.has_value() ? val.value() : std::vector<double>();
}

// Convenience setters
template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source) {
    // Convert to ConfigValue
    ConfigValue configValue;
    if constexpr (std::is_same_v<T, int>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, double>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::string>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        configValue = value;
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported type for Config::set");
    }
    
    set(key, configValue, source);
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, value, source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, value, source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

// Tab completion support
std::vector<std::string> Config::getTabCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(partial) == 0) {
            completions.push_back(entry.key);
        }
    }
    return completions;
}

// Print help for all configuration options
void Config::printHelp() const {
    std::cout << "=== Configuration Help ===" << std::endl;
    std::cout << "Common configuration options:" << std::endl;
    std::cout << "  --neuron_count=N          Number of neurons to create (default: 500)" << std::endl;
    std::cout << "  --region_count=N          Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection_probability=F Connection probability between neurons (0.0-1.0, default: 0.1)" << std::endl;
    std::cout << "  --stdp_ltp_weight=F       STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "  --stdp_ltd_weight=F       STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "  --simulation_timestep=F    Simulation time step in seconds (default: 0.001)" << std::endl;
    std::cout << "  --random_seed=N            Random seed for reproducibility (default: 42)" << std::endl;
    std::cout << "  --checkpoint_dir=S         Directory for checkpoint files (default: ./checkpoints)" << std::endl;
    std::cout << "  --enable_replay            Enable episodic memory replay" << std::endl;
    std::cout << "  --enable_development       Enable developmental processes" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
}

// Print configuration summary
std::string Config::getSummary() const {
    std::ostringstream oss;
    oss << "=== Configuration Summary ===" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << "" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
}

// Get configuration statistics
Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: stats.defaultEntries++; break;
            case ConfigSource::File: stats.fileEntries++; break;
            case ConfigSource::CommandLine: stats.commandLineEntries++; break;
            case ConfigSource::Runtime: stats.runtimeEntries++; break;
        }
    }
    
    return stats;
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
