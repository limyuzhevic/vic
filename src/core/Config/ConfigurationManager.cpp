#include "ConfigurationManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <unordered_set>

namespace nlm {

ConfigurationSection::ConfigurationSection(const std::string& name) : name_(name) {}

void ConfigurationSection::merge(const ConfigurationSection& other) {
    for (const auto& pair : other.values_) {
        values_[pair.first] = pair.second;
    }
}

// BrainConfigurationSection implementation
BrainConfigurationSection::BrainConfigurationSection() {
    // Set default values
    setNeuronCount(500);
    setRegionCount(1);
    setConnectionProbability(0.15f);
    setRandomSeed(42);
    setSimulationTimestep(0.001);
    
    enableHebbian();
    enableSTDP();
    enableStructural();
    
    enableRewardModulation();
    enableCuriosity();
    enableNovelty();
    
    enableDevelopment();
    
    setAgentVisionWidth(16);
    setAgentVisionHeight(16);
    setAgentTouchCount(8);
}

ConfigurationValidationResult BrainConfigurationSection::validate() const {
    ConfigurationValidationResult result;
    result.valid = true;
    
    // Validate neuron count
    int64_t neuronCount = getNeuronCount();
    if (neuronCount < 10 || neuronCount > 10000) {
        result.valid = false;
        result.errorMessage += "Invalid neuron count: " + std::to_string(neuronCount) + ". Must be between 10 and 10000. ";
    }
    
    // Validate region count
    int64_t regionCount = getRegionCount();
    if (regionCount < 1 || regionCount > 100) {
        result.valid = false;
        result.errorMessage += "Invalid region count: " + std::to_string(regionCount) + ". Must be between 1 and 100. ";
    }
    
    // Validate connection probability
    float connectionProb = getConnectionProbability();
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        result.valid = false;
        result.errorMessage += "Invalid connection probability: " + std::to_string(connectionProb) + ". Must be between 0.0 and 1.0. ";
    }
    
    // Validate simulation timestep
    double timestep = getSimulationTimestep();
    if (timestep < 0.0001f || timestep > 0.1f) {
        result.valid = false;
        result.errorMessage += "Invalid simulation timestep: " + std::to_string(timestep) + ". Must be between 0.0001s and 0.1s. ";
    }
    
    // Validate STDP weights
    float stdpLTP = getSTDPWeightLTP();
    float stdpLTD = getSTDPWeightLTD();
    if (stdpLTP <= 0.0f || stdpLTD <= 0.0f) {
        result.valid = false;
        result.errorMessage += "Invalid STDP weights: LTP=" + std::to_string(stdpLTP) + ", LTD=" + std::to_string(stdpLTD) + ". Must be positive. ";
    }
    
    // Validate structural plasticity rates
    float synRate = getSynaptogenesisRate();
    float pruneRate = getPruningRate();
    if (synRate < 0.0f || synRate > 0.1f) {
        result.warnings.push_back("Synaptogenesis rate out of recommended range: " + std::to_string(synRate));
    }
    
    if (pruneRate < 0.0f || pruneRate > 0.01f) {
        result.warnings.push_back("Pruning rate out of recommended range: " + std::to_string(pruneRate));
    }
    
    return result;
}

bool BrainConfigurationSection::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "[" << name_ << "]" << std::endl;
    
    for (const auto& pair : values_) {
        file << "  " << pair.first << " = ";
        
        if (std::is_same<std::decay_t<decltype(pair.second)>, std::string>::value) {
            file << "\"" << std::any_cast<std::string>(pair.second) << "\"";
        } else if (std::is_same<std::decay_t<decltype(pair.second)>, double>::value) {
            file << std::fixed << std::setprecision(6) << std::any_cast<double>(pair.second);
        } else if (std::is_same<std::decay_t<decltype(pair.second)>, float>::value) {
            file << std::fixed << std::setprecision(6) << std::any_cast<float>(pair.second);
        } else if (std::is_same<std::decay_t<decltype(pair.second)>, int64_t>::value) {
            file << std::any_cast<int64_t>(pair.second);
        } else if (std::is_same<std::decay_t<decltype(pair.second)>, bool>::value) {
            file << (std::any_cast<bool>(pair.second) ? "true" : "false");
        } else {
            file << std::any_cast<std::string>(pair.second);
        }
        
        file << std::endl;
    }
    
    file << std::endl;
    return true;
}

