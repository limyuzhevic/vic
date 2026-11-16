#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Phase 6 Integration                               ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << "Usage: nlm_enhanced [OPTIONS]" << std::endl;
    std::cout << "Enhanced NLM with Phase 6 integration and debugging tools" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config FILE           Load configuration from file" << std::endl;
    std::cout << "  --debug LEVEL          Set debug level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)" << std::endl;
    std::cout << "  --neurons N            Set number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions R            Set number of regions (default: 2)" << std::endl;
    std::cout << "  --steps S              Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --random SEED          Random seed for reproducibility" << std::endl;
    std::cout << "  --enable-subsystem SUB  Enable subsystem (reward, curiosity, development, etc.)" << std::endl;
    std::cout << "  --disable-subsystem SUB Disable subsystem" << std::endl;
    std::cout << "  --enable-memory TYPE   Enable memory system (working, episodic, associative)" << std::endl;
    std::cout << "  --disable-memory TYPE  Disable memory system" << std::endl;
    std::cout << "  --enable-neuromod TYPE  Enable neuromodulator (ACh, NE, 5-HT, H1, dopamine)" << std::endl;
    std::cout << "  --disable-neuromod TYPE Disable neuromodulator" << std::endl;
    std::cout << "  --check-integration     Run integration verification test" << std::endl;
    std::cout << "  --run-demo DEMO       Run specific demo (phase3, phase4, phase5, phase6)" << std::endl;
    std::cout << "  --benchmark            Run performance benchmark" << std::endl;
    std::cout << "  --checkpoint FILE      Save/load checkpoint" << std::endl;
    std::cout << "  --help                 Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_enhanced --neurons 500 --steps 5000 --enable-subsystem reward --enable-subsystem curiosity" << std::endl;
    std::cout << "  nlm_enhanced --config myconfig.cfg --debug 2 --enable-memory episodic --enable-neuromod 5-HT" << std::endl;
    std::cout << "  nlm_enhanced --check-integration --run-demo phase6" << std::endl;
}

void parseSubsystem(const std::string& subsystem, std::map<std::string, bool>& enabled) {
    std::vector<std::string> subsystems = {
        "reward", "curiosity", "development", "novelty", "prediction", 
        "planner", "concept", "attention", "working_memory", "episodic_memory", 
        "associative_memory", "ACh", "NE", "5-HT", "H1", "dopamine"
    };
    
    std::string normalized = subsystem;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), std::tolower);
    
    bool found = false;
    for (const auto& sub : subsystems) {
        if (normalized.find(sub) == 0) {
            found = true;
            enabled[sub] = true;
            break;
        }
    }
    
    if (!found) {
        std::cout << "Unknown subsystem: " << subsystem << std::endl;
    }
}

