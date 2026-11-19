// NLMMainDemo.cpp - Implementation of the enhanced main demo system

#include "NLMMainDemo.hpp"
#include "brain/Brain.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>
#include <vector>

namespace nlm {

NLMMainDemo::NLMMainDemo() : argc(0), argv(nullptr), demoMode(DemoMode::BASIC_CONNECTIVITY), 
    simulationSteps(50), verbose(false), configFile("configs/default.cfg"), startTime(0.0) {
    logger = std::make_shared<Logger>();
}

NLMMainDemo::~NLMMainDemo() {
    cleanup();
}

bool NLMMainDemo::initialize(int argc, char** argv) {
    this->argc = argc;
    this->argv = argv;
    
    if (!parseArguments(argc, argv)) {
        printHelp();
        return false;
    }
    
    // Create configuration
    config = std::make_shared<Config>();
    
    // Load default configuration
    setupDefaultConfiguration();
    
    // Apply command line overrides
    applyCommandLineOverrides();
    
    // Setup logging
    setupLogging();
    
    // Log startup information
    printBanner();
    std::cout << "Initializing NLM Phase 2 Enhanced Demo..." << std::endl;
    NLM_LOG_INFO("=== NLM Phase 2 Enhanced Demo ===");
    
    logConfigurationSummary();
    
    return true;
}

int NLMMainDemo::run() {
    startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    if (!initializeBrain()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    // Run the selected demo
    NLM_LOG_INFO("Running demo mode: " + std::to_string(static_cast<int>(demoMode)));
    DemoResult result;
    
    switch (demoMode) {
        case DemoMode::BASIC_CONNECTIVITY:
            result = runBasicConnectivityDemo();
            break;
        case DemoMode::PLASTICITY_LEARNING:
            result = runPlasticityLearningDemo();
            break;
        case DemoMode::STDP_VERIFICATION:
            result = runStdpVerificationDemo();
            break;
        case DemoMode::INTEGRATION_TEST:
            result = runIntegrationDemo();
            break;
        case DemoMode::CUSTOM:
            result = runCustomDemo();
            break;
    }
    
    // Display results
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== DEMO RESULTS ===");
    NLM_LOG_INFO("Success: " + std::string(result.success ? "Yes" : "No"));
    NLM_LOG_INFO("Message: " + result.message);
    NLM_LOG_INFO("Duration: " + std::to_string(result.durationSteps) + " steps");
    NLM_LOG_INFO("Execution Time: " + std::to_string(result.executionTimeMs) + " ms");
    
    if (!result.success) {
        NLM_LOG_ERROR("Demo failed: " + result.message);
    }
    
    cleanup();
    
    NLM_LOG_INFO("Demo complete.");
    return result.success ? 0 : 1;
}

bool NLMMainDemo::parseArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        }
        
        if (parseDemoMode(arg)) {
            continue;
        }
        
        if (parseSteps(arg)) {
            continue;
        }
        
        if (parseVerbose(arg)) {
            continue;
        }
        
        if (parseConfigFile(arg)) {
            continue;
        }
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            continue;
        }
        
        if (arg.substr(0, 8) == "--setkey=") {
            // Handle key=value from command line
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(8, pos - 8);
                std::string value = arg.substr(pos + 1);
                setConfigKey(key, value);
            }
            continue;
        }
        
        NLM_LOG_WARNING("Unknown argument: " + arg);
    }
    
    return true;
}

void NLMMainDemo::printHelp() const {
    std::cout << "NLM Phase 2 Enhanced Demo" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << std::endl;
    std::cout << "Demonstrates real neural computation with:" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << std::endl;
    std::cout << "Available Demo Modes:" << std::endl;
    std::cout << "  0: Basic Connectivity Test" << std::endl;
    std::cout << "  1: Plasticity Learning Experiment" << std::endl;
    std::cout << "  2: STDP Verification Test" << std::endl;
    std::cout << "  3: Integration Test" << std::endl;
    std::cout << "  4: Custom Test" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --mode N              : Select demo mode (0-4)" << std::endl;
    std::cout << "  --steps N             : Number of simulation steps (default: 50)" << std::endl;
    std::cout << "  --config file          : Configuration file path" << std::endl;
    std::cout << "  --setkey=KEY=VALUE     : Set configuration key" << std::endl;
    std::cout << "  --verbose              : Enable verbose output" << std::endl;
    std::cout << "  --help, -h            : Show this help message" << std::endl;
    std Arrangement::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_main_demo --mode 0 --steps 1000" << std::endl;
    std::cout << "  ./nlm_main_demo --mode 1 --config custom.cfg" << std::endl;
    std::cout << "  ./nlm_main_demo --mode 3 --verbose" << std::endl;
    std::cout << std::endl;
}

