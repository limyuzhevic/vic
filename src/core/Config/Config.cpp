#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// ConfigValue to JSON conversion
nlohmann::json configValueToJson(const ConfigValue& value) {
    try {
        return std::visit([](auto&& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return arg;
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return nlohmann::json(arg);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return nlohmann::json(arg);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return nlohmann::json(arg);
            } else {
                throw std::runtime_error("Unsupported type for JSON conversion");
            }
        }, value);
    } catch (const std::exception& e) {
        throw;
    }
}

// JSON to ConfigValue conversion
ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    try {
        if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_number_integer()) {
            return j.get<int>();
        } else if (j.is_number()) {
            if (j.is_number_integer()) {
                return j.get<int64_t>();
            } else {
                return j.get<double>();
            }
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_array()) {
            if (j.empty()) {
                return std::vector<int>();
            }
            
            const auto& first = j[0];
            if (first.is_string()) {
                return j.get<std::vector<std::string>>();
            } else if (first.is_number_integer()) {
                return j.get<std::vector<int>>();
            } else if (first.is_number()) {
                return j.get<std::vector<double>>();
            } else {
                throw std::runtime_error("Unsupported array element type in JSON");
            }
        } else {
            throw std::runtime_error("Unsupported JSON value type");
        }
    } catch (const std::exception& e) {
        throw;
    }
}

// ConfigValue to YAML conversion
YAML::Node configValueToYaml(const ConfigValue& value) {
    try {
        YAML::Node node;
        std::visit([&node](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                node = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                for (const auto& item : arg) {
                    node.push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                for (const auto& item : arg) {
                    node.push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                for (const auto& item : arg) {
                    node.push_back(item);
                }
            } else {
                throw std::runtime_error("Unsupported type for YAML conversion");
            }
        }, value);
        return node;
    } catch (const std::exception& e) {
        throw;
    }
}

// YAML to ConfigValue conversion
ConfigValue yamlToConfigValue(const YAML::Node& node) {
    try {
        if (node.IsNull()) {
            return std::vector<int>();
        }
        
        if (node.IsScalar()) {
            const std::string value = node.Scalar();
            
            if (value == "true" || value == "false") {
                return value == "true";
            } 
            else if (value.empty()) {
                return std::string();
            }
            else if (std::all_of(value.begin(), value.end(), [](char c) { return std::isdigit(c) || c == '-'; }) || 
                    (value[0] == '-' && value.size() > 1 && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                try {
                    return std::stoll(value);
                } catch (...) {
                    try {
                        return std::stod(value);
                    } catch (...) {
                        return value;
                    }
                }
            }
            else {
                try {
                    size_t pos;
                    double d = std::stod(value, &pos);
                    if (pos == value.size()) {
                        return d;
                    }
                } catch (...) {
                }
                return value;
            }
        } 
        else if (node.IsSequence()) {
            if (node.empty()) {
                return std::vector<int>();
            }
            
            std::vector<std::string> stringResult;
            std::vector<int> intResult;
            std::vector<double> doubleResult;
            
            bool hasStrings = true;
            bool hasInts = true;
            bool hasDoubles = true;
            
            for (const auto& item : node) {
                if (item.IsScalar()) {
                    const std::string val = item.Scalar();
                    stringResult.push_back(val);
                    
                    if (hasInts) {
                        try {
                            std::stoll(val);
                        } catch (...) {
                            hasInts = false;
                        }
                    }
                    
                    if (hasDoubles) {
                        try {
                            std::stod(val);
                        } catch (...) {
                            hasDoubles = false;
                        }
                    }
                } else {
                    hasStrings = false;
                    hasInts = false;
                    hasDoubles = false;
                    break;
                }
            }
            
            if (hasStrings && !stringResult.empty()) {
                return stringResult;
            } else if (hasInts && !intResult.empty()) {
                return intResult;
            } else if (hasDoubles && !doubleResult.empty()) {
                return doubleResult;
            } else if (!stringResult.empty()) {
                return stringResult;
            } else {
                throw std::runtime_error("Failed to parse YAML sequence elements");
            }
        } 
        else if (node.IsMap()) {
            throw std::runtime_error("YAML maps are not supported in ConfigValue");
        }
        
        throw std::runtime_error("Unsupported YAML node type");
    } catch (const std::exception& e) {
        throw;
    }
}

// File format detection
std::string detectFileFormat(const std::string& filepath) {
    if (filepath.empty()) {
        return "unknown";
    }
    
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".js") {
        return "json";
    } else if (ext == ".yaml" || ext == ".yml") {
        return "yaml";
    } else if (ext == ".cfg" || ext == ".config") {
        return "simple";
    } else {
        // Try to detect from content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return "unknown";
        }
        
        std::string line;
        std::getline(file, line);
        file.close();
        
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        if (line.empty()) {
            return "unknown";
        }
        
        if ((line[0] == '{' && line.back() == '}') || 
            (line[0] == '[' && line.back() == ']')) {
            return "json";
        }
        
        if (line.find(":") != std::string::npos && 
            (line.find("key1:") != std::string::npos || line.find("#") != std::string::npos)) {
            return "yaml";
        }
        
        return "simple";
    }
}

