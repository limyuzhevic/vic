#include "ConfigBuilder.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <sstream>
#include <algorithm>
#include <unordered_set>

namespace nlm {

ConfigBuilder::ConfigBuilder() {
    defineDefaultGroups();
}

void ConfigBuilder::addParameter(const Parameter& param) {
    // Check for duplicate
    for (const auto& existing : parameters_) {
        if (existing.key == param.key) {
            // Replace existing parameter
            size_t index = 0;
            for (; index < parameters_.size(); ++index) {
                if (parameters_[index].key == param.key) break;
            }
            if (index < parameters_.size()) {
                parameters_[index] = param;
            } else {
                parameters_.push_back(param);
            }
            return;
        }
    }
    parameters_.push_back(param);
}

void ConfigBuilder::addParameter(const std::string& key, int value, const std::string& description) {
    Parameter param(key, ConfigValue(value), description);
    categorizeParameter(param);
    addParameter(param);
}

void ConfigBuilder::addParameter(const std::string& key, int64_t value, const std::string& description) {
    Parameter param(key, ConfigValue(value), description);
    categorizeParameter(param);
    addParameter(param);
}

void ConfigBuilder::addParameter(const std::string& key, double value, const std::string& description) {
    Parameter param(key, ConfigValue(value), description);
    categorizeParameter(param);
    addParameter(param);
}

void ConfigBuilder::addParameter(const std::string& key, bool value, const std::string& description) {
    Parameter param(key, ConfigValue(value), description);
    categorizeParameter(param);
    addParameter(param);
}

void ConfigBuilder::addParameter(const std::string& key, const std::string& value, const std::string& description) {
    Parameter param(key, ConfigValue(value), description);
    categorizeParameter(param);
    addParameter(param);
}

void ConfigBuilder::addParameter(const std::string& key, const std::vector<int>& value, const std::string& description) {
    Parameter param(key, ConfigValue(value), description);
    categorizeParameter(param);
    addParameter(param);
}

void ConfigBuilder::addParameter(const std::string& key, const std::vector<double>& value, const std::string& description) {
    Parameter param(key, ConfigValue(value), description);
    categorizeParameter(param);
    addParameter(param);
}

void ConfigBuilder::addParameter(const std::string& key, const std::vector<std::string>& value, const std::string& description) {
    Parameter param(key, ConfigValue(value), description);
    categorizeParameter(param);
    addParameter(param);
}

void ConfigBuilder::addGroup(const ParameterGroup& group) {
    groups_.push_back(group);
}

void ConfigBuilder::addSimulationGroup() {
    addGroup(defineSimulationGroup());
}

void ConfigBuilder::addPlasticityGroup() {
    addGroup(definePlasticityGroup());
}

void ConfigBuilder::addNeuromodulationGroup() {
    addGroup(defineNeuromodulationGroup());
}

void ConfigBuilder::addDevelopmentGroup() {
    addGroup(defineDevelopmentGroup());
}

void ConfigBuilder::addMemoryGroup() {
    addGroup(defineMemoryGroup());
}

void ConfigBuilder::addMotorGroup() {
    addGroup(defineMotorGroup());
}

void ConfigBuilder::addRewardGroup() {
    addGroup(defineRewardGroup());
}

void ConfigBuilder::addEnvironmentGroup() {
    addGroup(defineEnvironmentGroup());
}

void ConfigBuilder::addLoggingGroup() {
    addGroup(defineLoggingGroup());
}

void ConfigBuilder::addVisualizationGroup() {
    addGroup(defineVisualizationGroup());
}

std::shared_ptr<Config> ConfigBuilder::buildConfig() const {
    auto config = std::make_shared<Config>();
    
    for (const auto& param : parameters_) {
        config->set(param.key, param.defaultValue, param.source);
    }
    
    return config;
}

std::shared_ptr<Config> ConfigBuilder::buildConfig(const std::map<std::string, ConfigValue>& customValues) const {
    auto config = std::make_shared<Config>();
    
    // Add all default parameters
    for (const auto& param : parameters_) {
        config->set(param.key, param.defaultValue, param.source);
    }
    
    // Apply custom values
    for (const auto& custom : customValues) {
        config->set(custom.first, custom.second, ConfigSource::Runtime);
    }
    
    return config;
}

bool ConfigBuilder::buildAndApply(ConfigManager& manager) const {
    auto config = buildConfig();
    
    if (!config) {
        return false;
    }
    
    // Replace manager config
    manager.clear();
    
    // This would need additional methods to apply config to manager
    // For now, just set the config directly
    // manager.setConfig(config);
    
    return true;
}

const ConfigBuilder::Parameter* ConfigBuilder::getParameter(const std::string& key) const {
    for (const auto& param : parameters_) {
        if (param.key == key) {
            return &param;
        }
    }
    return nullptr;
}

const ConfigBuilder::ParameterGroup* ConfigBuilder::getGroup(const std::string& groupName) const {
    for (const auto& group : groups_) {
        if (group.name == groupName) {
            return &group;
        }
    }
    return nullptr;
}

std::string ConfigBuilder::exportToJson() const {
    nlohmann::json json;
    
    // Add version
    json["version"] = "1.0";
    
    // Add parameters
    for (const auto& param : parameters_) {
        json["parameters"][param.key] = valueToString(param.defaultValue);
        json["parameters"][param.key]["description"] = param.description;
        json["parameters"][param.key]["category"] = param.category;
        json["parameters"][param.key]["source"] = static_cast<int>(param.source);
    }
    
    // Add groups
    for (const auto& group : groups_) {
        json["groups"][group.name]["description"] = group.description;
        for (const auto& param : group.parameters) {
            json["groups"][group.name]["parameters"].push_back(param.key);
        }
    }
    
    return json.dump(2);
}

bool ConfigBuilder::importFromJson(const std::string& jsonString) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        
        // Clear existing parameters
        parameters_.clear();
        groups_.clear();
        
