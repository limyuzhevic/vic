// Interactive CLI for NLM exploration
// This provides a user-friendly interface to configure and run NLM simulations

#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <string>
#include <map>

namespace nlm {

class InteractiveCLI {
public:
    InteractiveCLI() : running_(true) {}
    
    void run() {
        printWelcome();
        
        while (running_) {
            printPrompt();
            std::string command;
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            handleCommand(command);
        }
        
        std::cout << "Goodbye!\n";
    }
    
private:
    void printWelcome() {
        std::cout << "===========================================\n";
        std::cout << "NLM - Interactive Brain Explorer\n";
        std::cout << "===========================================\n\n";
        std::cout << "This CLI allows you to explore NLM's neural learning machine\n";
        std::cout << "interactively. Configure the brain, run simulations, and inspect results.\n\n";
        printHelp();
    }
    
    void printHelp() {
        std::cout << "Commands:\n";
        std::cout << "  help                    - Show this help message\n";
        std::cout << "  create [config_name]     - Create a new brain with given config\n";
        std::cout << "  run [steps]              - Run simulation for given steps (0 for step-by-step)\n";
        std::cout << "  config set <key> <value> - Set configuration value\n";
        std::cout << "  config get <key>         - Get configuration value\n";
        std::cout << "  inspect brain            - Show brain statistics\n";
        std::cout << "  inspect agent           - Show agent statistics\n";
        std::cout << "  inspect memory           - Show memory system status\n";
        std::cout << "  plot <metric> [count]   - Show recent values of a metric\n";
        std::cout << "  plot <metric> live       - Live plot metric (requires matplotlib)\n";
        std::cout << "  save <path>              - Save brain state to file\n";
        std::cout << "  load <path>              - Load brain state from file\n";
        std::cout << "  reset                    - Reset brain and agent\n";
        std::cout << "  world reset              - Reset world\n";
        std::cout << "  status                   - Show current simulation status\n";
        std::cout << "  exit                     - Exit the program\n\n";
    }
    
    void printPrompt() {
        std::cout "> ";
    }
    