bool isJsonFile(const std::string& filepath) {
    std::string format = detectFileFormat(filepath);
    return format == "json";
}

bool isYamlFile(const std::string& filepath) {
    std::string format = detectFileFormat(filepath);
    return format == "yaml";
}


Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
    std::string format = detectFileFormat(filepath);
    
    try {
        if (format == "json") {
            // Load JSON file
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json j;
            file >> j;
            
            // Clear existing entries
            clear();
            
            // Parse JSON object
            if (j.is_object()) {
                for (auto& [key, value] : j.items()) {
                    try {
                        ConfigValue configValue = jsonToConfigValue(value);
                        set(key, configValue, ConfigSource::File);
                    } catch (const std::exception& e) {
                        // Skip invalid entries
                        continue;
                    }
                }
            }
            
            return true;
        } else if (format == "yaml") {
            // Load YAML file
            YAML::Node doc;
            try {
                doc = YAML::LoadFile(filepath);
            } catch (const YAML::ParserException& e) {
                return false;
            }
            
            // Clear existing entries
            clear();
            
            // Parse YAML document
            if (doc.IsMap()) {
                for (const auto& pair : doc) {
                    const std::string& key = pair.first.as<std::string>();
                    try {
                        ConfigValue configValue = yamlToConfigValue(pair.second);
                        set(key, configValue, ConfigSource::File);
                    } catch (const std::exception& e) {
                        // Skip invalid entries
                        continue;
                    }
                }
            }
            
            return true;
        } else {
            // Load simple key=value format (original implementation)
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
                    
                    // Try to parse as int
                    try {
                        size_t posEnd;
                        int intVal = std::stoi(value, &posEnd);
                        if (posEnd == value.size()) {
                            set(key, intVal, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Try to parse as double
                    try {
                        size_t posEnd;
                        double doubleVal = std::stod(value, &posEnd);
                        if (posEnd == value.size()) {
                            set(key, doubleVal, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Try to parse as boolean
                    if (value == "true") {
                        set(key, true, ConfigSource::File);
                        continue;
                    } else if (value == "false") {
                        set(key, false, ConfigSource::File);
                        continue;
                    }
                    
                    // Default to string
                    set(key, value, ConfigSource::File);
                }
            }
            
            return true;
        }
    } catch (const std::exception& e) {
        return false;
    }
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
    if (filepath.empty() || pImpl->entries.empty()) {
        return false;
    }
    
    std::string format = detectFileFormat(filepath);
    
    try {
        if (format == "json") {
            // Save as JSON file
            nlohmann::json j = nlohmann::json::object();
            
            for (const auto& entry : pImpl->entries) {
                j[entry.key] = configValueToJson(entry.value);
            }
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << std::setw(4) << j << std::endl;
            return true;
        } else if (format == "yaml") {
            // Save as YAML file
            YAML::Emitter emitter;
            emitter << YAML::BeginMap;
            
            for (const auto& entry : pImpl->entries) {
                emitter << YAML::Key << entry.key;
                emitter << YAML::Value << configValueToYaml(entry.value);
            }
            
            emitter << YAML::EndMap;
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            emitter.Write(file);
            return true;
        } else {
            // Save in simple key=value format (original implementation)
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                
                // Get string representation of the value
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << " = \"" << arg << "\"\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                        file << entry.key << " = " << arg << "\n";
                    } else if constexpr (std::is_same_v<T, double>) {
                        file << entry.key << " = " << arg << "\n";
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << arg[i];
                        }
                        file << "]\n";
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << arg[i];
                        }
                        file << "]\n";
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << "\"" << arg[i] << "\"";
                        }
                        file << "]\n";
                    } else {
                        file << entry.key << " = \"unknown_type\"\n";
                    }
                }, entry.value);
            }
            
            return true;
        }
    } catch (const std::exception& e) {
        return false;
    }
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

