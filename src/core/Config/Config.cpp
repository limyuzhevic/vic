#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cmath>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Advanced configuration data
    AdvancedBrainConfig brainConfig;
    NeuromodulationConfig neuromodConfig;
    MemoryConfig memoryConfig;
    // Metadata
    std::unordered_map<std::string, std::string> parameterMetadata;
    std::chrono::system_clock::time_point lastModified;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
    lastModified = std::chrono::system_clock::now();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Initialize with default advanced configuration values
    pImpl->brainConfig.neuronCount = 2000;
    pImpl->brainConfig.regionCount = 4;
    pImpl->brainConfig.threshold = -50.0f;
    pImpl->brainConfig.restingPotential = -70.0f;
    pImpl->brainConfig.resetPotential = -75.0f;
    pImpl->brainConfig.membraneTimeConstant = 20.0f;
    pImpl->brainConfig.refractoryPeriod = 2.0f;
    pImpl->brainConfig.neuronModel = "LIF";
    pImpl->brainConfig.enableSpiking = true;
    pImpl->brainConfig.decayRate = 0.95f;
    pImpl->brainConfig.connectionProbability = 0.05f;
    pImpl->brainConfig.weightDistribution = "gaussian";
    pImpl->brainConfig.weightMean = 0.5f;
    pImpl->brainConfig.weightStd = 0.1f;
    pImpl->brainConfig.delay = 0.001f;
    pImpl->brainConfig.enablePlasticity = true;
    pImpl->brainConfig.plasticityType = "STDP";

    // Neuromodulation configuration defaults
    pImpl->neuromodConfig.dopamine.enable = true;
    pImpl->neuromodConfig.dopamine.scale = 2.0f;
    pImpl->neuromodConfig.dopamine.plasticityFactor = 1.5f;
    pImpl->neuromodConfig.dopamine.excitabilityFactor = 0.8f;
    pImpl->neuromodConfig.dopamine.learningRateModulation = true;
    pImpl->neuromodConfig.dopamine.rewardPredictionErrorGain = 0.1f;
    
    pImpl->neuromodConfig.curiosity.enable = true;
    pImpl->neuromodConfig.curiosity.explorationRate = 0.3f;
    pImpl->neuromodConfig.curiosity.noveltyThreshold = 0.5f;
    pImpl->neuromodConfig.curiosity.rewardShaping = "inverse";
    pImpl->neuromodConfig.curiosity.temperature = 1.0f;
    pImpl->neuromodConfig.curiosity.intrinsicMotivation = 0.5f;
    pImpl->neuromodConfig.curiosity.explorationBonus = true;

    // Memory configuration defaults
    pImpl->memoryConfig.workingMemory.capacity = 500;
    pImpl->memoryConfig.workingMemory.decayRate = 0.95f;
    pImpl->memoryConfig.workingMemory.competitionMethod = "soft_max";
    pImpl->memoryConfig.workingMemory.encodingStrength = 1.0f;
    pImpl->memoryConfig.workingMemory.forgettingMechanism = "competitive";
    pImpl->memoryConfig.workingMemory.enablePatternCompletion = true;
}

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
    // Currently uses simple key=value format
    
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

AdvancedBrainConfig Config::getAdvancedBrainConfig() const {
    return pImpl->brainConfig;
}

NeuromodulationConfig Config::getNeuromodulationConfig() const {
    return pImpl->neuromodConfig;
}

MemoryConfig Config::getMemoryConfig() const {
    return pImpl->memoryConfig;
}

void Config::optimizeForPerformance() {
    pImpl->brainConfig.neuronCount = 1000;
    pImpl->brainConfig.regionCount = 1;
    pImpl->brainConfig.enableSpiking = false;
    pImpl->brainConfig.connectionProbability = 0.005f;
    
    pImpl->neuromodConfig.dopamine.enable = false;
    pImpl->neuromodConfig.curiosity.enable = false;
    
    pImpl->memoryConfig.workingMemory.capacity = 200;
    pImpl->memoryConfig.workingMemory.decayRate = 0.98f;
}

