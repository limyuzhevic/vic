// NLM (熙然) Neural Simulation Framework - Main Entry Point
// Enhanced with modern configuration system and Python bindings

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <chrono>

// Version information
const std::string NLM_VERSION = "2.0.0";
const std::string NLM_PHASE = "Phase 6: Final Integration";

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Enhanced with modern configuration system                ║
    ║     and Python bindings                                        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printVersionInfo() {
    std::cout << "NLM (熙然) Neural Learning Machine" << std::endl;
    std::cout << "Version: " << NLM_VERSION << std::endl;
    std::cout << "Phase: " << NLM_PHASE << std::endl;
    std::cout << "Enhanced Configuration System" << std::endl;
    std::cout << "Modern Python Bindings" << std::endl;
    std::cout << "Experimental AI Research Platform" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM (熙然) Neural Simulation Framework" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config <file>           Configuration file (JSON or .cfg format)" << std::endl;
    std::cout << "  --steps <num>             Number of simulation steps" << std::endl;
    std::cout << "  --brain-file <file>       Save brain state to file" << std::endl;
    std::cout << "  --load <file>             Load brain state from file" << std::endl;
    std::cout << "  --verbose                 Enable verbose output" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --config configs/experiment.json --steps 10000" << std::endl;
    std::cout << "  " << programName << " --load brain_checkpoint.bin --brain-file new_brain.bin" << std::endl;
    std::cout << "  " << programName << " --verbose --steps 5000" << std::endl;
}

void runBasicSimulation(const std::shared_ptr<Config>& config) {
    std::cout << "=== Basic Neural Simulation ===" << std::endl;
    
    // Initialize brain with configuration
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    brain->logStatus();
    
    // Get simulation parameters
    int64_t steps = config->getOr<int64_t>("simulation_steps", 1000);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    std::cout << "Running " << steps << " simulation steps with timestep " 
              << timestep << "s" << std::endl;
    
    // Run simulation
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int64_t step = 0; step < steps; ++step) {
        brain->step(step, step * timestep);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << "/" << steps 
                      << " | Spikes: " << brain->getTotalSpikeCount()
                      << " | Firing: " << brain->getFiringNeuronCount() 
                      << " | Rate: " << brain->getAverageFiringRate() << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << std::endl;
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Duration: " << duration.count() << "ms" << std::endl;
    std::cout << "Average speed: " << (steps * 1000.0 / duration.count()) << " steps/ms" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Final firing rate: " << brain->getAverageFiringRate() << std::endl;
}

void runAgentWorldSimulation(const std::shared_ptr<Config>& config) {
    std::cout << "=== Agent-in-World Simulation ===" << std::endl;
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create agent interface
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Create and configure world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(20, 20, 8, 8); // width, height, visionWidth, visionHeight
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    agent->initialize(world);
    
    // Get simulation parameters
    int64_t steps = config->getOr<int64_t>("simulation_steps", 1000);
    double timestep = config->getOr<double>("simulation_timestep", 0.1);
    
    std::cout << "Running agent simulation for " << steps << " steps" << std::endl;
    std::cout << "World size: " << world->getWidth() << "x" << world->getHeight() << std::endl;
    
    // Run simulation loop
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int64_t step = 0; step < steps; ++step) {
        // Update world
        world->update(timestep);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step, step * timestep);
        
        // Get action from brain
        auto action = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply neuromodulation if enabled
        if (agent->isRewardModulationEnabled()) {
            // Simple reward based on position (move toward center)
            double distance = std::sqrt(std::pow(world->getAgentBody().x - 10.0, 2) + 
                                      std::pow(world->getAgentBody().y - 10.0, 2));
            double reward = std::max(0.0, 1.0 - distance / 10.0);
            agent->applyRewardModulation(reward, 0.0);
        }
        
        // Update development if enabled
        if (agent->isDevelopmentEnabled()) {
            agent->updateDevelopment(timestep);
        }
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << "/" << steps 
                      << " | Pos: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")"
                      << " | Energy: " << world->getAgentBody().energy
                      << " | Age: " << world->getAgentBody().age
                      << " | Curiosity: " << agent->getCuriosityLevel()
                      << " | Novelty: " << agent->getNoveltyLevel() << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << std::endl;
    std::cout << "=== Agent Simulation Complete ===" << std::endl;
    std::cout << "Duration: " << duration.count() << "ms" << std::endl;
    std::cout << "Final position: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")" << std::endl;
    std::cout << "Final energy: " << world->getAgentBody().energy << std::endl;
    std::cout << "Final age: " << world->getAgentBody().age << std::endl;
    std::cout << "Final developmental stage: " << brain->getDevelopmentalStage() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
}