bool BrainConfigurationSection::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        if (currentSection != name_) {
            continue;
        }
        
        size_t separatorPos = line.find(" = ");
        if (separatorPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, separatorPos);
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        
        std::string value = line.substr(separatorPos + 3);
        value.erase(0, value.find_first_not_of(" \t\""));
        value.erase(value.find_last_not_of(" \t\"") + 1);
        
        // Remove quotes from string values
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        
        // Determine type and set value
        if (key == "neuron_count") {
            try { setNeuronCount(std::stoll(value)); } catch (...) {}
        } else if (key == "region_count") {
            try { setRegionCount(std::stoll(value)); } catch (...) {}
        } else if (key == "connection_probability") {
            try { setConnectionProbability(std::stof(value)); } catch (...) {}
        } else if (key == "random_seed") {
            try { setRandomSeed(std::stoll(value)); } catch (...) {}
        } else if (key == "simulation_timestep") {
            try { setSimulationTimestep(std::stod(value)); } catch (...) {}
        } else if (key == "plasticity.hebbian.enable") {
            set(std::any_cast<bool>(values_["plasticity.hebbian.enable"]), value == "true" || value == "1" || value == "yes");
        } else if (key == "plasticity.stdp.enable") {
            set(std::any_cast<bool>(values_["plasticity.stdp.enable"]), value == "true" || value == "1" || value == "yes");
        } else if (key == "plasticity.structural.enable") {
            set(std::any_cast<bool>(values_["plasticity.structural.enable"]), value == "true" || value == "1" || value == "yes");
        } else if (key == "plasticity.stdp.ltp_weight") {
            try { setSTDPWeightLTP(std::stof(value)); } catch (...) {}
        } else if (key == "plasticity.stdp.ltd_weight") {
            try { setSTDPWeightLTD(std::stof(value)); } catch (...) {}
        } else if (key == "plasticity.stdp.tau") {
            try { setSTDPTimeConstant(std::stof(value)); } catch (...) {}
        } else if (key == "plasticity.structural.synaptogenesis_rate") {
            try { setSynaptogenesisRate(std::stof(value)); } catch (...) {}
        } else if (key == "plasticity.structural.pruning_rate") {
            try { setPruningRate(std::stof(value)); } catch (...) {}
        } else if (key == "neuromodulation.reward.enable") {
            set(std::any_cast<bool>(values_["neuromodulation.reward.enable"]), value == "true" || value == "1" || value == "yes");
        } else if (key == "neuromodulation.curiosity.enable") {
            set(std::any_cast<bool>(values_["neuromodulation.curiosity.enable"]), value == "true" || value == "1" || value == "yes");
        } else if (key == "neuromodulation.novelty.enable") {
            set(std::any_cast<bool>(values_["neuromodulation.novelty.enable"]), value == "true" || value == "1" || value == "yes");
        } else if (key == "development.enable") {
            set(std::any_cast<bool>(values_["development.enable"]), value == "true" || value == "1" || value == "yes");
        } else if (key == "agent.sensory.vision.width") {
            try { setAgentVisionWidth(std::stoi(value)); } catch (...) {}
        } else if (key == "agent.sensory.vision.height") {
            try { setAgentVisionHeight(std::stoi(value)); } catch (...) {}
        } else if (key == "agent.sensory.touch.count") {
            try { setAgentTouchCount(std::stoi(value)); } catch (...) {}
        }
    }
    
    return true;
}

std::string BrainConfigurationSection::toString() const {
    std::ostringstream oss;
    oss << "[" << name_ << "]" << std::endl;
    
    for (const auto& pair : values_) {
        oss << "  " << pair.first << " = ";
        
        if (std::is_same<std::decay_t<decltype(pair.second)>, std::string>::value) {
            oss << "\"" << std::any_cast<std::string>(pair.second) << "\"";
        } else if (std::is_same<std::decay_t<decltype(pair.second)>, double>::value) {
            oss << std::fixed << std::setprecision(6) << std::any_cast<double>(pair.second);
        } else if (std::is_same<std::decay_t<decltype(pair.second)>, float>::value) {
            oss << std::fixed << std::setprecision(6) << std::any_cast<float>(pair.second);
        } else if (std::is_same<std::decay_t<decltype(pair.second)>, int64_t>::value) {
            oss << std::any_cast<int64_t>(pair.second);
        } else if (std::is_same<std::decay_t<decltype(pair.second)>, bool>::value) {
            oss << (std::any_cast<bool>(pair.second) ? "true" : "false");
        } else {
            oss << std::any_cast<std::string>(pair.second);
        }
        
        oss << std::endl;
    }
    
    return oss.str();
}

