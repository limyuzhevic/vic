#include "ExperimentConfig.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace nlm {

struct ExperimentConfig::Impl {
    std::string filename;
    std::vector<std::string> validationErrors;
    
    Impl() = default;
    explicit Impl(const std::string& filename) : filename(filename) {}
};

ExperimentConfig::ExperimentConfig() {
    initializeDefaults();
    pImpl = std::make_unique<Impl>("");
}

ExperimentConfig::ExperimentConfig(const std::string& filename) {
    initializeDefaults();
    pImpl = std::make_unique<Impl>(filename);
    if (!filename.empty()) {
        loadFromFile(filename);
    }
}

ExperimentConfig::~ExperimentConfig() = default;

void ExperimentConfig::initializeDefaults() {
    // Set default values from the settings structure
    settings.environment_name = "default";
    settings.environment_width = 20.0f;
    settings.environment_height = 20.0f;
    
    settings.brain_neuron_count = 500;
    settings.brain_region_count = 2;
    settings.brain_connection_probability = 0.1f;
    settings.brain_random_seed = 42u;
    settings.brain_simulation_timestep = 0.001f;
    settings.brain_stdp_ltp_weight = 0.01f;
    settings.brain_stdp_ltd_weight = 0.012f;
    settings.brain_stdp_tau = 20.0f;
    settings.brain_synaptogenesis_rate = 0.0001f;
    settings.brain_pruning_rate = 0.00001f;
    
    settings.experiment_num_episodes = 5;
    settings.experiment_steps_per_episode = 200;
    settings.experiment_num_trials = 50;
    settings.experiment_name = "unnamed_experiment";
    settings.experiment_description = "";
    
    settings.log_filename = "experiment.log";
    settings.log_to_console = true;
    settings.log_detailed = false;
    
    settings.output_directory = "output";
    settings.output_format = "csv";
    settings.save_metrics = true;
    settings.save_brain_state = false;
    
    settings.performance_threshold = 0.8f;
    settings.max_steps = 10000;
    settings.timeout_seconds = 300.0f;
}

bool ExperimentConfig::loadFromFile(const std::string& filename) {
    pImpl->filename = filename;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        pImpl->validationErrors.push_back("Cannot open config file: " + filename);
        return false;
    }
    
    // Simple key-value parsing
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\n\r"));
            key.erase(key.find_last_not_of(" \t\n\r") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r"));
            value.erase(value.find_last_not_of(" \t\n\r") + 1);
            
            set(key, value);
        }
    }
    
    file.close();
    pImpl->validationErrors.clear();
    return true;
}

bool ExperimentConfig::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Save as key=value format
    file << "# Experiment Configuration\n";
    file << "# Generated: " << std::endl;
    
    file << "# Environment\n";
    file << "environment_name=" << settings.environment_name << "\n";
    file << "environment_width=" << settings.environment_width << "\n";
    file << "environment_height=" << settings.environment_height << "\n\n";
    
    file << "# Brain\n";
    file << "brain_neuron_count=" << settings.brain_neuron_count << "\n";
    file << "brain_region_count=" << settings.brain_region_count << "\n";
    file << "brain_connection_probability=" << settings.brain_connection_probability << "\n";
    file << "brain_random_seed=" << settings.brain_random_seed << "\n";
    file << "brain_simulation_timestep=" << settings.brain_simulation_timestep << "\n";
    file << "brain_stdp_ltp_weight=" << settings.brain_stdp_ltp_weight << "\n";
    file << "brain_stdp_ltd_weight=" << settings.brain_stdp_ltd_weight << "\n";
    file << "brain_stdp_tau=" << settings.brain_stdp_tau << "\n";
    file << "brain_synaptogenesis_rate=" << settings.brain_synaptogenesis_rate << "\n";
    file << "brain_pruning_rate=" << settings.brain_pruning_rate << "\n\n";
    
    file << "# Experiment\n";
    file << "experiment_num_episodes=" << settings.experiment_num_episodes << "\n";
    file << "experiment_steps_per_episode=" << settings.experiment_steps_per_episode << "\n";
    file << "experiment_num_trials=" << settings.experiment_num_trials << "\n";
    file << "experiment_name=" << settings.experiment_name << "\n";
    file << "experiment_description=" << settings.experiment_description << "\n\n";
    
    file.close();
    return true;
}