// Configuration validation implementation

#include "Config.hpp"

namespace nlm {
    // Define default configuration parameters
    std::vector<ConfigParameter> defaultConfigParameters() {
        return {
            // Core simulation parameters
            ConfigParameter("neuron_count", ConfigValue(1000), "Total number of neurons in the network", 1, std::numeric_limits<int64_t>::max()),
            ConfigParameter("region_count", ConfigValue(1), "Number of brain regions/regions", 1, std::numeric_limits<int64_t>::max()),
            ConfigParameter("connection_probability", ConfigValue(0.1), "Probability of connection between neurons", "[0.0, 1.0]"),
            ConfigParameter("random_seed", ConfigValue(42), "Random seed for reproducibility", 0, std::numeric_limits<int64_t>::max()),
            ConfigParameter("simulation_timestep", ConfigValue(0.001), "Simulation timestep in seconds", 0.0, std::numeric_limits<double>::max()),
            
            // Synaptic plasticity parameters
            ConfigParameter("stdp_ltp_weight", ConfigValue(0.01), "STDP LTP weight"),
            ConfigParameter("stdp_ltd_weight", ConfigValue(0.012), "STDP LTD weight"),
            ConfigParameter("stdp_tau", ConfigValue(20.0), "STDP time constant", 0.0, std::numeric_limits<double>::max()),
            
            // Development parameters
            ConfigParameter("synaptogenesis_rate", ConfigValue(0.0001), "Synaptogenesis rate", 0.0, std::numeric_limits<double>::max()),
            ConfigParameter("pruning_rate", ConfigValue(0.00001), "Pruning rate", 0.0, std::numeric_limits<double>::max()),
            
            // Learning parameters
            ConfigParameter("replay_interval", ConfigValue(100), "Replay interval in steps", 0, std::numeric_limits<int64_t>::max()),
            ConfigParameter("consolidation_interval", ConfigValue(1000), "Consolidation interval in steps", 0, std::numeric_limits<int64_t>::max()),
        };
    }
    
    // Initialize static members for Config::Impl
    struct Config::Impl {
        std::vector<ConfigEntry> entries;
        std::vector<ConfigParameter> registeredParameters;
        std::vector<ValidationError> validationErrors;
        
        Impl() {
            registeredParameters = defaultConfigParameters();
        }
    };
    
    Config::Config() : pImpl(std::make_unique<Impl>()) {}
    
    Config::~Config() = default;
    
    Config::Config(Config&&) noexcept = default;
    
    Config& Config::operator=(Config&&) noexcept = default;
    