// ExperimentConfigurationSection implementation
ExperimentConfigurationSection::ExperimentConfigurationSection() {
    setExperimentType("learning");
    setExperimentSteps(1000);
    setExperimentReward(1.0f);
    
    enableMetrics();
    disableBenchmark();
    disableDebugMode();
    
    setLogLevel("info");
    setLogFile("nlm_simulation.log");
}

ConfigurationValidationResult ExperimentConfigurationSection::validate() const {
    ConfigurationValidationResult result;
    result.valid = true;
    
    // Validate experiment type
    std::string expType = getExperimentType();
    std::unordered_set<std::string> validTypes = {"learning", "benchmark", "debug", "visualization"};
    if (validTypes.find(expType) == validTypes.end()) {
        result.valid = false;
        result.errorMessage += "Invalid experiment type: " + expType + ". Valid types: learning, benchmark, debug, visualization. ";
    }
    
    // Validate experiment steps
    int steps = getExperimentSteps();
    if (steps < 1 || steps > 1000000) {
        result.valid = false;
        result.errorMessage += "Invalid experiment steps: " + std::to_string(steps) + ". Must be between 1 and 1000000. ";
    }
    
    // Validate experiment reward
    float reward = getExperimentReward();
    if (reward < -100.0f || reward > 100.0f) {
        result.valid = false;
        result.errorMessage += "Invalid experiment reward: " + std::to_string(reward) + ". Must be between -100.0 and 100.0. ";
    }
    
    // Validate log level
    std::string logLevel = getLogLevel();
    std::unordered_set<std::string> validLogLevels = {"debug", "info", "warning", "error", "critical"};
    if (validLogLevels.find(logLevel) == validLogLevels.end()) {
        result.warnings.push_back("Invalid log level: " + logLevel + ". Using 'info' instead.");
    }
    
    return result;
}

// ConfigurationManager implementation
struct ConfigurationManager::Impl {
    std::shared_ptr<BrainConfigurationSection> brainSection;
    std::shared_ptr<ExperimentConfigurationSection> experimentSection;
    std::shared_ptr<Logger> logger;
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->brainSection = std::make_shared<BrainConfigurationSection>();
    pImpl->experimentSection = std::make_shared<ExperimentConfigurationSection>();
    pImpl->logger = Logger::getGlobal();
}

ConfigurationManager::~ConfigurationManager() = default;

ConfigurationManager& ConfigurationManager::getGlobal() {
    static std::shared_ptr<ConfigurationManager> instance = std::make_shared<ConfigurationManager>();
    return *instance;
}

void ConfigurationManager::setGlobal(std::shared_ptr<ConfigurationManager> manager) {
    // Note: This is a simplified implementation. In a real system, you might want
    // to implement proper global state management.
}

BrainConfigurationSection& ConfigurationManager::getBrainConfig() {
    return *pImpl->brainSection;
}

ExperimentConfigurationSection& ConfigurationManager::getExperimentConfig() {
    return *pImpl->experimentSection;
}

std::vector<std::string> ConfigurationManager::getSectionNames() const {
    return {pImpl->brainSection->getName(), pImpl->experimentSection->getName()};
}

void ConfigurationManager::merge(const ConfigurationManager& other) {
    pImpl->brainSection->merge(*other.pImpl->brainSection);
    pImpl->experimentSection->merge(*other.pImpl->experimentSection);
}

bool ConfigurationManager::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << pImpl->brainSection->toString();
    file << pImpl->experimentSection->toString();
    
    return true;
}

bool ConfigurationManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        if (currentSection == "brain") {
            pImpl->brainSection->loadFromFile("");
        } else if (currentSection == "experiment") {
            // We need to modify loadFromFile to accept a string stream
            // For now, this is a placeholder
        }
    }
    
    return true;
}

