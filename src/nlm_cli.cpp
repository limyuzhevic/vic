#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"

namespace nlm {

class NLMCli {
public:
    NLMCli() : running_(false), shouldExit_(false) {
        signal(SIGINT, handleSignal);
        signal(SIGTERM, handleSignal);
    }
    
    ~NLMCli() {
        running_ = false;
        if (monitorThread_.joinable()) {
            monitorThread_.join();
        }
    }
    
    bool parseArgs(int argc, char** argv);
    bool run() {
        running_ = true;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Load configuration
        if (!config_.loadFromFile("configs/default.cfg")) {
            std::cerr << "Failed to load config file, using defaults" << std::endl;
        }
        
        // Load command line overrides
        config_.loadFromArgs(argc, argv);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->initialize(16, 16);
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Configure agent based on config
        if (config_.getOr<bool>("enable_reward_modulation", true)) {
            agent->enableRewardModulation(true);
        }
        if (config_.getOr<bool>("enable_structural_plasticity", true)) {
            agent->enableStructuralPlasticity(true);
        }
        if (config_.getOr<bool>("enable_development", true)) {
            agent->enableDevelopment(true);
        }
        if (config_.getOr<bool>("enable_curiosity", true)) {
            agent->enableCuriosity(true);
        }
        
        // Start monitoring thread if requested
        if (config_.getOr<bool>("enable_monitoring", false)) {
            startMonitoringThread();
        }
        
        // Run simulation
        runSimulation(brain, agent, world);
        
        running_ = false;
        return true;
    }
    
    void startMonitoringThread() {
        monitorThread_ = std::thread(&NLMCli::monitorLoop, this);
    }
    
    void monitorLoop() {
        while (running_ && !shouldExit_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // Log status
            NLM_LOG_INFO("=== MONITORING STATUS ===");
            NLM_LOG_INFO("Brain stats: Neurons=" + std::to_string(brain_->getTotalNeuronCount()) +
                        " Synapses=" + std::to_string(brain_->getTotalSynapseCount()) +
                        " Spikes=" + std::to_string(brain_->getTotalSpikeCount()));
            
            auto* workingMemory = brain_->getWorkingMemory();
            if (workingMemory) {
                NLM_LOG_INFO("Working Memory: " + std::to_string(workingMemory->getActiveTraces()) + " traces");
            }
            
            auto* episodicMemory = brain_->getEpisodicMemory();
            if (episodicMemory) {
                NLM_LOG_INFO("Episodic Memory: " + std::to_string(episodicMemory->getEpisodeCount()) + " episodes");
            }
            
            auto* dopamine = brain_->getDopamine();
            if (dopamine) {
                NLM_LOG_INFO("Dopamine Level: " + std::to_string(dopamine->getLevel()));
            }
            
            NLM_LOG_INFO("");
        }
    }
    
    void runSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
        int maxSteps = config_.getOr<int>("max_steps", 1000);
        bool enableDevelopment = config_.getOr<bool>("enable_development", true);
        