    bool Config::loadFromFile(const std::string& filepath) {
        if (filepath.empty()) {
            return false;
        }
        
        std::string format = detectFileFormat(filepath);
        
        try {
            if (format == "json") {
                // Load JSON file
                std::ifstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                nlohmann::json j;
                file >> j;
                
                // Clear existing entries
                clear();
                
                // Parse JSON object
                if (j.is_object()) {
                    for (auto& [key, value] : j.items()) {
                        try {
                            ConfigValue configValue = jsonToConfigValue(value);
                            set(key, configValue, ConfigSource::File);
                        } catch (const std::exception& e) {
                            // Skip invalid entries
                            continue;
                        }
                    }
                }
                
                return true;
            } else if (format == "yaml") {
                // Load YAML file
                YAML::Node doc;
                try {
                    doc = YAML::LoadFile(filepath);
                } catch (const YAML::ParserException& e) {
                    return false;
                }
                
                // Clear existing entries
                clear();
                
                // Parse YAML document
                if (doc.IsMap()) {
                    for (const auto& pair : doc) {
                        const std::string& key = pair.first.as<std::string>();
                        try {
                            ConfigValue configValue = yamlToConfigValue(pair.second);
                            set(key, configValue, ConfigSource::File);
                        } catch (const std::exception& e) {
                            // Skip invalid entries
                            continue;
                        }
                    }
                }
                
                return true;
            } else {
                // Load simple key=value format (original implementation)
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
                        
                        // Try to parse as int
                        try {
                            size_t posEnd;
                            int intVal = std::stoi(value, &posEnd);
                            if (posEnd == value.size()) {
                                set(key, intVal, ConfigSource::File);
                                continue;
                            }
                        } catch (...) {}
                        
                        // Try to parse as double
                        try {
                            size_t posEnd;
                            double doubleVal = std::stod(value, &posEnd);
                            if (posEnd == value.size()) {
                                set(key, doubleVal, ConfigSource::File);
                                continue;
                            }
                        } catch (...) {}
                        
                        // Try to parse as boolean
                        if (value == "true") {
                            set(key, true, ConfigSource::File);
                            continue;
                        } else if (value == "false") {
                            set(key, false, ConfigSource::File);
                            continue;
                        }
                        
                        // Default to string
                        set(key, value, ConfigSource::File);
                    }
                }
                
                return true;
            }
        } catch (const std::exception& e) {
            return false;
        }
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
        if (filepath.empty() || pImpl->entries.empty()) {
            return false;
        }
        
        std::string format = detectFileFormat(filepath);
        
        try {
            if (format == "json") {
                // Save as JSON file
                nlohmann::json j = nlohmann::json::object();
                
                for (const auto& entry : pImpl->entries) {
                    j[entry.key] = configValueToJson(entry.value);
                }
                
                std::ofstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                file << std::setw(4) << j << std::endl;
                return true;
            } else if (format == "yaml") {
                // Save as YAML file
                YAML::Emitter emitter;
                emitter << YAML::BeginMap;
                
                for (const auto& entry : pImpl->entries) {
                    emitter << YAML::Key << entry.key;
                    emitter << YAML::Value << configValueToYaml(entry.value);
                }
                
                emitter << YAML::EndMap;
                
                std::ofstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                emitter.Write(file);
                return true;
            } else {
                // Save in simple key=value format (original implementation)
                std::ofstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                for (const auto& entry : pImpl->entries) {
                    file << "# " << entry.description << "\n";
                    
                    // Get string representation of the value
                    std::visit([&file](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            file << entry.key << " = \"" << arg << "\"\n";
                        } else if constexpr (std::is_same_v<T, bool>) {
                            file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                            file << entry.key << " = " << arg << "\n";
                        } else if constexpr (std::is_same_v<T, double>) {
                            file << entry.key << " = " << arg << "\n";
                        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                            file << entry.key << " = [";
                            for (size_t i = 0; i < arg.size(); ++i) {
                                if (i > 0) file << ", ";
                                file << arg[i];
                            }
                            file << "]\n";
                        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                            file << entry.key << " = [";
                            for (size_t i = 0; i < arg.size(); ++i) {
                                if (i > 0) file << ", ";
                                file << arg[i];
                            }
                            file << "]\n";
                        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            file << entry.key << " = [";
                            for (size_t i = 0; i < arg.size(); ++i) {
                                if (i > 0) file << ", ";
                                file << "\"" << arg[i] << "\"";
                            }
                            file << "]\n";
                        } else {
                            file << entry.key << " = \"unknown_type\"\n";
                        }
                    }, entry.value);
                }
                
                return true;
            }
        } catch (const std::exception& e) {
            return false;
        }
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
        // Validate the value against its schema
        std::vector<ValidationError> errors = validateParameter(key);
        
        // Check if key exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            pImpl->validationErrors = errors;
        } else {
            // Check if key is registered
            bool isRegistered = false;
            for (const auto& param : pImpl->registeredParameters) {
                if (param.key == key) {
                    isRegistered = true;
                    break;
                }
            }
            
            if (!isRegistered) {
                // Add to entries even if not registered (for backward compatibility)
                pImpl->entries.emplace_back(key, value, source);
                pImpl->validationErrors = errors;
            } else {
                // Register new parameter
                pImpl->entries.emplace_back(key, value, source);
                pImpl->validationErrors = errors;
            }
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
        pImpl->validationErrors.clear();
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
    
    std::vector<ValidationError> Config::validateConfig() const {
        std::vector<ValidationError> allErrors;
        
        for (const auto& entry : pImpl->entries) {
            std::vector<ValidationError> errors = validateParameter(entry.key);
            allErrors.insert(allErrors.end(), errors.begin(), errors.end());
        }
        
        return allErrors;
    }
    
    std::vector<ValidationError> Config::validateParameter(const std::string& key) const {
        std::vector<ValidationError> errors;
        
        // Find the entry
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return errors; // Entry not found, return empty
        }
        
        // Find the parameter definition
        auto paramIt = std::find_if(pImpl->registeredParameters.begin(), 
                                   pImpl->registeredParameters.end(),
                                   [&key](const ConfigParameter& p) { return p.key == key; });
        
        if (paramIt == pImpl->registeredParameters.end()) {
            return errors; // Parameter not registered, return empty
        }
        
        const ConfigParameter& param = *paramIt;
        
        // Validate based on type
        bool isValid = false;
        std::string errorMsg;
        
        if (std::holds_alternative<int>(it->value) || std::holds_alternative<int64_t>(it->value)) {
            isValid = validateInt(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be an integer";
            }
        } else if (std::holds_alternative<double>(it->value)) {
            isValid = validateDouble(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be a double";
            }
        } else if (std::holds_alternative<bool>(it->value)) {
            isValid = validateBool(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be a boolean";
            }
        } else if (std::holds_alternative<std::string>(it->value)) {
            isValid = validateString(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be a string";
            }
        } else if (std::holds_alternative<std::vector<int>>(it->value) ||
                   std::holds_alternative<std::vector<double>>(it->value) ||
                   std::holds_alternative<std::vector<std::string>>(it->value)) {
            isValid = validateArray(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be an array";
            }
        }
        
        if (!isValid) {
            // Try to get more specific error message
            if (param.positive) {
                errorMsg = validatePositive(it->value);
            } else if (param.nonNegative) {
                errorMsg = validateNonNegative(it->value);
            } else if (param.probabilityRange) {
                errorMsg = validateProbabilityRange(it->value);
            } else if (param.minValue.has_value() && param.maxValue.has_value()) {
                errorMsg = validateRange(it->value, param.minValue.value(), param.maxValue.value());
            } else if (param.minIntValue.has_value() && param.maxIntValue.has_value()) {
                errorMsg = validateIntRange(it->value, param.minIntValue.value(), param.maxIntValue.value());
            } else if (param.minLength.has_value() && param.maxLength.has_value()) {
                errorMsg = validateLength(it->value, param.minLength.value(), param.maxLength.value());
            } else if (!param.pattern.empty()) {
                errorMsg = validatePattern(it->value, param.pattern);
            }
            
            errors.emplace_back(key, "Parameter '" + key + "' is invalid: " + errorMsg);
        }
        
        return errors;
    }
    
    std::string Config::getValidationError(const std::string& key) const {
        std::vector<ValidationError> errors = validateParameter(key);
        if (!errors.empty()) {
            return errors[0].message;
        }
        return "";
    }
    
    void Config::registerParameter(const ConfigParameter& param) {
        // Check if parameter already exists
        auto it = std::find_if(pImpl->registeredParameters.begin(), 
                              pImpl->registeredParameters.end(),
                              [&param](const ConfigParameter& p) { return p.key == param.key; });
        
        if (it == pImpl->registeredParameters.end()) {
            pImpl->registeredParameters.push_back(param);
        }
    }
    
    std::vector<ConfigParameter> Config::getRegisteredParameters() const {
        return pImpl->registeredParameters;
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

// Include validation forward declarations at end of file
#include "Config.hpp"

namespace nlm {
    // Define default configuration parameters
    std::vector<ConfigParameter> defaultConfigParameters() {
        return {
            // Core simulation parameters
            ConfigParameter("neuron_count", ConfigValue(1000), "Total number of neurons in the network", 1, std::numeric_limits<int64_t>::max()),
            ConfigParameter("region_count", ConfigValue(1), "Number of brain regions/regions", 1, std::numeric_limits<int64_t>::max()),
            ConfigParameter("connection_probability", ConfigValue(0.1), "Probability of connection between neurons", "[0.0, 1.0]"),
            ConfigParameter("random_seed", ConfigValue(42), "Random seed for reproducibility", 0, std::numeric_limits<int64_t>::max()),
            ConfigParameter("simulation_timestep", ConfigValue(0.001), "Simulation timestep in seconds", 0.0, std::numeric_limits<double>::max()),
            
            // Synaptic plasticity parameters
            ConfigParameter("stdp_ltp_weight", ConfigValue(0.01), "STDP LTP weight"),
            ConfigParameter("stdp_ltd_weight", ConfigValue(0.012), "STDP LTD weight"),
            ConfigParameter("stdp_tau", ConfigValue(20.0), "STDP time constant", 0.0, std::numeric_limits<double>::max()),
            
            // Development parameters
            ConfigParameter("synaptogenesis_rate", ConfigValue(0.0001), "Synaptogenesis rate", 0.0, std::numeric_limits<double>::max()),
            ConfigParameter("pruning_rate", ConfigValue(0.00001), "Pruning rate", 0.0, std::numeric_limits<double>::max()),
            
            // Learning parameters
            ConfigParameter("replay_interval", ConfigValue(100), "Replay interval in steps", 0, std::numeric_limits<int64_t>::max()),
            ConfigParameter("consolidation_interval", ConfigValue(1000), "Consolidation interval in steps", 0, std::numeric_limits<int64_t>::max()),
        };
    }
    
    // Initialize static members for Config::Impl
    struct Config::Impl {
        std::vector<ConfigEntry> entries;
        std::vector<ConfigParameter> registeredParameters;
        std::vector<ValidationError> validationErrors;
        
        Impl() {
            registeredParameters = defaultConfigParameters();
        }
    };
    
    Config::Config() : pImpl(std::make_unique<Impl>()) {}
    
    Config::~Config() = default;
    
    Config::Config(Config&&) noexcept = default;
    
    Config& Config::operator=(Config&&) noexcept = default;
    
    bool Config::loadFromFile(const std::string& filepath) {
        if (filepath.empty()) {
            return false;
        }
        
        std::string format = detectFileFormat(filepath);
        
        try {
            if (format == "json") {
                // Load JSON file
                std::ifstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                nlohmann::json j;
                file >> j;
                
                // Clear existing entries
                clear();
                
                // Parse JSON object
                if (j.is_object()) {
                    for (auto& [key, value] : j.items()) {
                        try {
                            ConfigValue configValue = jsonToConfigValue(value);
                            set(key, configValue, ConfigSource::File);
                        } catch (const std::exception& e) {
                            // Skip invalid entries
                            continue;
                        }
                    }
                }
                
                return true;
            } else if (format == "yaml") {
                // Load YAML file
                YAML::Node doc;
                try {
                    doc = YAML::LoadFile(filepath);
                } catch (const YAML::ParserException& e) {
                    return false;
                }
                
                // Clear existing entries
                clear();
                
                // Parse YAML document
                if (doc.IsMap()) {
                    for (const auto& pair : doc) {
                        const std::string& key = pair.first.as<std::string>();
                        try {
                            ConfigValue configValue = yamlToConfigValue(pair.second);
                            set(key, configValue, ConfigSource::File);
                        } catch (const std::exception& e) {
                            // Skip invalid entries
                            continue;
                        }
                    }
                }
                
                return true;
            } else {
                // Load simple key=value format (original implementation)
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
                        
                        // Try to parse as int
                        try {
                            size_t posEnd;
                            int intVal = std::stoi(value, &posEnd);
                            if (posEnd == value.size()) {
                                set(key, intVal, ConfigSource::File);
                                continue;
                            }
                        } catch (...) {}
                        
                        // Try to parse as double
                        try {
                            size_t posEnd;
                            double doubleVal = std::stod(value, &posEnd);
                            if (posEnd == value.size()) {
                                set(key, doubleVal, ConfigSource::File);
                                continue;
                            }
                        } catch (...) {}
                        
                        // Try to parse as boolean
                        if (value == "true") {
                            set(key, true, ConfigSource::File);
                            continue;
                        } else if (value == "false") {
                            set(key, false, ConfigSource::File);
                            continue;
                        }
                        
                        // Default to string
                        set(key, value, ConfigSource::File);
                    }
                }
                
                return true;
            }
        } catch (const std::exception& e) {
            return false;
        }
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
        if (filepath.empty() || pImpl->entries.empty()) {
            return false;
        }
        
        std::string format = detectFileFormat(filepath);
        
        try {
            if (format == "json") {
                // Save as JSON file
                nlohmann::json j = nlohmann::json::object();
                
                for (const auto& entry : pImpl->entries) {
                    j[entry.key] = configValueToJson(entry.value);
                }
                
                std::ofstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                file << std::setw(4) << j << std::endl;
                return true;
            } else if (format == "yaml") {
                // Save as YAML file
                YAML::Emitter emitter;
                emitter << YAML::BeginMap;
                
                for (const auto& entry : pImpl->entries) {
                    emitter << YAML::Key << entry.key;
                    emitter << YAML::Value << configValueToYaml(entry.value);
                }
                
                emitter << YAML::EndMap;
                
                std::ofstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                emitter.Write(file);
                return true;
            } else {
                // Save in simple key=value format (original implementation)
                std::ofstream file(filepath);
                if (!file.is_open()) {
                    return false;
                }
                
                for (const auto& entry : pImpl->entries) {
                    file << "# " << entry.description << "\n";
                    
                    // Get string representation of the value
                    std::visit([&file](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            file << entry.key << " = \"" << arg << "\"\n";
                        } else if constexpr (std::is_same_v<T, bool>) {
                            file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                            file << entry.key << " = " << arg << "\n";
                        } else if constexpr (std::is_same_v<T, double>) {
                            file << entry.key << " = " << arg << "\n";
                        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                            file << entry.key << " = [";
                            for (size_t i = 0; i < arg.size(); ++i) {
                                if (i > 0) file << ", ";
                                file << arg[i];
                            }
                            file << "]\n";
                        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                            file << entry.key << " = [";
                            for (size_t i = 0; i < arg.size(); ++i) {
                                if (i > 0) file << ", ";
                                file << arg[i];
                            }
                            file << "]\n";
                        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            file << entry.key << " = [";
                            for (size_t i = 0; i < arg.size(); ++i) {
                                if (i > 0) file << ", ";
                                file << "\"" << arg[i] << "\"";
                            }
                            file << "]\n";
                        } else {
                            file << entry.key << " = \"unknown_type\"\n";
                        }
                    }, entry.value);
                }
                
                return true;
            }
        } catch (const std::exception& e) {
            return false;
        }
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
        // Validate the value against its schema
        std::vector<ValidationError> errors = validateParameter(key);
        
        // Check if key exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            pImpl->validationErrors = errors;
        } else {
            // Check if key is registered
            bool isRegistered = false;
            for (const auto& param : pImpl->registeredParameters) {
                if (param.key == key) {
                    isRegistered = true;
                    break;
                }
            }
            
            if (!isRegistered) {
                // Add to entries even if not registered (for backward compatibility)
                pImpl->entries.emplace_back(key, value, source);
                pImpl->validationErrors = errors;
            } else {
                // Register new parameter
                pImpl->entries.emplace_back(key, value, source);
                pImpl->validationErrors = errors;
            }
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
        pImpl->validationErrors.clear();
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
    
    std::vector<ValidationError> Config::validateConfig() const {
        std::vector<ValidationError> allErrors;
        
        for (const auto& entry : pImpl->entries) {
            std::vector<ValidationError> errors = validateParameter(entry.key);
            allErrors.insert(allErrors.end(), errors.begin(), errors.end());
        }
        
        return allErrors;
    }
    
    std::vector<ValidationError> Config::validateParameter(const std::string& key) const {
        std::vector<ValidationError> errors;
        
        // Find the entry
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return errors; // Entry not found, return empty
        }
        
        // Find the parameter definition
        auto paramIt = std::find_if(pImpl->registeredParameters.begin(), 
                                   pImpl->registeredParameters.end(),
                                   [&key](const ConfigParameter& p) { return p.key == key; });
        
        if (paramIt == pImpl->registeredParameters.end()) {
            return errors; // Parameter not registered, return empty
        }
        
        const ConfigParameter& param = *paramIt;
        
        // Validate based on type
        bool isValid = false;
        std::string errorMsg;
        
        if (std::holds_alternative<int>(it->value) || std::holds_alternative<int64_t>(it->value)) {
            isValid = validateInt(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be an integer";
            }
        } else if (std::holds_alternative<double>(it->value)) {
            isValid = validateDouble(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be a double";
            }
        } else if (std::holds_alternative<bool>(it->value)) {
            isValid = validateBool(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be a boolean";
            }
        } else if (std::holds_alternative<std::string>(it->value)) {
            isValid = validateString(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be a string";
            }
        } else if (std::holds_alternative<std::vector<int>>(it->value) ||
                   std::holds_alternative<std::vector<double>>(it->value) ||
                   std::holds_alternative<std::vector<std::string>>(it->value)) {
            isValid = validateArray(it->value, param);
            if (!isValid) {
                errorMsg = "Value must be an array";
            }
        }
        
        if (!isValid) {
            // Try to get more specific error message
            if (param.positive) {
                errorMsg = validatePositive(it->value);
            } else if (param.nonNegative) {
                errorMsg = validateNonNegative(it->value);
            } else if (param.probabilityRange) {
                errorMsg = validateProbabilityRange(it->value);
            } else if (param.minValue.has_value() && param.maxValue.has_value()) {
                errorMsg = validateRange(it->value, param.minValue.value(), param.maxValue.value());
            } else if (param.minIntValue.has_value() && param.maxIntValue.has_value()) {
                errorMsg = validateIntRange(it->value, param.minIntValue.value(), param.maxIntValue.value());
            } else if (param.minLength.has_value() && param.maxLength.has_value()) {
                errorMsg = validateLength(it->value, param.minLength.value(), param.maxLength.value());
            } else if (!param.pattern.empty()) {
                errorMsg = validatePattern(it->value, param.pattern);
            }
            
            errors.emplace_back(key, "Parameter '" + key + "' is invalid: " + errorMsg);
        }
        
        return errors;
    }
    
    std::string Config::getValidationError(const std::string& key) const {
        std::vector<ValidationError> errors = validateParameter(key);
        if (!errors.empty()) {
            return errors[0].message;
        }
        return "";
    }
    
    void Config::registerParameter(const ConfigParameter& param) {
        // Check if parameter already exists
        auto it = std::find_if(pImpl->registeredParameters.begin(), 
                              pImpl->registeredParameters.end(),
                              [&param](const ConfigParameter& p) { return p.key == param.key; });
        
        if (it == pImpl->registeredParameters.end()) {
            pImpl->registeredParameters.push_back(param);
        }
    }
    
    std::vector<ConfigParameter> Config::getRegisteredParameters() const {
        return pImpl->registeredParameters;
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

// Include validation forward declarations at end of file
#include "Config.hpp"
