#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Advanced configuration objects
    LearningRateConfig learningRates;
    NetworkArchitectureConfig networkArchitecture;
    NeuromodulationConfig neuromodulation;
    DevelopmentConfig development;
    MemoryConfig memory;
    PredictionConfig prediction;
    CognitionConfig cognition;
    PerformanceConfig performance;
    
    // Validation errors
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values for all advanced configurations
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Set default configuration entries for basic settings
    set("neuron_count", 100, ConfigSource::Default);
    set("region_count", 1, ConfigSource::Default);
    set("connection_probability", 0.1, ConfigSource::Default);
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Advanced configuration defaults are initialized in the struct
}

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
    // For now, use simple key=value format with enhanced parsing
    
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
    
    // Update advanced configuration from file
    updateAdvancedConfigFromFile(filepath);
    
    return true;
}

void Config::updateAdvancedConfigFromFile(const std::string& filepath) {
    // Create a backup config for comparison
    auto backupConfig = *this;
    
    // Try to parse advanced configuration from file
    // This is a simplified implementation - real implementation would use JSON/YAML
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Look for advanced configuration keys
        if (line.find("learning_rate") != std::string::npos && line.find("=" ) != std::string::npos) {
            // Extract learning rate settings
            parseLearningRateFromLine(line);
        } else if (line.find("network_architecture") != std::string::npos && line.find("=" ) != std::string::npos) {
            // Extract network architecture settings
            parseNetworkArchitectureFromLine(line);
        } else if (line.find("neuromodulation") != std::string::npos && line.find("=" ) != std::string::npos) {
            // Extract neuromodulation settings
            parseNeuromodulationFromLine(line);
        }
    }
}

void Config::parseLearningRateFromLine(const std::string& line) {
    // Simplified parsing - in practice, use JSON/YAML parser
    size_t pos = line.find("=");
    if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        try {
            if (key == "learning_rate.basal") {
                pImpl->learningRates.basalRate = std::stod(value);
            } else if (key == "learning_rate.stdp_ltp") {
                pImpl->learningRates.stdpLTP = std::stod(value);
            } else if (key == "learning_rate.stdp_ltd") {
                pImpl->learningRates.stdpLTD = std::stod(value);
            } else if (key == "learning_rate.hebbian") {
                pImpl->learningRates.hebbianStrength = std::stod(value);
            } else if (key == "learning_rate.homeostatic") {
                pImpl->learningRates.homeostatic = std::stod(value);
            } else if (key == "learning_rate.neuromodulation") {
                pImpl->learningRates.neuromodulation = std::stod(value);
            }
        } catch (...) {
            // Ignore parsing errors
        }
    }
}

void Config::parseNetworkArchitectureFromLine(const std::string& line) {
    size_t pos = line.find("=");
    if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        try {
            if (key == "network_architecture.neuron_count") {
                pImpl->networkArchitecture.neuronCount = std::stoull(value);
            } else if (key == "network_architecture.region_count") {
                pImpl->networkArchitecture.regionCount = std::stoull(value);
            } else if (key == "network_architecture.connection_prob") {
                pImpl->networkArchitecture.connectionProbability = std::stod(value);
            } else if (key == "network_architecture.architecture") {
                pImpl->networkArchitecture.architecture = value;
            } else if (key == "network_architecture.structural_plasticity") {
                pImpl->networkArchitecture.structuralPlasticity = std::stod(value);
            } else if (key == "network_architecture.pruning_rate") {
                pImpl->networkArchitecture.pruningRate = std::stod(value);
            }
        } catch (...) {
            // Ignore parsing errors
        }
    }
}

