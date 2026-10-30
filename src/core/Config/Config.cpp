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
    // First try to parse as JSON if the file has .json extension
    // Otherwise fall back to simple key=value format for backward compatibility
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath);
    } else {
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open configuration file: " + filepath);
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
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
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                throw std::runtime_error("Error in configuration file " + filepath + 
                                        " at line " + std::to_string(lineNum) +
                                        ": " + std::string(e.what()));
            }
        } else {
            throw std::runtime_error("Invalid configuration format in file " + filepath +
                                    " at line " + std::to_string(lineNum) +
                                    ": expected 'key=value' format");
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open JSON configuration file: " + filepath);
    }
    
    // Simple JSON parser - can be extended to use full JSON library
    // Parse key-value pairs: {"key": "value", "number": 42, "array": [1,2,3]}
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    
    // Remove whitespace
    std::string cleaned;
    for (char c : content) {
        if (!std::isspace(c)) {
            cleaned += c;
        }
    }
    
    if (cleaned.empty() || cleaned[0] != '{' || cleaned.back() != '}') {
        throw std::runtime_error("Invalid JSON format in file " + filepath + ": must start with { and end with }");
    }
    
    // Simple parsing - can be enhanced with proper JSON library
    // For now, handle basic key-value pairs and arrays
    size_t pos = 1;  // Skip opening brace
    int keyCount = 0;
    while (pos < cleaned.size() - 1) {
        // Find next key
        size_t keyStart = cleaned.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = cleaned.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = cleaned.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Validate key format
        if (key.empty() || key.find(' ') != std::string::npos) {
            throw std::runtime_error("Invalid key name in JSON file: '" + key + "'");
        }
        
        // Find colon after key
        size_t colonPos = cleaned.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = cleaned.find_first_not_of(": \t", colonPos);
        if (valueStart == std::string::npos) break;
        
        // Find value end (comma or closing brace)
        size_t valueEnd = cleaned.find(',', valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = cleaned.find('}', valueStart);
        }
        if (valueEnd == std::string::npos) break;
        
        std::string valueStr = cleaned.substr(valueStart, valueEnd - valueStart);
        
        // Parse value based on content
        try {
            ConfigValue parsedValue = parseJSONValue(valueStr);
            set(key, parsedValue, ConfigSource::File);
            ++keyCount;
        } catch (const std::exception& e) {
            throw std::runtime_error("Error parsing JSON value for key '" + key + "' in file " + filepath + ": " + std::string(e.what()));
        }
        
        // Move to next position
        pos = valueEnd + 1;
        // Skip comma and whitespace
        while (pos < cleaned.size() && (cleaned[pos] == ',' || std::isspace(cleaned[pos]))) {
            pos++;
        }
    }
    
    if (keyCount == 0) {
        throw std::runtime_error("No valid key-value pairs found in JSON file " + filepath);
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
        // Handle --help and --version flags
        else if (arg == "--help" || arg == "-h") {
            printHelp();
        } else if (arg == "--version") {
            printVersion();
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
        
        // Format based on value type for round-trip compatibility
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << val << "\"\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, double> ||
                                std::is_same_v<T, bool>) {
                file << entry.key << " = " << val << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << val[i] << "\"";
                }
                file << "]\n";
            }
        }, entry.value);
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
    // Validate configuration key
    if (key.empty()) {
        throw std::runtime_error("Configuration key cannot be empty");
    }
    
    if (key.find(' ') != std::string::npos) {
        throw std::runtime_error("Configuration key cannot contain spaces: '" + key + "'");
    }
    
    if (key.find('=') != std::string::npos) {
        throw std::runtime_error("Configuration key cannot contain '=' character: '" + key + "'");
    }
    
    // Validate configuration value based on key type
    validateConfigValue(key, value);
    
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
void Config::printHelp() const {
    std::cout << "NLM Configuration Help" << std::endl;
    std::cout << "=====================" << std::endl << std::endl;
    
    std::cout << "Basic Configuration Options:" << std::endl;
    std::cout << "  --neuron_count=N        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --region_count=N        Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection_probability=X Probability of connections (default: 0.15)" << std::endl;
    std::cout << "  --random_seed=N         Random seed for reproducibility" << std::endl;
    std::cout << "  --simulation_timestep=X  Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neural Dynamics Parameters:" << std::endl;
    std::cout << "  --stdp_ltp_weight=X      STDP LTP weight (default: 0.02)" << std::endl;
    std::cout << "  --stdp_ltd_weight=X      STDP LTD weight (default: 0.015)" << std::endl;
    std::cout << "  --stdp_tau=X             STDP time constant (default: 20)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Structural Plasticity:" << std::endl;
    std::cout << "  --synaptogenesis_rate=X  Rate of new synapse formation (default: 0.0001)" << std::endl;
    std::cout << "  --pruning_rate=X         Rate of synapse elimination (default: 0.00001)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Memory System Parameters:" << std::endl;
    std::cout << "  --working_memory_capacity=N Capacity of working memory" << std::endl;
    std::cout << "  --episodic_memory_capacity=N Capacity of episodic memory" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuromodulation Parameters:" << std::endl;
    std::cout << "  --dopamine_decay=X       Dopamine decay rate (default: 0.95)" << std::endl;
    std::cout << "  --curiosity_threshold=X  Curiosity activation threshold (default: 0.3)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Development Parameters:" << std::endl;
    std::cout << "  --plasticity_decay_rate=X Plasticity decay rate (default: 0.95)" << std::endl;
    std::cout << "  --development_stage_duration=X Duration of each development stage (default: 1000)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "File Loading:" << std::endl;
    std::cout << "  --config=FILEPATH        Load configuration from file (supports .cfg and .json)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage Examples:" << std::endl;
    std::cout << "  ./nlm --neuron_count=1000 --region_count=3 --random_seed=123" << std::endl;
    std::cout << "  ./nlm --config=experiment.json --stdp_ltp_weight=0.03" << std::endl;
    std::cout << "  ./nlm -h                             Display this help" << std::endl;
    std::cout << "  ./nlm --version                      Show version information" << std::endl;
    std::cout << std::endl;
}

