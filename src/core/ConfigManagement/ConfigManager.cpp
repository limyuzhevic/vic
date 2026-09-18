#include "core/ConfigManagement/ConfigManager.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nlm {

void ConfigManager::initializeDefaults() {
    config_ = std::make_shared<Config>();
    // Set default values based on the DEFAULT_SCHEMA from Config.hpp
    setPhase2Defaults();
}

void ConfigManager::setPhase2Defaults() {
    currentPhase_ = "Phase2";
    // Phase 2 specific defaults
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void ConfigManager::setPhase3Defaults() {
    currentPhase_ = "Phase3";
    // Phase 3 specific defaults (development and learning)
    setPhase2Defaults(); // Start with Phase 2 defaults
    config_->set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
    config_->set("development_pruning_rate", 0.0001f, ConfigSource::Default);
    config_->set("dopamine_baseline", 0.5f, ConfigSource::Default);
}

void ConfigManager::setPhase4Defaults() {
    currentPhase_ = "Phase4";
    // Phase 4 specific defaults (cognition and planning)
    setPhase3Defaults(); // Start with Phase 3 defaults
    config_->set("reward_discount_factor", 0.95f, ConfigSource::Default);
    config_->set("planning_horizon", 10, ConfigSource::Default);
    config_->set("working_memory_capacity", 5, ConfigSource::Default);
}

void ConfigManager::setPhase6Defaults() {
    currentPhase_ = "Phase6";
    // Phase 6 specific defaults (integrated multi-phase)
    setPhase4Defaults(); // Start with Phase 4 defaults
    config_->set("meta_learning_rate", 0.01f, ConfigSource::Default);
    config_->set("transfer_learning_enabled", true, ConfigSource::Default);
    config_->set("continual_learning", true, ConfigSource::Default);
}

bool ConfigManager::loadFromFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::string format = detectFormat(filepath);
    if (format == "json") {
        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromJson(buffer.str());
    } else if (format == "yaml" || format == "yml") {
        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromYaml(buffer.str());
    } else if (format == "cfg") {
        return loadFromFileInternal(filepath);
    }
    
    return false;
}