void Config::parseNeuromodulationFromLine(const std::string& line) {
    size_t pos = line.find("=");
    if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        try {
            if (key == "neuromodulation.dopamine_enabled") {
                pImpl->neuromodulation.dopamineEnabled = (value == "true" || value == "1");
            } else if (key == "neuromodulation.serotonin_enabled") {
                pImpl->neuromodulation.serotoninEnabled = (value == "true" || value == "1");
            } else if (key == "neuromodulation.acetylcholine_enabled") {
                pImpl->neuromodulation.acetylcholineEnabled = (value == "true" || value == "1");
            } else if (key == "neuromodulation.noradrenaline_enabled") {
                pImpl->neuromodulation.noradrenalineEnabled = (value == "true" || value == "1");
            } else if (key == "neuromodulation.dopamine_baseline") {
                pImpl->neuromodulation.dopamineBaseline = std::stod(value);
            } else if (key == "neuromodulation.dopamine_max") {
                pImpl->neuromodulation.dopamineMax = std::stod(value);
            } else if (key == "neuromodulation.novelty_sensitivity") {
                pImpl->neuromodulation.noveltySensitivity = std::stod(value);
            } else if (key == "neuromodulation.reward_prediction_gain") {
                pImpl->neuromodulation.rewardPredictionGain = std::stod(value);
            }
        } catch (...) {
            // Ignore parsing errors
        }
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
    
    // Update advanced configuration when relevant keys are set
    updateAdvancedConfigFromKey(key, value);
}

bool Config::setLearningRates(const LearningRateConfig& config) {
    if (!config.validate()) {
        pImpl->validationErrors.push_back("Invalid learning rate configuration");
        return false;
    }
    
    pImpl->learningRates = config;
    
    // Update basic configuration
    set("learning_rate.basal", config.basalRate, ConfigSource::Runtime);
    set("learning_rate.stdp_ltp", config.stdpLTP, ConfigSource::Runtime);
    set("learning_rate.stdp_ltd", config.stdpLTD, ConfigSource::Runtime);
    set("learning_rate.hebbian", config.hebbianStrength, ConfigSource::Runtime);
    set("learning_rate.homeostatic", config.homeostatic, ConfigSource::Runtime);
    set("learning_rate.neuromodulation", config.neuromodulation, ConfigSource::Runtime);
    
    return true;
}

