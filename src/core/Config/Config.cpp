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
    bool hasHelp = false;
    bool hasList = false;
    bool hasValidate = false;
    bool hasReset = false;
    
    // First pass: collect flags
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help command
        if (arg == "--help" || arg == "-h") {
            hasHelp = true;
        }
        // Handle list command
        else if (arg == "--list" || arg == "-l") {
            hasList = true;
        }
        // Handle validate command
        else if (arg == "--validate" || arg == "-v") {
            hasValidate = true;
        }
        // Handle reset command
        else if (arg == "--reset" || arg == "-r") {
            hasReset = true;
        }
    }
    
    // Handle help command
    if (hasHelp) {
        printHelp();
        return true;
    }
    
    // Handle list command
    if (hasList) {
        listConfiguration();
        return true;
    }
    
    // Handle reset command
    if (hasReset) {
        resetToDefaults();
        return true;
    }
    
    // Handle validation command
    if (hasValidate) {
        if (!validateConfiguration()) {
            NLM_LOG_ERROR("Configuration validation failed");
        }
        return true;
    }
    
    // Normal command-line parsing (original logic)
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

void Config::printHelp() const {
    NLM_LOG_INFO("Available configuration commands:");
    NLM_LOG_INFO("  -h, --help                  Display this help message");
    NLM_LOG_INFO("  -l, --list                 List all configuration keys");
    NLM_LOG_INFO("  -v, --validate             Validate current configuration");
    NLM_LOG_INFO("  -r, --reset                Reset to default configuration");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Usage examples:");
    NLM_LOG_INFO("  --config=config_file       Load configuration from file");
    NLM_LOG_INFO("  --neuron_count=1000       Set neuron count to 1000");
    NLM_LOG_INFO("  --connection_probability=0.2");
    NLM_LOG_INFO("  --random_seed=42          Set random seed to 42");
}

void Config::listConfiguration() const {
    NLM_LOG_INFO("Available configuration keys:");
    for (const auto& entry : pImpl->entries) {
        NLM_LOG_INFO("  " + entry.key + " (" + std::to_string(static_cast<int>(entry.source)) + ")");
    }
}

void Config::resetToDefaults() {
    clear();
    // Re-apply default values
    set("random_seed", 42, ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", 1000, ConfigSource::Default);
    set("region_count", 1, ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    NLM_LOG_INFO("Configuration reset to defaults");
}

bool Config::validateConfiguration() const {
    bool isValid = true;
    
    // Check required configurations
    if (!has("neuron_count") || getOr<int64_t>("neuron_count", 0) <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count configuration: must be positive");
        isValid = false;
    }
    
    if (!has("region_count") || getOr<int64_t>("region_count", 0) <= 0) {
        NLM_LOG_ERROR("Invalid region_count configuration: must be positive");
        isValid = false;
    }
    
    if (!has("connection_probability") || 
        getOr<float>("connection_probability", 1.1f) < 0.0f || 
        getOr<float>("connection_probability", 1.1f) > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability configuration: must be between 0.0 and 1.0");
        isValid = false;
    }
    
    if (!has("simulation_timestep") || getOr<double>("simulation_timestep", 0.0) <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep configuration: must be positive");
        isValid = false;
    }
    
    // Check for conflicting configurations
    if (has("neuron_count") && has("region_count")) {
        int64_t neuronCount = getOr<int64_t>("neuron_count", 0);
        int64_t regionCount = getOr<int64_t>("region_count", 0);
        if (neuronCount / regionCount < 1) {
            NLM_LOG_ERROR("Invalid configuration: too many regions for given neuron count");
            isValid = false;
        }
    }
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation passed");
    }
    
    return isValid;
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