        // Import parameters
        if (json.contains("parameters")) {
            for (auto it = json["parameters"].begin(); it != json["parameters"].end(); ++it) {
                std::string key = it->begin().key();
                std::string value = it->begin().get<std::string>();
                
                Parameter param;
                param.key = key;
                parseSimpleType(value, param.defaultValue);
                param.description = it->begin()["description"].get<std::string>();
                param.category = it->begin()["category"].get<std::string>();
                
                if (it->begin().contains("source")) {
                    param.source = static_cast<ConfigSource>(it->begin()["source"].get<int>());
                }
                
                addParameter(param);
            }
        }
        
        // Import groups
        if (json.contains("groups")) {
            for (auto it = json["groups"].begin(); it != json["groups"].end(); ++it) {
                std::string name = it->begin().key();
                ParameterGroup group(name, it->begin()["description"].get<std::string>());
                
                if (it->begin().contains("parameters")) {
                    for (const auto& paramName : it->begin()["parameters"].get<std::vector<std::string>>()) {
                        const Parameter* param = getParameter(paramName);
                        if (param) {
                            group.parameters.push_back(*param);
                        }
                    }
                }
                
                groups_.push_back(group);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

std::string ConfigBuilder::exportToYaml() const {
    YAML::Emitter emitter;
    
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "version" << YAML::Value << "1.0";
    
    emitter << YAML::Key << "parameters" << YAML::Value << YAML::BeginMap;
    for (const auto& param : parameters_) {
        emitter << YAML::Key << param.key << YAML::Value;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "value" << YAML::Value << valueToString(param.defaultValue);
        emitter << YAML::Key << "description" << YAML::Value << param.description;
        emitter << YAML::Key << "category" << YAML::Value << param.category;
        emitter << YAML::Key << "source" << YAML::Value << static_cast<int>(param.source);
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndMap;
    
    emitter << YAML::Key << "groups" << YAML::Value << YAML::BeginMap;
    for (const auto& group : groups_) {
        emitter << YAML::Key << group.name << YAML::Value;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "description" << YAML::Value << group.description;
        emitter << YAML::Key << "parameters" << YAML::Value << YAML::BeginSeq;
        for (const auto& param : group.parameters) {
            emitter << param.key;
        }
        emitter << YAML::EndSeq;
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndMap;
    
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

bool ConfigBuilder::importFromYaml(const std::string& yamlString) {
    try {
        auto yaml = YAML::Load(yamlString);
        
        // Clear existing parameters
        parameters_.clear();
        groups_.clear();
        
        // Import parameters
        if (yaml["parameters"]) {
            for (const auto& paramNode : yaml["parameters"]) {
                std::string key = paramNode.begin()->first.as<std::string>();
                std::string value = paramNode[key]["value"].as<std::string>();
                
                Parameter param;
                param.key = key;
                parseSimpleType(value, param.defaultValue);
                param.description = paramNode[key]["description"].as<std::string>();
                param.category = paramNode[key]["category"].as<std::string>();
                
                if (paramNode[key]["source"]) {
                    param.source = static_cast<ConfigSource>(paramNode[key]["source"].as<int>());
                }
                
                addParameter(param);
            }
        }
        
        // Import groups
        if (yaml["groups"]) {
            for (const auto& groupNode : yaml["groups"]) {
                std::string name = groupNode.begin()->first.as<std::string>();
                ParameterGroup group(name, groupNode[name]["description"].as<std::string>());
                
                if (groupNode[name]["parameters"]) {
                    for (const auto& paramName : groupNode[name]["parameters"].as<std::vector<std::string>>()) {
                        const Parameter* param = getParameter(paramName);
                        if (param) {
                            group.parameters.push_back(*param);
                        }
                    }
                }
                
                groups_.push_back(group);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        return false;
    }
}

std::vector<std::string> ConfigBuilder::getCategories() const {
    std::unordered_set<std::string> categories;
    
    for (const auto& param : parameters_) {
        categories.insert(param.category);
    }
    
    return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<ConfigBuilder::Parameter> ConfigBuilder::getParametersInCategory(const std::string& category) const {
    std::vector<Parameter> result;
    
    for (const auto& param : parameters_) {
        if (param.category == category) {
            result.push_back(param);
        }
    }
    
    return result;
}

void ConfigBuilder::categorizeParameter(Parameter& param) {
    // Default categorization based on key
    std::string key = toLower(param.key);
    
    if (key.find("neuron") != std::string::npos || 
        key.find("region") != std::string::npos ||
        key.find("connection") != std::string::npos) {
        param.category = "simulation";
    } else if (key.find("stdp") != std::string::npos ||
               key.find("plastic") != std::string::npos ||
               key.find("learning") != std::string::npos) {
        param.category = "plasticity";
    } else if (key.find("dopamine") != std::string::npos ||
               key.find("neuromod") != std::string::npos ||
               key.find("curiosity") != std::string::npos) {
        param.category = "neuromodulation";
    } else if (key.find("development") != std::string::npos ||
               key.find("matur") != std::string::npos) {
        param.category = "development";
    } else if (key.find("memory") != std::string::npos ||
               key.find("working") != std::string::npos) {
        param.category = "memory";
    } else if (key.find("motor") != std::string::npos ||
               key.find("action") != std::string::npos) {
        param.category = "motor";
    } else if (key.find("reward") != std::string::npos ||
               key.find("value") != std::string::npos) {
        param.category = "reward";
    } else if (key.find("environ") != std::string::npos ||
               key.find("world") != std::string::npos) {
        param.category = "environment";
    } else if (key.find("log") != std::string::npos ||
               key.find("level") != std::string::npos) {
        param.category = "logging";
    } else if (key.find("visual") != std::string::npos ||
               key.find("gui") != std::string::npos) {
        param.category = "visualization";
    } else {
        param.category = "general";
    }
}

ConfigValue ConfigBuilder::convertValue(const std::string& type, const std::string& value) const {
    // Convert string to ConfigValue based on type
    if (type == "int" || type == "int64") {
        return std::stoll(value);
    } else if (type == "double" || type == "float") {
        return std::stod(value);
    } else if (type == "bool") {
        return value == "true";
    } else {
        return value;
    }
}

std::string ConfigBuilder::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

void ConfigBuilder::defineDefaultGroups() {
    defineSimulationGroup();
    definePlasticityGroup();
    defineNeuromodulationGroup();
    defineDevelopmentGroup();
    defineMemoryGroup();
    defineMotorGroup();
    defineRewardGroup();
    defineEnvironmentGroup();
    defineLoggingGroup();
    defineVisualizationGroup();
}

void ConfigBuilder::defineSimulationGroup() {
    ParameterGroup group("Simulation", "Core simulation parameters");
    group.parameters.push_back(Parameter("random_seed", 42, "Random seed for reproducible experiments"));
    group.parameters.push_back(Parameter("simulation_timestep", 0.001, "Simulation timestep in seconds"));
    group.parameters.push_back(Parameter("neuron_count", 1000, "Number of neurons in the brain"));
    group.parameters.push_back(Parameter("region_count", 2, "Number of brain regions"));
    group.parameters.push_back(Parameter("connection_probability", 0.1, "Connection probability for random connectivity"));
    group.parameters.push_back(Parameter("max_simulation_steps", 10000, "Maximum number of simulation steps"));
    group.parameters.push_back(Parameter("simulation_time_limit", 0.0, "Simulation time limit in seconds (0 = unlimited)"));
    addGroup(group);
    
    // Also add to parameters list
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::definePlasticityGroup() {
    ParameterGroup group("Plasticity", "Synaptic plasticity parameters");
    group.parameters.push_back(Parameter("plasticity_learning_rate", 0.01, "Base learning rate for plasticity"));
    group.parameters.push_back(Parameter("stdp_ltp_weight", 0.01, "STDP LTP weight"));
    group.parameters.push_back(Parameter("stdp_ltd_weight", 0.012, "STDP LTD weight"));
    group.parameters.push_back(Parameter("stdp_tau", 20.0, "STDP time constant"));
    group.parameters.push_back(Parameter("hebbian_weight_increment", 0.01, "Hebbian weight increment"));
    group.parameters.push_back(Parameter("hebbian_weight_decrement", 0.01, "Hebbian weight decrement"));
    group.parameters.push_back(Parameter("plasticity_mod", 0.5, "Plasticity modulation factor (development)"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::defineNeuromodulationGroup() {
    ParameterGroup group("Neuromodulation", "Neuromodulator parameters");
    group.parameters.push_back(Parameter("dopamine_baseline", 0.1, "Dopamine baseline level"));
    group.parameters.push_back(Parameter("dopamine_peak", 1.0, "Dopamine peak level"));
    group.parameters.push_back(Parameter("dopamine_decay_rate", 0.1, "Dopamine decay rate per ms"));
    group.parameters.push_back(Parameter("dopamine_release_rate", 1.0, "Dopamine release rate per reward unit"));
    group.parameters.push_back(Parameter("curiosity_level", 0.5, "Curiosity level for exploration"));
    group.parameters.push_back(Parameter("prediction_error", 0.0, "Prediction error signal"));
    group.parameters.push_back(Parameter("neurally_modulated", true, "Enable neural modulation"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::defineDevelopmentGroup() {
    ParameterGroup group("Development", "Development system parameters");
    group.parameters.push_back(Parameter("development_stage", 0.0, "Development stage (-100=embryonic, 0=normal, 100=adult)"));
    group.parameters.push_back(Parameter("development_synaptogenesis_rate", 0.001, "Synaptogenesis rate"));
    group.parameters.push_back(Parameter("development_pruning_rate", 0.0001, "Pruning rate"));
    group.parameters.push_back(Parameter("maturation_progress_step", 0.01, "Maturation progress step"));
    group.parameters.push_back(Parameter("mature_threshold", -55.0, "Membrane potential threshold for maturity"));
    group.parameters.push_back(Parameter("mature_resting_potential", -70.0, "Mature neuron resting potential"));
    group.parameters.push_back(Parameter("mature_time_constant", 20.0, "Mature neuron time constant"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::defineMemoryGroup() {
    ParameterGroup group("Memory", "Memory system parameters");
    group.parameters.push_back(Parameter("working_memory_capacity", 100, "Working memory capacity"));
    group.parameters.push_back(Parameter("initial_working_memory_capacity", 100, "Initial working memory capacity"));
    group.parameters.push_back(Parameter("consolidation_interval", 10, "Memory consolidation interval (steps)"));
    group.parameters.push_back(Parameter("replay_frequency", 0.1, "Memory replay frequency"));
    group.parameters.push_back(Parameter("replay_duration", 100, "Memory replay duration (steps)"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::defineMotorGroup() {
    ParameterGroup group("Motor", "Motor system parameters");
    group.parameters.push_back(Parameter("planning_depth", 5, "Motor planning depth"));
    group.parameters.push_back(Parameter("planning_confidence", 0.5, "Default planning confidence"));
    group.parameters.push_back(Parameter("curiosity_level", 0.5, "Curiosity level"));
    group.parameters.push_back(Parameter("prediction_error", 0.0, "Prediction error"));
    group.parameters.push_back(Parameter("action_selection_temperature", 1.0, "Action selection temperature"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::defineRewardGroup() {
    ParameterGroup group("Reward", "Reward system parameters");
    group.parameters.push_back(Parameter("reward_scaling_factor", 1.0, "Reward scaling factor"));
    group.parameters.push_back(Parameter("reward_history_size", 100, "Reward history size"));
    group.parameters.push_back(Parameter("positive_reward_threshold", 1.0, "Positive reward threshold"));
    group.parameters.push_back(Parameter("negative_reward_threshold", -1.0, "Negative reward threshold"));
    group.parameters.push_back(Parameter("reward_discount_factor", 0.99, "Reward discount factor"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::defineEnvironmentGroup() {
    ParameterGroup group("Environment", "Environment parameters");
    group.parameters.push_back(Parameter("environment_name", "GridWorld", "Environment name"));
    group.parameters.push_back(Parameter("environment_width", 10, "Environment width"));
    group.parameters.push_back(Parameter("environment_height", 10, "Environment height"));
    group.parameters.push_back(Parameter("environment_boundary_wrap", true, "Wrap environment boundaries"));
    group.parameters.push_back(Parameter("environment_seed", 42, "Environment random seed"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::defineLoggingGroup() {
    ParameterGroup group("Logging", "Logging parameters");
    group.parameters.push_back(Parameter("log_level", "INFO", "Log level (DEBUG/INFO/WARNING/ERROR/FATAL)"));
    group.parameters.push_back(Parameter("log_to_file", false, "Enable logging to file"));
    group.parameters.push_back(Parameter("log_filename", "nlm.log", "Log filename"));
    group.parameters.push_back(Parameter("log_file_max_size", 10485760, "Maximum log file size in bytes"));
    group.parameters.push_back(Parameter("log_backup_count", 5, "Number of backup log files"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

void ConfigBuilder::defineVisualizationGroup() {
    ParameterGroup group("Visualization", "Visualization parameters");
    group.parameters.push_back(Parameter("visualization_enabled", false, "Enable visualization"));
    group.parameters.push_back(Parameter("visualization_update_rate", 30.0, "Visualization update rate (Hz)"));
    group.parameters.push_back(Parameter("visualization_backend", "terminal", "Visualization backend"));
    group.parameters.push_back(Parameter("visualization_window_width", 800, "Visualization window width"));
    group.parameters.push_back(Parameter("visualization_window_height", 600, "Visualization window height"));
    addGroup(group);
    
    for (const auto& param : group.parameters) {
        addParameter(param);
    }
}

} // namespace nlm