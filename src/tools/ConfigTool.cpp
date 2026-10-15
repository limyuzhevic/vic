#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <ctime>

// NLM core includes
#include "core/Config/Config.hpp"

using namespace nlm;

class ConfigTool {
public:
    void showConfig(const std::string& configFile = "") {
        std::cout << "=== NLM Configuration Tool ===\n\n";
        
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            std::cout << "Loading configuration from: " << configFile << std::endl;
            if (!config->loadFromFile(configFile)) {
                std::cerr << "Error: Could not load configuration file!" << std::endl;
                return;
            }
        } else {
            std::cout << "Using default configuration." << std::endl;
        }
        
        std::cout << "\nCurrent Configuration:" << std::endl;
        std::cout << config->summary() << std::endl;
        
        std::cout << "\nConfiguration Statistics:" << std::endl;
        std::cout << "  Total entries: " << config->getKeys().size() << std::endl;
        
        // Show configuration by source
        std::cout << "\nConfiguration by Source:" << std::endl;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                std::cout << "  " << key << " = " << value.value() << std::endl;
            }
        }
        
        // Validate configuration
        std::cout << "\nConfiguration Validation:" << std::endl;
        if (!validateConfiguration(config)) {
            std::cout << "  Configuration is valid." << std::endl;
        } else {
            std::cout << "  Configuration has issues (see warnings above)." << std::endl;
        }
    }
    
    void setConfigValue(const std::string& key, const std::string& value, const std::string& configFile = "") {
        std::cout << "=== Setting Configuration Value ===\n\n";
        
        auto config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        // Determine value type based on key or content
        if (value == "true" || value == "false") {
            config->set(key, value == "true", ConfigSource::Runtime);
        } else {
            // Try to parse as number
            try {
                size_t pos;
                double doubleVal = std::stod(value, &pos);
                if (pos == value.size()) {
                    config->set(key, doubleVal, ConfigSource::Runtime);
                } else {
                    config->set(key, value, ConfigSource::Runtime);
                }
            } catch (...) {
                config->set(key, value, ConfigSource::Runtime);
            }
        }
        
        std::cout << "Set " << key << " = " << value << std::endl;
        
        // Save to file if specified
        if (!configFile.empty()) {
            if (config->saveToFile(configFile)) {
                std::cout << "Saved configuration to: " << configFile << std::endl;
            } else {
                std::cerr << "Error: Could not save configuration to file!" << std::endl;
            }
        }
    }
    
    void createTemplateConfig(const std::string& configFile = "configs/default.cfg") {
        std::cout << "=== Creating Default Configuration Template ===\n\n";
        
        auto config = std::make_shared<Config>();
        
        // Set some sensible defaults for NLM Phase 6
        config->set("brain.neuron_count", 1000, ConfigSource::Default, "Total number of neurons in the brain");
        config->set("brain.synapse_density", 0.1, ConfigSource::Default, "Synapse connectivity density (0.0-1.0)");
        config->set("brain.connection_probability", 0.05, ConfigSource::Default, "Probability of connection between neurons");
        config->set("brain.v_thresh", -50.0, ConfigSource::Default, "Neuron firing threshold (mV)");
        config->set("brain.v_rest", -70.0, ConfigSource::Default, "Resting potential (mV)");
        config->set("brain.v_reset", -75.0, ConfigSource::Default, "Reset potential after spike (mV)");
        config->set("brain.tau_mem", 20.0, ConfigSource::Default, "Membrane time constant (ms)");
        config->set("brain.tau_ref", 2.0, ConfigSource::Default, "Refractory period (ms)");
        
        // Plasticity configuration
        config->set("plasticity.stdp.enable", true, ConfigSource::Default, "Enable Spike-Timing Dependent Plasticity");
        config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default, "STDP learning rate");
        config->set("plasticity.stdp.tau_plus", 20.0, ConfigSource::Default, "STDP potentiation time constant (ms)");
        config->set("plasticity.stdp.tau_minus", 20.0, ConfigSource::Default, "STDP depression time constant (ms)");
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default, "Enable Hebbian plasticity");
        config->set("plasticity.structural.enable", true, ConfigSource::Default, "Enable structural plasticity (synaptogenesis/pruning)");
        
        // Neuromodulation configuration
        config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default, "Dopamine modulation scale");
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default, "Enable curiosity-driven exploration");
        config->set("neuromod.novelty.enable", true, ConfigSource::Default, "Enable novelty detection");
        
        // Development configuration
        config->set("development.initial_plasticity", 1.0, ConfigSource::Default, "Initial plasticity factor");
        config->set("development.critical_period_length", 300.0, ConfigSource::Default, "Critical period duration (simulation steps)");
        
        // Simulation configuration
        config->set("simulation.timestep", 0.001, ConfigSource::Default, "Simulation timestep (s)");
        config->set("simulation.max_steps", 10000, ConfigSource::Default, "Maximum simulation steps");
        config->set("random_seed", 42, ConfigSource::Default, "Random seed for reproducibility");
        
        // World configuration
        config->set("world.width", 20.0, ConfigSource::Default, "World width");
        config->set("world.height", 20.0, ConfigSource::Default, "World height");
        config->set("world.vision_width", 8, ConfigSource::Default, "Agent vision width");
        config->set("world.vision_height", 8, ConfigSource::Default, "Agent vision height");
        
        // Save configuration
        if (config->saveToFile(configFile)) {
            std::cout << "Created configuration template: " << configFile << std::endl;
            std::cout << "\nConfiguration includes settings for:" << std::endl;
            std::cout << "  - Brain parameters (neurons, synapses, membrane properties)" << std::endl;
            std::cout << "  - Plasticity rules (STDP, Hebbian, structural)" << std::endl;
            std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
            std::cout << "  - Development stages" << std::endl;
            std::cout << "  - Simulation settings" << std::endl;
            std::cout << "  - World parameters" << std::endl;
        } else {
            std::cerr << "Error: Could not create configuration file!" << std::endl;
        }
    }
    
