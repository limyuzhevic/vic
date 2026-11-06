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
    bool helpRequested = false;
    bool versionRequested = false;
    std::string configFile;
    
    // Enhanced command line argument parsing with better error handling
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help and -h
        if (arg == "--help" || arg == "-h" || arg == "--usage") {
            helpRequested = true;
            // Continue to collect all arguments for help generation
        }
        // Handle --version and -v
        else if (arg == "--version" || arg == "-v") {
            versionRequested = true;
            // Continue to collect all arguments
        }
        // Handle --config and -c
        else if ((arg == "--config" || arg == "-c") && i + 1 < argc) {
            configFile = argv[++i];
            // Try to load from config file
            if (loadFromFile(configFile)) {
                NLM_LOG_INFO("Loaded configuration from command line config file: " + configFile);
            } else {
                NLM_LOG_WARNING("Configuration file not found or invalid: " + configFile);
            }
        }
        // Handle --key=value format
        else if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate and set the value
                if (!setFromString(key, value)) {
                    NLM_LOG_WARNING("Invalid value for argument " + key + ": " + value);
                }
            } else {
                NLM_LOG_WARNING("Malformed argument (expected --key=value): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && arg.size() > 1) {
            if (arg[1] == '-' || i + 1 >= argc) {
                // It's --something or a flag-like argument without value
                if (arg == "--verbose" || arg == "--quiet" || arg == "--debug") {
                    set(arg.substr(2), true, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Unrecognized argument: " + arg);
                }
            } else {
                // It's -key value format
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                // Validate and set the value
                if (!setFromString(key, value)) {
                    NLM_LOG_WARNING("Invalid value for argument " + key + ": " + value);
                }
            }
        }
        // Handle positional arguments (for future extensibility)
        else if (!helpRequested && !versionRequested && arg.substr(0, 2) != "--") {
            // Could be a positional argument, store for advanced usage
            // Currently treating as unrecognized for backward compatibility
            NLM_LOG_WARNING("Unrecognized positional argument: " + arg);
        }
    }
    
    // Handle help and version requests
    if (helpRequested) {
        printHelp();
        return false; // Don't continue with help/version
    }
    
    if (versionRequested) {
        printVersion();
        return false; // Don't continue with help/version
    }
    
    return true;
}

void Config::printHelp() const {
    std::cout << "NLM (Neural Learning Machine) - Help" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm [--help] [--version] [--config=<file>] [args...]" << std::endl;
    std::cout << std::endl;
    std::cout << "General Options:" << std::endl;
    std::cout << "  --help, -h          Show this help message" << std::endl;
    std::cout << "  --version, -v       Show version information" << std::endl;
    std::cout << "  --config=<file>, -c Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Demo-Specific Options:" << std::endl;
    printDemoOptions();
    std::cout << std::endl;
    std::cout << "Common Configuration Keys:" << std::endl;
    printCommonOptions();
    std::cout << std::endl;
}

void Config::printVersion() const {
    std::cout << "NLM (Neural Learning Machine) Version 0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Built with C++20, CMake 3.16+" << std::endl;
}

bool Config::setFromString(const std::string& key, const std::string& value) {
    // Try to parse the value based on key patterns
    // This is a simple heuristic - more sophisticated parsing could be added
    
    // Check for numeric patterns
    bool isInt = true;
    bool isFloat = true;
    for (char c : value) {
        if (c == '-' || c == '+') continue;
        if (c == '.') {
            isInt = false;
            continue;
        }
        if (!std::isdigit(c)) {
            isInt = false;
            isFloat = false;
            break;
        }
    }
    
    // Check if it's a float (contains . and has digits after it)
    if (isFloat && value.find('.') != std::string::npos) {
        size_t dotPos = value.find('.');
        if (dotPos == 0 || dotPos == value.size() - 1) {
            isFloat = false; // . at start or end is not a valid float
        } else {
            // Check if there are any non-digit characters after the dot
            std::string afterDot = value.substr(dotPos + 1);
            for (char c : afterDot) {
                if (!std::isdigit(c)) {
                    isFloat = false;
                    break;
                }
            }
        }
    }
    
    if (isInt) {
        try {
            int intVal = std::stoi(value);
            set(key, intVal, ConfigSource::CommandLine);
            return true;
        } catch (const std::exception&) {
            // Fall through to float parsing
        }
    }
    
    if (isFloat) {
        try {
            float floatVal = std::stof(value);
            set(key, floatVal, ConfigSource::CommandLine);
            return true;
        } catch (const std::exception&) {
            // Fall through to boolean parsing
        }
    }
    
    // Try boolean parsing
    std::string lowerValue = toLower(value);
    if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "1" || 
        lowerValue == "on" || lowerValue == "enable" || lowerValue == "enabled") {
        set(key, true, ConfigSource::CommandLine);
        return true;
    }
    if (lowerValue == "false" || lowerValue == "no" || lowerValue == "0" || 
        lowerValue == "off" || lowerValue == "disable" || lowerValue == "disabled") {
        set(key, false, ConfigSource::CommandLine);
        return true;
    }
    
    // Try vector parsing (e.g., "1,2,3" or "1.5,2.7,3.9")
    if (value.find(',') != std::string::npos) {
        try {
            std::vector<float> vec;
            std::stringstream ss(value);
            std::string token;
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                if (!token.empty()) {
                    float val = std::stof(token);
                    vec.push_back(val);
                }
            }
            if (!vec.empty()) {
                set(key, vec, ConfigSource::CommandLine);
                return true;
            }
        } catch (const std::exception&) {
            // Fall through to string parsing
        }
    }
    
    // Default to string
    set(key, value, ConfigSource::CommandLine);
    return true;
}

void Config::printDemoOptions() const {
    // Common demo-specific options
    std::cout << "  Brain Options:" << std::endl;
    std::cout << "    --neuron_count=<n>     Number of neurons (default: 1000)" << std::endl;
    std::cout << "    --region_count=<n>      Number of neural regions (default: 1)" << std::endl;
    std::cout << "    --connection_probability=<p> Probability of connections (default: 0.1)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "  Simulation Options:" << std::endl;
    std::cout << "    --simulation_timestep=<t> Time step in seconds (default: 0.001)" << std::endl;
    std::cout << "    --max_steps=<n>         Maximum simulation steps" << std::endl;
    std::cout << std::endl;
    
    std::cout << "  Plasticity Options:" << std::endl;
    std::cout << "    --stdp_ltp_weight=<w>   STDP LTP weight (default: 0.01)" << std::endl;
    std::cout << "    --stdp_ltd_weight=<w>   STDP LTD weight (default: 0.012)" << std::endl;
    std::cout << "    --stdp_tau=<t>          STDP time constant (default: 20)" << std::endl;
    std::cout << std::endl;
}

void Config::printCommonOptions() const {
    // Print all registered configuration keys
    for (const auto& entry : pImpl->entries) {
        std::cout << "    --" << entry.key << "=<value>     " << entry.description << std::endl;
    }
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
