#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <algorithm>

// Include NLM headers
#include "src/agent/AgentBrain.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/memory/WorkingMemory.hpp"
#include "src/memory/SemanticMemory.hpp"
#include "src/memory/ProceduralMemory.hpp"
#include "src/memory/AssociativeMemory.hpp"

// Forward declaration for the Python interface if needed
namespace pynlm {
    class PythonInterface;
}

class CommandLineInterface {
public:
    CommandLineInterface() : running_(false), config_(std::make_unique<nlm::Config>()) {
        initializeLogger();
        initializeComponents();
    }
    
    ~CommandLineInterface() {
        running_ = false;
        if (simulationThread_.joinable()) {
            simulationThread_.join();
        }
    }
    
    void run() {
        printWelcomeMessage();
        printHelp();
        
        std::string command;
        while (running_) {
            std::cout << "nlm> ";
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            handleCommand(command);
        }
    }
    
    void startSimulation(int steps, double timestep, const std::string& worldConfig = "") {
        running_ = true;
        
        simulationThread_ = std::thread([this, steps, timestep, worldConfig]() {
            runSimulation(steps, timestep, worldConfig);
        });
        
        std::cout << "Starting simulation for " << steps << " steps (timestep=" << timestep << ")" << std::endl;
    }
    
    void stopSimulation() {
        running_ = false;
        if (simulationThread_.joinable()) {
            simulationThread_.join();
        }
        std::cout << "Simulation stopped" << std::endl;
    }
    