bool ConfigManager::loadFromJson(const std::string& jsonString) {
    if (!config_) {
        initializeDefaults();
    }
    
    try {
        auto json = nlohmann::json::parse(jsonString);
        // Merge the JSON into the current config
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = static_cast<int64_t>(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Handle arrays
                if (!it->empty()) {
                    if (it->front().is_number_integer()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : *it) {
                            intArray.push_back(item.get<int64_t>());
                        }
                        value = intArray;
                    } else if (it->front().is_number_float()) {
                        std::vector<double> floatArray;
                        for (const auto& item : *it) {
                            floatArray.push_back(item.get<double>());
                        }
                        value = floatArray;
                    } else if (it->front().is_string()) {
                        std::vector<std::string> stringArray;
                        for (const auto& item : *it) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        value = stringArray;
                    }
                }
            }
            
            config_->set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::loadFromYaml(const std::string& yamlString) {
    if (!config_) {
        initializeDefaults();
    }
    
    try {
        auto yaml = YAML::Load(yamlString);
        // Iterate through YAML map
        for (const auto& node : yaml) {
            std::string key = node.first.as<std::string>();
            ConfigValue value;
            
            if (node.second.IsScalar()) {
                if (node.second.IsInt()) {
                    value = node.second.as<int64_t>();
                } else if (node.second.IsFloat()) {
                    value = node.second.as<double>();
                } else if (node.second.IsBool()) {
                    value = node.second.as<bool>();
                } else {
                    value = node.second.as<std::string>();
                }
            } else if (node.second.IsSequence()) {
                // Handle arrays
                if (!node.second.empty()) {
                    if (node.second[0].IsInt()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : node.second) {
                            intArray.push_back(item.as<int64_t>());
                        }
                        value = intArray;
                    } else if (node.second[0].IsFloat()) {
                        std::vector<double> floatArray;
                        for (const auto& item : node.second) {
                            floatArray.push_back(item.as<double>());
                        }
                        value = floatArray;
                    } else if (node.second[0].IsScalar()) {
                        std::vector<std::string> stringArray;
                        for (const auto& item : node.second) {
                            stringArray.push_back(item.as<std::string>());
                        }
                        value = stringArray;
                    }
                }
            }
            
            config_->set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing YAML: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::loadFromEnv(const std::unordered_map<std::string, std::string>& envVars) {
    if (!config_) {
        initializeDefaults();
    }
    
    bool loaded = false;
    for (const auto& [key, value] : envVars) {
        if (key.rfind("NLM_", 0) == 0) {
            std::string configKey = key.substr(4); // Remove "NLM_" prefix
            // Convert string value to appropriate type based on key
            if (key.find("SEED") != std::string::npos) {
                try {
                    int64_t intValue = std::stoll(value);
                    config_->set(configKey, intValue, ConfigSource::Runtime);
                    loaded = true;
                } catch (...) {}
            } else if (key.find("RATE") != std::string::npos || 
                       key.find("TIME") != std::string::npos ||
                       key.find("FACTOR") != std::string::npos) {
                try {
                    double doubleValue = std::stod(value);
                    config_->set(configKey, doubleValue, ConfigSource::Runtime);
                    loaded = true;
                } catch (...) {}
            } else if (key.find("ENABLED") != std::string::npos ||
                       key.find("FLAG") != std::string::npos) {
                bool boolValue = (value == "1" || value == "true" || value == "yes");
                config_->set(configKey, boolValue, ConfigSource::Runtime);
                loaded = true;
            } else {
                config_->set(configKey, value, ConfigSource::Runtime);
                loaded = true;
            }
        }
    }
    
    return loaded;
}

bool ConfigManager::loadFromCommandLine(int argc, char** argv) {
    if (!config_) {
        initializeDefaults();
    }
    
    bool loaded = false;
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 7) == "--config") {
            // Handle --config=file
            if (arg.find('=') != std::string::npos) {
                std::string configFile = arg.substr(arg.find('=') + 1);
                if (loadFromFile(configFile)) {
                    loaded = true;
                }
            } else if (i + 1 < argc) {
                // Handle --config file
                if (loadFromFile(argv[++i])) {
                    loaded = true;
                }
            }
        } else if (arg.substr(0, 2) == "--") {
            // Handle --key=value or --key value
            if (arg.find('=') != std::string::npos) {
                std::string key = arg.substr(2, arg.find('=') - 2);
                std::string valueStr = arg.substr(arg.find('=') + 1);
                
                // Convert value based on key
                if (key.find("seed") != std::string::npos) {
                    try {
                        int64_t value = std::stoll(valueStr);
                        config_->set(key, value, ConfigSource::CommandLine);
                        loaded = true;
                    } catch (...) {}
                } else if (key.find("rate") != std::string::npos || 
                           key.find("time") != std::string::npos ||
                           key.find("factor") != std::string::npos ||
                           key.find("step") != std::string::npos) {
                    try {
                        double value = std::stod(valueStr);
                        config_->set(key, value, ConfigSource::CommandLine);
                        loaded = true;
                    } catch (...) {}
                } else if (key.find("enabled") != std::string::npos ||
                           key.find("flag") != std::string::npos) {
                    bool value = (valueStr == "1" || valueStr == "true" || valueStr == "yes");
                    config_->set(key, value, ConfigSource::CommandLine);
                    loaded = true;
                } else {
                    config_->set(key, valueStr, ConfigSource::CommandLine);
                    loaded = true;
                }
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                // Handle --key value
                std::string key = arg.substr(2);
                std::string value = argv[i + 1];
                
                if (key.find("seed") != std::string::npos) {
                    try {
                        int64_t valueInt = std::stoll(value);
                        config_->set(key, valueInt, ConfigSource::CommandLine);
                        loaded = true;
                    } catch (...) {}
                } else if (key.find("rate") != std::string::npos || 
                           key.find("time") != std::string::npos ||
                           key.find("factor") != std::string::npos ||
                           key.find("step") != std::string::npos) {
                    try {
                        double valueDouble = std::stod(value);
                        config_->set(key, valueDouble, ConfigSource::CommandLine);
                        loaded = true;
                    } catch (...) {}
                } else if (key.find("enabled") != std::string::npos ||
                           key.find("flag") != std::string::npos) {
                    bool valueBool = (value == "1" || value == "true" || value == "yes");
                    config_->set(key, valueBool, ConfigSource::CommandLine);
                    loaded = true;
                } else {
                    config_->set(key, value, ConfigSource::CommandLine);
                    loaded = true;
                }
                ++i; // Skip the value argument
            }
        }
    }
    
    return loaded;
}

bool ConfigManager::saveToFile(const std::string& filepath) const {
    std::string format = detectFormat(filepath);
    if (format == "json") {
        std::ofstream file(filepath);
        file << exportToJson();
        return file.good();
    } else if (format == "yaml" || format == "yml") {
        std::ofstream file(filepath);
        file << exportToYaml();
        return file.good();
    } else if (format == "cfg") {
        return saveToFileInternal(filepath);
    }
    
    return false;
}

std::string ConfigManager::exportToJson() const {
    nlohmann::json jsonData;
    if (config_) {
        for (const auto& key : config_->getKeys()) {
            auto valueOpt = config_->getConfigValue(key);
            if (valueOpt.has_value()) {
                jsonData[key] = configValueToJson(valueOpt.value());
            }
        }
    }
    return jsonData.dump(2);
}

std::string ConfigManager::exportToYaml() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    if (config_) {
        for (const auto& key : config_->getKeys()) {
            auto valueOpt = config_->getConfigValue(key);
            if (valueOpt.has_value()) {
                emitter << YAML::Key << key;
                emitter << YAML::Value << configValueToYaml(valueOpt.value());
            }
        }
    }
    
    emitter << YAML::EndMap;
    return std::string(emitter.GetLastModified().c_str());
}