bool Config::setNetworkArchitecture(const NetworkArchitectureConfig& config) {
    if (!config.validate()) {
        pImpl->validationErrors.push_back("Invalid network architecture configuration");
        return false;
    }
    
    pImpl->networkArchitecture = config;
    
    // Update basic configuration
    set("network_architecture.neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Runtime);
    set("network_architecture.region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Runtime);
    set("network_architecture.connection_prob", config.connectionProbability, ConfigSource::Runtime);
    set("network_architecture.architecture", config.architecture, ConfigSource::Runtime);
    set("network_architecture.structural_plasticity", config.structuralPlasticity, ConfigSource::Runtime);
    set("network_architecture.pruning_rate", config.pruningRate, ConfigSource::Runtime);
    
    return true;
}

bool Config::setNeuromodulation(const NeuromodulationConfig& config) {
    if (!config.validate()) {
        pImpl->validationErrors.push_back("Invalid neuromodulation configuration");
        return false;
    }
    
    pImpl->neuromodulation = config;
    
    // Update basic configuration
    set("neuromodulation.dopamine_enabled", config.dopamineEnabled, ConfigSource::Runtime);
    set("neuromodulation.serotonin_enabled", config.serotoninEnabled, ConfigSource::Runtime);
    set("neuromodulation.acetylcholine_enabled", config.acetylcholineEnabled, ConfigSource::Runtime);
    set("neuromodulation.noradrenaline_enabled", config.noradrenalineEnabled, ConfigSource::Runtime);
    set("neuromodulation.dopamine_baseline", config.dopamineBaseline, ConfigSource::Runtime);
    set("neuromodulation.dopamine_max", config.dopamineMax, ConfigSource::Runtime);
    set("neuromodulation.novelty_sensitivity", config.noveltySensitivity, ConfigSource::Runtime);
    set("neuromodulation.reward_prediction_gain", config.rewardPredictionGain, ConfigSource::Runtime);
    
    return true;
}

bool Config::setDevelopment(const DevelopmentConfig& config) {
    if (!config.validate()) {
        pImpl->validationErrors.push_back("Invalid development configuration");
        return false;
    }
    
    pImpl->development = config;
    
    // Update basic configuration
    set("development.enabled", config.developmentEnabled, ConfigSource::Runtime);
    set("development.maturation_rate", config.maturationRate, ConfigSource::Runtime);
    set("development.critical_period", config.criticalPeriod, ConfigSource::Runtime);
    set("development.plasticity_decay", config.plasticityDecay, ConfigSource::Runtime);
    set("development.structural", config.structuralDevelopment, ConfigSource::Runtime);
    
    return true;
}

bool Config::setMemory(const MemoryConfig& config) {
    if (!config.validate()) {
        pImpl->validationErrors.push_back("Invalid memory configuration");
        return false;
    }
    
    pImpl->memory = config;
    
    // Update basic configuration
    set("memory.working_enabled", config.workingMemoryEnabled, ConfigSource::Runtime);
    set("memory.episodic_enabled", config.episodicMemoryEnabled, ConfigSource::Runtime);
    set("memory.semantic_enabled", config.semanticMemoryEnabled, ConfigSource::Runtime);
    set("memory.procedural_enabled", config.proceduralMemoryEnabled, ConfigSource::Runtime);
    set("memory.consolidation_rate", config.consolidationRate, ConfigSource::Runtime);
    set("memory.decay_rate", config.decayRate, ConfigSource::Runtime);
    set("memory.capacity", config.capacity, ConfigSource::Runtime);
    set("memory.replay_enabled", config.replayEnabled, ConfigSource::Runtime);
    
    return true;
}

bool Config::setPrediction(const PredictionConfig& config) {
    if (!config.validate()) {
        pImpl->validationErrors.push_back("Invalid prediction configuration");
        return false;
    }
    
    pImpl->prediction = config;
    
    // Update basic configuration
    set("prediction.enabled", config.predictionEnabled, ConfigSource::Runtime);
    set("prediction.gain", config.predictionGain, ConfigSource::Runtime);
    set("prediction.error_gain", config.predictionErrorGain, ConfigSource::Runtime);
    set("prediction.forward_model", config.forwardModelEnabled, ConfigSource::Runtime);
    set("prediction.inverse_model", config.inverseModelEnabled, ConfigSource::Runtime);
    
    return true;
}

bool Config::setCognition(const CognitionConfig& config) {
    if (!config.validate()) {
        pImpl->validationErrors.push_back("Invalid cognition configuration");
        return false;
    }
    
    pImpl->cognition = config;
    
    // Update basic configuration
    set("cognition.planning_enabled", config.planningEnabled, ConfigSource::Runtime);
    set("cognition.attention_enabled", config.attentionEnabled, ConfigSource::Runtime);
    set("cognition.concept_formation_enabled", config.conceptFormationEnabled, ConfigSource::Runtime);
    set("cognition.social_learning_enabled", config.socialLearningEnabled, ConfigSource::Runtime);
    set("cognition.planning_horizon", config.planningHorizon, ConfigSource::Runtime);
    set("cognition.attention_threshold", config.attentionThreshold, ConfigSource::Runtime);
    set("cognition.concept_formation_rate", config.conceptFormationRate, ConfigSource::Runtime);
    set("cognition.social_learning_rate", config.socialLearningRate, ConfigSource::Runtime);
    
    return true;
}

bool Config::setPerformance(const PerformanceConfig& config) {
    if (!config.validate()) {
        pImpl->validationErrors.push_back("Invalid performance configuration");
        return false;
    }
    
    pImpl->performance = config;
    
    // Update basic configuration
    set("performance.vectorization", config.vectorizationEnabled, ConfigSource::Runtime);
    set("performance.parallelization", config.parallelizationEnabled, ConfigSource::Runtime);
    set("performance.memory_pool", config.memoryPoolEnabled, ConfigSource::Runtime);
    set("performance.event_driven", config.eventDrivenEnabled, ConfigSource::Runtime);
    set("performance.thread_count", static_cast<int64_t>(config.threadCount), ConfigSource::Runtime);
    set("performance.memory_pool_size", static_cast<int64_t>(config.memoryPoolSize), ConfigSource::Runtime);
    set("performance.simulation_speed", config.simulationSpeed, ConfigSource::Runtime);
    set("performance.checkpointing", config.checkpointingEnabled, ConfigSource::Runtime);
    set("performance.checkpoint_path", config.checkpointPath, ConfigSource::Runtime);
    
    return true;
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

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    if (!validateLearningRates()) return false;
    if (!validateNetworkArchitecture()) return false;
    if (!validateNeuromodulation()) return false;
    if (!validateDevelopment()) return false;
    if (!validateMemory()) return false;
    if (!validatePrediction()) return false;
    if (!validateCognition()) return false;
    if (!validatePerformance()) return false;
    
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

bool Config::validateLearningRates() const {
    if (!pImpl->learningRates.validate()) {
        pImpl->validationErrors.push_back("Invalid learning rates: basal rate, STDP weights, Hebbian strength, homeostatic rate, or neuromodulation influence out of valid range");
        return false;
    }
    return true;
}

bool Config::validateNetworkArchitecture() const {
    if (!pImpl->networkArchitecture.validate()) {
        pImpl->validationErrors.push_back("Invalid network architecture: neuron count, region count, connection probability, architecture type, structural plasticity, or pruning rate out of valid range");
        return false;
    }
    return true;
}

bool Config::validateNeuromodulation() const {
    if (!pImpl->neuromodulation.validate()) {
        pImpl->validationErrors.push_back("Invalid neuromodulation: dopamine baseline/max, novelty sensitivity, or reward prediction gain out of valid range");
        return false;
    }
    return true;
}

bool Config::validateDevelopment() const {
    if (!pImpl->development.validate()) {
        pImpl->validationErrors.push_back("Invalid development: maturation rate, critical period, or plasticity decay out of valid range");
        return false;
    }
    return true;
}

bool Config::validateMemory() const {
    if (!pImpl->memory.validate()) {
        pImpl->validationErrors.push_back("Invalid memory: consolidation rate, decay rate, or capacity out of valid range");
        return false;
    }
    return true;
}

bool Config::validatePrediction() const {
    if (!pImpl->prediction.validate()) {
        pImpl->validationErrors.push_back("Invalid prediction: prediction gain or prediction error gain out of valid range");
        return false;
    }
    return true;
}

bool Config::validateCognition() const {
    if (!pImpl->cognition.validate()) {
        pImpl->validationErrors.push_back("Invalid cognition: planning horizon, attention threshold, concept formation rate, or social learning rate out of valid range");
        return false;
    }
    return true;
}

bool Config::validatePerformance() const {
    if (!pImpl->performance.validate()) {
        pImpl->validationErrors.push_back("Invalid performance: thread count, memory pool size, or simulation speed out of valid range");
        return false;
    }
    return true;
}

LearningRateConfig Config::getLearningRates() const {
    return pImpl->learningRates;
}

NetworkArchitectureConfig Config::getNetworkArchitecture() const {
    return pImpl->networkArchitecture;
}

NeuromodulationConfig Config::getNeuromodulation() const {
    return pImpl->neuromodulation;
}

DevelopmentConfig Config::getDevelopment() const {
    return pImpl->development;
}

MemoryConfig Config::getMemory() const {
    return pImpl->memory;
}

PredictionConfig Config::getPrediction() const {
    return pImpl->prediction;
}

CognitionConfig Config::getCognition() const {
    return pImpl->cognition;
}

PerformanceConfig Config::getPerformance() const {
    return pImpl->performance;
}