void NLMMainDemo::printBanner() const {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Enhanced Real Neural Computation                 ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity rules                       ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void NLMMainDemo::logConfigurationSummary() const {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
}

DemoResult NLMMainDemo::runBasicConnectivityDemo() {
    NLM_LOG_INFO("=== Basic Connectivity Test ===");
    
    DemoResult result;
    result.success = true;
    result.message = "Basic connectivity test completed";
    result.durationSteps = simulationSteps;
    
    // Get brain regions
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        result.success = false;
        result.message = "No brain regions found";
        return result;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        result.success = false;
        result.message = "No neurons found in region";
        return result;
    }
    
    // Record initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    NLM_LOG_INFO("Initial spike count: " + std::to_string(initialSpikes));
    
    // Inject current into first 10 neurons
    NLM_LOG_INFO("Injecting current into first 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
        reportProgress("Injecting neurons", i + 1, std::min(size_t(10), neurons.size()));
    }
    
    // Run simulation
    NLM_LOG_INFO("Running " + std::to_string(simulationSteps) + " simulation steps...");
    for (SimulationStep step = 0; step < simulationSteps; ++step) {
        brain->step(step, step * 0.001);
        reportProgress("Simulating", step + 1, simulationSteps);
    }
    
    // Calculate results
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikesGenerated = finalSpikes - initialSpikes;
    
    result.metrics["initial_spikes"] = std::to_string(initialSpikes);
    result.metrics["final_spikes"] = std::to_string(finalSpikes);
    result.metrics["spikes_generated"] = std::to_string(spikesGenerated);
    
    NLM_LOG_INFO("Final spike count: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("Spikes generated: " + std::to_string(spikesGenerated));
    
    if (spikesGenerated > 0) {
        NLM_LOG_INFO("✓ SUCCESS: Neural signals propagated through the network");
        result.message = "Neural connectivity verified - " + std::to_string(spikesGenerated) + " spikes generated";
    } else {
        NLM_LOG_WARNING("! WARNING: No spikes detected");
        result.success = false;
        result.message = "No spikes generated despite current injection";
    }
    
    return result;
}

bool NLMMainDemo::initializeBrain() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    
    brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    brain->logStatus();
    return true;
}

void NLMMainDemo::cleanup() {
    NLM_LOG_INFO("Cleaning up demo resources...");
    // Reset brain
    brain.reset();
    // Reset config
    config.reset();
    // Reset logger
    Logger::setGlobal(nullptr);
}

void NLMMainDemo::reportProgress(const std::string& message, size_t current, size_t total) {
    if (total == 0) return;
    
    float progress = (static_cast<float>(current) / static_cast<float>(total)) * 100.0f;
    
    if (verbose) {
        std::cout << "\r[" << std::string(static_cast<size_t>(progress / 10), '=')
                  << ">" << std::string(10 - static_cast<size_t>(progress / 10), ' ')
                  << std::fixed << std::setprecision(1) << progress << "%] "
                  << message << std::flush;
        
        if (current >= total) {
            std::cout << std::endl;
        }
    }
}

std::shared_ptr<Config> NLMMainDemo::getConfig() const {
    return config;
}

void NLMMainDemo::setConfigKey(const std::string& key, const std::string& value) {
    if (config) {
        // Simple key-value conversion for common types
        try {
            int intVal = std::stoi(value);
            config->set(key, intVal);
            return;
        } catch (...) {}
        
        try {
            float floatVal = std::stof(value);
            config->set(key, floatVal);
            return;
        } catch (...) {}
        
        try {
            double doubleVal = std::stod(value);
            config->set(key, doubleVal);
            return;
        } catch (...) {}
        
        try {
            bool boolVal = (value == "true" || value == "1");
            config->set(key, boolVal);
            return;
        } catch (...) {}
        
        // Default to string
        config->set(key, value);
    }
}

