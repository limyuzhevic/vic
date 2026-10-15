// Simplified NLM CLI - focuses on ease of use and clear options

#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

namespace nlm {

class SimpleNLMCLI {
public:
    SimpleNLMCLI(int argc, char** argv) : argc(argc), argv(argv) {}
    
    bool run() {
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        printBanner();
        
        try {
            // Parse command line arguments
            if (!parseArguments()) {
                return false;
            }
            
            // Load configuration
            auto config = std::make_shared<Config>();
            
            // Try to load from file if provided
            std::string configFile = "configs/default.cfg";
            for (size_t i = 1; i < static_cast<size_t>(argc); ++i) {
                std::string arg(argv[i]);
                if (arg.substr(0, 7) == "--config") {
                    if (arg.find('=') != std::string::npos) {
                        configFile = arg.substr(arg.find('=') + 1);
                    } else if (i + 1 < static_cast<size_t>(argc)) {
                        configFile = argv[++i];
                    }
                }
            }
            
            // Load config from file
            if (config->loadFromFile(configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + configFile);
            } else {
                NLM_LOG_INFO("Using default configuration.");
            }
            
            // Override with command line args
            config->loadFromArgs(argc, argv);
            
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
            
            // Structural plasticity parameters
            config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
            config->set("pruning_rate", 0.00001f, ConfigSource::Default);
            
            // Log configuration summary
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Configuration:");
            NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
            NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
            NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
            NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
            NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
            NLM_LOG_INFO("");
            
            // Initialize brain
            NLM_LOG_INFO("Initializing NLM Brain (Phase 6 - Integrated)...");
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain!");
                return false;
            }
            
            brain->logStatus();
            
            // Run tests based on mode
            return runTests(brain, config);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error: ") + e.what());
            printUsage();
            return false;
        }
    }
    
private:
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Command Line Interface (CLI)                             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    void printUsage() {
        std::cout << R"(
Usage: nlm [OPTIONS]

Options:
  --config=file.cfg    Load configuration from file (JSON or key=value format)
  --neuron-count=N     Number of neurons (default: 500)
  --region-count=N     Number of regions (default: 1)
  --connection-prob=P  Connection probability (default: 0.15)
  --random-seed=N      Random seed for reproducibility (default: 42)
  --timestep=T         Simulation timestep (default: 0.001)
  --mode=MODE          Run mode: 'plasticity', 'stdp', 'basic', 'all' (default: 'all')
  --help               Show this help message
  --version            Show version information

Examples:
  nlm --mode=plasticity
  nlm --config=custom.cfg --neuron-count=1000
  nlm --timestep=0.0005 --connection-prob=0.2

For more information, visit: https://github.com/nlm/neural-learning-machine

)" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM v2.0 (Phase 6 - Integrated Artificial Brain)" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Features: Real LIF neurons, STDP, Hebbian plasticity, Structural plasticity" << std::endl;
    }
    
    bool parseArguments() {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help") {
                printUsage();
                return false;
            } else if (arg == "--version") {
                printVersion();
                return false;
            }
        }
        return true;
    }
    
    bool runTests(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        std::string mode = config->getOr<std::string>("mode", "all");
        
        if (mode == "plasticity") {
            return runPlasticityTest(brain);
        } else if (mode == "stdp") {
            return runStdpTest(brain);
        } else if (mode == "basic") {
            return runBasicTest(brain);
        } else if (mode == "all") {
            return runAllTests(brain, config);
        } else {
            NLM_LOG_ERROR("Unknown mode: " + mode);
            NLM_LOG_ERROR("Available modes: plasticity, stdp, basic, all");
            return false;
        }
    }
    
    bool runBasicTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Basic Connectivity Test ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto neurons = brain->getTotalNeuronCount();
        if (neurons < 10) {
            NLM_LOG_INFO("  Not enough neurons for basic test");
            return true;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < 10 && i < neurons; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Internal, 50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        }
        return true;
    }
    
    bool runPlasticityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
        
        // Reset and enable plasticity on synapses
        brain->reset();
        brain->initialize();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (500 steps)...");
        
        for (SimulationStep step = 0; step < 500; ++step) {
            // Create input pattern - inject current into neurons
            size_t neuronsToExcite = brain->getTotalNeuronCount() / 4;
            for (size_t i = 0; i < neuronsToExcite && i < 20; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Internal, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("✓ Plasticity test completed");
        return true;
    }
    
    bool runStdpTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== STDP Verification Test ===");
        
        // Reset and enable only STDP
        brain->reset();
        brain->initialize();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto& synapses = region->getSynapses();
            if (synapses.size() >= 5) {
                NLM_LOG_INFO("  Testing STDP on 5 synapses:");
                
                // Record initial weights
                std::vector<float> beforeWeights;
                for (size_t i = 0; i < 5; ++i) {
                    beforeWeights.push_back(synapses[i]->getWeight());
                    synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
                    NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                                " before: " + std::to_string(beforeWeights[i]));
                }
                
                // Create correlated activity: fire pre then post to trigger LTP
                NLM_LOG_INFO("");
                NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
                
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    for (int trial = 0; trial < 20; ++trial) {
                        // Fire pre-synaptic neuron
                        Neuron* preNeuron = neurons[0];
                        Neuron* postNeuron = neurons[1];
                        
                        // Pre fires first
                        preNeuron->injectCurrent(60.0f);
                        brain->step(trial * 2, trial * 2 * 0.001);
                        
                        // Then post fires
                        postNeuron->injectCurrent(60.0f);
                        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                    }
                }
                
                // Record after weights
                NLM_LOG_INFO("  After correlated activity:");
                float totalDelta = 0.0f;
                for (size_t i = 0; i < 5; ++i) {
                    float delta = synapses[i]->getWeight() - beforeWeights[i];
                    NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                                " after: " + std::to_string(synapses[i]->getWeight()) +
                                " (Δ=" + std::to_string(delta) + ")");
                    totalDelta += delta;
                }
                
                NLM_LOG_INFO("");
                if (totalDelta > 0.001f) {
                    NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
                } else if (totalDelta < -0.001f) {
                    NLM_LOG_INFO("  ! STDP reversed: Check parameters");
                } else {
                    NLM_LOG_INFO("  ! No change: STDP may not be triggering");
                }
            } else {
                NLM_LOG_INFO("  Not enough synapses for STDP test");
            }
        }
        return true;
    }
    
    bool runAllTests(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Running All Tests ===");
        
        // Test 1: Basic connectivity
        brain->reset();
        brain->initialize();
        if (!runBasicTest(brain)) return false;
        
        // Test 2: Plasticity learning experiment
        brain->reset();
        brain->initialize();
        if (!runPlasticityTest(brain)) return false;
        
        // Test 3: STDP verification
        brain->reset();
        brain->initialize();
        if (!runStdpTest(brain)) return false;
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== All Tests Complete ===");
        return true;
    }
    
    int argc;
    char** argv;
};

} // namespace nlm

int main(int argc, char** argv) {
    nlm::SimpleNLMCLI cli(argc, argv);
    bool success = cli.run();
    return success ? 0 : 1;
}
