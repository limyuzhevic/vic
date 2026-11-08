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
    bool showHelp = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help flag
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Boolean flag (--verbose, --debug)
                set(arg.substr(2), true, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format or --key value
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                // Check if value looks like a boolean
                if (value == "true" || value == "false" || value == "1" || value == "0" ||
                    value == "yes" || value == "no" || value == "on" || value == "off") {
                    bool boolVal = (value == "true" || value == "1" || value == "yes" || value == "on");
                    set(key, boolVal, ConfigSource::CommandLine);
                } else {
                    // Try to determine if it's a number
                    try {
                        // Check if it's an integer
                        size_t pos;
                        int64_t intVal = std::stoll(value, &pos);
                        if (pos == value.size()) {
                            set(key, intVal, ConfigSource::CommandLine);
                            continue;
                        }
                        // Check if it's a float
                        double doubleVal = std::stod(value, &pos);
                        if (pos == value.size()) {
                            set(key, doubleVal, ConfigSource::CommandLine);
                            continue;
                        }
                    } catch (...) {
                        // Fall through to string
                    }
                    set(key, value, ConfigSource::CommandLine);
                }
            } else {
                // Boolean flag (-v, -d)
                set(key, true, ConfigSource::CommandLine);
            }
        } else {
            // Unknown argument
            // std::cerr << "Warning: Unknown argument: " << arg << std::endl;
        }
    }
    
    if (showHelp) {
        // Print help information
        std::cout << "NLM Configuration Options:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --config=file                 Load configuration from file" << std::endl;
        std::cout << "  --key=value                   Set configuration key to value" << std::endl;
        std::cout << "  -key value                    Set configuration key to value" << std::endl;
        std::cout << "  -key                           Set boolean flag to true" << std::endl;
        std::cout << "\nCommon keys:" << std::endl;
        std::cout << "  random_seed                   Random seed (int64)" << std::endl;
        std::cout << "  simulation_timestep           Timestep duration (double)" << std::endl;
        std::cout << "  neuron_count                  Number of neurons (uint64)" << std::endl;
        std::cout << "  region_count                  Number of regions (uint64)" << std::endl;
        std::cout << "  log_level                     Logging level (string)" << std::endl;
        std::cout << "  visualization_enabled         Enable visualization (bool)" << std::endl;
        std::cout << "  --verbose, --debug           Enable verbose debugging" << std::endl;
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
        
        // Convert ConfigValue to string representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
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

// Validation methods
bool Config::validate(std::vector<std::string>& errors) const {
    errors.clear();
    
    // Check for duplicate keys
    if (hasDuplicateKeys()) {
        errors.push_back("Configuration contains duplicate keys");
        return false;
    }
    
    // Validate specific keys based on expected types
    for (const auto& entry : pImpl->entries) {
        // Type-specific validation
        if (entry.key == "random_seed") {
            if (!std::holds_alternative<int64_t>(entry.value) && 
                !std::holds_alternative<int>(entry.value)) {
                errors.push_back("random_seed must be an integer");
            }
        } else if (entry.key == "simulation_timestep") {
            if (!std::holds_alternative<double>(entry.value) && 
                !std::holds_alternative<float>(entry.value)) {
                errors.push_back("simulation_timestep must be a number");
            } else {
                double timestep = std::visit([](auto&& v) { return static_cast<double>(v); }, entry.value);
                if (timestep <= 0.0) {
                    errors.push_back("simulation_timestep must be positive");
                }
            }
        } else if (entry.key == "neuron_count") {
            if (!std::holds_alternative<uint64_t>(entry.value) && 
                !std::holds_alternative<size_t>(entry.value) &&
                !std::holds_alternative<int64_t>(entry.value) &&
                !std::holds_alternative<int>(entry.value)) {
                errors.push_back("neuron_count must be a positive integer");
            } else {
                uint64_t count = std::visit([](auto&& v) { return static_cast<uint64_t>(v); }, entry.value);
                if (count == 0) {
                    errors.push_back("neuron_count must be positive");
                }
            }
        } else if (entry.key == "log_level") {
            if (!std::holds_alternative<std::string>(entry.value)) {
                errors.push_back("log_level must be a string");
            }
        } else if (entry.key == "visualization_enabled") {
            if (!std::holds_alternative<bool>(entry.value)) {
                errors.push_back("visualization_enabled must be a boolean");
            }
        }
    }
    
    return errors.empty();
}

bool Config::hasDuplicateKeys() const {
    std::set<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (keys.find(entry.key) != keys.end()) {
            return true;
        }
        keys.insert(entry.key);
    }
    return false;
}

const std::vector<ConfigEntry>& Config::getEntries() const {
    return pImpl->entries;
}

std::string Config::getValueType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "NOT_FOUND";
    }
    
    if (std::holds_alternative<int>(it->value)) return "int";
    if (std::holds_alternative<int64_t>(it->value)) return "int64_t";
    if (std::holds_alternative<uint64_t>(it->value)) return "uint64_t";
    if (std::holds_alternative<float>(it->value)) return "float";
    if (std::holds_alternative<double>(it->value)) return "double";
    if (std::holds_alternative<bool>(it->value)) return "bool";
    if (std::holds_alternative<std::string>(it->value)) return "string";
    if (std::holds_alternative<std::vector<int>>(it->value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(it->value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(it->value)) return "vector<string>";
    
    return "UNKNOWN";
}

template<typename T>
bool Config::canConvertTo(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    // Check if the stored type can be converted to target type T
    if constexpr (std::is_same_v<T, int>) {
        return std::holds_alternative<int>(it->value) ||
               std::holds_alternative<int64_t>(it->value) ||
               std::holds_alternative<uint64_t>(it->value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return std::holds_alternative<int>(it->value) ||
               std::holds_alternative<int64_t>(it->value) ||
               std::holds_alternative<uint64_t>(it->value);
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        return std::holds_alternative<int>(it->value) ||
               std::holds_alternative<int64_t>(it->value) ||
               std::holds_alternative<uint64_t>(it->value);
    } else if constexpr (std::is_same_v<T, float>) {
        return std::holds_alternative<int>(it->value) ||
               std::holds_alternative<int64_t>(it->value) ||
               std::holds_alternative<uint64_t>(it->value) ||
               std::holds_alternative<float>(it->value) ||
               std::holds_alternative<double>(it->value);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::holds_alternative<int>(it->value) ||
               std::holds_alternative<int64_t>(it->value) ||
               std::holds_alternative<uint64_t>(it->value) ||
               std::holds_alternative<float>(it->value) ||
               std::holds_alternative<double>(it->value);
    } else if constexpr (std::is_same_v<T, bool>) {
        return std::holds_alternative<bool>(it->value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return std::holds_alternative<std::string>(it->value);
    }
    
    return false;
}

template<typename T>
std::optional<T> Config::getWithValidation(const std::string& key) const {
    // First check if we can convert to the target type
    if (!canConvertTo<T>(key)) {
        return std::nullopt;
    }
    
    return get<T>(key);
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