template<typename T>
T ExperimentConfig::get(const std::string& key) const {
    // Implementation would use Config class if available
    // For now, return defaults
    return T();
}

template<typename T>
void ExperimentConfig::set(const std::string& key, const T& value) {
    // Implementation would use Config class if available
    // For now, do nothing
}

std::string ExperimentConfig::getString(const std::string& key) const {
    return settings.environment_name; // Placeholder
}

int ExperimentConfig::getInt(const std::string& key) const {
    return 0; // Placeholder
}

float ExperimentConfig::getFloat(const std::string& key) const {
    return 0.0f; // Placeholder
}

bool ExperimentConfig::getBool(const std::string& key) const {
    return false; // Placeholder
}

uint64_t ExperimentConfig::getUint64(const std::string& key) const {
    return 0; // Placeholder
}

bool ExperimentConfig::validate() const {
    pImpl->validationErrors.clear();
    return validateInternal();
}

std::vector<std::string> ExperimentConfig::getValidationErrors() const {
    return pImpl->validationErrors;
}

bool ExperimentConfig::validateInternal() const {
    bool valid = true;
    
    if (settings.environment_width <= 0.0f) {
        pImpl->validationErrors.push_back("Environment width must be positive");
        valid = false;
    }
    
    if (settings.environment_height <= 0.0f) {
        pImpl->validationErrors.push_back("Environment height must be positive");
        valid = false;
    }
    
    if (settings.brain_neuron_count <= 0) {
        pImpl->validationErrors.push_back("Brain neuron count must be positive");
        valid = false;
    }
    
    if (settings.brain_random_seed == 0) {
        pImpl->validationErrors.push_back("Brain random seed cannot be zero");
        valid = false;
    }
    
    return valid;
}

std::shared_ptr<Config> ExperimentConfig::createDefaultEnvironmentConfig() {
    auto config = std::make_shared<Config>();
    config->set("environment_width", 20);
    config->set("environment_height", 20);
    config->set("vision_width", 16);
    config->set("vision_height", 16);
    return config;
}

std::shared_ptr<Config> ExperimentConfig::createDefaultBrainConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(500));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    return config;
}

std::shared_ptr<Config> ExperimentConfig::createMinimalConfig() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(100));
    config->set("random_seed", uint64_t(42));
    return config;
}

std::shared_ptr<ExperimentConfig> ExperimentConfig::createPhase3Config(int episodes, int steps) {
    auto config = std::make_shared<ExperimentConfig>();
    config->settings.experiment_num_episodes = episodes;
    config->settings.experiment_steps_per_episode = steps;
    config->settings.experiment_name = "Phase3Experiment";
    config->settings.experiment_description = "Phase 3: World Interaction Demo";
    return config;
}

std::shared_ptr<ExperimentConfig> ExperimentConfig::createPhase4Config(size_t trials) {
    auto config = std::make_shared<ExperimentConfig>();
    config->settings.experiment_num_trials = trials;
    config->settings.experiment_name = "Phase4Experiment";
    config->settings.experiment_description = "Phase 4: Emerging Cognition";
    return config;
}

std::shared_ptr<ExperimentConfig> ExperimentConfig::createAblationConfig(const std::string& mechanism) {
    auto config = std::make_shared<ExperimentConfig>();
    config->settings.experiment_name = "AblationExperiment";
    config->settings.experiment_description = "Ablation study for " + mechanism;
    config->setConfigValue("ablation_mechanism", mechanism);
    return config;
}

} // namespace nlm
