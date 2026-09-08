#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <memory>
#include <unordered_map>
#include <any>
#include <functional>
#include <stdexcept>
#include <variant>
#include <iostream>

namespace nlm {

// YAML parser helper functions
namespace yaml_parser {
    std::string toString(const std::string& val) { return val; }
    std::string toString(int val) { return std::to_string(val); }
    std::string toString(float val) { return std::to_string(val); }
    std::string toString(double val) { return std::to_string(val); }
    std::string toString(bool val) { return val ? "true" : "false"; }
}

// JSON/YAML parser
class ConfigParser {
public:
    static bool parseYAML(const std::string& content, nlohmann::json& out) {
        // This would use a real YAML parser like yaml-cpp
        // For now, return false to indicate not implemented
        return false;
    }
    
    static bool parseJSON(const std::string& content, nlohmann::json& out) {
        try {
            out = nlohmann::json::parse(content);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
};

// Advanced command line parser
class AdvancedCLIParser {
public:
    struct Option {
        std::string longName;
        char shortName;
        std::string description;
        bool hasValue;
        std::string defaultValue;
        std::string category;
        std::string type;
        std::shared_ptr<ConfigValidator> validator;
    };
    
    AdvancedCLIParser() {
        // Register common options
        registerOption("--memory-capacity", 'm', 
                      "Maximum number of memory traces", true, "1000", 
                      "memory", "int");
        registerOption("--neuron-count", 'n',
                      "Total number of neurons to create", true, "1000",
                      "system", "int");
        registerOption("--simulation-timestep", 't',
                      "Simulation time step duration", true, "0.001",
                      "system", "double");
        registerOption("--checkpoint-dir", 'c',
                      "Directory for checkpoints", true, "./checkpoints",
                      "performance", "string");
        registerOption("--enable-reward-modulation", 'r',
                      "Enable reward-based neuromodulation", false, "true",
                      "neuromodulation", "bool");
        registerOption("--enable-structural-plasticity", 's',
                      "Enable structural plasticity", false, "true",
                      "plasticity", "bool");
        registerOption("--simulation-speed", 'v',
                      "Simulation speed multiplier", true, "1.0",
                      "performance", "float");
        registerOption("--help", 'h',
                      "Show help message", false, "", "system", "flag");
    }
    
    void registerOption(const std::string& longName, char shortName, 
                       const std::string& description, bool hasValue,
                       const std::string& defaultValue, const std::string& category,
                       const std::string& type,
                       std::shared_ptr<ConfigValidator> validator = nullptr) {
        Option opt;
        opt.longName = longName;
        opt.shortName = shortName;
        opt.description = description;
        opt.hasValue = hasValue;
        opt.defaultValue = defaultValue;
        opt.category = category;
        opt.type = type;
        opt.validator = validator;
        options_.push_back(opt);
    }
    
    bool parse(int argc, char** argv, Config& config) {
        std::vector<std::string> args(argv, argv + argc);
        
        for (size_t i = 1; i < args.size(); ++i) {
            std::string arg = args[i];
            
            // Check for --help
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            }
            
            // Check for --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                config.set(key, value);
            }
            // Check for --key value format
            else if (arg.find('=') == std::string::npos) {
                // Find matching option
                for (const auto& opt : options_) {
                    if (opt.longName == arg || 
                        (opt.shortName != '\0' && "-" + std::string(1, opt.shortName) == arg)) {
                        
                        if (opt.hasValue && i + 1 < args.size()) {
                            std::string value = args[++i];
                            
                            // Convert value based on type
                            if (opt.type == "int") {
                                try {
                                    int intVal = std::stoi(value);
                                    config.set(opt.longName.substr(2), intVal);
                                } catch (...) {
                                    // Invalid value, use default
                                    config.set(opt.longName.substr(2), 
                                              std::stoi(opt.defaultValue));
                                }
                            } else if (opt.type == "float") {
                                try {
                                    float floatVal = std::stof(value);
                                    config.set(opt.longName.substr(2), floatVal);
                                } catch (...) {
                                    config.set(opt.longName.substr(2), 
                                              std::stof(opt.defaultValue));
                                }
                            } else if (opt.type == "double") {
                                try {
                                    double doubleVal = std::stod(value);
                                    config.set(opt.longName.substr(2), doubleVal);
                                } catch (...) {
                                    config.set(opt.longName.substr(2), 
                                              std::stod(opt.defaultValue));
                                }
                            } else if (opt.type == "bool") {
                                config.set(opt.longName.substr(2), 
                                          value == "true" || value == "1" || value == "yes");
                            } else if (opt.type == "string") {
                                config.set(opt.longName.substr(2), value);
                            }
                        } else if (!opt.hasValue) {
                            config.set(opt.longName.substr(2), !opt.defaultValue.empty() && 
                                                           (opt.defaultValue == "true" || 
                                                            opt.defaultValue == "1" || 
                                                            opt.defaultValue == "yes"));
                        }
                        
                        break;
                    }
                }
            }
        }
        
        return true;
    }
    
