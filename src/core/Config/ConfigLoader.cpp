#include "ConfigLoader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace nlm {

ConfigLoader::ConfigLoader() {
    // Register default JSON parser for .json files
    registerJsonParser(".json", [](const std::string& content) {
        return nlohmann::json::parse(content);
    });
    
    // Register YAML parser for .yaml and .yml files if available
    // (Implementation depends on YAML library availability)
}

bool ConfigLoader::loadFromJsonString(const std::string& jsonString, ConfigSource source) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        return parseJsonObject(json, source);
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

bool ConfigLoader::loadFromJsonFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return loadFromJsonString(buffer.str(), source);
}

bool ConfigLoader::loadFromYamlString(const std::string& yamlString, ConfigSource source) {
    // YAML parsing implementation would go here
    // For now, treat as simple key=value format
    std::istringstream stream(yamlString);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (!parseKeyValuePair(line, source)) {
            // Log warning for parse error
        }
    }
    
    return true;
}

bool ConfigLoader::loadFromYamlFile(const std::string& filepath, ConfigSource source) {
    return loadFromYamlString("", source); // TODO: Implement proper YAML parsing
}

bool ConfigLoader::parseCommandLine(int argc, char** argv, Config& config, ConfigSource source) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help flag
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
        
        // Handle --config file.cfg
        if (arg.substr(0, 7) == "--config") {
            std::string configFile = "configs/default.cfg";
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            loadFromJsonFile(configFile, ConfigSource::File);
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                config.set(key, value, source);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                config.set(key, value, source);
            }
        }
    }
    
    return true;
}

void ConfigLoader::registerJsonParser(const std::string& extension, JsonParser parser) {
    jsonParsers_[extension] = std::move(parser);
}

std::vector<ConfigValidator::ValidationResult> ConfigLoader::validate() const {
    if (validator_) {
        return validator_->validateAll(config_);
    }
    return {};
}

std::vector<std::string> ConfigLoader::getKeys() const {
    return config_.getKeys();
}

bool ConfigLoader::parseKeyValuePair(const std::string& line, ConfigSource source) {
    std::string trimmedLine = trim(line);
    
    // Skip empty lines and comments
    if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == '/' || 
        (trimmedLine.size() >= 2 && trimmedLine.substr(0, 2) == "//")) {
        return true;
    }
    
    // Parse simple key=value pairs
    size_t pos = trimmedLine.find('=');
    if (pos == std::string::npos) {
        return false; // Invalid format
    }
    
    std::string key = trim(trimmedLine.substr(0, pos));
    std::string value = trim(trimmedLine.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return parseSimpleType(value, config_.getOr(value, ConfigValue()));
}

bool ConfigLoader::parseJsonObject(const nlohmann::json& json, ConfigSource source) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it->begin().key();
        ConfigValue value;
        
        try {
            // Convert JSON value to ConfigValue
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                // Handle array conversion
                if (it->empty()) {
                    value = std::vector<double>{};
                } else if (std::all_of(it->begin(), it->end(), [](const auto& item) { return item.is_number(); })) {
                    value = it->get<std::vector<double>>();
                } else if (std::all_of(it->begin(), it->end(), [](const auto& item) { return item.is_string(); })) {
                    std::vector<std::string> strVec;
                    for (const auto& item : *it) {
                        strVec.push_back(item.get<std::string>());
                    }
                    value = strVec;
                } else if (std::all_of(it->begin(), it->end(), [](const auto& item) { return item.is_number_integer(); })) {
                    std::vector<int> intVec;
                    for (const auto& item : *it) {
                        intVec.push_back(item.get<int>());
                    }
                    value = intVec;
                }
            }
        } catch (const std::exception& e) {
            return false;
        }
        
        if (!value.index()) {
            return false;
        }
        
        config_.set(key, value, source);
    }
    
    return true;
}

bool ConfigLoader::parseSimpleType(const std::string& value, ConfigValue& out) const {
    // Try to parse as boolean
    if (value == "true") {
        out = true;
        return true;
    } else if (value == "false") {
        out = false;
        return true;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            out = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            out = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Default to string
    out = value;
    return true;
}

std::string ConfigLoader::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigLoader::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void ConfigLoader::printHelp() {
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config file.cfg             Load configuration from file" << std::endl;
    std::cout << "  --neurons N                   Set number of neurons" << std::endl;
    std::cout << "  --regions R                   Set number of regions" << std::endl;
    std::cout << "  --stdp-ltp LTP_VALUE          Set STDP LTP weight" << std::endl;
    std::cout << "  --stdp-ltd LTD_VALUE          Set STDP LTD weight" << std::endl;
    std::cout << "  --plasticity-mod MOD          Set plasticity modulation factor" << std::endl;
    std::cout << "  --development-stage STAGE     Set development stage (normal/adult/embryonic)" << std::endl;
    std::cout << "  --random-seed SEED             Set random seed" << std::endl;
    std::cout << "  --simulation-timestep T        Set simulation timestep (seconds)" << std::endl;
    std::cout << "  --connection-prob P           Set connection probability" << std::endl;
    std::cout << "  --log-level LEVEL             Set logging level (DEBUG/INFO/WARNING/ERROR/FATAL)" << std::endl;
    std::cout << "  --visualization                Enable visualization" << std::endl;
    std::cout << "  --log-to-file                  Enable logging to file" << std::endl;
}

bool ConfigLoader::parseCommandLine(int argc, char** argv, Config& config, ConfigSource source) {
    // Handle help flag before any other processing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false; // Indicate help was shown
        }
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --config file.cfg
        if (arg.substr(0, 7) == "--config") {
            std::string configFile = "configs/default.cfg";
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            loadFromJsonFile(configFile, ConfigSource::File);
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                config.set(key, value, source);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                config.set(key, value, source);
            }
        }
    }
    
    return true;
}

void ConfigLoader::printHelp() {
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config file.cfg             Load configuration from file" << std::endl;
    std::cout << "  --neurons N                   Set number of neurons" << std::endl;
    std::cout << "  --regions R                   Set number of regions" << std::endl;
    std::cout << "  --stdp-ltp LTP_VALUE          Set STDP LTP weight" << std::endl;
    std::cout << "  --stdp-ltd LTD_VALUE          Set STDP LTD weight" << std::endl;
    std::cout << "  --plasticity-mod MOD          Set plasticity modulation factor" << std::endl;
    std::cout << "  --development-stage STAGE     Set development stage (normal/adult/embryonic)" << std::endl;
    std::cout << "  --random-seed SEED             Set random seed" << std::endl;
    std::cout << "  --simulation-timestep T        Set simulation timestep (seconds)" << std::endl;
    std::cout << "  --connection-prob P           Set connection probability" << std::endl;
    std::cout << "  --log-level LEVEL             Set logging level (DEBUG/INFO/WARNING/ERROR/FATAL)" << std::endl;
    std::cout << "  --visualization                Enable visualization" << std::endl;
    std::cout << "  --log-to-file                  Enable logging to file" << std::endl;
}

} // namespace nlm