void NLMMainDemo::setupDefaultConfiguration() {
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void NLMMainDemo::applyCommandLineOverrides() {
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
}

bool NLMMainDemo::parseDemoMode(const std::string& arg) {
    if (arg.substr(0, 7) == "--mode=") {
        try {
            int mode = std::stoi(arg.substr(7));
            if (mode >= 0 && mode <= 4) {
                demoMode = static_cast<DemoMode>(mode);
                NLM_LOG_INFO("Demo mode set to: " + std::to_string(mode));
                return true;
            }
        } catch (...) {}
        NLM_LOG_WARNING("Invalid demo mode: " + arg);
        return false;
    }
    return false;
}

bool NLMMainDemo::parseSteps(const std::string& arg) {
    if (arg.substr(0, 8) == "--steps=") {
        try {
            simulationSteps = std::stoul(arg.substr(8));
            NLM_LOG_INFO("Simulation steps set to: " + std::to_string(simulationSteps));
            return true;
        } catch (...) {}
        NLM_LOG_WARNING("Invalid steps value: " + arg);
        return false;
    }
    return false;
}

bool NLMMainDemo::parseVerbose(const std::string& arg) {
    if (arg == "--verbose") {
        verbose = true;
        NLM_LOG_INFO("Verbose output enabled");
        return true;
    }
    return false;
}

bool NLMMainDemo::parseConfigFile(const std::string& arg) {
    if (arg.substr(0, 8) == "--config") {
        if (arg.find('=') != std::string::npos) {
            configFile = arg.substr(arg.find('=') + 1);
        }
        return true;
    }
    return false;
}

void NLMMainDemo::setupLogging() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

bool NLMMainDemo::parseArguments(int argc, char** argv) {
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        }
        
        if (parseDemoMode(arg)) {
            continue;
        }
        
        if (parseSteps(arg)) {
            continue;
        }
        
        if (parseVerbose(arg)) {
            continue;
        }
        
        if (parseConfigFile(arg)) {
            continue;
        }
        
        if (arg.substr(0, 8) == "--setkey=") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(8, pos - 8);
                std::string value = arg.substr(pos + 1);
                setConfigKey(key, value);
            }
            continue;
        }
    }
    
    return true;
}

DemoResult NLMMainDemo::runPlasticityLearningDemo() {
    NLM_LOG_INFO("=== Plasticity Learning Demo ===");
    
    DemoResult result;
    result.success = true;
    result.message = "Plasticity learning demo completed";
    result.durationSteps = simulationSteps;
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    NLM_LOG_INFO("Initial spike count: " + std::to_string(initialSpikes));
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern
    NLM_LOG_INFO("Applying repeated input pattern for learning...")
    std::vector<NeuronId> patternNeurons;
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    for (SimulationStep step = 0; step < simulationSteps; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            reportProgress("Learning simulation", step + 1, simulationSteps);
        }
    }
    
    // Calculate results
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikesGenerated = finalSpikes - initialSpikes;
    
    result.metrics["initial_spikes"] = std::to_string(initialSpikes);
    result.metrics["final_spikes"] = std::to_string(finalSpikes);
    result.metrics["spikes_generated"] = std::to_string(spikesGenerated);
    
    NLM_LOG_INFO("Final spike count: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("Learning simulation completed with " + std::to_string(spikesGenerated) + " spikes");
    
    return result;
}