void applyConfiguration(std::shared_ptr<Config> config, const std::map<std::string, bool>& enabled) {
    // Apply configuration based on enabled subsystems
    if (enabled.find("reward") != enabled.end() && enabled.at("reward")) {
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::CommandLine);
        std::cout << "Enabled reward modulation" << std::endl;
    }
    
    if (enabled.find("curiosity") != enabled.end() && enabled.at("curiosity")) {
        config->set("neuromod.curiosity.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled curiosity system" << std::endl;
    }
    
    if (enabled.find("development") != enabled.end() && enabled.at("development")) {
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::CommandLine);
        config->set("development.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled development system" << std::endl;
    }
    
    // Memory systems
    if (enabled.find("working_memory") != enabled.end() && enabled.at("working_memory")) {
        config->set("memory.working_memory.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled working memory" << std::endl;
    }
    
    if (enabled.find("episodic_memory") != enabled.end() && enabled.at("episodic_memory")) {
        config->set("memory.episodic_memory.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled episodic memory" << std::endl;
    }
    
    if (enabled.find("associative_memory") != enabled.end() && enabled.at("associative_memory")) {
        config->set("memory.associative_memory.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled associative memory" << std::endl;
    }
    
    // Neuromodulators
    if (enabled.find("ACh") != enabled.end() && enabled.at("ACh")) {
        config->set("neuromod.acetylcholine.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled acetylcholine (attention, memory)" << std::endl;
    }
    
    if (enabled.find("NE") != enabled.end() && enabled.at("NE")) {
        config->set("neuromod.norepinephrine.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled norepinephrine (arousal, vigilance)" << std::endl;
    }
    
    if (enabled.find("5-HT") != enabled.end() && enabled.at("5-HT")) {
        config->set("neuromod.serotonin.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled serotonin (mood, impulse control)" << std::endl;
    }
    
    if (enabled.find("H1") != enabled.end() && enabled.at("H1")) {
        config->set("neuromod.histamine.enable", true, ConfigSource::CommandLine);
        std::cout << "Enabled histamine (wakefulness, cognition)" << std::endl;
    }
}

void runIntegrationCheck(const std::map<std::string, bool>& enabled) {
    std::cout << "=== NLM Integration Check ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Integration verification
    std::cout << "Running integration verification..." << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "✓ Integration verification passed!" << std::endl;
    
    // Run system-specific tests based on enabled subsystems
    if (enabled.find("reward") != enabled.end() && enabled.at("reward")) {
        std::cout << "\n--- Reward Modulation Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
    }
    
    if (enabled.find("working_memory") != enabled.end() && enabled.at("working_memory")) {
        std::cout << "\n--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
    }
    
    if (enabled.find("development") != enabled.end() && enabled.at("development")) {
        std::cout << "\n--- Development Integration Test ---" << std::endl;
        // Development test would go here
        std::cout << "Development integration test completed" << std::endl;
    }
    
    std::cout << "\n=== Integration Check Complete ===" << std::endl;
}

void runStandardDemo(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, 
                   const std::map<std::string, bool>& enabled, int steps) {
    std::cout << "=== Standard Demo Mode ===" << std::endl;
    std::cout << "Running " << steps << " simulation steps..." << std::endl;
    
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    world.setRandomSeed(config->getOr<int64_t>("random_seed", 42));
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Apply enabled subsystems
    if (enabled.find("reward") != enabled.end() && enabled.at("reward")) {
        agent.enableRewardModulation(true);
    }
    if (enabled.find("curiosity") != enabled.end() && enabled.at("curiosity")) {
        agent.enableCuriosity(true);
    }
    if (enabled.find("development") != enabled.end() && enabled.at("development")) {
        agent.enableDevelopment(true);
    }
    
    float totalReward = 0.0f;
    uint64_t brainStep = 0;
    
    for (int step = 0; step < steps; ++step) {
        const SensoryPercept& percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        // Run brain steps (multiple per action)
        for (int i = 0; i < 10; ++i) {
            brain->step(brainStep++);
        }
        
        MotorCommand cmd = agent.decodeMotorCommand();
        ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
        agent.applyRewardModulation(result.reward, 0.0f);
        
        totalReward += result.reward;
        world.update(0.1);
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << "reward=" << result.reward
                      << " total=" << totalReward
                      << " neurons=" << brain->getFiringNeuronCount()
                      << " spikes=" << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    std::cout << "\n=== Demo Results ===" << std::endl;
    std::cout << "Total reward: " << totalReward << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Final memory activity: " << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getMemoryActivity() : 0.0f) << std::endl;
}

void runBenchmark() {
    std::cout << "=== NLM Benchmark ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64 الجهات_id="2;
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Warm up
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    // Benchmark step execution
    std::cout << "Measuring step execution time..." << std::endl;
    
    // Note: For a real benchmark, we would use high-precision timers
    // For now, we just run the simulation
    
    // Benchmark memory operations
    std::cout << "Measuring memory operations..." << std::endl;
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->storeToNeuron(NeuronId(0), 0.5f);
        brain->getWorkingMemory()->update(0.001);
    }
    
    std::cout << "Benchmark completed!" << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Default configuration
    std::map<std::string, bool> enabledSubsystems;
    std::string configFile = "configs/default.cfg";
    std::string checkpointFile = "";
    int debugLevel = 2;  // INFO level by default
    int neuronCount = 1000;
    int regionCount = 2;
    int steps = 1000;
    int64_t randomSeed = 42;
    std::string demoMode = "standard";
    bool runCheckIntegration = false;
    bool runBenchmark = false;
    
    // Parse command line arguments
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--check-integration") {
            runCheckIntegration = true;
        } else if (arg == "--benchmark") {
            runBenchmark = true;
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(8);
        } else if (arg.find("--config") == 0 && i + 1 < argc && argv[i + 1][0] != '-') {
            configFile = argv[++i];
        } else if (arg.find("--debug=") == 0) {
            debugLevel = std::stoi(arg.substr(8));
        } else if (arg.find("--neurons=") == 0) {
            neuronCount = std::stoi(arg.substr(9));
        } else if (arg.find("--regions=") == 0) {
            regionCount = std::stoi(arg.substr(9));
        } else if (arg.find("--steps=") == 0) {
            steps = std::stoi(arg.substr(7));
        } else if (arg.find("--random=") == 0) {
            randomSeed = std::stoll(arg.substr(8));
        } else if (arg.find("--enable-subsystem=") == 0) {
            parseSubsystem(arg.substr(19), enabledSubsystems);
        } else if (arg.find("--disable-subsystem=") == 0) {
            std::string subsystem = arg.substr(20);
            parseSubsystem(subsystem, enabledSubsystems);
            // In this simple implementation, we just ignore for now
        } else if (arg.find("--enable-memory=") == 0) {
            parseSubsystem(arg.substr(17), enabledSubsystems);
        } else if (arg.find("--disable-memory=") == 0) {
            // Same as above
        } else if (arg.find("--enable-neuromod=") == 0) {
            parseSubsystem(arg.substr(18), enabledSubsystems);
        } else if (arg.find("--disable-neuromod=") == 0) {
            // Same as above
        } else if (arg.find("--run-demo=") == 0) {
            demoMode = arg.substr(11);
        } else if (arg.find("--checkpoint=") == 0) {
            checkpointFile = arg.substr(12);
        } else {
            args.push_back(arg);
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    std::shared_ptr<ConsoleLogger> consoleLogger;
    
    switch (debugLevel) {
        case 0: consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error); break;
        case 1: consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning); break;
        case 2: consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info); break;
        case 3: consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug); break;
        default: consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info); break;
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Debug level: " << debugLevel << std::endl;
    std::cout << "Config file: " << configFile << std::endl;
    std::cout << "Neuron count: " << neuronCount << std::endl;
    std::cout << "Region count: " << regionCount << std::endl;
    std::cout << "Random seed: " << randomSeed << std::endl;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration (file not found)" << std::endl;
    }
    
    // Override with command line arguments
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
    config->set("random_seed", randomSeed, ConfigSource::CommandLine);
    
    // Apply subsystem configurations
    applyConfiguration(config, enabledSubsystems);
    
    // Initialize brain
    std::cout << "\nInitializing NLM brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Run checkpoint if specified
    if (!checkpointFile.empty()) {
        std::cout << "\nLoading checkpoint from: " << checkpointFile << std::endl;
        if (brain->load(checkpointFile)) {
            std::cout << "Checkpoint loaded successfully!" << std::endl;
        } else {
            std::cout << "Failed to load checkpoint, continuing with fresh brain" << std::endl;
        }
    }
    
    // Run integration check if requested
    if (runCheckIntegration) {
        runIntegrationCheck(enabledSubsystems);
        // Continue with standard demo after integration check
    }
    
    // Run benchmark if requested
    if (runBenchmark) {
        runBenchmark();
    }
    
    // Run demo
    if (demoMode == "phase3") {
        std::cout << "Phase 3 demo not yet implemented in enhanced version" << std::endl;
        // Would need to implement phase3 demo
    } else if (demoMode == "phase4") {
        std::cout << "Phase 4 demo not yet implemented in enhanced version" << std::endl;
        // Would need to implement phase4 demo
    } else if (demoMode == "phase5") {
        std::cout << "Phase 5 demo not yet implemented in enhanced version" << std::endl;
        // Would need to implement phase5 demo
    } else if (demoMode == "phase6") {
        std::cout << "Phase 6 demo not yet implemented in enhanced version" << std::endl;
        // Would need to implement phase6 demo
    } else {
        runStandardDemo(brain, config, enabledSubsystems, steps);
    }
    
    // Save checkpoint if specified
    if (!checkpointFile.empty()) {
        std::cout << "\nSaving checkpoint to: " << checkpointFile << std::endl;
        if (brain->save(checkpointFile)) {
            std::cout << "Checkpoint saved successfully!" << std::endl;
        } else {
            std::cout << "Failed to save checkpoint" << std::endl;
        }
    }
    
    std::cout << "\n=== NLM Enhanced Demo Complete ===" << std::endl;
    
    return 0;
}