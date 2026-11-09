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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
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
    bool helpRequested = false;
    bool versionRequested = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help and -h
        if (arg == "--help" || arg == "-h") {
            helpRequested = true;
            continue;
        }
        
        // Handle --version and -v
        if (arg == "--version" || arg == "-v") {
            versionRequested = true;
            continue;
        }
        
        // Handle --config=file or --config file
        if (arg.substr(0, 8) == "--config") {
            std::string configFile;
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            
            if (!configFile.empty()) {
                if (!loadFromFile(configFile)) {
                    NLM_LOG_ERROR("Failed to load configuration from: " + configFile);
                    return false;
                }
            }
            continue;
        }
        
        // Handle other key=value or -key value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid --key=value format: " + arg);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Unrecognized argument: " + arg);
        }
    }
    
    // Print help if requested
    if (helpRequested) {
        printHelp();
        return false;  // Don't proceed with help/version
    }
    
    // Print version if requested
    if (versionRequested) {
        printVersion();
        return false;  // Don't proceed with help/version
    }
    
    return true;
}

void Config::printHelp() const {
    NLM_LOG_INFO("=== NLM Configuration Help ===");
    NLM_LOG_INFO("--help, -h          : Show this help message");
    NLM_LOG_INFO("--version, -v       : Show version information");
    NLM_LOG_INFO("--config=file       : Load configuration from file");
    NLM_LOG_INFO("--key=value         : Set configuration key-value pair");
    NLM_LOG_INFO("-key value          : Set configuration key-value pair (alternative format)");
    NLM_LOG_INFO("\nCommon configuration keys:");
    NLM_LOG_INFO("  random_seed           : Random number generator seed");
    NLM_LOG_INFO("  simulation_timestep   : Simulation timestep (seconds)");
    NLM_LOG_INFO("  neuron_count          : Total number of neurons");
    NLM_LOG_INFO("  region_count          : Number of brain regions");
    NLM_LOG_INFO("  connection_probability : Synapse connection probability");
    NLM_LOG_INFO("  stdp_ltp_weight       : STDP LTP weight");
    NLM_LOG_INFO("  stdp_ltd_weight       : STDP LTD weight");
    NLM_LOG_INFO("  synaptogenesis_rate   : Synapse creation rate");
    NLM_LOG_INFO("  pruning_rate          : Synapse removal rate");
}

void Config::printVersion() const {
    NLM_LOG_INFO("NLM - Neural Learning Machine v2.0 (Phase 6)");
    NLM_LOG_INFO("An experimental artificial developmental brain system");
    NLM_LOG_INFO("Build date: " << __DATE__ << " " << __TIME__);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM on " << __DATE__ << " " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (Source: " << static_cast<int>(entry.source) << ")\n";
        
        // Write the key=value pair with proper formatting
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes in the string value
                std::string escaped = val;
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                file << entry.key << " = \"" << escaped << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (val ? "true" : "false") << "\n";
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << entry.key << " = " << val << "\n";
            } else {
                file << entry.key << " = " << "\"" << "unformatted_value" << "\"\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

void Config::printHelp() const {
    NLM_LOG_INFO("=== NLM Configuration Help ===");
    NLM_LOG_INFO("--help, -h          : Show this help message");
    NLM_LOG_INFO("--version, -v       : Show version information");
    NLM_LOG_INFO("--config=file       : Load configuration from file");
    NLM_LOG_INFO("--key=value         : Set configuration key-value pair");
    NLM_LOG_INFO("-key value          : Set configuration key-value pair (alternative format)");
    NLM_LOG_INFO("\nCommon configuration keys:");
    NLM_LOG_INFO("  random_seed           : Random number generator seed");
    NLM_LOG_INFO("  simulation_timestep   : Simulation timestep (seconds)");
    NLM_LOG_INFO("  neuron_count          : Total number of neurons");
    NLM_LOG_INFO("  region_count          : Number of brain regions");
    NLM_LOG_INFO("  connection_probability : Synapse connection probability");
    NLM_LOG_INFO("  stdp_ltp_weight       : STDP LTP weight");
    NLM_LOG_INFO("  stdp_ltd_weight       : STDP LTD weight");
    NLM_LOG_INFO("  synaptogenesis_rate   : Synapse creation rate");
    NLM_LOG_INFO("  pruning_rate          : Synapse removal rate");
}

void Config::printVersion() const {
    NLM_LOG_INFO("NLM - Neural Learning Machine v2.0 (Phase 6)");
    NLM_LOG_INFO("An experimental artificial developmental brain system");
    NLM_LOG_INFO("Build date: " << __DATE__ << " " << __TIME__);
}

void Config::addEntry(const std::string& key, const ConfigValue& value, const std::string& description) {
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.description = description;
    entry.source = ConfigSource::Default;
    pImpl->entries.push_back(entry);
    NLM_LOG_INFO("Added configuration: " << key << " = " << value);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