        NLM_LOG_INFO("=== NLM SIMULATION ===");
        NLM_LOG_INFO("Starting simulation with " + std::to_string(maxSteps) + " steps");
        
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        for (uint64_t step = 0; step < maxSteps && !shouldExit_; ++step) {
            // Get observation
            SensoryPercept percept = world->observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world->computeReward(brain->getRegions()[0].get());
            totalReward += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
            
            // Update development
            if (enableDevelopment) {
                agent->updateDevelopment(0.001);
            }
            
            // Collect metrics
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            
            // Periodic status
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Reward: " + std::to_string(totalReward / (step + 1)) +
                            " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                            " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                                brain->getWorkingMemory()->getActiveTraces() : 0));
            }
            
            // Check for early exit
            if (shouldExit_) break;
        }
        
        NLM_LOG_INFO("=== SIMULATION COMPLETE ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(totalReward / maxSteps));
        NLM_LOG_INFO("Avg firing rate: " + std::to_string(totalFiringRate / maxSteps));
        NLM_LOG_INFO("Steps completed: " + std::to_string(maxSteps));
    }
    
    static void handleSignal(int signum) {
        shouldExit_ = true;
        std::cout << "\nReceived signal " << signum << ", shutting down..." << std::endl;
    }
    
    void printUsage() const {
        std::cout << "NLM — Neural Learning Machine" << std::endl;
        std::cout << "Usage: nlm <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  run          Run simulation with current config" << std::endl;
        std::cout << "  config       Configuration management" << std::endl;
        std::cout << "  benchmark    Run performance benchmarks" << std::endl;
        std::cout << "  monitor      Start monitoring dashboard" << std::endl;
        std::cout << "  script       Run experiment from script file" << std::endl;
        std::cout << "  plugin       Plugin management" << std::endl;
        std::cout << "  export       Export brain state" << std::endl;
        std::cout << "  help         Show this help" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm run --config my_config.json" << std::endl;
        std::cout << "  nlm monitor --port 8080" << std::endl;
        std::cout << "  nlm benchmark --test neural_dynamics" << std::endl;
    }
    
    bool handleConfigCommand(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "Configuration options:" << std::endl;
            std::cout << "  show           Show current configuration" << std::endl;
            std::cout << "  set <key> <value>  Set a configuration value" << std::endl;
            std::cout << "  load <file>     Load configuration from file" << std::endl;
            std::cout << "  save <file>     Save configuration to file" << std::endl;
            std::cout << "  reset           Reset to defaults" << std::endl;
            return true;
        }
        
        std::string subcommand = args[0];
        
        if (subcommand == "show") {
            std::cout << config_.summary() << std::endl;
        } else if (subcommand == "set" && args.size() >= 3) {
            std::string key = args[1];
            std::string value = args[2];
            
            // Try to parse as different types
            try {
                // Try int
                size_t pos;
                int intVal = std::stoi(value);
                config_.set(key, intVal, ConfigSource::Runtime);
                std::cout << "Set " << key << " = " << intVal << std::endl;
            } catch (...) {
                try {
                    // Try double
                    double doubleVal = std::stod(value);
                    config_.set(key, doubleVal, ConfigSource::Runtime);
                    std::cout << "Set " << key << " = " << doubleVal << std::endl;
                } catch (...) {
                    try {
                        // Try bool
                        if (value == "true") {
                            config_.set(key, true, ConfigSource::Runtime);
                            std::cout << "Set " << key << " = true" << std::endl;
                        } else if (value == "false") {
                            config_.set(key, false, ConfigSource::Runtime);
                            std::cout << "Set " << key << " = false" << std::endl;
                        } else {
                            // String
                            config_.set(key, value, ConfigSource::Runtime);
                            std::cout << "Set " << key << " = \"" << value << "\"" << std::endl;
                        }
                    } catch (...) {
                        std::cout << "Failed to parse value: " << value << std::endl;
                        return false;
                    }
                }
            }
        } else if (subcommand == "load" && args.size() >= 2) {
            if (config_.loadFromFile(args[1])) {
                std::cout << "Loaded configuration from " << args[1] << std::endl;
            } else {
                std::cout << "Failed to load configuration from " << args[1] << std::endl;
                return false;
            }
        } else if (subcommand == "save" && args.size() >= 2) {
            if (config_.saveToFile(args[1])) {
                std::cout << "Saved configuration to " << args[1] << std::endl;
            } else {
                std::cout << "Failed to save configuration to " << args[1] << std::endl;
                return false;
            }
        } else if (subcommand == "reset") {
            config_.clear();
            std::cout << "Configuration reset to defaults" << std::endl;
        } else {
            std::cout << "Unknown config subcommand: " << subcommand << std::endl;
            return false;
        }
        
        return true;
    }
    
    bool parseArgs(int argc, char** argv) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        if (args.empty()) {
            printUsage();
            return false;
        }
        
        std::string command = args[0];
        std::vector<std::string> commandArgs(args.begin() + 1, args.end());
        
        if (command == "run") {
            return run();
        } else if (command == "config") {
            return handleConfigCommand(commandArgs);
        } else if (command == "help") {
            printUsage();
            return true;
        } else if (command == "version") {
            std::cout << "NLM Version 0.7.0 (Phase 7 Enhanced)" << std::endl;
            return true;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            printUsage();
            return false;
        }
    }
    
private:
    Config config_;
    std::thread monitorThread_;
    std::atomic<bool> running_;
    std::atomic<bool> shouldExit_;
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm

// Main function
int main(int argc, char* argv[]) {
    nlm::NLMCli cli;
    return cli.parseArgs(argc, argv) ? 0 : 1;
}