bool ConfigurationManager::loadFromStream(std::istream& stream) {
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Parse key-value pairs
        size_t separatorPos = line.find(" = ");
        if (separatorPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, separatorPos);
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        
        std::string value = line.substr(separatorPos + 3);
        value.erase(0, value.find_first_not_of(" \t\""));
        value.erase(value.find_last_not_of(" \t\"") + 1);
        
        // Remove quotes from string values
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        
        // Set value in appropriate section
        if (currentSection == "brain") {
            if (key == "neuron_count") {
                try { pImpl->brainSection->setNeuronCount(std::stoll(value)); } catch (...) {}
            } else if (key == "region_count") {
                try { pImpl->brainSection->setRegionCount(std::stoll(value)); } catch (...) {}
            } else if (key == "connection_probability") {
                try { pImpl->brainSection->setConnectionProbability(std::stof(value)); } catch (...) {}
            } else if (key == "random_seed") {
                try { pImpl->brainSection->setRandomSeed(std::stoll(value)); } catch (...) {}
            } else if (key == "simulation_timestep") {
                try { pImpl->brainSection->setSimulationTimestep(std::stod(value)); } catch (...) {}
            } else {
                pImpl->brainSection->set(key, value);
            }
        } else if (currentSection == "experiment") {
            if (key == "type") {
                pImpl->experimentSection->setExperimentType(value);
            } else if (key == "steps") {
                try { pImpl->experimentSection->setExperimentSteps(std::stoi(value)); } catch (...) {}
            } else if (key == "reward") {
                try { pImpl->experimentSection->setExperimentReward(std::stof(value)); } catch (...) {}
            } else {
                pImpl->experimentSection->set(key, value);
            }
        }
    }
    
    return true;
}

ConfigurationValidationResult ConfigurationManager::validate() const {
    ConfigurationValidationResult result;
    result.valid = true;
    
    // Validate brain configuration
    auto brainValidation = pImpl->brainSection->validate();
    if (!brainValidation.valid) {
        result.valid = false;
        result.errorMessage += "Brain configuration error: " + brainValidation.errorMessage + " ";
    }
    
    // Validate experiment configuration
    auto experimentValidation = pImpl->experimentSection->validate();
    if (!experimentValidation.valid) {
        result.valid = false;
        result.errorMessage += "Experiment configuration error: " + experimentValidation.errorMessage + " ";
    }
    
    // Add warnings
    result.warnings.insert(result.warnings.end(), brainValidation.warnings.begin(), brainValidation.warnings.end());
    result.warnings.insert(result.warnings.end(), experimentValidation.warnings.begin(), experimentValidation.warnings.end());
    
    return result;
}

void ConfigurationManager::reset() {
    pImpl->brainSection = std::make_shared<BrainConfigurationSection>();
    pImpl->experimentSection = std::make_shared<ExperimentConfigurationSection>();
}

void ConfigurationManager::applyTo(std::shared_ptr<Config> config) const {
    if (!config) {
        return;
    }
    
    // Apply brain configuration
    config->set("neuron_count", static_cast<int64_t>(pImpl->brainSection->getNeuronCount()), ConfigSource::Config);
    config->set("region_count", static_cast<int64_t>(pImpl->brainSection->getRegionCount()), ConfigSource::Config);
    config->set("connection_probability", pImpl->brainSection->getConnectionProbability(), ConfigSource::Config);
    config->set("random_seed", static_cast<int64_t>(pImpl->brainSection->getRandomSeed()), ConfigSource::Config);
    config->set("simulation_timestep", pImpl->brainSection->getSimulationTimestep(), ConfigSource::Config);
    
    // Apply plasticity configuration
    config->set("stdp_ltp_weight", pImpl->brainSection->getSTDPWeightLTP(), ConfigSource::Config);
    config->set("stdp_ltd_weight", pImpl->brainSection->getSTDPWeightLTD(), ConfigSource::Config);
    config->set("stdp_tau", pImpl->brainSection->getSTDPTimeConstant(), ConfigSource::Config);
    config->set("synaptogenesis_rate", pImpl->brainSection->getSynaptogenesisRate(), ConfigSource::Config);
    config->set("pruning_rate", pImpl->brainSection->getPruningRate(), ConfigSource::Config);
    
    // Apply neuromodulation configuration
    config->set("reward_modulation_enabled", pImpl->brainSection->isRewardModulationEnabled(), ConfigSource::Config);
    config->set("curiosity_enabled", pImpl->brainSection->isCuriosityEnabled(), ConfigSource::Config);
    config->set("novelty_enabled", pImpl->brainSection->isNoveltyEnabled(), ConfigSource::Config);
    
    // Apply development configuration
    config->set("development_enabled", pImpl->brainSection->isDevelopmentEnabled(), ConfigSource::Config);
}

std::string ConfigurationManager::toString() const {
    std::ostringstream oss;
    oss << pImpl->brainSection->toString();
    oss << pImpl->experimentSection->toString();
    return oss.str();
}