void Config::validateConfigValue(const std::string& key, const ConfigValue& value) {
    // Validate configuration values based on key
    if (key == "random_seed") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                if (val < 0) {
                    throw std::runtime_error("random_seed must be a positive integer");
                }
            } else if constexpr (std::is_same_v<T, int>) {
                if (val < 0) {
                    throw std::runtime_error("random_seed must be a positive integer");
                }
            } else {
                throw std::runtime_error("random_seed must be an integer type");
            }
        }, value);
    } else if (key == "simulation_timestep") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                if (val <= 0.0 || val > 1.0) {
                    throw std::runtime_error("simulation_timestep must be between 0.0 and 1.0 seconds");
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (val <= 0.0f || val > 1.0f) {
                    throw std::runtime_error("simulation_timestep must be between 0.0 and 1.0 seconds");
                }
            } else {
                throw std::runtime_error("simulation_timestep must be a numeric type");
            }
        }, value);
    } else if (key == "neuron_count") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                if (val < 1) {
                    throw std::runtime_error("neuron_count must be at least 1");
                }
            } else if constexpr (std::is_same_v<T, int>) {
                if (val < 1) {
                    throw std::runtime_error("neuron_count must be at least 1");
                }
            } else {
                throw std::runtime_error("neuron_count must be an integer type");
            }
        }, value);
    } else if (key == "region_count") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                if (val < 1) {
                    throw std::runtime_error("region_count must be at least 1");
                }
            } else if constexpr (std::is_same_v<T, int>) {
                if (val < 1) {
                    throw std::runtime_error("region_count must be at least 1");
                }
            } else {
                throw std::runtime_error("region_count must be an integer type");
            }
        }, value);
    } else if (key == "connection_probability") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                if (val < 0.0 || val > 1.0) {
                    throw std::runtime_error("connection_probability must be between 0.0 and 1.0");
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (val < 0.0f || val > 1.0f) {
                    throw std::runtime_error("connection_probability must be between 0.0 and 1.0");
                }
            } else {
                throw std::runtime_error("connection_probability must be a numeric type");
            }
        }, value);
    } else if (key == "stdp_ltp_weight") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                if (val < 0.0 || val > 1.0) {
                    throw std::runtime_error("stdp_ltp_weight must be between 0.0 and 1.0");
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (val < 0.0f || val > 1.0f) {
                    throw std::runtime_error("stdp_ltp_weight must be between 0.0 and 1.0");
                }
            } else {
                throw std::runtime_error("stdp_ltp_weight must be a numeric type");
            }
        }, value);
    } else if (key == "stdp_ltd_weight") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                if (val < 0.0 || val > 1.0) {
                    throw std::runtime_error("stdp_ltd_weight must be between 0.0 and 1.0");
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (val < 0.0f || val > 1.0f) {
                    throw std::runtime_error("stdp_ltd_weight must be between 0.0 and 1.0");
                }
            } else {
                throw std::runtime_error("stdp_ltd_weight must be a numeric type");
            }
        }, value);
    } else if (key == "stdp_tau") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                if (val < 1.0 || val > 1000.0) {
                    throw std::runtime_error("stdp_tau must be between 1.0 and 1000.0");
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (val < 1.0f || val > 1000.0f) {
                    throw std::runtime_error("stdp_tau must be between 1.0 and 1000.0");
                }
            } else {
                throw std::runtime_error("stdp_tau must be a numeric type");
            }
        }, value);
    } else if (key == "synaptogenesis_rate") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                if (val < 0.0 || val > 1.0) {
                    throw std::runtime_error("synaptogenesis_rate must be between 0.0 and 1.0");
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (val < 0.0f || val > 1.0f) {
                    throw std::runtime_error("synaptogenesis_rate must be between 0.0 and 1.0");
                }
            } else {
                throw std::runtime_error("synaptogenesis_rate must be a numeric type");
            }
        }, value);
    } else if (key == "pruning_rate") {
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double>) {
                if (val < 0.0 || val > 1.0) {
                    throw std::runtime_error("pruning_rate must be between 0.0 and 1.0");
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (val < 0.0f || val > 1.0f) {
                    throw std::runtime_error("pruning_rate must be between 0.0 and 1.0");
                }
            } else {
                throw std::runtime_error("pruning_rate must be a numeric type");
            }
        }, value);
    }
}