void Config::optimizeForLearning() {
    pImpl->brainConfig.neuronCount = 5000;
    pImpl->brainConfig.regionCount = 4;
    pImpl->brainConfig.enableSpiking = true;
    pImpl->brainConfig.connectionProbability = 0.02f;
    pImpl->brainConfig.enablePlasticity = true;
    
    pImpl->neuromodConfig.dopamine.enable = true;
    pImpl->neuromodConfig.dopamine.scale = 3.0f;
    pImpl->neuromodConfig.dopamine.plasticityFactor = 2.0f;
    pImpl->neuromodConfig.dopamine.learningRateModulation = true;
    
    pImpl->neuromodConfig.curiosity.enable = true;
    pImpl->neuromodConfig.curiosity.explorationRate = 0.5f;
    pImpl->neuromodConfig.curiosity.explorationBonus = true;
    
    pImpl->neuromodConfig.novelty.enable = true;
    pImpl->neuromodConfig.novelty.noveltyWeight = 2.0f;
    
    pImpl->memoryConfig.workingMemory.capacity = 1000;
    pImpl->memoryConfig.workingMemory.decayRate = 0.9f;
    pImpl->memoryConfig.episodicMemory.maxEpisodes = 2000;
    pImpl->memoryConfig.episodicMemory.enableSystematicReplay = true;
}

void Config::optimizeForBiologicalAccuracy() {
    pImpl->brainConfig.threshold = -55.0f;
    pImpl->brainConfig.restingPotential = -65.0f;
    pImpl->brainConfig.resetPotential = -70.0f;
    pImpl->brainConfig.membraneTimeConstant = 20.0f;
    pImpl->brainConfig.refractoryPeriod = 2.0f;
    pImpl->brainConfig.neuronModel = "exponential";
    
    pImpl->neuromodConfig.dopamine.scale = 1.5f;
    pImpl->neuromodConfig.dopamine.plasticityFactor = 1.2f;
    pImpl->neuromodConfig.dopamine.excitabilityFactor = 0.9f;
    
    pImpl->neuromodConfig.curiosity.explorationRate = 0.2f;
    pImpl->neuromodConfig.curiosity.temperature = 0.5f;
    
    pImpl->neuromodConfig.novelty.threshold = 0.15f;
    pImpl->neuromodConfig.novelty.decayRate = 0.95f;
    pImpl->neuromodConfig.novelty.adaptationWindow = 200;
    
    pImpl->memoryConfig.workingMemory.capacity = 2000;
    pImpl->memoryConfig.workingMemory.decayRate = 0.97f;
    pImpl->memoryConfig.workingMemory.competitionMethod = "gaussian";
}

void Config::optimizeForSpeed() {
    pImpl->brainConfig.neuronCount = 1000;
    pImpl->brainConfig.regionCount = 1;
    pImpl->brainConfig.enableSpiking = false;
    pImpl->brainConfig.connectionProbability = 0.005f;
    
    pImpl->neuromodConfig.dopamine.enable = false;
    pImpl->neuromodConfig.curiosity.enable = false;
    pImpl->neuromodConfig.novelty.enable = false;
    
    pImpl->memoryConfig.workingMemory.capacity = 200;
    pImpl->memoryConfig.episodicMemory.maxEpisodes = 100;
}

void Config::addParameterMetadata(const std::string& key, const std::string& description,
                                 const std::string& unit, const std::string& validationRules) {
    pImpl->parameterMetadata[key] = description + " | " + unit + " | " + validationRules;
}

std::string Config::getParameterMetadata(const std::string& key) const {
    auto it = pImpl->parameterMetadata.find(key);
    if (it != pImpl->parameterMetadata.end()) {
        return it->second;
    }
    return "";
}