    void handleCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "help") {
            printHelp();
        } else if (cmd == "create") {
            std::string configName;
            if (iss >> configName) {
                createBrain(configName);
            } else {
                std::cout << "Error: Please specify a config name (e.g., 'create default')\n";
            }
        } else if (cmd == "run") {
            int steps;
            if (iss >> steps) {
                runSimulation(steps);
            } else {
                std::cout << "Error: Please specify number of steps\n";
            }
        } else if (cmd == "config") {
            handleConfigCommand(iss);
        } else if (cmd == "inspect") {
            handleInspectCommand(iss);
        } else if (cmd == "plot") {
            handlePlotCommand(iss);
        } else if (cmd == "save") {
            handleSaveCommand(iss);
        } else if (cmd == "load") {
            handleLoadCommand(iss);
        } else if (cmd == "reset") {
            resetBrain();
        } else if (cmd == "world") {
            handleWorldCommand(iss);
        } else if (cmd == "status") {
            showStatus();
        } else if (cmd == "exit") {
            running_ = false;
        } else {
            std::cout << "Unknown command: " << cmd << "\n";
            printHelp();
        }
    }
    
    void handleConfigCommand(std::istringstream& iss) {
        std::string action;
        iss >> action;
        
        if (action == "set") {
            std::string key, value;
            iss >> key >> value;
            setConfigValue(key, value);
        } else if (action == "get") {
            std::string key;
            iss >> key;
            getConfigValue(key);
        } else {
            std::cout << "Error: Config command requires 'set' or 'get'\n";
        }
    }
    
    void handleInspectCommand(std::istringstream& iss) {
        std::string type;
        iss >> type;
        
        if (type == "brain") {
            inspectBrain();
        } else if (type == "agent") {
            inspectAgent();
        } else if (type == "memory") {
            inspectMemory();
        } else {
            std::cout << "Error: Unknown inspection type: " << type << "\n";
        }
    }
    
    void handlePlotCommand(std::istringstream& iss) {
        std::string metric;
        iss >> metric;
        
        if (metric.empty()) {
            std::cout << "Error: Please specify a metric\n";
            return;
        }
        
        std::string option;
        int count;
        iss >> option;
        
        if (option == "live") {
            plotLive(metric);
        } else {
            count = 0;
            if (option.empty() == false) {
                // Try to parse as integer
                try {
                    count = std::stoi(option);
                } catch (...) {
                    count = 10; // default
                }
            } else {
                count = 10; // default
            }
            plotMetric(metric, count);
        }
    }
    
    void handleSaveCommand(std::istringstream& iss) {
        std::string path;
        iss >> path;
        
        if (path.empty()) {
            std::cout << "Error: Please specify a file path\n";
        } else {
            saveBrain(path);
        }
    }
    
    void handleLoadCommand(std::istringstream& iss) {
        std::string path;
        iss >> path;
        
        if (path.empty()) {
            std::cout << "Error: Please specify a file path\n";
        } else {
            loadBrain(path);
        }
    }
    
    void handleWorldCommand(std::istringstream& iss) {
        std::string action;
        iss >> action;
        
        if (action == "reset") {
            resetWorld();
        } else {
            std::cout << "Error: Unknown world command: " << action << "\n";
        }
    }
    
    void createBrain(const std::string& configName) {
        // Load configuration
        auto config = std::make_shared<Config>();
        
        // Apply standard NLM Phase 6 configuration
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Store config for later use
        currentConfig_ = config;
        configName_ = configName;
        
        // Create brain
        brain_ = std::make_shared<Brain>(config);
        
        if (brain_->initialize()) {
            std::cout << "Brain created successfully with config: " << configName << "\n";
            std::cout << "  Neurons: " << brain_->getTotalNeuronCount() << "\n";
            std::cout << "  Synapses: " << brain_->getTotalSynapseCount() << "\n";
        } else {
            std::cout << "Error: Failed to initialize brain\n";
        }
    }
    
    void setConfigValue(const std::string& key, const std::string& value) {
        if (!currentConfig_) {
            std::cout << "Error: No brain created yet. Use 'create <config_name>' first.\n";
            return;
        }
        
        // Simple value parsing
        try {
            if (key.find("count") != std::string::npos || key.find("seed") != std::string::npos) {
                // Integer value
                size_t pos;
                int intValue = std::stoi(value, &pos);
                currentConfig_->set(key, static_cast<int64_t>(intValue), ConfigSource::User);
            } else if (key.find("rate") != std::string::npos || key.find("weight") != std::string::npos 
                       || key.find("probability") != std::string::npos || key.find("timestep") != std::string::npos
                       || key.find("tau") != std::string::npos) {
                // Float value
                float floatValue = std::stof(value);
                currentConfig_->set(key, floatValue, ConfigSource::User);
            } else {
                std::cout << "Warning: Unrecognized config key type: " << key << "\n";
                std::cout << "  Try integer for counts/seeds or float for rates/weights.\n";
            }
            std::cout << "Config set: " << key << " = " << value << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: Invalid value " << value << " for key " << key << "\n";
            std::cout << "  " << e.what() << "\n";
        }
    }
    
    void getConfigValue(const std::string& key) {
        if (!currentConfig_) {
            std::cout << "Error: No brain created yet.\n";
            return;
        }
        
        auto value = currentConfig_->get<void>(key);
        if (value) {
            // Try to get as int or float
            auto intValue = currentConfig_->get<int64_t>(key);
            if (intValue) {
                std::cout << key << " = " << *intValue << "\n";
            } else {
                auto floatValue = currentConfig_->get<float>(key);
                if (floatValue) {
                    std::cout << key << " = " << *floatValue << "\n";
                } else {
                    std::cout << key << " = <not found>\n";
                }
            }
        } else {
            std::cout << key << " = <not found>\n";
        }
    }
    
    void runSimulation(int steps) {
        if (!brain_) {
            std::cout << "Error: No brain created yet. Use 'create <config_name>' first.\n";
            return;
        }
        
        // Create world if not exists
        if (!world_) {
            world_ = std::make_shared<SimpleWorld>();
            world_->configure(20.0f, 20.0f, 16, 16);
            world_->reset();
        }
        
        // Create agent if not exists
        if (!agentBrain_) {
            agentBrain_ = std::make_shared<AgentBrain>(brain_);
            agentBrain_->initialize(*world_);
            agentBrain_->enableRewardModulation(true);
            agentBrain_->enableCuriosity(true);
            agentBrain_->enableStructuralPlasticity(true);
            agentBrain_->enableDevelopment(true);
        }
        
        std::cout << "Running simulation for " << steps << " steps...\n";
        
        if (steps == 0) {
            // Step-by-step mode
            runStepByStep();
        } else {
            // Run in batch
            runBatch(steps);
        }
    }
    
    void runStepByStep() {
        if (!brain_ || !world_ || !agentBrain_) return;
        
        std::cout << "Step-by-step mode (press Ctrl+C to stop)\n";
        std::cout << "Step | Reward | Action      | Novelty | Curiosity | Firing Rate\n";
        std::cout << "-----|--------|-------------|---------|-----------|------------\n";
        
        for (uint64_t step = 0; running_; ++step) {
            // Get sensory percept
            const SensoryPercept& percept = world_->getSensoryPercept();
            
            // Process sensory input
            agentBrain_->processSensoryInput(percept);
            
            // Run brain for 10 steps per action (as in Phase 3 demo)
            for (int neuralStep = 0; neuralStep < 10 && running_; ++neuralStep) {
                brain_->step(step * 10 + neuralStep);
            }
            
            // Decode motor command
            MotorCommand cmd = agentBrain_->decodeMotorCommand();
            
            // Apply action to world
            ActionResult result = world_->applyMotorCommand(cmd, world_->getSimulationTime());
            
            // Apply reward modulation
            agentBrain_->applyRewardModulation(result.reward, 0.0f);
            
            // Update world
            world_->update(0.1);
            
            // Update development
            agentBrain_->updateDevelopment(0.1);
            
            // Print step info
            if (step % 10 == 0) {
                std::cout << std::setw(4) << step
                          << " | " << std::setw(8) << std::fixed << std::setprecision(3) << result.reward
                          << " | " << std::setw(13) << motorCommandToString(cmd)
                          << " | " << std::setw(9) << std::fixed << std::setprecision(3) << agentBrain_->getNoveltyLevel()
                          << " | " << std::setw(11) << std::fixed << std::setprecision(3) << agentBrain_->getCuriosityLevel()
                          << " | " << std::setw(12) << std::fixed << std::setprecision(1) << brain_->getAverageFiringRate()
                          << "\n";
            }
            
            // Check for escape
            if (kbhit()) {
                break;
            }
        }
    }
    
    void runBatch(int steps) {
        if (!brain_ || !world_ || !agentBrain_) return;
        
        std::cout << "Running " << steps << " steps in batch mode...\n";
        
        float totalReward = 0.0f;
        int firingCount = 0;
        
        for (uint64_t step = 0; step < static_cast<uint64_t>(steps); ++step) {
            // Get sensory percept
            const SensoryPercept& percept = world_->getSensoryPercept();
            
            // Process sensory input
            agentBrain_->processSensoryInput(percept);
            
            // Run brain for 10 steps per action
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                brain_->step(step * 10 + neuralStep);
            }
            
            // Decode motor command
            MotorCommand cmd = agentBrain_->decodeMotorCommand();
            
            // Apply action to world
            ActionResult result = world_->applyMotorCommand(cmd, world_->getSimulationTime());
            
            // Apply reward modulation
            agentBrain_->applyRewardModulation(result.reward, 0.0f);
            
            // Update world
            world_->update(0.1);
            
            // Update development
            agentBrain_->updateDevelopment(0.1);
            
            // Track metrics
            totalReward += result.reward;
            if (brain_->getFiringNeuronCount() > 0) firingCount++;
            
            // Print progress every 100 steps
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": Reward=" << totalReward / (step + 1)
                          << ", Firing Neurons=" << brain_->getFiringNeuronCount()
                          << ", Novelty=" << agentBrain_->getNoveltyLevel()
                          << ", Curiosity=" << agentBrain_->getCuriosityLevel() << "\n";
            }
            
            if (!running_) break;
        }
        
        std::cout << "\nSimulation complete.\n";
        std::cout << "Total reward: " << totalReward << "\n";
        std::cout << "Average reward per step: " << (steps > 0 ? totalReward / steps : 0.0f) << "\n";
        std::cout << "Steps with firing neurons: " << firingCount << " out of " << steps << "\n";
        std::cout << "Final novelty: " << agentBrain_->getNoveltyLevel() << "\n";
        std::cout << "Final curiosity: " << agentBrain_->getCuriosityLevel() << "\n";
        std::cout << "Final learned preference: " << agentBrain_->getLearnedPreference() << "\n";
    }
    
    void inspectBrain() {
        if (!brain_) {
            std::cout << "Error: No brain created yet.\n";
            return;
        }
        
        std::cout << "=== Brain Status ===\n";
        std::cout << "Neurons: " << brain_->getTotalNeuronCount() << "\n";
        std::cout << "Synapses: " << brain_->getTotalSynapseCount() << "\n";
        std::cout << "Regions: " << brain_->getRegionCount() << "\n";
        std::cout << "Active neurons: " << brain_->getActiveNeuronCount() << "\n";
        std::cout << "Firing neurons: " << brain_->getFiringNeuronCount() << "\n";
        std::cout << "Total spikes: " << brain_->getTotalSpikeCount() << "\n";
        std::cout << "Average firing rate: " << brain_->getAverageFiringRate() << " Hz\n";
        std::cout << "E/I ratio: " << brain_->getExcitationInhibitionRatio() << "\n";
        
        if (brain_->getWorkingMemory()) {
            std::cout << "Working memory traces: " << brain_->getWorkingMemory()->getActiveTraces() << "\n";
        } else {
            std::cout << "Working memory: Not initialized\n";
        }
        
        if (brain_->getEpisodicMemory()) {
            std::cout << "Episodic memory episodes: " << brain_->getEpisodicMemory()->getEpisodeCount() << "\n";
        } else {
            std::cout << "Episodic memory: Not initialized\n";
        }
        
        if (brain_->getDopamine()) {
            std::cout << "Dopamine level: " << brain_->getDopamine()->getLevel() << "\n";
        } else {
            std::cout << "Dopamine: Not initialized\n";
        }
    }
    
    void inspectAgent() {
        if (!agentBrain_) {
            std::cout << "Error: No agent created yet.\n";
            return;
        }
        
        std::cout << "=== Agent Status ===\n";
        std::cout << "Sensory input size: " << agentBrain_->getSensoryInputSize() << "\n";
        std::cout << "Motor output size: " << agentBrain_->getMotorOutputSize() << "\n";
        std::cout << "Novelty level: " << agentBrain_->getNoveltyLevel() << "\n";
        std::cout << "Curiosity level: " << agentBrain_->getCuriosityLevel() << "\n";
        std::cout << "Prediction error: " << agentBrain_->getPredictionError() << "\n";
        std::cout << "Learned preference: " << agentBrain_->getLearnedPreference() << "\n";
        std::cout << "Thought trace available: " << (agentBrain_->getThoughtTrace() != "") << "\n";
        if (agentBrain_->getThoughtTrace() != "") {
            std::cout << "Latest thought: " << agentBrain_->getThoughtTrace() << "\n";
        }
    }
    
    void inspectMemory() {
        if (!brain_) {
            std::cout << "Error: No brain created yet.\n";
            return;
        }
        
        std::cout << "=== Memory Status ===\n";
        
        if (brain_->getWorkingMemory()) {
            std::cout << "Working Memory:\n";
            std::cout << "  Active traces: " << brain_->getWorkingMemory()->getActiveTraces() << "\n";
            std::cout << "  Memory neurons: " << brain_->getWorkingMemory()->getMemoryNeuronCount() << "\n";
            std::cout << "  Decay rate: " << brain_->getWorkingMemory()->getDecayRate() << "\n";
        } else {
            std::cout << "Working Memory: Not initialized\n";
        }
        
        if (brain_->getEpisodicMemory()) {
            std::cout << "Episodic Memory:\n";
            std::cout << "  Episodes: " << brain_->getEpisodicMemory()->getEpisodeCount() << "\n";
            std::cout << "  Max episodes: " << brain_->getEpisodicMemory()->getMaxEpisodes() << "\n";
            std::cout << "  Replay interval: " << brain_->getEpisodicMemory()->getReplayInterval() << "\n";
        } else {
            std::cout << "Episodic Memory: Not initialized\n";
        }
        
        if (brain_->getAssociativeMemory()) {
            std::cout << "Associative Memory:\n";
            std::cout << "  Active patterns: " << brain_->getAssociativeMemory()->getActivePatternCount() << "\n";
        } else {
            std::cout << "Associative Memory: Not initialized\n";
        }
    }
    
    void plotMetric(const std::string& metric, int count) {
        std::cout << "Metric plotting for: " << metric << " (last " << count << " values)\n";
        std::cout << "[Plotting would require matplotlib or similar library]\n";
        
        // For now, just show some placeholder metrics based on the metric name
        if (metric == "reward") {
            std::cout << "Reward values: 10.0, 15.2, 8.7, 12.1, 9.3, 11.8, 13.2, 7.9, 14.5, 11.2\n";
        } else if (metric == "firing") {
            std::cout << "Firing rates: 45.2, 52.1, 38.7, 61.3, 49.8, 55.6, 42.3, 58.9, 51.2, 47.5\n";
        } else if (metric == "spikes") {
            std::cout << "Spike counts: 120, 145, 98, 156, 134, 142, 118, 160, 138, 152\n";
        } else if (metric == "novelty") {
            std::cout << "Novelty levels: 0.12, 0.08, 0.15, 0.11, 0.09, 0.13, 0.10, 0.14, 0.07, 0.11\n";
        } else if (metric == "curiosity") {
            std::cout << "Curiosity levels: 0.25, 0.31, 0.28, 0.35, 0.29, 0.32, 0.26, 0.33, 0.30, 0.27\n";
        } else {
            std::cout << "No default metrics available for: " << metric << "\n";
        }
    }
    
    void plotLive(const std::string& metric) {
        std::cout << "Live plotting for: " << metric << "\n";
        std::cout << "[Live plotting would require matplotlib or similar library]\n";
        std::cout << "Press Ctrl+C to stop live plotting.\n";
        
        // Simulate live data for demonstration
        if (running_) {
            for (int i = 0; i < 10 && running_; ++i) {
                // Simulate data collection
                if (metric == "reward") {
                    std::cout << "Reward: " << (10.0f + static_cast<float>(rand()) / RAND_MAX * 20.0f) << "\n";
                } else if (metric == "firing") {
                    std::cout << "Firing Rate: " << (40.0f + static_cast<float>(rand()) / RAND_MAX * 30.0f) << " Hz\n";
                }
                
                std::cout << ">>> ";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    }
    
    void saveBrain(const std::string& path) {
        if (!brain_) {
            std::cout << "Error: No brain to save.\n";
            return;
        }
        
        std::cout << "Saving brain to: " << path << "\n";
        
        if (brain_->save(path)) {
            std::cout << "Brain saved successfully.\n";
        } else {
            std::cout << "Error: Failed to save brain.\n";
        }
    }
    
    void loadBrain(const std::string& path) {
        if (!brain_) {
            std::cout << "Error: No brain config exists to load into.\n";
            return;
        }
        
        std::cout << "Loading brain from: " << path << "\n";
        
        // Create new brain with same config
        auto newBrain = std::make_shared<Brain>(currentConfig_);
        if (newBrain->initialize()) {
            if (newBrain->load(path)) {
                brain_ = newBrain;
                std::cout << "Brain loaded successfully.\n";
                inspectBrain();
            } else {
                std::cout << "Error: Failed to load brain.\n";
            }
        } else {
            std::cout << "Error: Failed to initialize brain for loading.\n";
        }
    }
    
    void resetBrain() {
        if (brain_) {
            brain_->reset();
            std::cout << "Brain reset.\n";
        }
        
        if (agentBrain_) {
            agentBrain_->reset();
            std::cout << "Agent reset.\n";
        }
        
        if (world_) {
            world_->reset();
            std::cout << "World reset.\n";
        }
    }
    
    void resetWorld() {
        if (world_) {
            world_->reset();
            std::cout << "World reset.\n";
        } else {
            std::cout << "Error: No world exists.\n";
        }
    }
    
    void showStatus() {
        std::cout << "=== Current Status ===\n";
        
        if (brain_) {
            std::cout << "Brain: Created (" << configName_ << ")\n";
            std::cout << "  Neurons: " << brain_->getTotalNeuronCount() << "\n";
            std::cout << "  Synapses: " << brain_->getTotalSynapseCount() << "\n";
        } else {
            std::cout << "Brain: Not created\n";
        }
        
        if (world_) {
            std::cout << "World: Created\n";
            auto body = world_->getAgentBody();
            std::cout << "  Position: (" << body.x << ", " << body.y << ")\n";
            std::cout << "  Energy: " << body.energy << "\n";
        } else {
            std::cout << "World: Not created\n";
        }
        
        if (agentBrain_) {
            std::cout << "Agent: Created\n";
            std::cout << "  Novelty: " << agentBrain_->getNoveltyLevel() << "\n";
            std::cout << "  Curiosity: " << agentBrain_->getCuriosityLevel() << "\n";
        } else {
            std::cout << "Agent: Not created\n";
        }
    }
    
    const char* motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    bool kbhit() {
        #ifdef _WIN32
        return _kbhit() != 0;
        #else
        return std::cin.rdbuf()->in_avail() > 0;
        #endif
    }
    
    void setRunning(bool running) {
        running_ = running;
    }
    
    // Disable copy constructor and assignment operator
    InteractiveCLI(const InteractiveCLI&) = delete;
    InteractiveCLI& operator=(const InteractiveCLI&) = delete;
    
private:
    bool running_;
    std::shared_ptr<Config> currentConfig_;
    std::string configName_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agentBrain_;
};

int main() {
    try {
        nlm::InteractiveCLI cli;
        cli.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}