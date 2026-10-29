// NLM (熙然) - Neural Learning Machine
// Main application implementation
//

#include "Main.hpp"
#include "Banner.hpp"
#include "Experiments.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <algorithm>

using namespace nlm;

NLMApplication::NLMApplication() : verboseOutput(false) {
    logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

NLMApplication::~NLMApplication() {
    Logger::setGlobal(nullptr);
}

int NLMApplication::run(int argc, char** argv) {
    printBanner();
    
    std::string configFile = "configs/default.cfg";
    std::string mode = "default";
    int steps = 0;
    bool showHelp = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                mode = argv[++i];
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            }
        } else if (arg == "--verbose") {
            verboseOutput = true;
        } else if (arg == "--version") {
            std::cout << "NLM Neural Learning Machine v2.0.0 (Phase 2)" << std::endl;
            std::cout << "Built for C++20 with real spiking neural computation" << std::endl;
            return 0;
        } else {
            std::cout << "Warning: Unknown option " << arg << " ignored" << std::endl;
        }
    }
    
    if (showHelp) {
        showInfo();
        return 0;
    }
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " + configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Log configuration summary
    std::cout << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
    std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
    std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
    std::cout << std::endl;
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    std::cout << "Simulation clock initialized with timestep: " << timestep << "s" << std::endl;
    
    // Initialize brain
    std::cout << std::endl;
    std::cout << "Initializing NLM Brain..." << std::endl;
    auto brain = createAndInitializeBrain(config);
    
    if (!brain) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    // Run experiments based on mode
    if (mode == "full" || mode == "default") {
        // Reset brain for first experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 1: Basic connectivity
        runBasicConnectivityTest(brain);
        
        // Reset and run Test 2: Plasticity learning experiment
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        
        runStdpVerification(brain);
    } else if (mode == "connectivity") {
        runBasicConnectivityTest(brain);
    } else if (mode == "plasticity") {
        runPlasticityExperiment(brain);
    } else if (mode == "stdp") {
        runStdpVerification(brain);
    } else {
        std::cout << "ERROR: Unknown mode '" + mode + "'. Available modes: full, connectivity, plasticity, stdp" << std::endl;
        return 1;
    }
    
    // Final brain status
    std::cout << std::endl;
    std::cout << "=== Final Brain Status ===" << std::endl;
    brain->logStatus();
    
    std::cout << std::endl;
    std::cout << "=== Phase 2 Complete ===" << std::endl;
    
    showInfo();
    
    return 0;
}

bool NLMApplication::runPhase2Experiments(const std::shared_ptr<Config>& config) {
    // Create brain
    auto brain = createAndInitializeBrain(config);
    if (!brain) {
        return false;
    }
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    
    runStdpVerification(brain);
    
    return true;
}

bool NLMApplication::runExperiment(const std::string& mode, const std::shared_ptr<Config>& config, int steps) {
    auto brain = createAndInitializeBrain(config);
    if (!brain) {
        return false;
    }
    
    if (mode == "connectivity") {
        runBasicConnectivityTest(brain);
    } else if (mode == "plasticity") {
        runPlasticityExperiment(brain);
    } else if (mode == "stdp") {
        runStdpVerification(brain);
    } else {
        return false;
    }
    
    return true;
}

void NLMApplication::initializeLogging(const std::shared_ptr<Config>& config) {
    // Initialize logger based on configuration
}

std::shared_ptr<Brain> NLMApplication::createAndInitializeBrain(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return nullptr;
    }
    
    brain->logStatus();
    return brain;
}

void NLMApplication::showInfo() const {
    std::cout << std::endl;
    std::cout << "NLM Neural Learning Machine" << std::endl;
    std::cout << "Version: 2.0.0 (Phase 2)" << std::endl;
    std::cout << "Real spiking neural computation with LIF neurons" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neuron dynamics" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
}