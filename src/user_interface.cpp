#include <string>
#include <vector>
#include <iostream>

using namespace nlm;

// Simple user interface commands
class UserInterface {
public:
    static void printHelp() {
        std::cout << "=== NLM User Interface Commands ===" << std::endl;
        std::cout << "help                    - Show this help message" << std::endl;
        std::cout << "brain status           - Show current brain status" << std::endl;
        std::cout << "brain reset            - Reset the brain to initial state" << std::endl;
        std::cout << "brain initialize       - Initialize the brain" << std::endl;
        std::cout << "brain stats            - Show detailed brain statistics" << std::endl;
        std::cout << "config show            - Show current configuration" << std::endl;
        std::cout << "config set <key> <value> - Set configuration value" << std::endl;
        std::cout << "simulate <steps>       - Run simulation for specified steps" << std::endl;
        std::cout << "experiment <name>      - Run specific experiment" << std::endl;
        std::cout << "quit                   - Exit the program" << std::endl;
        std::cout << "" << std::endl;
    }
    
    static void processCommand(const std::string& command, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        std::vector<std::string> tokens = splitString(command);
        if (tokens.empty()) return;
        
        std::string cmd = tokens[0];
        
        if (cmd == "help") {
            printHelp();
        } else if (cmd == "brain") {
            processBrainCommand(tokens, brain);
        } else if (cmd == "config") {
            processConfigCommand(tokens, config);
        } else if (cmd == "simulate") {
            processSimulateCommand(tokens, brain);
        } else if (cmd == "experiment") {
            processExperimentCommand(tokens, brain);
        } else if (cmd == "quit") {
            std::cout << "Exiting NLM..." << std::endl;
            exit(0);
        } else {
            std::cout << "Unknown command: " << cmd << ". Type 'help' for available commands." << std::endl;
        }
    }
    
private:
    static std::vector<std::string> splitString(const std::string& str) {
        std::vector<std::string> tokens;
        std::string token;
        for (char c : str) {
            if (c == ' ') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    static void processBrainCommand(const std::vector<std::string>& tokens, std::shared_ptr<Brain> brain) {
        if (tokens.size() < 2) {
            std::cout << "Brain command requires subcommand. Available: status, reset, initialize, stats" << std::endl;
            return;
        }
        
        std::string subcommand = tokens[1];
        
        if (subcommand == "status") {
            if (brain) {
                brain->logStatus();
            } else {
                std::cout << "Brain not initialized" << std::endl;
            }
        } else if (subcommand == "reset") {
            if (brain) {
                brain->reset();
                std::cout << "Brain reset completed" << std::endl;
            } else {
                std::cout << "Brain not initialized" << std::endl;
            }
        } else if (subcommand == "initialize") {
            if (brain) {
                if (brain->initialize()) {
                    std::cout << "Brain initialization completed" << std::endl;
                } else {
                    std::cout << "Brain initialization failed" << std::endl;
                }
            } else {
                std::cout << "Brain not created" << std::endl;
            }
        } else if (subcommand == "stats") {
            if (brain) {
                std::cout << "=== Brain Statistics ===" << std::endl;
                std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
                std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
                std::cout << "Regions: " << brain->getRegionCount() << std::endl;
                std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
                std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
                std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
                std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
                std::cout << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            } else {
                std::cout << "Brain not initialized" << std::endl;
            }
        } else {
            std::cout << "Unknown brain subcommand: " << subcommand << std::endl;
        }
    }
    
    static void processConfigCommand(const std::vector<std::string>& tokens, std::shared_ptr<Config> config) {
        if (tokens.size() < 2) {
            std::cout << "Config command requires subcommand. Available: show, set" << std::endl;
            return;
        }
        
        std::string subcommand = tokens[1];
        
        if (subcommand == "show") {
            std::cout << "=== Current Configuration ===" << std::endl;
            // Show all config values
            std::cout << "random_seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
            std::cout << "simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.0) << std::endl;
            std::cout << "neuron_count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
            std::cout << "region_count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
            std::cout << "connection_probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
            std::cout << "stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.0f) << std::endl;
            std::cout << "stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.0f) << std::endl;
            std::cout << "synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0f) << std::endl;
            std::cout << "pruning_rate: " << config->getOr<float>("pruning_rate", 0.0f) << std::endl;
        } else if (subcommand == "set") {
            if (tokens.size() < 4) {
                std::cout << "Config set requires key and value: config set <key> <value>" << std::endl;
                return;
            }
            
            std::string key = tokens[2];
            std::string value = tokens[3];
            
            try {
                if (key == "random_seed") {
                    config->set(key, static_cast<int64_t>(std::stoi(value)), ConfigSource::User);
                    std::cout << "Set " << key << " = " << value << std::endl;
                } else if (key == "simulation_timestep" || key == "stdp_tau") {
                    config->set(key, std::stod(value), ConfigSource::User);
                    std::cout << "Set " << key << " = " << value << std::endl;
                } else if (key == "neuron_count" || key == "region_count") {
                    config->set(key, static_cast<int64_t>(std::stoi(value)), ConfigSource::User);
                    std::cout << "Set " << key << " = " << value << std::endl;
                } else if (key == "connection_probability" || key == "stdp_ltp_weight" || 
                           key == "stdp_ltd_weight" || key == "synaptogenesis_rate" || 
                           key == "pruning_rate") {
                    config->set(key, std::stof(value), ConfigSource::User);
                    std::cout << "Set " << key << " = " << value << std::endl;
                } else {
                    std::cout << "Unknown configuration key: " << key << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Error setting configuration: " << e.what() << std::endl;
            }
        } else {
            std::cout << "Unknown config subcommand: " << subcommand << std::endl;
        }
    }
    
    static void processSimulateCommand(const std::vector<std::string>& tokens, std::shared_ptr<Brain> brain) {
        if (tokens.size() < 2) {
            std::cout << "Simulate command requires number of steps: simulate <steps>" << std::endl;
            return;
        }
        
        try {
            int steps = std::stoi(tokens[1]);
            if (steps <= 0) {
                std::cout << "Steps must be positive" << std::endl;
                return;
            }
            
            std::cout << "Running simulation for " << steps << " steps..." << std::endl;
            
            for (int step = 0; step < steps; ++step) {
                brain->step(step);
                if (step % 100 == 0) {
                    std::cout << "Step " << step << ": " << brain->getTotalSpikeCount() 
                              << " total spikes, " << brain->getFiringNeuronCount() 
                              << " firing neurons" << std::endl;
                }
            }
            
            std::cout << "Simulation completed after " << steps << " steps" << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "Error during simulation: " << e.what() << std::endl;
        }
    }
    
    static void processExperimentCommand(const std::vector<std::string>& tokens, std::shared_ptr<Brain> brain) {
        if (tokens.size() < 2) {
            std::cout << "Experiment command requires experiment name: experiment <name>" << std::endl;
            return;
        }
        
        std::string experimentName = tokens[1];
        
        if (experimentName == "plasticity") {
            std::cout << "Running plasticity learning experiment..." << std::endl;
            // Run plasticity experiment
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
            
            for (SimulationStep step = 0; step < 100; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                brain->step(step);
                std::cout << "Step " << step << ": " << brain->getTotalSpikeCount() 
                          << " total spikes" << std::endl;
            }
            
            std::cout << "Plasticity experiment completed" << std::endl;
        } else if (experimentName == "stdp") {
            std::cout << "Running STDP verification experiment..." << std::endl;
            // Run STDP verification
            if (auto* region = brain->getRegion(RegionId(1))) {
                auto& synapses = region->getSynapses();
                if (synapses.size() >= 5) {
                    // Record initial weights
                    std::vector<float> beforeWeights;
                    for (size_t i = 0; i < 5; ++i) {
                        beforeWeights.push_back(synapses[i]->getWeight());
                        synapses[i]->enablePlasticity(false, true, false);
                    }
                    
                    // Create correlated activity
                    auto neurons = region->getAllNeurons();
                    if (neurons.size() >= 2) {
                        for (int trial = 0; trial < 10; ++trial) {
                            neurons[0]->injectCurrent(60.0f);
                            brain->step(trial * 2, trial * 2 * 0.001);
                            neurons[1]->injectCurrent(60.0f);
                            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                        }
                    }
                    
                    // Check for weight changes
                    float totalDelta = 0.0f;
                    for (size_t i = 0; i < 5; ++i) {
                        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
                    }
                    
                    if (totalDelta > 0.001f) {
                        std::cout << "✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
                    } else if (totalDelta < -0.001f) {
                        std::cout << "! STDP reversed: Check parameters" << std::endl;
                    } else {
                        std::cout << "! No change: STDP may not be triggering" << std::endl;
                    }
                }
            }
            
            std::cout << "STDP experiment completed" << std::endl;
        } else {
            std::cout << "Unknown experiment: " << experimentName << ". Available: plasticity, stdp" << std::endl;
        }
    }
};

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "NLM Interactive Mode" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl << std::endl;
    
    // Initialize NLM components
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create default configuration
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Create brain
    std::shared_ptr<Brain> brain;
    try {
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "Failed to initialize brain" << std::endl;
            return 1;
        }
        
        std::cout << "NLM brain initialized successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error creating brain: " << e.what() << std::endl;
        return 1;
    }
    
    // Interactive command loop
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        
        try {
            UserInterface::processCommand(line, brain, config);
        } catch (const std::exception& e) {
            std::cout << "Error processing command: " << e.what() << std::endl;
        }
    }
    
    return 0;
}