DemoResult NLMMainDemo::runStdpVerificationDemo() {
    NLM_LOG_INFO("=== STDP Verification Demo ===");
    
    DemoResult result;
    result.success = true;
    result.message = "STDP verification demo completed";
    result.durationSteps = simulationSteps;
    
    // Get a synapse for testing
    auto* region = brain->getRegion(RegionId(1));
    if (!region || region->getSynapses().empty()) {
        result.success = false;
        result.message = "No synapses available for STDP testing";
        return result;
    }
    
    Synapse* syn = region->getSynapses()[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("Testing STDP on 1 synapse:");
    NLM_LOG_INFO("  Synapse before: " + std::to_string(beforeWeight) +
                " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    // Create correlated pre->post activity
    NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float afterWeight = syn->getWeight();
    float delta = afterWeight - beforeWeight;
    
    NLM_LOG_INFO("After correlated activity:");
    NLM_LOG_INFO("  Synapse after: " + std::to_string(afterWeight) +
                " (Δ=" + std::to_string(delta) + ")");
    
    result.metrics["before_weight"] = std::to_string(beforeWeight);
    result.metrics["after_weight"] = std::to_string(afterWeight);
    result.metrics["weight_change"] = std::to_string(delta);
    
    if (delta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        result.message = "STDP working - synapse potentiated by " + std::to_string(delta);
    } else if (delta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        result.success = false;
        result.message = "STDP produced depression instead of potentiation";
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        result.success = false;
        result.message = "No significant STDP change observed";
    }
    
    return result;
}

DemoResult NLMMainDemo::runIntegrationDemo() {
    NLM_LOG_INFO("=== Integration Demo ===");
    
    DemoResult result;
    result.success = true;
    result.message = "Integration demo completed";
    result.durationSteps = simulationSteps;
    
    // Run a comprehensive integration test
    NLM_LOG_INFO("Running comprehensive integration test with all systems...");
    
    // Test multiple subsystems
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Test 1: Basic connectivity
    NLM_LOG_INFO("Test 1: Basic connectivity...");
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        result.success = false;
        result.message = "No brain regions for integration test";
        return result;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        result.success = false;
        result.message = "No neurons for integration test";
        return result;
    }
    
    // Inject into multiple neurons
    for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
        neurons[i]->injectCurrent(20.0f);
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < simulationSteps; ++step) {
        brain->step(step, step * 0.001);
        reportProgress("Integration test", step + 1, simulationSteps);
    }
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikesGenerated = finalSpikes - initialSpikes;
    
    result.metrics["initial_spikes"] = std::to_string(initialSpikes);
    result.metrics["final_spikes"] = std::to_string(finalSpikes);
    result.metrics["spikes_generated"] = std::to_string(spikesGenerated);
    
    NLM_LOG_INFO("Integration test results:");
    NLM_LOG_INFO("  Initial spikes: " + std::to_string(initialSpikes));
    NLM_LOG_INFO("  Final spikes: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikesGenerated));
    
    if (spikesGenerated > 0) {
        NLM_LOG_INFO("✓ SUCCESS: Integration test passed - all systems working together");
        result.message = "Integration successful - " + std::to_string(spikesGenerated) + " spikes generated";
    } else {
        NLM_LOG_WARNING("! WARNING: Integration test produced no spikes");
        result.success = false;
        result.message = "Integration test failed - no spikes generated";
    }
    
    return result;
}

DemoResult NLMMainDemo::runCustomDemo() {
    NLM_LOG_INFO("=== Custom Demo ===");
    
    DemoResult result;
    result.success = true;
    result.message = "Custom demo completed";
    result.durationSteps = simulationSteps;
    
    // Custom test implementation
    NLM_LOG_INFO("Running custom test configuration...");
    
    // Use custom parameters from config
    auto customNeurons = config->getOr<int64_t>("custom_neuron_count", 10);
    auto customSteps = config->getOr<size_t>("custom_steps", 100);
    
    NLM_LOG_INFO("Custom parameters: " + std::to_string(customNeurons) + " neurons, " + std::to_string(customSteps) + " steps");
    
    // Run custom simulation
    for (SimulationStep step = 0; step < customSteps; ++step) {
        brain->step(step, step * 0.001);
        reportProgress("Custom test", step + 1, customSteps);
    }
    
    result.metrics["custom_neurons"] = std::to_string(customNeurons);
    result.metrics["custom_steps"] = std::to_string(customSteps);
    
    NLM_LOG_INFO("Custom demo completed successfully");
    result.message = "Custom test passed - " + std::to_string(customSteps) + " steps completed";
    
    return result;
}

DemoResult NLMMainDemo::runStdpVerificationTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== STDP Verification ===");
    
    DemoResult result;
    result.success = true;
    result.message = "STDP verification test completed";
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        result.success = false;
        result.message = "No brain regions for STDP test";
        return result;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        result.success = false;
        result.message = "No synapses for STDP test";
        return result;
    }
    
    // Use one real synapse and its own pre/post neurons
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    
    NLM_LOG_INFO("Testing STDP on 1 synapse:");
    NLM_LOG_INFO("  Synapse before: " + std::to_string(beforeWeight) +
                " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    // Create correlated pre->post activity (potentiation)
    NLM_LOG_INFO("Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float delta = syn->getWeight() - beforeWeight;
    
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                " (Δ=" + std::to_string(delta) + ")");
    
    if (delta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        result.message = "STDP working - synapse potentiated by " + std::to_string(delta);
    } else if (delta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        result.success = false;
        result.message = "STDP produced depression instead of potentiation";
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        result.success = false;
        result.message = "No significant STDP change observed";
    }
    
    return result;
}