private:
    bool validateConfiguration(const std::shared_ptr<Config>& config) {
        // Basic validation - check for required parameters
        std::vector<std::string> requiredParams = {
            "brain.neuron_count",
            "brain.connection_probability",
            "brain.v_thresh",
            "brain.v_rest"
        };
        
        bool valid = true;
        for (const auto& param : requiredParams) {
            if (!config->has(param)) {
                std::cerr << "Warning: Missing required parameter: " << param << std::endl;
                valid = false;
            }
        }
        
        // Check parameter ranges
        auto neuronCount = config->getOr<int64_t>("brain.neuron_count", 1000);
        if (neuronCount <= 0) {
            std::cerr << "Warning: brain.neuron_count must be positive (got " << neuronCount << ")" << std::endl;
            valid = false;
        }
        
        auto connProb = config->getOr<double>("brain.connection_probability", 0.1);
        if (connProb < 0.0 || connProb > 1.0) {
            std::cerr << "Warning: brain.connection_probability must be between 0 and 1 (got " << connProb << ")" << std::endl;
            valid = false;
        }
        
        auto threshold = config->getOr<double>("brain.v_thresh", -50.0);
        if (threshold > 0) {
            std::cerr << "Warning: brain.v_thresh should be negative (got " << threshold << ")" << std::endl;
            valid = false;
        }
        
        return valid;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: nlm_config [command] [options]" << std::endl;
        std::cout << "\nCommands:" << std::endl;
        std::cout << "  show [file]      Show current configuration" << std::endl;
        std::cout << "  set <key> <value> Set configuration value" << std::endl;
        std::cout << "  template [file]   Create default configuration template" << std::endl;
        std::cout << "  help             Show this help message" << std::endl;
        return 0;
    }
    
    std::string command = argv[1];
    ConfigTool tool;
    
    if (command == "show") {
        std::string configFile = (argc > 2) ? argv[2] : "";
        tool.showConfig(configFile);
    } else if (command == "set") {
        if (argc < 4) {
            std::cerr << "Error: Missing arguments for set command" << std::endl;
            return 1;
        }
        std::string key = argv[2];
        std::string value = argv[3];
        std::string configFile = (argc > 4) ? argv[4] : "";
        tool.setConfigValue(key, value, configFile);
    } else if (command == "template") {
        std::string configFile = (argc > 2) ? argv[2] : "configs/default.cfg";
        tool.createTemplateConfig(configFile);
    } else if (command == "help") {
        std::cout << "Usage: nlm_config [command] [options]" << std::endl;
        std::cout << "\nCommands:" << std::endl;
        std::cout << "  show [file]      Show current configuration" << std::endl;
        std::cout << "  set <key> <value> Set configuration value" << std::endl;
        std::cout << "  template [file]   Create default configuration template" << std::endl;
        std::cout << "  help             Show this help message" << std::endl;
    } else {
        std::cerr << "Error: Unknown command: " << command << std::endl;
        return 1;
    }
    
    return 0;
}