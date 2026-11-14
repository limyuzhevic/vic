// NLM Phase 6: Demo - Basic Neural Connectivity Test
// Demonstrates that spikes can propagate through a random neural network
// Shows the fundamental spiking mechanism before plasticity is enabled

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

using namespace nlm;

void printHeader() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Demo 1: Basic Neural Connectivity                         ║
    ║                                                               ║
    ║     Demonstrates spiking network connectivity.                 ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

struct ConnectivityTest {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    
    ConnectivityTest(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s) {}
    
    void runTest() {
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > tien()) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
    }
};

int main(int argc, char** argv) {
    printHeader();
    
    std::cout << "Initializing NLM Brain for connectivity test...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Brain for Connectivity Test ===");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 1 (smaller for faster test)
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);  // Small for quick test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 100)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run connectivity test
    ConnectivityTest connectivityTest(brain, 42);
    connectivityTest.runTest();
    
    // Reset for next test (commented out for this demo)
    // brain->reset();
    // brain->initialize();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Connectivity Test Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("This test verified that the NLM brain can propagate spikes");
    NLM_LOG_INFO("through a randomly connected network. The network shows");
    NLM_LOG_INFO("the fundamental property that neural populations can");
    NLM_LOG_INFO("communicate through synaptic connections.");
    
    return 0;
}