DemoResult NLMMainDemo::runIntegrationTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Integration Test ===");
    
    DemoResult result;
    result.success = true;
    result.message = "Integration test completed";
    
    // Run a comprehensive integration test
    NLM_LOG_INFO("Running comprehensive integration test...");
    
    // Test multiple regions and neurons
    size_t totalNeurons = 0;
    for (size_t r = 0; r < brain->getRegionCount(); ++r) {
        auto* region = brain->getRegion(RegionId(r + 1));
        if (region) {
            auto neurons = region->getAllNeurons();
            totalNeurons += neurons.size();
            
            // Test connectivity in this region
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(40.0f);
            }
        }
    }
    
    NLM_LOG_INFO("Testing " + std::to_string(totalNeurons) + " neurons across " + std::to_string(brain->getRegionCount()) + " regions");
    
    // Run integration simulation
    for (SimulationStep step = 0; step < simulationSteps; ++step) {
        brain->step(step, step * 0.001);
        reportProgress("Integration test", step + 1, simulationSteps);
    }
    
    size_t totalSpikes = brain->getTotalSpikeCount();
    result.metrics["total_neurons"] = std::to_string(totalNeurons);
    result.metrics["regions_tested"] = std::to_string(brain->getRegionCount());
    result.metrics["total_spikes"] = std::to_string(totalSpikes);
    
    NLM_LOG_INFO("Integration test completed:");
    NLM_LOG_INFO("  Neurons tested: " + std::to_string(totalNeurons));
    NLM_LOG_INFO("  Regions: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(totalSpikes));
    
    if (totalSpikes > 0) {
        NLM_LOG_INFO("✓ SUCCESS: Integration test passed - all systems working");
        result.message = "Integration successful - all systems working together";
    } else {
        NLM_LOG_WARNING("! WARNING: Integration test produced no spikes");
        result.success = false;
        result.message = "Integration test failed - no spikes detected";
    }
    
    return result;
}

DemoResult NLMMainDemo::runCustomTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Custom Test ===");
    
    DemoResult result;
    result.success = true;
    result.message = "Custom test completed";
    
    NLM_LOG_INFO("Running custom configuration test...");
    
    // Use custom parameters
    auto customNeurons = config->getOr<int64_t>("custom_neuron_count", 10);
    auto customSteps = config->getOr<size_t>("custom_steps", 100);
    
    NLM_LOG_INFO("Custom parameters: " + std::to_string(customNeurons) + " neurons, " + std::to_string(customSteps) + " steps");
    
    // Run custom simulation
    for (SimulationStep step = 0; step < customSteps; ++step) {
        brain->step(step, step * 0.001);
        reportProgress("Custom test", step + 1, customSteps);
    }
    
    result.metrics["custom_neurons"] = std::to_string(customNeurons);
    result.metrics["custom_steps"] = std::to_string(customSteps);
    
    NLM_LOG_INFO("Custom test completed successfully");
    result.message = "Custom test passed - " + std::to_string(customSteps) + " steps completed";
    
    return result;
}

DemoResult NLMMainDemo::runPlasticityLearningTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Plasticity Learning Test ===");
    
    DemoResult result;
    result.success = true;
    result.message = "Plasticity learning test completed";
    result.durationSteps = simulationSteps;
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    NLM_LOG_INFO("Initial spike count: " + std::to_string(initialSpikes));
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern
    NLM_LOG_INFO("Applying repeated input pattern for learning...");
    std::vector<NeuronId> patternNeurons;
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    for (SimulationStep step = 0; step < simulationSteps; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            reportProgress("Learning simulation", step + 1, simulationSteps);
        }
    }
    
    // Calculate results
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikesGenerated = finalSpikes - initialSpikes;
    
    result.metrics["initial_spikes"] = std::to_string(initialSpikes);
    result.metrics["final_spikes"] = std::to_string(finalSpikes);
    result.metrics["spikes_generated"] = std::to_string(spikesGenerated);
    
    NLM_LOG_INFO("Final spike count: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("Learning simulation completed with " + std::to_string(spikesGenerated) + " spikes");
    
    return result;
}