bool Config::loadFromJSON(const std::string& jsonData) {
    try {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        
        // Load advanced brain configuration
        if (j.contains("brain")) {
            const auto& brain = j["brain"];
            pImpl->brainConfig.neuronCount = brain.value("neuron_count", pImpl->brainConfig.neuronCount);
            pImpl->brainConfig.regionCount = brain.value("region_count", pImpl->brainConfig.regionCount);
            pImpl->brainConfig.threshold = brain.value("neuron_threshold", pImpl->brainConfig.threshold);
            pImpl->brainConfig.restingPotential = brain.value("neuron_resting_potential", pImpl->brainConfig.restingPotential);
            pImpl->brainConfig.resetPotential = brain.value("neuron_reset_potential", pImpl->brainConfig.resetPotential);
            pImpl->brainConfig.membraneTimeConstant = brain.value("neuron_membrane_time_constant", pImpl->brainConfig.membraneTimeConstant);
            pImpl->brainConfig.refractoryPeriod = brain.value("neuron_refractory_period", pImpl->brainConfig.refractoryPeriod);
            pImpl->brainConfig.neuronModel = brain.value("neuron_model", pImpl->brainConfig.neuronModel);
            pImpl->brainConfig.enableSpiking = brain.value("neuron_enable_spiking", pImpl->brainConfig.enableSpiking);
            pImpl->brainConfig.decayRate = brain.value("neuron_decay_rate", pImpl->brainConfig.decayRate);
            
            pImpl->brainConfig.connectionProbability = brain.value("connection_probability", pImpl->brainConfig.connectionProbability);
            pImpl->brainConfig.weightDistribution = brain.value("weight_distribution", pImpl->brainConfig.weightDistribution);
            pImpl->brainConfig.weightMean = brain.value("weight_mean", pImpl->brainConfig.weightMean);
            pImpl->brainConfig.weightStd = brain.value("weight_std", pImpl->brainConfig.weightStd);
            pImpl->brainConfig.delay = brain.value("delay", pImpl->brainConfig.delay);
            pImpl->brainConfig.enablePlasticity = brain.value("enable_plasticity", pImpl->brainConfig.enablePlasticity);
            pImpl->brainConfig.plasticityType = brain.value("plasticity_type", pImpl->brainConfig.plasticityType);
        }
        
        // Load neuromodulation configuration
        if (j.contains("neuromod")) {
            const auto& neuromod = j["neuromod"];
            if (neurom.contains("dopamine")) {
                const auto& dopamine = neuromod["dopamine"];
                pImpl->neuromodConfig.dopamine.enable = dopamine.value("enable", pImpl->neuromodConfig.dopamine.enable);
                pImpl->neuromodConfig.dopamine.scale = dopamine.value("scale", pImpl->neuromodConfig.dopamine.scale);
                pImpl->neuromodConfig.dopamine.plasticityFactor = dopamine.value("plasticity_factor", pImpl->neuromodConfig.dopamine.plasticityFactor);
                pImpl->neuromodConfig.dopamine.excitabilityFactor = dopamine.value("excitability_factor", pImpl->neuromodConfig.dopamine.excitabilityFactor);
                pImpl->neuromodConfig.dopamine.learningRateModulation = dopamine.value("learning_rate_modulation", pImpl->neuromodConfig.dopamine.learningRateModulation);
                pImpl->neuromodConfig.dopamine.rewardPredictionErrorGain = dopamine.value("reward_prediction_error_gain", pImpl->neuromodConfig.dopamine.rewardPredictionErrorGain);
            }
            
            if (neurom.contains("curiosity")) {
                const auto& curiosity = neuromod["curiosity"];
                pImpl->neuromodConfig.curiosity.enable = curiosity.value("enable", pImpl->neuromodConfig.curiosity.enable);
                pImpl->neuromodConfig.curiosity.explorationRate = curiosity.value("exploration_rate", pImpl->neuromodConfig.curiosity.explorationRate);
                pImpl->neuromodConfig.curiosity.noveltyThreshold = curiosity.value("novelty_threshold", pImpl->neuromodConfig.curiosity.noveltyThreshold);
                pImpl->neuromodConfig.curiosity.rewardShaping = curiosity.value("reward_shaping", pImpl->neuromodConfig.curiosity.rewardShaping);
                pImpl->neuromodConfig.curiosity.temperature = curiosity.value("temperature", pImpl->neuromodConfig.curiosity.temperature);
                pImpl->neuromodConfig.curiosity.intrinsicMotivation = curiosity.value("intrinsic_motivation", pImpl->neuromodConfig.curiosity.intrinsicMotivation);
                pImpl->neuromodConfig.curiosity.explorationBonus = curiosity.value("exploration_bonus", pImpl->neuromodConfig.curiosity.explorationBonus);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string Config::toJSON() const {
    nlohmann::json j;
    
    // Convert advanced brain configuration
    j["brain"]["neuron_count"] = pImpl->brainConfig.neuronCount;
    j["brain"]["region_count"] = pImpl->brainConfig.regionCount;
    j["brain"]["neuron_model"] = pImpl->brainConfig.neuronModel;
    j["brain"]["neuron_threshold"] = pImpl->brainConfig.threshold;
    j["brain"]["neuron_resting_potential"] = pImpl->brainConfig.restingPotential;
    j["brain"]["neuron_reset_potential"] = pImpl->brainConfig.resetPotential;
    j["brain"]["neuron_membrane_time_constant"] = pImpl->brainConfig.membraneTimeConstant;
    j["brain"]["neuron_refractory_period"] = pImpl->brainConfig.refractoryPeriod;
    j["brain"]["neuron_enable_spiking"] = pImpl->brainConfig.enableSpiking;
    j["brain"]["neuron_decay_rate"] = pImpl->brainConfig.decayRate;
    
    j["brain"]["connection_probability"] = pImpl->brainConfig.connectionProbability;
    j["brain"]["weight_distribution"] = pImpl->brainConfig.weightDistribution;
    j["brain"]["weight_mean"] = pImpl->brainConfig.weightMean;
    j["brain"]["weight_std"] = pImpl->brainConfig.weightStd;
    j["brain"]["delay"] = pImpl->brainConfig.delay;
    j["brain"]["enable_plasticity"] = pImpl->brainConfig.enablePlasticity;
    j["brain"]["plasticity_type"] = pImpl->brainConfig.plasticityType;
    
    // Convert neuromodulation configuration
    j["neuromod"]["dopamine"]["enable"] = pImpl->neuromodConfig.dopamine.enable;
    j["neuromod"]["dopamine"]["scale"] = pImpl->neuromodConfig.dopamine.scale;
    j["neuromod"]["dopamine"]["plasticity_factor"] = pImpl->neuromodConfig.dopamine.plasticityFactor;
    j["neuromod"]["dopamine"]["excitability_factor"] = pImpl->neuromodConfig.dopamine.excitabilityFactor;
    j["neuromod"]["dopamine"]["learning_rate_modulation"] = pImpl->neuromodConfig.dopamine.learningRateModulation;
    j["neuromod"]["dopamine"]["reward_prediction_error_gain"] = pImpl->neuromodConfig.dopamine.rewardPredictionErrorGain;
    
    j["neuromod"]["curiosity"]["enable"] = pImpl->neuromodConfig.curiosity.enable;
    j["neuromod"]["curiosity"]["exploration_rate"] = pImpl->neuromodConfig.curiosity.explorationRate;
    j["neuromod"]["curiosity"]["novelty_threshold"] = pImpl->neuromodConfig.curiosity.noveltyThreshold;
    j["neuromod"]["curiosity"]["reward_shaping"] = pImpl->neuromodConfig.curiosity.rewardShaping;
    j["neuromod"]["curiosity"]["temperature"] = pImpl->neuromodConfig.curiosity.temperature;
    j["neuromod"]["curiosity"]["intrinsic_motivation"] = pImpl->neuromodConfig.curiosity.intrinsicMotivation;
    j["neuromod"]["curiosity"]["exploration_bonus"] = pImpl->neuromodConfig.curiosity.explorationBonus;
    
    return j.dump(2);
}

bool Config::validate() const {
    // Basic validation
    if (pImpl->brainConfig.neuronCount < 100 || pImpl->brainConfig.neuronCount > 100000) {
        return false;
    }
    
    if (pImpl->brainConfig.regionCount < 1) {
        return false;
    }
    
    if (pImpl->brainConfig.threshold > 0.0f || pImpl->brainConfig.threshold < -100.0f) {
        return false;
    }
    
    if (pImpl->brainConfig.restingPotential < -100.0f || pImpl->brainConfig.restingPotential > 0.0f) {
        return false;
    }
    
    if (pImpl->neuromodConfig.dopamine.scale < 0.0f || pImpl->neuromodConfig.dopamine.scale > 10.0f) {
        return false;
    }
    
    if (pImpl->neuromodConfig.curiosity.explorationRate < 0.0f || pImpl->neuromodConfig.curiosity.explorationRate > 1.0f) {
        return false;
    }
    
    if (pImpl->neuromodConfig.curiosity.temperature <= 0.0f) {
        return false;
    }
    
    if (pImpl->memoryConfig.workingMemory.capacity < 10) {
        return false;
    }
    
    if (pImpl->memoryConfig.workingMemory.capacity > 100000) {
        return false;
    }
    
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (pImpl->brainConfig.neuronCount < 100 || pImpl->brainConfig.neuronCount > 100000) {
        errors.push_back("Invalid neuron count: " + std::to_string(pImpl->brainConfig.neuronCount) + ". Must be between 100 and 100,000");
    }
    
    if (pImpl->brainConfig.regionCount < 1) {
        errors.push_back("Invalid region count: " + std::to_string(pImpl->brainConfig.regionCount) + ". Must be at least 1");
    }
    
    if (pImpl->brainConfig.threshold > 0.0f || pImpl->brainConfig.threshold < -100.0f) {
        errors.push_back("Invalid neuron threshold: " + std::to_string(pImpl->brainConfig.threshold) + ". Must be between -100.0 and 0.0");
    }
    
    if (pImpl->brainConfig.restingPotential < -100.0f || pImpl->brainConfig.restingPotential > 0.0f) {
        errors.push_back("Invalid resting potential: " + std::to_string(pImpl->brainConfig.restingPotential) + ". Must be between -100.0 and 0.0");
    }
    
    if (pImpl->neuromodConfig.dopamine.scale < 0.0f || pImpl->neuromodConfig.dopamine.scale > 10.0f) {
        errors.push_back("Invalid dopamine scale: " + std::to_string(pImpl->neuromodConfig.dopamine.scale) + ". Must be between 0.0 and 10.0");
    }
    
    if (pImpl->neuromodConfig.curiosity.explorationRate < 0.0f || pImpl->neuromodConfig.curiosity.explorationRate > 1.0f) {
        errors.push_back("Invalid curiosity exploration rate: " + std::to_string(pImpl->neuromodConfig.curiosity.explorationRate) + ". Must be between 0.0 and 1.0");
    }
    
    if (pImpl->neuromodConfig.curiosity.temperature <= 0.0f) {
        errors.push_back("Invalid curiosity temperature: " + std::to_string(pImpl->neuromodConfig.curiosity.temperature) + ". Must be > 0.0");
    }
    
    if (pImpl->memoryConfig.workingMemory.capacity < 10) {
        errors.push_back("Invalid working memory capacity: " + std::to_string(pImpl->memoryConfig.workingMemory.capacity) + ". Must be >= 10");
    }
    
    if (pImpl->memoryConfig.workingMemory.capacity > 100000) {
        errors.push_back("Invalid working memory capacity: " + std::to_string(pImpl->memoryConfig.workingMemory.capacity) + ". Must be <= 100,000");
    }
    
    return errors;
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