    void printHelp() const {
        std::cout << "NLM Configuration Options:" << std::endl;
        std::cout << "=========================" << std::endl;
        
        for (const auto& opt : options_) {
            std::cout << "  ";
            if (opt.shortName != '\0') {
                std::cout << "-" << opt.shortName << ", ";
            }
            std::cout << "--" << opt.longName.substr(2) << (opt.hasValue ? "=<value>" : "") << std::endl;
            std::cout << "    " << opt.description << std::endl;
            std::cout << "    Type: " << opt.type << ", Category: " << opt.category << std::endl;
            if (opt.hasValue && !opt.defaultValue.empty()) {
                std::cout << "    Default: " << opt.defaultValue << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
private:
    std::vector<Option> options_;
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::unique_ptr<ConfigSection>> sections;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Try JSON first
    nlohmann::json jsonData;
    if (ConfigParser::parseJSON(content, jsonData)) {
        // Convert JSON to Config
        convertFromJSON(jsonData);
        return true;
    }
    
    // Try YAML second
    nlohmann::json yamlData;
    if (ConfigParser::parseYAML(content, yamlData)) {
        convertFromJSON(yamlData);
        return true;
    }
    
    // Fall back to simple key=value format
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
    AdvancedCLIParser parser;
    Config config;
    
    if (!parser.parse(argc, argv, config)) {
        return false; // Help was shown
    }
    
    // Merge parsed configuration
    for (const auto& entry : config.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
    
    for (const auto& pair : config.pImpl->sections) {
        if (!pImpl->sections.count(pair.first)) {
            pImpl->sections[pair.first] = std::make_unique<ConfigSection>(*pair.second);
        }
    }
    
    return true;
}

void Config::convertFromJSON(const nlohmann::json& json) {
    if (json.is_object()) {
        for (const auto& item : json.items()) {
            std::string key = item.key();
            
            if (item.value().is_number_integer()) {
                set(key, item.value().get<int>(), ConfigSource::File);
            } else if (item.value().is_number_float()) {
                set(key, item.value().get<float>(), ConfigSource::File);
            } else if (item.value().is_boolean()) {
                set(key, item.value().get<bool>(), ConfigSource::File);
            } else if (item.value().is_string()) {
                set(key, item.value().get<std::string>(), ConfigSource::File);
            } else if (item.value().is_array()) {
                // For arrays, store as string representation
                set(key, item.value().dump(), ConfigSource::File);
            }
        }
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save as JSON format for better compatibility
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&json, &entry](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                json[entry.key] = val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                json[entry.key] = val;
            } else if constexpr (std::is_same_v<T, double>) {
                json[entry.key] = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                json[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                json[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                json[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json[entry.key] = val;
            }
        }, entry.value);
    }
    
    file << json.dump(4); // Pretty print with 4-space indentation
    
    return true;
}

void Config::validateAll() const {
    // Validate flat entries
    for (const auto& entry : pImpl->entries) {
        if (entry.validator && entry.value.has_value()) {
            if (!entry.validator->validate(entry.value)) {
                throw std::runtime_error("Invalid configuration value for key '" + 
                    entry.key + "': " + entry.validator->getErrorMessage(entry.value));
            }
        }
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // First try sections
    for (const auto& pair : pImpl->sections) {
        if (pair.second->has(key)) {
            try {
                return pair.second->get<T>(key);
            } catch (const std::runtime_error&) {
                // Key exists but wrong type, continue searching
            }
        }
    }
    
    // Fall back to flat entries
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
    // Check sections first
    for (const auto& pair : pImpl->sections) {
        if (pair.second->has(key)) return true;
    }
    
    // Check flat entries
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    // Remove from sections
    for (auto& pair : pImpl->sections) {
        auto& entries = pair.second->getEntries();
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
            entries.end());
    }
    
    // Remove from flat entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    
    // Get keys from sections
    for (const auto& pair : pImpl->sections) {
        for (const auto& entry : pair.second->getEntries()) {
            keys.push_back(entry.key);
        }
    }
    
    // Add keys from flat entries (avoiding duplicates)
    for (const auto& entry : pImpl->entries) {
        if (!has(entry.key)) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->sections.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration with " << pImpl->sections.size() << " sections and " 
        << pImpl->entries.size() << " entries:\n";
    
    for (const auto& pair : pImpl->sections) {
        oss << pair.first << ":\n";
        for (const auto& entry : pair.second->getEntries()) {
            oss << "  " << entry.key;
            if (entry.value.has_value()) {
                oss << " = ";
                if (entry.value.type() == typeid(int)) {
                    oss << std::any_cast<int>(entry.value);
                } else if (entry.value.type() == typeid(float)) {
                    oss << std::any_cast<float>(entry.value);
                } else if (entry.value.type() == typeid(std::string)) {
                    oss << "\"" << std::any_cast<std::string>(entry.value) << "\"";
                } else if (entry.value.type() == typeid(bool)) {
                    oss << (std::any_cast<bool>(entry.value) ? "true" : "false");
                }
            } else {
                oss << " = [NOT SET]";
            }
            oss << (entry.required ? " (REQUIRED)" : "") << "\n";
        }
    }
    
    if (!pImpl->entries.empty()) {
        oss << "\nLegacy flat entries:\n";
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
