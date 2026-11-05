#include <sstream>
#include <chrono>
#include <iomanip>
#include <functional>
#include <unordered_map>

namespace nlm {

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Skipping malformed configuration line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Skipping configuration line " + std::to_string(lineNumber) + " with empty key");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
        NLM_LOG_DEBUG("Loaded config: " + key + " = " + value + " (line " + std::to_string(lineNumber) + ")");
    }
    
    if (lineNumber == 0) {
        NLM_LOG_WARNING("Configuration file " + filepath + " is empty or contains only comments");
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Loaded config from command line: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Malformed command line argument: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_DEBUG("Loaded config from command line: " + key + " = " + value);
        }
        else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " + arg);
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimeString() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        std::visit([&file, &entry](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, char>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << "UNKNOWN_TYPE\n";
            }
        }, entry.value);
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

std::string Config::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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

// Configuration validation schema
struct ConfigValidation {
    bool required;
    std::function<bool(const std::string&)> validate;
    std::string description;
    
    ConfigValidation(bool req = false, 
                    std::function<bool(const std::string&)> val = nullptr,
                    const std::string& desc = "")
        : required(req), validate(val), description(desc) {}
};

using ConfigSchema = std::unordered_map<std::string, ConfigValidation>;

// Configuration validation
bool Config::validate(const ConfigSchema& schema) const {
    bool success = true;
    
    for (const auto& [key, validation] : schema) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required configuration key: " + key);
            if (validation.required) {
                NLM_LOG_ERROR("Required configuration key missing: " + key);
                success = false;
            }
            continue;
        }
        
        // Validate the value based on schema validation
        if (!validation.validate(getOr<std::string>(key, ""))) {
            NLM_LOG_ERROR("Invalid configuration value for key: " + key);
            success = false;
        }
    }
    
    return success;
}

// Get configuration schema for validation
ConfigSchema Config::getSchema() {
    ConfigSchema schema;
    
    // Core configuration keys with validation rules
    schema["neuron_count"] = {
        true,
        [](const std::string& value) {
            try {
                size_t val = std::stoull(value);
                return val > 0 && val <= 100000;
            } catch (...) {
                return false;
            }
        },
        "Number of neurons in the brain (1-100000)"
    };
    
    schema["region_count"] = {
        true,
        [](const std::string& value) {
            try {
                size_t val = std::stoull(value);
                return val > 0 && val <= 100;
            } catch (...) {
                return false;
            }
        },
        "Number of brain regions (1-100)"
    };
    
    schema["connection_probability"] = {
        true,
        [](const std::string& value) {
            try {
                float val = std::stof(value);
                return val >= 0.0f && val <= 1.0f;
            } catch (...) {
                return false;
            }
        },
        "Probability of synapse formation (0.0-1.0)"
    };
    
    schema["simulation_timestep"] = {
        true,
        [](const std::string& value) {
            try {
                double val = std::stod(value);
                return val > 0.0 && val <= 1.0;
            } catch (...) {
                return false;
            }
        },
        "Simulation time step in seconds (0.001-1.0)"
    };
    
    schema["stdp_ltp_weight"] = {
        true,
        [](const std::string& value) {
            try {
                float val = std::stof(value);
                return val >= 0.0f && val <= 1.0f;
            } catch (...) {
                return false;
            }
        },
        "STDP long-term potentiation weight (0.0-1.0)"
    };
    
    schema["stdp_ltd_weight"] = {
        true,
        [](const std::string& value) {
            try {
                float val = std::stof(value);
                return val >= 0.0f && val <= 1.0f;
            } catch (...) {
                return false;
            }
        },
        "STDP long-term depression weight (0.0-1.0)"
    };
    
    schema["stdp_tau"] = {
        true,
        [](const std::string& value) {
            try {
                float val = std::stof(value);
                return val > 0.0f && val <= 1000.0f;
            } catch (...) {
                return false;
            }
        },
        "STDP time constant (0.01-1000.0)"
    };
    
    schema["synaptogenesis_rate"] = {
        true,
        [](const std::string& value) {
            try {
                float val = std::stof(value);
                return val >= 0.0f && val <= 0.01f;
            } catch (...) {
                return false;
            }
        },
        "Synaptogenesis rate (0.0-0.01)"
    };
    
    schema["pruning_rate"] = {
        true,
        [](const std::string& value) {
            try {
                float val = std::stof(value);
                return val >= 0.0f && val <= 0.001f;
            } catch (...) {
                return false;
            }
        },
        "Pruning rate (0.0-0.001)"
    };
    
    return schema;
}

// Validate a single configuration value against schema
bool Config::validateKey(const std::string& key, const std::string& value) const {
    auto schema = getSchema();
    auto it = schema.find(key);
    if (it == schema.end()) {
        NLM_LOG_WARNING("Unknown configuration key: " + key);
        return true; // Unknown keys are allowed for forward compatibility
    }
    
    return it->second.validate(value);
}

// Get current time string helper
std::string Config::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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