bool ConfigurationManager::fromString(const std::string& configString) {
    std::istringstream stream(configString);
    return loadFromStream(stream);
}

bool ConfigurationManager::isValidLogLevel(const std::string& level) {
    std::unordered_set<std::string> validLevels = {"debug", "info", "warning", "error", "critical"};
    return validLevels.find(level) != validLevels.end();
}

bool ConfigurationManager::isValidExperimentType(const std::string& type) {
    std::unordered_set<std::string> validTypes = {"learning", "benchmark", "debug", "visualization"};
    return validTypes.find(type) != validTypes.end();
}

bool ConfigurationManager::isValidNeuronCount(int64_t count) {
    return count >= 10 && count <= 10000;
}

bool ConfigurationManager::isValidRegionCount(int64_t count) {
    return count >= 1 && count <= 100;
}

bool ConfigurationManager::isValidConnectionProbability(float prob) {
    return prob >= 0.0f && prob <= 1.0f;
}

std::string ConfigurationManager::getBrainConfigHelp() {
    return R"(
Brain Configuration Options:
  neuron_count              Number of neurons (10-10000, default: 500)
  region_count              Number of neural regions (1-100, default: 1)
  connection_probability    Connection probability (0.0-1.0, default: 0.15)
  random_seed               Random seed for reproducible results (default: 42)
  simulation_timestep       Simulation timestep in seconds (0.0001s-0.1s, default: 0.001)
  plasticity.hebbian.enable Enable Hebbian plasticity (default: true)
  plasticity.stdp.enable     Enable STDP plasticity (default: true)
  plasticity.structural.enable Enable structural plasticity (default: true)
  plasticity.stdp.ltp_weight  STDP LTP weight (default: 0.02)
  plasticity.stdp.ltd_weight  STDP LTD weight (default: 0.015)
  plasticity.stdp.tau        STDP time constant (default: 20.0)
  plasticity.structural.synaptogenesis_rate Synaptogenesis rate (default: 0.0001)
  plasticity.structural.pruning_rate   Pruning rate (default: 0.00001)
  neuromodulation.reward.enable Enable reward modulation (default: true)
  neuromodulation.curiosity.enable Enable curiosity (default: true)
  neuromodulation.novelty.enable Enable novelty detection (default: true)
  development.enable        Enable development (default: true)
  agent.sensory.vision.width Vision width (default: 16)
  agent.sensory.vision.height Vision height (default: 16)
  agent.sensory.touch.count   Touch sensor count (default: 8)
)";
}

std::string ConfigurationManager::getExperimentConfigHelp() {
    return R"(
Experiment Configuration Options:
  type                      Experiment type (learning, benchmark, debug, visualization, default: learning)
  steps                     Number of simulation steps (1-1000000, default: 1000)
  reward                    Reward multiplier (-100.0-100.0, default: 1.0)
  metrics.enable            Enable metrics collection (default: true)
  benchmark.enable          Enable benchmark mode (default: false)
  benchmark.iterations      Number of benchmark iterations (default: 100)
  debug.enable              Enable debug mode (default: false)
  log.level                 Log level (debug, info, warning, error, critical, default: info)
  log.file                  Log file path (default: nlm_simulation.log)
)";
}