void saveBrainState(const std::string& filepath, const std::shared_ptr<Brain>& brain) {
    if (brain->save(filepath)) {
        std::cout << "Brain state saved to: " << filepath << std::endl;
    } else {
        std::cerr << "Failed to save brain state to: " << filepath << std::endl;
    }
}

void loadBrainState(const std::string& filepath, const std::shared_ptr<Config>& config) {
    auto brain = std::make_shared<Brain>(config);
    if (brain->load(filepath)) {
        std::cout << "Brain state loaded from: " << filepath << std::endl;
        brain->logStatus();
    } else {
        std::cerr << "Failed to load brain state from: " << filepath << std::endl;
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string brainFile = "";
    std::string loadFile = "";
    int64_t steps = 1000;
    bool verbose = false;
    bool show_help = false;
    bool show_version = false;
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            show_help = true;
        } else if (arg == "--version") {
            show_version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 6) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                steps = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                steps = std::stoll(argv[++i]);
            }
        } else if (arg.substr(0, 13) == "--brain-file") {
            if (arg.find('=') != std::string::npos) {
                brainFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                brainFile = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--load") {
            if (arg.find('=') != std::string::npos) {
                loadFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                loadFile = argv[++i];
            }
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
            show_help = true;
        }
    }
    
    if (show_help) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (show_version) {
        printVersionInfo();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6 Final Integration ===");
    NLM_LOG_INFO("Version: " + std::string(NLM_VERSION));
    NLM_LOG_INFO("Configuration file: " + configFile);
    NLM_LOG_INFO("Simulation steps: " + std::to_string(steps));
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
        
        // Override with command line arguments
        config->loadFromArgs(argc, argv);
        
        // Override simulation steps
        config->set("simulation_steps", steps, ConfigSource::CommandLine);
        
    } else {
        NLM_LOG_WARNING("Using default configuration (file not found: " + configFile + ")");
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration summary:");
    NLM_LOG_INFO(config->summary());
    
    std::cout << std::endl;
    printVersionInfo();
    std::cout << std::endl;
    
    // Check if we need to load/save brain state
    std::shared_ptr<Brain> brain;
    
    if (!loadFile.empty()) {
        NLM_LOG_INFO("Loading brain state from: " + loadFile);
        brain = std::make_shared<Brain>(config);
        if (!brain->load(loadFile)) {
            std::cerr << "Failed to load brain state. Starting with fresh brain." << std::endl;
            brain = std::make_shared<Brain>(config);
        }
    } else {
        // Create fresh brain
        brain = std::make_shared<Brain>(config);
    }
    
    if (!brainFile.empty()) {
        NLM_LOG_INFO("Will save brain state after simulation to: " + brainFile);
    }
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Choose simulation mode based on configuration
    std::string environment = config->getOr<std::string>("environment", "basic");
    
    if (environment == "agent_world" || environment.find("agent") != std::string::npos) {
        runAgentWorldSimulation(config);
    } else {
        runBasicSimulation(config);
    }
    
    // Save brain state if requested
    if (!brainFile.empty()) {
        saveBrainState(brainFile, brain);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Simulation Complete ===");
    NLM_LOG_INFO("Brain state saved to: " + brainFile);
    NLM_LOG_INFO("Thank you for using NLM (熙然) Neural Learning Machine!");
    
    return 0;
}