    void runSimulation(int steps, double timestep, const std::string& worldConfig) {
        try {
            // Create brain with configuration
            auto brain = createBrain();
            brain->initialize();
            
            // Create world
            auto world = createWorld(worldConfig);
            world->configure(20, 20, 8, 8);  // width, height, visionWidth, visionHeight
            world->reset();
            
            // Create agent
            auto agent = createAgent(brain, world);
            agent->initialize(*world);
            
            // Enable features if configured
            enableFeatures(agent);
            
            std::cout << "Starting simulation..." << std::endl;
            std::cout << "Brain neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "World size: 20x20, vision: 8x8" << std::endl;
            
            // Run simulation loop
            for (int step = 0; step < steps && running_; ++step) {
                // Update world
                world->update(timestep);
                
                // Get sensory percept
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Brain thinking
                brain->step(step);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply action to world
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward modulation
                if (agent->isRewardModulationEnabled()) {
                    float reward = calculateReward(world, action);
                    float predictedReward = agent->getPredictionError();
                    agent->applyRewardModulation(reward, predictedReward);
                }
                
                // Update development
                if (agent->isDevelopmentEnabled()) {
                    agent->updateDevelopment(timestep);
                }
                
                // Print progress every 10% or at milestones
                if (step % (steps / 10) == 0 || step == steps - 1) {
                    std::cout << "Step " << step << "/" << steps << " - " 
                             << "Firing neurons: " << brain->getFiringNeuronCount() 
                             << ", Spikes: " << brain->getTotalSpikeCount()
                             << ", Reward: " << reward 
                             << ", Dopamine: " << agent->getNeuromodulationLevel()
                             << std::endl;
                }
                
                // Sleep briefly to control simulation speed
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            
            std::cout << "Simulation completed" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "Error in simulation: " << e.what() << std::endl;
            stopSimulation();
        }
    }
    
    void printStats() const {
        std::cout << "=== NLM System Statistics ===" << std::endl;
        std::cout << "Memory Systems:" << std::endl;
        
        if (workingMemory_) {
            std::cout << "  Working Memory: " << workingMemory_->getCurrentSize() << "/" 
                     << workingMemory_->getCapacity() << " items" << std::endl;
        }
        
        if (semanticMemory_) {
            std::cout << "  Semantic Memory: " << semanticMemory_->getFactCount() << " facts" 
                     << " (Avg confidence: " << semanticMemory_->getAverageConfidence() << ")" << std::endl;
        }
        
        if (proceduralMemory_) {
            std::cout << "  Procedural Memory: " << proceduralMemory_->getSkillCount() << " skills" 
                     << " (Avg proficiency: " << proceduralMemory_->getAverageProficiency() << ")" << std::endl;
        }
        
        if (associativeMemory_) {
            std::cout << "  Associative Memory: " << associativeMemory_->getAssociationCount() 
                     << " associations (Avg strength: " << associativeMemory_->getAverageAssociationStrength() << ")" << std::endl;
        }
        
        std::cout << "===============================" << std::endl;
    }
    
    void printHelp() const {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help                    - Show this help message" << std::endl;
        std::cout << "  sim <steps> <timestep>  - Start simulation with given steps and timestep" << std::endl;
        std::cout << "  stop                    - Stop current simulation" << std::endl;
        std::cout << "  stats                   - Show system statistics" << std::endl;
        std::cout << "  config <key> [value]    - Get/set configuration values" << std::endl;
        std::cout << "  enable <feature>        - Enable a feature (reward, curiosity, development)" << std::endl;
        std::cout << "  disable <feature>       - Disable a feature" << std::endl;
        std::cout << "  world <width> <height>  - Configure world size" << std::endl;
        std::cout << "  vision <width> <height> - Configure agent vision" << std::endl;
        std::cout << "  reset                   - Reset all components" << std::endl;
        std::cout << "  quit, exit              - Exit the program" << std::endl;
        std::cout << std::endl;
        std::cout << "Features: reward, curiosity, development, structural_plasticity" << std::endl;
        std::cout << "================================" << std::endl;
    }
    
    void enableFeature(const std::string& feature) {
        if (agent_) {
            if (feature == "reward") {
                agent_->enableRewardModulation(true);
                std::cout << "Enabled reward modulation" << std::endl;
            } else if (feature == "curiosity") {
                agent_->enableCuriosity(true);
                std::cout << "Enabled curiosity" << std::endl;
            } else if (feature == "development") {
                agent_->enableDevelopment(true);
                std::cout << "Enabled development" << std::endl;
            } else if (feature == "structural_plasticity") {
                agent_->enableStructuralPlasticity(true);
                std::cout << "Enabled structural plasticity" << std::endl;
            } else {
                std::cout << "Unknown feature: " << feature << std::endl;
            }
        }
    }
    
    void disableFeature(const std::string& feature) {
        if (agent_) {
            if (feature == "reward") {
                agent_->enableRewardModulation(false);
                std::cout << "Disabled reward modulation" << std::endl;
            } else if (feature == "curiosity") {
                agent_->enableCuriosity(false);
                std::cout << "Disabled curiosity" << std::endl;
            } else if (feature == "development") {
                agent_->enableDevelopment(false);
                std::cout << "Disabled development" << std::endl;
            } else if (feature == "structural_plasticity") {
                agent_->enableStructuralPlasticity(false);
                std::cout << "Disabled structural plasticity" << std::endl;
            } else {
                std::cout << "Unknown feature: " << feature << std::endl;
            }
        }
    }
    
    void setWorldConfig(int width, int height, int visionWidth, int visionHeight) {
        if (world_) {
            world_->configure(width, height, visionWidth, visionHeight);
            std::cout << "World configured: " << width << "x" << height 
                     << ", vision: " << visionWidth << "x" << visionHeight << std::endl;
        }
    }
    
    void saveConfig(const std::string& filename) {
        if (config_->saveToFile(filename)) {
            std::cout << "Configuration saved to " << filename << std::endl;
        } else {
            std::cout << "Failed to save configuration" << std::endl;
        }
    }
    
    void loadConfig(const std::string& filename) {
        if (config_->loadFromFile(filename)) {
            std::cout << "Configuration loaded from " << filename << std::endl;
        } else {
            std::cout << "Failed to load configuration" << std::endl;
        }
    }
    
private:
    void initializeLogger() {
        nlm::Logger::initialize();
        nlm::Logger::setLevel(nlm::LogLevel::Info);
        NLM_LOG_INFO("NLM Command Line Interface initialized");
    }
    
    void initializeComponents() {
        // Create brain
        brain_ = createBrain();
        
        // Create world
        world_ = createWorld("");
        
        // Create agent
        agent_ = createAgent(brain_, world_);
        
        // Create memory systems
        workingMemory_ = std::make_unique<nlm::WorkingMemory>();
        workingMemory_->setCapacity(100);
        
        semanticMemory_ = std::make_unique<nlm::SemanticMemory>();
        semanticMemory_->setMaxFacts(1000);
        
        proceduralMemory_ = std::make_unique<nlm::ProceduralMemory>();
        proceduralMemory_->setMaxSkills(50);
        
        associativeMemory_ = std::make_unique<nlm::AssociativeMemory>();
        associativeMemory_->setMaxAssociations(10000);
        
        // Initialize memory systems with brain
        workingMemory_->initialize(brain_.get());
        semanticMemory_->initialize(brain_.get());
        proceduralMemory_->initialize(brain_.get());
        associativeMemory_->initialize(brain_.get());
        
        std::cout << "Components initialized successfully" << std::endl;
    }
    
    std::unique_ptr<nlm::Brain> createBrain() {
        auto config = nlm::createDefaultConfig();
        return nlm::createBrain(config);
    }
    
    std::unique_ptr<SimpleWorld> createWorld(const std::string& config) {
        auto world = nlm::createSimpleWorld();
        world->configure(20, 20, 8, 8);  // Default size
        return world;
    }
    
    std::unique_ptr<AgentBrain> createAgent(std::unique_ptr<nlm::Brain>& brain, 
                                           std::unique_ptr<SimpleWorld>& world) {
        return nlm::createAgentBrain(std::move(brain));
    }
    
    void enableFeatures(std::unique_ptr<AgentBrain>& agent) {
        if (agent) {
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            agent->enableStructuralPlasticity(true);
        }
    }
    
    float calculateReward(std::unique_ptr<SimpleWorld>& world, nlm::MotorCommand action) {
        // Simple reward calculation based on action success
        return 0.1f;  // Placeholder
    }
    
    void handleCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "help") {
            printHelp();
        } else if (cmd == "quit" || cmd == "exit") {
            running_ = false;
        } else if (cmd == "sim") {
            int steps;
            double timestep;
            if (iss >> steps >> timestep) {
                std::string worldConfig;
                if (iss >> worldConfig) {
                    startSimulation(steps, timestep, worldConfig);
                } else {
                    startSimulation(steps, timestep);
                }
            } else {
                std::cout << "Usage: sim <steps> <timestep>" << std::endl;
            }
        } else if (cmd == "stop") {
            stopSimulation();
        } else if (cmd == "stats") {
            printStats();
        } else if (cmd == "config") {
            handleConfigCommand(iss);
        } else if (cmd == "enable") {
            std::string feature;
            if (iss >> feature) {
                enableFeature(feature);
            } else {
                std::cout << "Usage: enable <feature>" << std::endl;
            }
        } else if (cmd == "disable") {
            std::string feature;
            if (iss >> feature) {
                disableFeature(feature);
            } else {
                std::cout << "Usage: disable <feature>" << std::endl;
            }
        } else if (cmd == "world") {
            int width, height;
            if (iss >> width >> height) {
                setWorldConfig(width, height, 8, 8);
            } else {
                std::cout << "Usage: world <width> <height>" << std::endl;
            }
        } else if (cmd == "vision") {
            int width, height;
            if (iss >> width >> height) {
                if (world_) {
                    world_->configure(world_->getWidth(), world_->getHeight(), width, height);
                    std::cout << "Vision configured: " << width << "x" << height << std::endl;
                }
            } else {
                std::cout << "Usage: vision <width> <height>" << std::endl;
            }
        } else if (cmd == "reset") {
            resetAll();
        } else if (cmd == "save") {
            std::string filename;
            if (iss >> filename) {
                saveConfig(filename);
            } else {
                std::cout << "Usage: save <filename>" << std::endl;
            }
        } else if (cmd == "load") {
            std::string filename;
            if (iss >> filename) {
                loadConfig(filename);
            } else {
                std::cout << "Usage: load <filename>" << std::endl;
            }
        } else {
            std::cout << "Unknown command: " << cmd << std::endl;
            std::cout << "Type 'help' for available commands" << std::endl;
        }
    }
    
    void handleConfigCommand(std::istringstream& iss) {
        std::string key;
        if (std::getline(iss >> std::ws, key)) {
            std::string value;
            if (std::getline(iss, value)) {
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\n\r"));
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                
                if (!value.empty()) {
                    // Set configuration value
                    config_->set(key, value);
                    std::cout << "Set " << key << " = " << value << std::endl;
                } else {
                    // Get configuration value
                    auto result = config_->get<std::string>(key);
                    if (result.has_value()) {
                        std::cout << key << " = " << result.value() << std::endl;
                    } else {
                        std::cout << "Unknown configuration key: " << key << std::endl;
                    }
                }
            }
        }
    }
    
    void resetAll() {
        if (agent_) {
            agent_->reset();
        }
        if (world_) {
            world_->reset();
        }
        if (workingMemory_) {
            workingMemory_->clear();
        }
        if (semanticMemory_) {
            semanticMemory_->clear();
        }
        if (proceduralMemory_) {
            proceduralMemory_->clear();
        }
        if (associativeMemory_) {
            associativeMemory_->clear();
        }
        
        std::cout << "All components reset" << std::endl;
    }
    
    void printWelcomeMessage() const {
        std::cout << "=== NLM Brain Simulation System ===" << std::endl;
        std::cout << "Neural Network Learning Machine - Advanced AI Simulation" << std::endl;
        std::cout << "Version: 2.0.0 - Enhanced Edition" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << std::endl;
    }
    
    // Component references
    std::unique_ptr<nlm::Config> config_;
    std::unique_ptr<nlm::Brain> brain_;
    std::unique_ptr<SimpleWorld> world_;
    std::unique_ptr<AgentBrain> agent_;
    std::unique_ptr<nlm::WorkingMemory> workingMemory_;
    std::unique_ptr<nlm::SemanticMemory> semanticMemory_;
    std::unique_ptr<nlm::ProceduralMemory> proceduralMemory_;
    std::unique_ptr<nlm::AssociativeMemory> associativeMemory_;
    
    // State
    bool running_;
    std::thread simulationThread_;
};

int main(int argc, char* argv[]) {
    CommandLineInterface cli;
    
    // Handle command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h          Show this help message" << std::endl;
            std::cout << "  --version           Show version information" << std::endl;
            std::cout << "  --run <steps> <ts>   Run simulation with given parameters" << std::endl;
            return 0;
        } else if (arg == "--version") {
            std::cout << "NLM Version 2.0.0 - Enhanced Brain Simulation System" << std::endl;
            return 0;
        } else if (arg == "--run") {
            if (argc >= 4) {
                int steps = std::stoi(argv[2]);
                double timestep = std::stod(argv[3]);
                cli.startSimulation(steps, timestep);
                std::cout << "Press Enter to stop simulation..." << std::endl;
                std::cin.ignore();
                cli.stopSimulation();
                return 0;
            }
        }
    }
    
    // Run interactive interface
    cli.run();
    return 0;
}
