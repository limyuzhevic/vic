#include "JSONParser.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct JSONParser::Impl {
    std::unordered_map<std::string, std::string> supportedExtensions = {
        {".json", "application/json"},
        {".jsonl", "application/json-line"},
        {".json5", "application/json5"}
    };
};

JSONParser::JSONParser() : pImpl(std::make_unique<Impl>()) {}
JSONParser::~JSONParser() = default;

bool JSONParser::parseToConfig(const std::string& filepath, std::shared_ptr<Config> config) {
    // Check if file exists and is JSON
    if (!isJSONFile(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Validate JSON structure
        auto validationErrors = validateJSON(jsonData);
        if (!validationErrors.empty()) {
            // Log validation errors but continue
            for (const auto& error : validationErrors) {
                // Log error (in production)
            }
        }
        
        // Parse JSON to config
        bool success = parseToConfigFromString(file.rdbuf()->str(), config);
        return success;
        
    } catch (const std::exception& e) {
        // Log parsing error (in production)
        return false;
    }
}

bool JSONParser::parseToConfigFromString(const std::string& jsonString, std::shared_ptr<Config> config) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        // Check for required top-level structure
        if (!jsonData.contains("nlm")) {
            // For backward compatibility, treat as simple key-value pairs
            return parseSimpleJSON(jsonData, config);
        }
        
        // Parse nested structure
        const auto& nlmSection = jsonData["nlm"];
        
        // Parse brain configuration
        if (nlmSection.contains("brain")) {
            parseBrainSection(nlmSection["brain"], config);
        }
        
        // Parse plasticity configuration
        if (nlmSection.contains("plasticity")) {
            parsePlasticitySection(nlmSection["plasticity"], config);
        }
        
        // Parse neuromodulation configuration
        if (nlmSection.contains("neuromodulation")) {
            parseNeuromodulationSection(nlmSection["neuromodulation"], config);
        }
        
        // Parse development configuration
        if (nlmSection.contains("development")) {
            parseDevelopmentSection(nlmSection["development"], config);
        }
        
        // Parse agent configuration
        if (nlmSection.contains("agent")) {
            parseAgentSection(nlmSection["agent"], config);
        }
        
        // Parse environment configuration
        if (nlmSection.contains("environment")) {
            parseEnvironmentSection(nlmSection["environment"], config);
        }
        
        // Parse simulation configuration
        if (nlmSection.contains("simulation")) {
            parseSimulationSection(nlmSection["simulation"], config);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

nlohmann::json JSONParser::configToJSON(const Config& config) const {
    nlohmann::json jsonData;
    
    // Create nlm section
    jsonData["nlm"] = nlohmann::json::object();
    auto& nlmSection = jsonData["nlm"];
    
    // Parse brain configuration
    parseConfigSectionToJSON(config, "brain", nlmSection["brain"]);
    
    // Parse plasticity configuration
    parseConfigSectionToJSON(config, "plasticity", nlmSection["plasticity"]);
    
    // Parse neuromodulation configuration
    parseConfigSectionToJSON(config, "neuromodulation", nlmSection["neuromodulation"]);
    
    // Parse development configuration
    parseConfigSectionToJSON(config, "development", nlmSection["development"]);
    
    // Parse agent configuration
    parseConfigSectionToJSON(config, "agent", nlmSection["agent"]);
    
    // Parse environment configuration
    parseConfigSectionToJSON(config, "environment", nlmSection["environment"]);
    
    // Parse simulation configuration
    parseConfigSectionToJSON(config, "simulation", nlmSection["simulation"]);
    
    return jsonData;
}

std::vector<std::string> JSONParser::validateJSON(const nlohmann::json& json) const {
    std::vector<std::string> errors;
    
    if (!json.is_object()) {
        errors.push_back("JSON root must be an object");
        return errors;
    }
    
    // Check version if present
    if (json.contains("version")) {
        std::string version = json["version"];
        if (version != getSupportedVersion()) {
            errors.push_back("Unsupported JSON version: " + version + ". Supported: " + getSupportedVersion());
        }
    }
    
    return errors;
}

std::vector<std::string> JSONParser::dotNotationToPath(const std::string& key) const {
    std::vector<std::string> path;
    std::string current;
    
    for (char c : key) {
        if (c == '.') {
            if (!current.empty()) {
                path.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        path.push_back(current);
    }
    
    return path;
}

std::string JSONParser::pathToDotNotation(const std::vector<std::string>& path) const {
    std::string result;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) result += ".";
        result += path[i];
    }
    return result;
}

bool JSONParser::isJSONFile(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return pImpl->supportedExtensions.find(extension) != pImpl->supportedExtensions.end();
}

bool JSONParser::parseSimpleJSON(const nlohmann::json& json, std::shared_ptr<Config> config) const {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        
        ConfigValue value;
        if (parseValue(it.value(), value, key)) {
            config->set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool JSONParser::parseNestedObject(const nlohmann::json& jsonObject, std::shared_ptr<Config> config,
                                   const std::string& prefix) const {
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        std::string key = it.key();
        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
        
        if (it.value().is_object()) {
            // Recursively parse nested object
            parseNestedObject(it.value(), config, fullKey);
        } else {
            ConfigValue value;
            if (parseValue(it.value(), value, fullKey)) {
                config->set(fullKey, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

void JSONParser::parseBrainSection(const nlohmann::json& brainJson, std::shared_ptr<Config> config) const {
    if (brainJson.contains("neuron_count")) {
        config->set("brain.neuron_count", brainJson["neuron_count"].get<int64_t>(), ConfigSource::File);
    }
    
    if (brainJson.contains("region_count")) {
        config->set("brain.region_count", brainJson["region_count"].get<int64_t>(), ConfigSource::File);
    }
    
    if (brainJson.contains("connection_probability")) {
        config->set("brain.connection_probability", brainJson["connection_probability"].get<double>(), ConfigSource::File);
    }
    
    if (brainJson.contains("initial_weight_mean")) {
        config->set("brain.initial_weight_mean", brainJson["initial_weight_mean"].get<double>(), ConfigSource::File);
    }
    
    if (brainJson.contains("initial_weight_std")) {
        config->set("brain.initial_weight_std", brainJson["initial_weight_std"].get<double>(), ConfigSource::File);
    }
    
    if (brainJson.contains("v_thresh")) {
        config->set("brain.v_thresh", brainJson["v_thresh"].get<double>(), ConfigSource::File);
    }
    
    if (brainJson.contains("v_rest")) {
        config->set("brain.v_rest", brainJson["v_rest"].get<double>(), ConfigSource::File);
    }
    
    if (brainJson.contains("v_reset")) {
        config->set("brain.v_reset", brainJson["v_reset"].get<double>(), ConfigSource::File);
    }
    
    if (brainJson.contains("tau_mem")) {
        config->set("brain.tau_mem", brainJson["tau_mem"].get<double>(), ConfigSource::File);
    }
    
    if (brainJson.contains("tau_ref")) {
        config->set("brain.tau_ref", brainJson["tau_ref"].get<double>(), ConfigSource::File);
    }
}

void JSONParser::parsePlasticitySection(const nlohmann::json& plasticityJson, std::shared_ptr<Config> config) const {
    if (plasticityJson.contains("stdp")) {
        const auto& stdpJson = plasticityJson["stdp"];
        if (stdpJson.contains("enable")) {
            config->set("plasticity.stdp.enable", stdpJson["enable"].get<bool>(), ConfigSource::File);
        }
        if (stdpJson.contains("learning_rate")) {
            config->set("plasticity.stdp.learning_rate", stdpJson["learning_rate"].get<double>(), ConfigSource::File);
        }
        if (stdpJson.contains("tau_plus")) {
            config->set("plasticity.stdp.tau_plus", stdpJson["tau_plus"].get<double>(), ConfigSource::File);
        }
        if (stdpJson.contains("tau_minus")) {
            config->set("plasticity.stdp.tau_minus", stdpJson["tau_minus"].get<double>(), ConfigSource::File);
        }
    }
    
    if (plasticityJson.contains("hebbian")) {
        const auto& hebbianJson = plasticityJson["hebbian"];
        if (hebbianJson.contains("enable")) {
            config->set("plasticity.hebbian.enable", hebbianJson["enable"].get<bool>(), ConfigSource::File);
        }
        if (hebbianJson.contains("learning_rate")) {
            config->set("plasticity.hebbian.learning_rate", hebbianJson["learning_rate"].get<double>(), ConfigSource::File);
        }
    }
    
    if (plasticityJson.contains("structural")) {
        const auto& structuralJson = plasticityJson["structural"];
        if (structuralJson.contains("enable")) {
            config->set("plasticity.structural.enable", structuralJson["enable"].get<bool>(), ConfigSource::File);
        }
        if (structuralJson.contains("synaptogenesis_rate")) {
            config->set("plasticity.structural.synaptogenesis_rate", structuralJson["synaptogenesis_rate"].get<double>(), ConfigSource::File);
        }
        if (structuralJson.contains("pruning_rate")) {
            config->set("plasticity.structural.pruning_rate", structuralJson["pruning_rate"].get<double>(), ConfigSource::File);
        }
    }
}

void JSONParser::parseNeuromodulationSection(const nlohmann::json& neuromodJson, std::shared_ptr<Config> config) const {
    if (neuromodJson.contains("dopamine")) {
        const auto& dopamineJson = neuromodJson["dopamine"];
        if (dopamineJson.contains("scale")) {
            config->set("neuromod.dopamine.scale", dopamineJson["scale"].get<double>(), ConfigSource::File);
        }
    }
    
    if (neuromodJson.contains("curiosity")) {
        const auto& curiosityJson = neuromodJson["curiosity"];
        if (curiosityJson.contains("enable")) {
            config->set("neuromod.curiosity.enable", curiosityJson["enable"].get<bool>(), ConfigSource::File);
        }
    }
    
    if (neuromodJson.contains("novelty")) {
        const auto& noveltyJson = neuromodJson["novelty"];
        if (noveltyJson.contains("enable")) {
            config->set("neuromod.novelty.enable", noveltyJson["enable"].get<bool>(), ConfigSource::File);
        }
    }
}

void JSONParser::parseDevelopmentSection(const nlohmann::json& developmentJson, std::shared_ptr<Config> config) const {
    if (developmentJson.contains("enable")) {
        config->set("development.enable", developmentJson["enable"].get<bool>(), ConfigSource::File);
    }
    
    if (developmentJson.contains("critical_period_length")) {
        config->set("development.critical_period_length", developmentJson["critical_period_length"].get<double>(), ConfigSource::File);
    }
    
    if (developmentJson.contains("maturation_rate")) {
        config->set("development.maturation_rate", developmentJson["maturation_rate"].get<double>(), ConfigSource::File);
    }
    
    if (developmentJson.contains("aging_rate")) {
        config->set("development.aging_rate", developmentJson["aging_rate"].get<double>(), ConfigSource::File);
    }
}

void JSONParser::parseAgentSection(const nlohmann::json& agentJson, std::shared_ptr<Config> config) const {
    if (agentJson.contains("enable_reward_modulation")) {
        config->set("agent.enable_reward_modulation", agentJson["enable_reward_modulation"].get<bool>(), ConfigSource::File);
    }
    
    if (agentJson.contains("enable_structural_plasticity")) {
        config->set("agent.enable_structural_plasticity", agentJson["enable_structural_plasticity"].get<bool>(), ConfigSource::File);
    }
    
    if (agentJson.contains("enable_development")) {
        config->set("agent.enable_development", agentJson["enable_development"].get<bool>(), ConfigSource::File);
    }
    
    if (agentJson.contains("enable_curiosity")) {
        config->set("agent.enable_curiosity", agentJson["enable_curiosity"].get<bool>(), ConfigSource::File);
    }
    
    if (agentJson.contains("exploration_rate")) {
        config->set("agent.exploration_rate", agentJson["exploration_rate"].get<double>(), ConfigSource::File);
    }
}

void JSONParser::parseEnvironmentSection(const nlohmann::json& environmentJson, std::shared_ptr<Config> config) const {
    if (environmentJson.contains("name")) {
        config->set("environment.name", environmentJson["name"].get<std::string>(), ConfigSource::File);
    }
    
    if (environmentJson.contains("width")) {
        config->set("environment.width", environmentJson["width"].get<int64_t>(), ConfigSource::File);
    }
    
    if (environmentJson.contains("height")) {
        config->set("environment.height", environmentJson["height"].get<int64_t>(), ConfigSource::File);
    }
    
    if (environmentJson.contains("max_energy")) {
        config->set("environment.max_energy", environmentJson["max_energy"].get<double>(), ConfigSource::File);
    }
    
    if (environmentJson.contains("vision_width")) {
        config->set("environment.vision_width", environmentJson["vision_width"].get<int64_t>(), ConfigSource::File);
    }
    
    if (environmentJson.contains("vision_height")) {
        config->set("environment.vision_height", environmentJson["vision_height"].get<int64_t>(), ConfigSource::File);
    }
}

void JSONParser::parseSimulationSection(const nlohmann::json& simulationJson, std::shared_ptr<Config> config) const {
    if (simulationJson.contains("max_steps")) {
        config->set("simulation.max_steps", simulationJson["max_steps"].get<int64_t>(), ConfigSource::File);
    }
    
    if (simulationJson.contains("time_limit")) {
        config->set("simulation.time_limit", simulationJson["time_limit"].get<double>(), ConfigSource::File);
    }
    
    if (simulationJson.contains("verbose")) {
        config->set("simulation.verbose", simulationJson["verbose"].get<bool>(), ConfigSource::File);
    }
}

void JSONParser::parseConfigSectionToJSON(const Config& config, const std::string& section,
                                         nlohmann::json& jsonSection) const {
    for (const auto& key : config.getKeys()) {
        if (key.find(section + ".") == 0) {
            std::string subKey = key.substr(section.length() + 1);
            
            auto value = config.get<ConfigValue>(key);
            if (value) {
                jsonSection[subKey] = *value;
            }
        }
    }
}

bool JSONParser::parseValue(const nlohmann::json& jsonValue, ConfigValue& configValue,
                           const std::string& key) const {
    if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int64_t>();
        return true;
    } else if (jsonValue.is_number_float()) {
        configValue = jsonValue.get<double>();
        return true;
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
        return true;
    } else if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
        return true;
    } else if (jsonValue.is_array()) {
        // Parse array - for now, convert to string representation
        std::string arrayStr = "[";
        for (size_t i = 0; i < jsonValue.size(); ++i) {
            if (i > 0) arrayStr += ",";
            arrayStr += jsonValue[i].dump();
        }
        arrayStr += "]";
        configValue = arrayStr;
        return true;
    }
    
    return false;
}

std::string JSONParser::jsonValueToString(const nlohmann::json& jsonValue) const {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number()) {
        return std::to_string(jsonValue.get<double>());
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>() ? "true" : "false";
    }
    
    return jsonValue.dump();
}

} // namespace nlm