bool ConfigManager::validate() const {
    if (!config_) {
        return false;
    }
    
    return config_->validate();
}

bool ConfigManager::validateSchema(const nlohmann::json& schema) const {
    if (!config_) {
        return false;
    }
    
    config_->validateSchema(schema);
    return true;
}

void ConfigManager::set(const std::string& key, const std::string& value, ConfigSource source) {
    if (!config_) {
        initializeDefaults();
    }
    config_->set(key, value, source);
}

void ConfigManager::set(const std::string& key, int64_t value, ConfigSource source) {
    if (!config_) {
        initializeDefaults();
    }
    config_->set(key, value, source);
}

void ConfigManager::set(const std::string& key, double value, ConfigSource source) {
    if (!config_) {
        initializeDefaults();
    }
    config_->set(key, value, source);
}

void ConfigManager::set(const std::string& key, bool value, ConfigSource source) {
    if (!config_) {
        initializeDefaults();
    }
    config_->set(key, value, source);
}

std::string ConfigManager::detectFormat(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext == ".json") return "json";
    else if (ext == ".yaml" || ext == ".yml") return "yaml";
    else if (ext == ".cfg") return "cfg";
    
    // Try to detect from content
    std::ifstream file(filepath);
    if (!file.is_open()) return "unknown";
    
    std::string firstLine;
    std::getline(file, firstLine);
    
    if (firstLine.find('{') != std::string::npos) return "json";
    else if (firstLine.find("#") != std::string::npos && firstLine.find("=") != std::string::npos) return "cfg";
    
    return "unknown";
}

bool ConfigManager::isFormatSupported(const std::string& format) {
    std::string lowerFormat = format;
    std::transform(lowerFormat.begin(), lowerFormat.end(), lowerFormat.begin(), ::tolower);
    
    return lowerFormat == "json" || lowerFormat == "yaml" || lowerFormat == "yml" || lowerFormat == "cfg";
}

std::string ConfigManager::getFormatExtension(const std::string& format) {
    std::string lowerFormat = format;
    std::transform(lowerFormat.begin(), lowerFormat.end(), lowerFormat.begin(), ::tolower);
    
    if (lowerFormat == "json") return ".json";
    else if (lowerFormat == "yaml") return ".yaml";
    else if (lowerFormat == "yml") return ".yml";
    else if (lowerFormat == "cfg") return ".cfg";
    
    return "";
}

bool ConfigManager::saveCheckpoint(const std::string& prefix) {
    if (!config_) {
        return false;
    }
    
    std::string timestamp = std::to_string(std::time(nullptr));
    std::string jsonFile = generateConfigFileName(prefix + "_" + timestamp, "json");
    std::string cfgFile = generateConfigFileName(prefix + "_" + timestamp, "cfg");
    
    bool jsonSaved = saveToFile(jsonFile);
    bool cfgSaved = saveToFile(cfgFile);
    
    if (jsonSaved && cfgSaved) {
        NLM_LOG_INFO("Checkpoint saved to: " + jsonFile + " and " + cfgFile);
        return true;
    }
    
    return false;
}

bool ConfigManager::loadCheckpoint(const std::string& filepath) {
    if (filepath.empty()) {
        // Try to load the most recent checkpoint
        std::vector<std::string> checkpoints;
        std::string prefix = "checkpoint_";
        
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.path().extension() == ".json" && 
                entry.path().filename().string().find(prefix) != std::string::npos) {
                checkpoints.push_back(entry.path().string());
            }
        }
        
        if (checkpoints.empty()) {
            return false;
        }
        
        // Sort by modification time (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(), 
            [](const std::string& a, const std::string& b) {
                return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
            });
        
        return loadFromFile(checkpoints[0]);
    } else {
        return loadFromFile(filepath);
    }
}

std::string ConfigManager::generateConfigFileName(const std::string& prefix, const std::string& format) const {
    std::filesystem::path path(prefix);
    path += getFormatExtension(format);
    return path.string();
}

std::unordered_map<std::string, std::string> ConfigManager::parseEnvironmentVariables() const {
    std::unordered_map<std::string, std::string> envVars;
    
    // Get common environment variables
    const char* home = std::getenv("HOME");
    if (home) {
        envVars["NLM_HOME"] = home;
    }
    
    const char* user = std::getenv("USER");
    if (user) {
        envVars["NLM_USER"] = user;
    }
    
    const char* path = std::getenv("PATH");
    if (path) {
        envVars["NLM_PATH"] = path;
    }
    
    return envVars;
}

std::string ConfigManager::getStatsSummary() const {
    std::stringstream ss;
    ss << "Configuration Statistics:" << std::endl;
    
    if (config_) {
        ss << "  Keys: " << config_->getKeys().size() << std::endl;
        ss << "  Phase: " << currentPhase_ << std::endl;
        ss << "  Performance Time: " << perfStats_.getElapsedTimeMs() << " ms" << std::endl;
        ss << "  Steps: " << perfStats_.stepCount << std::endl;
        ss << "  Memory Usage: " << perfStats_.memoryUsage << " bytes" << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
