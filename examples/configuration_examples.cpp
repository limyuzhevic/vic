// Configuration examples
#include "src/core/Config/Config.hpp"
#include <memory>
#include <iostream>
#include <fstream>

void saveConfig(const std::string& filename, const std::shared_ptr<nlm::Config>& config) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "# NLM Configuration Example" << std::endl;
        file << std::endl;
        file << "# Core configuration" << std::endl;
        file << "neuron_count = " << config->getInt("neuron_count") << std::endl;
        file << "region_count = " << config->getInt("region_count") << std::endl;
        file << "connection_probability = " << config->getFloat("connection_probability") << std::endl;
        file << "simulation_timestep = " << config->getFloat("simulation_timestep") << std::endl;
        file << std::endl;
        file << "# Plasticity configuration" << std::endl;
        file << "plasticity_learning_rate = " << config->getFloat("plasticity_learning_rate") << std::endl;
        file << "stdp_ltp_weight = " << config->getFloat("stdp_ltp_weight") << std::endl;
        file << "stdp_ltd_weight = " << config->getFloat("stdp_ltd_weight") << std::endl;
        file << "stdp_time_constant = " << config->getFloat("stdp_time_constant") << std::endl;
        file << std::endl;
        file << "# Development configuration" << std::endl;
        file << "development_synaptogenesis_rate = " << config->getFloat("development_synaptogenesis_rate") << std::endl;
        file << "development_pruning_rate = " << config->getFloat("development_pruning_rate") << std::endl;
        file << std::endl;
        file << "# Neuromodulation configuration" << std::endl;
        file << "dopamine_baseline = " << config->getFloat("dopamine_baseline") << std::endl;
        file << "curiosity_rate = " << config->getFloat("curiosity_rate") << std::endl;
        file << std::endl;
        file << "# Memory configuration" << std::endl;
        file << "working_memory_capacity = " << config->getInt("working_memory_capacity") << std::endl;
        file << "episodic_memory_max_episodes = " << config->getInt("episodic_memory_max_episodes") << std::endl;
        file << std::endl;
        file << "# Environment configuration" << std::endl;
        file << "environment_name = " << config->getString("environment_name") << std::endl;
        file << "environment_width = " << config->getInt("environment_width") << std::endl;
        file << "environment_height = " << config->getInt("environment_height") << std::endl;
        file << "reward_discount_factor = " << config->getFloat("reward_discount_factor") << std::endl;
        file << std::endl;
        file << "# Logging configuration" << std::endl;
        file << "log_level = " << config->getString("log_level") << std::endl;
        file << "log_to_file = " << (config->getBool("log_to_file") ? "true" : "false") << std::endl;
        file << "log_filename = " << config->getString("log_filename") << std::endl;
        file << std::endl;
        file << "# Performance configuration" << std::endl;
        file << "max_simulation_steps = " << config->getInt("max_simulation_steps") << std::endl;
        file << "simulation_time_limit = " << config->getFloat("simulation_time_limit") << std::endl;
        file << "visualization_enabled = " << (config->getBool("visualization_enabled") ? "true" : "false") << std::endl;
        file << "visualization_update_rate = " << config->getFloat("visualization_update_rate") << std::endl;
        
        file.close();
        std::cout << "Configuration saved to " << filename << std::endl;
    } else {
        std::cerr << "Failed to save configuration to " << filename << std::endl;
    }
}

void loadConfig(const std::string& filename, std::shared_ptr<nlm::Config>& config) {
    if (config->loadFromFile(filename)) {
        std::cout << "Configuration loaded from " << filename << std::endl;
        std::cout << config->summary() << std::endl;
    } else {
        std::cerr << "Failed to load configuration from " << filename << std::endl;
    }
}

int main() {
    std::cout << "Configuration examples" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // Create default configuration
    auto config = std::make_shared<nlm::Config>();
    
    std::cout << "Default configuration:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Save configuration to file
    saveConfig("basic_config.cfg", config);
    
    // Load configuration from file
    loadConfig("basic_config.cfg", config);
    
    // Modify configuration
    config->set("neuron_count", 2000);
    config->set("region_count", 3);
    config->set("connection_probability", 0.2);
    
    std::cout << "Modified configuration:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Save modified configuration
    saveConfig("advanced_config.cfg", config);
    
    // Load modified configuration
    loadConfig("advanced_config.cfg", config);
    
    std::cout << "Configuration examples completed successfully!" << std::endl;
    return 0;
}