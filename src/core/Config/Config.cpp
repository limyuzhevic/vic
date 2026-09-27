#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cmath>
#include "core/Error/Error.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    ConfigFormat format = detectFormat(filepath);
    
    switch (format) {
#ifdef NLM_WITH_JSON
        case ConfigFormat::JSON:
            return loadFromJSON(filepath);
#endif
#ifdef NLM_WITH_YAML
        case ConfigFormat::YAML:
            return loadFromYAML(filepath);
#endif
        case ConfigFormat::Simple:
            return loadFromSimple(filepath);
        default:
            ErrorReporter::getGlobal().logError("Unknown configuration format for file: " + filepath);
            return false;
    }
}

#ifdef NLM_WITH_JSON
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ErrorReporter::getGlobal().logError("Failed to open JSON file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        clear();
        
        for (const auto& [key, value] : jsonData.items()) {
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Check array type
                if (!value.empty()) {
                    auto firstType = value[0].type();
                    if (firstType == nlohmann::json::value_t::number_integer) {
                        std::vector<int> intArray;
                        intArray.reserve(value.size());
                        for (const auto& item : value) {
                            intArray.push_back(item.get<int>());
                        }
                        configValue = intArray;
                    } else if (firstType == nlohmann::json::value_t::number_float) {
                        std::vector<double> doubleArray;
                        doubleArray.reserve(value.size());
                        for (const auto& item : value) {
                            doubleArray.push_back(item.get<double>());
                        }
                        configValue = doubleArray;
                    } else if (firstType == nlohmann::json::value_t::string) {
                        std::vector<std::string> stringArray;
                        stringArray.reserve(value.size());
                        for (const auto& item : value) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        configValue = stringArray;
                    }
                }
            }
            
            if (!configValue.index() == std::variant_npos) {
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        ErrorReporter::getGlobal().logError("Error parsing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}
#endif

#ifdef NLM_WITH_YAML
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        ErrorReporter::getGlobal().logError("Failed to open YAML file: " + filepath);
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        clear();
        
        for (const auto& node : yamlData) {
            if (node.first.IsScalar()) {
                std::string key = node.first.Scalar();
                ConfigValue configValue;
                
                if (node.second.IsScalar()) {
                    std::string value = node.second.Scalar();
                    configValue = value;
                } else if (node.second.IsSequence()) {
                    // Check sequence type
                    if (!node.second.empty()) {
                        auto firstType = node.second[0].Type();
                        if (firstType == YAML::NodeType::INT) {
                            std::vector<int> intArray;
                            for (const auto& item : node.second) {
                                intArray.push_back(item.as<int>());
                            }
                            configValue = intArray;
                        } else if (firstType == YAML::NodeType::REAL) {
                            std::vector<double> doubleArray;
                            for (const auto& item : node.second) {
                                doubleArray.push_back(item.as<double>());
                            }
                            configValue = doubleArray;
                        } else if (firstType == YAML::NodeType::STRING) {
                            std::vector<std::string> stringArray;
                            for (const auto& item : node.second) {
                                stringArray.push_back(item.as<std::string>());
                            }
                            configValue = stringArray;
                        }
                    }
                } else if (node.second.IsScalar()) {
                    std::string value = node.second.Scalar();
                    
                    // Try to parse as different types
                    if (value == "true" || value == "false") {
                        configValue = (value == "true");
                    } else {
                        try {
                            size_t pos;
                            int intVal = std::stoi(value, &pos);
                            if (pos == value.size()) {
                                configValue = intVal;
                            } else {
                                double doubleVal = std::stod(value, &pos);
                                if (pos == value.size()) {
                                    configValue = doubleVal;
                                } else {
                                    configValue = value;
                                }
                            }
                        } catch (...) {
                            configValue = value;
                        }
                    }
                }
                
                if (!configValue.index() == std::variant_npos) {
                    set(key, configValue, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        ErrorReporter::getGlobal().logError("Error parsing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}
#endif

bool Config::loadFromSimple(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

Config::ConfigFormat Config::detectFormat(const std::string& filepath) {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".cfg") {
#ifdef NLM_WITH_JSON
        return ConfigFormat::JSON;
#else
        return ConfigFormat::Simple; // Fallback to simple format if JSON not available
#endif
    } else if (ext == ".yaml" || ext == ".yml") {
#ifdef NLM_WITH_YAML
        return ConfigFormat::YAML;
#else
        return ConfigFormat::Simple; // Fallback to simple format if YAML not available
#endif
    } else {
        // For .cfg files, try to detect format by content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return ConfigFormat::Unknown;
        }
        
        std::string firstLine;
        std::getline(file, firstLine);
        
        // Check if it looks like JSON (starts with { or [)
        firstLine = trim(firstLine);
        if (firstLine.empty()) {
            return ConfigFormat::Simple;
        }
        
        if (firstLine.front() == '{' || firstLine.front() == '[') {
#ifdef NLM_WITH_JSON
            return ConfigFormat::JSON;
#endif
        }
        
        // Check if it looks like YAML (colon-separated)
        if (firstLine.find(':') != std::string::npos) {
#ifdef NLM_WITH_YAML
            return ConfigFormat::YAML;
#endif
        }
        
        return ConfigFormat::Simple;
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

#ifdef NLM_WITH_JSON
bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = std::visit([](const auto& value) -> nlohmann::json {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return value;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    return static_cast<int64_t>(value);
                } else if constexpr (std::is_same_v<T, double>) {
                    return value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    return value;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    nlohmann::json array = nlohmann::json::array();
                    for (const auto& item : value) {
                        array.push_back(item);
                    }
                    return array;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    nlohmann::json array = nlohmann::json::array();
                    for (const auto& item : value) {
                        array.push_back(item);
                    }
                    return array;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    nlohmann::json array = nlohmann::json::array();
                    for (const auto& item : value) {
                        array.push_back(item);
                    }
                    return array;
                } else {
                    return "";
                }
            }, entry.value);
        }
        
        file << std::setw(4) << jsonData << std::endl;
        return true;
    } catch (const std::exception& e) {
        ErrorReporter::getGlobal().logError("Error writing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}
#endif

#ifdef NLM_WITH_YAML
bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginDoc;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key << YAML::Value;
            
            emitter << YAML::BeginSeq;
            emitter << YAML::Flow;
            
            std::visit([&emitter](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << value;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    emitter << static_cast<int64_t>(value);
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << (value ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    emitter << YAML::Flow;
                    for (const auto& item : value) {
                        emitter << item;
                    }
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::Flow;
                    for (const auto& item : value) {
                        emitter << item;
                    }
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::Flow;
                    for (const auto& item : value) {
                        emitter << item;
                    }
                }
            }, entry.value);
            
            emitter << YAML::EndSeq;
        }
        
        emitter << YAML::EndDoc;
        file << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        ErrorReporter::getGlobal().logError("Error writing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}
#endif

bool Config::saveToSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << static_cast<int64_t>(value) << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [\"";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << "\", \"";
                    file << value[i];
                }
                file << "\"]\n";
            } else {
                file << entry.key << " = \"\"\n";
            }
        }, entry.value);
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    ConfigFormat format = detectFormat(filepath);
    
    switch (format) {
#ifdef NLM_WITH_JSON
        case ConfigFormat::JSON:
            return saveToJSON(filepath);
#endif
#ifdef NLM_WITH_YAML
        case ConfigFormat::YAML:
            return saveToYAML(filepath);
#endif
        case ConfigFormat::Simple:
            return saveToSimple(filepath);
        default:
            ErrorReporter::getGlobal().logError("Unknown configuration format for file: " + filepath);
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

Config::ConfigValue Config::parseValue(const std::string& str) {
    // Try to parse as different types
    if (str == "true" || str == "false") {
        return (str == "true");
    }
    
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
        
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
        
        return str;
    } catch (...) {
        return str;
    }
}

bool Config::validateConfig(const Config& config, std::vector<std::string>& errors) {
    errors.clear();
    
    // Validate common configuration values
    auto validateValue = [&](const std::string& key, const ConfigValue& value) {
        try {
            if (key == "random_seed") {
                std::optional<int64_t> seed = config.get<int64_t>(key);
                if (seed.has_value() && (*seed < 0)) {
                    errors.push_back("random_seed must be non-negative");
                }
            } else if (key == "simulation_timestep") {
                std::optional<double> timestep = config.get<double>(key);
                if (timestep.has_value() && (*timestep <= 0.0)) {
                    errors.push_back("simulation_timestep must be positive");
                }
            } else if (key == "neuron_count") {
                std::optional<int64_t> count = config.get<int64_t>(key);
                if (count.has_value() && (*count <= 0)) {
                    errors.push_back("neuron_count must be positive");
                }
            } else if (key == "region_count") {
                std::optional<int64_t> count = config.get<int64_t>(key);
                if (count.has_value() && (*count <= 0)) {
                    errors.push_back("region_count must be positive");
                }
            } else if (key == "connection_probability") {
                std::optional<double> prob = config.get<double>(key);
                if (prob.has_value() && (*prob < 0.0 || *prob > 1.0)) {
                    errors.push_back("connection_probability must be between 0.0 and 1.0");
                }
            } else if (key == "plasticity_learning_rate") {
                std::optional<double> lr = config.get<double>(key);
                if (lr.has_value() && (*lr < 0.0 || *lr > 1.0)) {
                    errors.push_back("plasticity_learning_rate must be between 0.0 and 1.0");
                }
            } else if (key == "environment_width" || key == "environment_height") {
                std::optional<int64_t> size = config.get<int64_t>(key);
                if (size.has_value() && (*size <= 0)) {
                    errors.push_back(key + " must be positive");
                }
            } else if (key == "max_simulation_steps") {
                std::optional<int64_t> steps = config.get<int64_t>(key);
                if (steps.has_value() && (*steps <= 0)) {
                    errors.push_back("max_simulation_steps must be positive");
                }
            }
        } catch (const std::bad_variant_access&) {
            // Type conversion failed
            errors.push_back("Invalid type for configuration key: " + key);
        }
    };
    
    for (const auto& entry : config.getKeys()) {
        validateValue(entry, config.get<ConfigValue>(entry).value_or(ConfigValue{}));
    }
    
    return errors.empty();
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