std::unordered_map<std::string, std::string> ConfigurationManager::parseCommandLineArgs(int argc, char** argv) {
    std::unordered_map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args["help"] = "";
        } else if (arg == "--version") {
            args["version"] = "";
        } else if (arg.find("--config") == 0) {
            if (arg.find("=") != std::string::npos) {
                args["config"] = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                args["config"] = argv[++i];
            }
        } else if (arg.find("--neuron-count") == 0) {
            if (arg.find("=") != std::string::npos) {
                args["neuron_count"] = arg.substr(arg.find("=") + 1);
            }
        } else if (arg.find("--region-count") == 0) {
            if (arg.find("=") != std::string::npos) {
                args["region_count"] = arg.substr(arg.find("=") + 1);
            }
        } else if (arg.find("--simulation-timestep") == 0) {
            if (arg.find("=") != std::string::npos) {
                args["simulation_timestep"] = arg.substr(arg.find("=") + 1);
            }
        } else if (arg.find("--random-seed") == 0) {
            if (arg.find("=") != std::string::npos) {
                args["random_seed"] = arg.substr(arg.find("=") + 1);
            }
        } else if (arg.find("--plasticity-enable") == 0) {
            args["plasticity_enable"] = "true";
        } else if (arg.find("--plasticity-disable") == 0) {
            args["plasticity_enable"] = "false";
        } else if (arg.find("--stdp-enable") == 0) {
            args["stdp_enable"] = "true";
        } else if (arg.find("--stdp-disable") == 0) {
            args["stdp_enable"] = "false";
        } else if (arg.find("--hebbian-enable") == 0) {
            args["hebbian_enable"] = "true";
        } else if (arg.find("--hebbian-disable") == 0) {
            args["hebbian_enable"] = "false";
        } else if (arg.find("--structural-enable") == 0) {
            args["structural_enable"] = "true";
        } else if (arg.find("--structural-disable") == 0) {
            args["structural_enable"] = "false";
        } else if (arg.find("--reward-modulation-enable") == 0) {
            args["reward_modulation_enable"] = "true";
        } else if (arg.find("--reward-modulation-disable") == 0) {
            args["reward_modulation_enable"] = "false";
        } else if (arg.find("--curiosity-enable") == 0) {
            args["curiosity_enable"] = "true";
        } else if (arg.find("--curiosity-disable") == 0) {
            args["curiosity_enable"] = "false";
        } else if (arg.find("--novelty-enable") == 0) {
            args["novelty_enable"] = "true";
        } else if (arg.find("--novelty-disable") == 0) {
            args["novelty_enable"] = "false";
        } else if (arg.find("--development-enable") == 0) {
            args["development_enable"] = "true";
        } else if (arg.find("--development-disable") == 0) {
            args["development_enable"] = "false";
        } else if (arg.find("--experiment-mode") == 0) {
            args["experiment_mode"] = "true";
        } else if (arg.find("--benchmark-mode") == 0) {
            args["benchmark_mode"] = "true";
        } else if (arg.find("--profile") == 0) {
            args["profile"] = "true";
        } else if (arg.find("--verbose") == 0 || arg.find("-v") == 0) {
            args["verbose"] = "true";
        } else if (arg.find("--log-file") == 0) {
            if (arg.find("=") != std::string::npos) {
                args["log_file"] = arg.substr(arg.find("=") + 1);
            }
        } else if (arg.find("--log-level") == 0) {
            if (arg.find("=") != std::string::npos) {
                args["log_level"] = arg.substr(arg.find("=") + 1);
            }
        }
    }
    
    return args;
}

std::string ConfigurationManager::getCommandLineHelp() {
    return R"(
Command Line Options:
  --help, -h              Show help message
  --version               Show version information
  --config=FILE, --config FILE  Load configuration from file
  --neuron-count=COUNT    Set number of neurons
  --region-count=COUNT    Set number of neural regions
  --simulation-timestep=DT Set simulation timestep in seconds
  --random-seed=SEED      Set random seed for reproducible results
  --plasticity-enable      Enable plasticity
  --plasticity-disable     Disable plasticity
  --stdp-enable           Enable STDP plasticity
  --stdp-disable          Disable STDP plasticity
  --hebbian-enable        Enable Hebbian plasticity
  --hebbian-disable       Disable Hebbian plasticity
  --structural-enable     Enable structural plasticity
  --structural-disable    Disable structural plasticity
  --reward-modulation-enable Enable reward modulation
  --reward-modulation-disable   Disable reward modulation
  --curiosity-enable      Enable curiosity-based exploration
  --curiosity-disable     Disable curiosity-based exploration
  --novelty-enable        Enable novelty detection
  --novelty-disable       Disable novelty detection
  --development-enable    Enable developmental stages
  --development-disable   Disable developmental stages
  --experiment-mode       Enable experiment mode
  --benchmark-mode       Run benchmark tests
  --profile              Enable performance profiling
  --verbose, -v          Enable verbose logging
  --log-file=FILE        Write logs to file
  --log-level=LEVEL      Set log level (debug, info, warning, error)
)";
}

// Convenience function implementations
BrainConfigurationSection& getBrainConfiguration() {
    return ConfigurationManager::getGlobal().getBrainConfig();
}

ExperimentConfigurationSection& getExperimentConfiguration() {
    return ConfigurationManager::getGlobal().getExperimentConfig();
}

void initializeConfiguration() {
    // Initialize default configuration
    ConfigurationManager::getGlobal();
}

void cleanupConfiguration() {
    // Cleanup configuration
    // Note: This is a simplified implementation
}

} // namespace nlm
