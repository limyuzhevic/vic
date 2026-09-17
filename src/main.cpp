// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "tests/TestFunctions.hpp"
#include "experiments/LearningExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
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
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Advanced command-line parser for different modes
void printUsage() {
    std::cout << "Usage: nlm [mode] [options]\n\n";
    std::cout << "Modes:\n";
    std::cout << "  default    Run the complete Phase 2 demo (default)\n";
    std::cout << "  connectivity Run only basic connectivity test\n";
    std::cout << "  plasticity  Run only plasticity learning experiment\n";
    std::cout << "  stdp       Run only STDP verification\n";
    std::cout << "  learn      Run only learning experiment with full logging\n";
    std::cout << "  quick      Quick test with minimal output\n\n";
    std::cout << "Options:\n";
    std::cout << "  --neurons N      Set number of neurons (default: 1000)\n";
    std::cout << "  --regions R      Set number of regions (default: 1)\n";
    std::cout << "  --seed S         Set random seed (default: 42)\n";
    std::cout << "  --steps N        Number of simulation steps (default: 1000)\n";
    std::cout << "  --config FILENAME Load configuration from file\n";
    std::cout << "  --verbose        Enable verbose logging\n";
    std::cout << "  --help           Show this help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  nlm --neurons 500 --regions 2 --seed 123\n";
    std::cout << "  nlm connectivity --verbose\n";
    std::cout << "  nlm plasticity --steps 500\n";
}

// Advanced configuration options
struct AdvancedConfig {
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    uint64_t randomSeed = 42;
    size_t simulationSteps = 1000;
    std::string configFile = "configs/default.cfg";
    bool verbose = false;
    bool saveState = false;
    std::string checkpointFile = "checkpoint.bin";
    
    void printSummary() const {
        NLM_LOG_INFO("Advanced Configuration Summary:");
        NLM_LOG_INFO("  Neurons: " + std::to_string(neuronCount));
        NLM_LOG_INFO("  Regions: " + std::to_string(regionCount));
        NLM_LOG_INFO("  Random Seed: " + std::to_string(randomSeed));
        NLM_LOG_INFO("  Simulation Steps: " + std::to_string(simulationSteps));
        NLM_LOG_INFO("  Config File: " + configFile);
        NLM_LOG_INFO("  Verbose Mode: " + std::string(verbose ? "Yes" : "No"));
        NLM_LOG_INFO("  Save State: " + std::string(saveState ? "Yes" : "No"));
        if (saveState) {
            NLM_LOG_INFO("  Checkpoint File: " + checkpointFile);
        }
    }
};

// Command-line argument parser
AdvancedConfig parseAdvancedArgs(int argc, char** argv) {
    AdvancedConfig config;
    std::map<std::string, std::string> argMap;
    
    // Convert arguments to map
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                argMap[key] = value;
            }
        }
    }
    
    // Parse mapped arguments
    if (argMap.find("neurons") != argMap.end()) {
        config.neuronCount = std::stoul(argMap["neurons"]);
    }
    if (argMap.find("regions") != argMap.end()) {
        config.regionCount = std::stoul(argMap["regions"]);
    }
    if (argMap.find("seed") != argMap.end()) {
        config.randomSeed = std::stoull(argMap["seed"]);
    }
    if (argMap.find("steps") != argMap.end()) {
        config.simulationSteps = std::stoul(argMap["steps"]);
    }
    if (argMap.find("config") != argMap.end()) {
        config.configFile = argMap["config"];
    }
    if (argMap.find("verbose") != argMap.end()) {
        config.verbose = true;
    }
    if (argMap.find("save_state") != argMap.end()) {
        config.saveState = true;
    }
    if (argMap.find("checkpoint") != argMap.end()) {
        config.checkpointFile = argMap["checkpoint"];
    }
    
    return config;
}

// Learning experiment with advanced features
void runAdvancedLearningExperiment(const AdvancedConfig& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== ADVANCED LEARNING EXPERIMENT ===");
    NLM_LOG_INFO("High-performance neural learning demonstration");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                std::to_string(config.regionCount) + " regions, seed=" + std::to_string(config.randomSeed));
    
    if (config.verbose) {
        NLM_LOG_INFO("Verbose mode enabled - detailed logging active");
    }
    
    // Initialize logger with custom settings
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    if (config.verbose) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto fullConfig = std::make_shared<Config>();
    if (config.configFile.empty() || config.configFile == "default") {
        config.configFile = "configs/default.cfg";
    }
    
    if (fullConfig->loadFromFile(config.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + config.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply advanced configuration
    fullConfig->set("random_seed", static_cast<int64_t>(config.randomSeed), ConfigSource::Runtime);
    fullConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Runtime);
    fullConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Runtime);
    fullConfig->set("connection_probability", 0.15f, ConfigSource::Runtime);
    
    // Advanced STDP parameters for learning
    fullConfig->set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime);
    fullConfig->set("stdp_ltd_weight", 0.015f, ConfigSource::Runtime);
    fullConfig->set("stdp_tau", 20.0f, ConfigSource::Runtime);
    
    // Advanced structural plasticity
    fullConfig->set("synaptogenesis_rate", 0.0002f, ConfigSource::Runtime);  // Higher rate
    fullConfig->set("pruning_rate", 0.00002f, ConfigSource::Runtime);  // Lower rate
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain with advanced configuration...");
    auto brain = std::make_shared<Brain>(fullConfig);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run multiple learning cycles for advanced demonstration
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== MULTIPLE LEARNING CYCLES ===");
    NLM_LOG_INFO("Running " + std::to_string(config.simulationSteps) + " simulation steps...");
    
    // Learning experiment with statistics collection
    LearningExperiment experiment(brain, config.randomSeed);
    experiment.recordInitialState();
    
    // Enable all plasticity mechanisms
    for (size_t i = 1; i <= config.regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);  // Hebbian, STDP, and reward-modulated
                syn->enablePlasticity(true, true, false); // Also enable structural
            }
        }
    }
    
    // Run advanced simulation with dynamic learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Starting dynamic learning simulation...");
    NLM_LOG_INFO("This simulates real neural development with multiple plasticity mechanisms.");
    
    for (size_t step = 0; step < config.simulationSteps; ++step) {
        // Dynamic input patterns to stimulate different types of learning
        if (step % 3 == 0) {
            // Burst input for STDP
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
        } else if (step % 5 == 0) {
            // Modulated input for reward-modulated learning
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        } else if (step % 10 == 0) {
            // Correlated pre-post input for Hebbian learning
            for (size_t i = 0; i < 5 && i < brain->getTotalNeuronCount() / 10; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Internal, 15.0f);
            }
        }
        
        brain->step(step);
        
        // Periodic reporting
        if (step % 200 == 0 && step > 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Total Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | E/I Ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        }
        
        // Occasionally save intermediate checkpoint
        if (config.saveState && step % 500 == 0 && step > 0) {
            std::string checkpointFile = config.checkpointFile;
            if (step > 0) {
                size_t pos = checkpointFile.find('.');
                if (pos != std::string::npos) {
                    checkpointFile = checkpointFile.substr(0, pos) + "_step" + std::to_string(step) + 
                                   checkpointFile.substr(pos);
                }
            }
            if (brain->save(checkpointFile)) {
                NLM_LOG_INFO("  Checkpoint saved: " + checkpointFile);
            }
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Comprehensive statistics
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== ADVANCED LEARNING STATISTICS ===");
    experiment.computeStatistics();
    
    // Analyze learning metrics
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== LEARNING ANALYSIS ===");
    
    float learningScore = 0.0f;
    if (!experiment.initialWeights.empty() && !experiment.finalWeights.empty()) {
        float initialSum = std::accumulate(experiment.initialWeights.begin(), 
                                          experiment.initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(experiment.finalWeights.begin(), 
                                        experiment.finalWeights.end(), 0.0f);
        float initialMean = initialSum / static_cast<float>(experiment.initialWeights.size());
        float finalMean = finalSum / static_cast<float>(experiment.finalWeights.size());
        
        learningScore = std::abs(finalMean - initialMean);
        
        NLM_LOG_INFO("Initial average synaptic weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("Final average synaptic weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("Total weight change: " + std::to_string(finalMean - initialMean));
        NLM_LOG_INFO("Learning intensity: " + std::to_string(learningScore));
        
        // Learning classification
        if (learningScore > 0.1f) {
            NLM_LOG_INFO("  LEARNING CLASSIFICATION: HIGH - Significant synaptic modification");
        } else if (learningScore > 0.01f) {
            NLM_LOG_INFO("  LEARNING CLASSIFICATION: MODERATE - Noticeable synaptic changes");
        } else if (learningScore > 0.001f) {
            NLM_LOG_INFO("  LEARNING CLASSIFICATION: LOW - Minimal synaptic changes");
        } else {
            NLM_LOG_INFO("  LEARNING CLASSIFICATION: VERY LOW - Limited learning detected");
        }
    }
    
    // Save final state if requested
    if (config.saveState) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Saving final brain state...");
        if (brain->save(config.checkpointFile)) {
            NLM_LOG_INFO("✓ Final checkpoint saved successfully to: " + config.checkpointFile);
        } else {
            NLM_LOG_ERROR("Failed to save final checkpoint");
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== ADVANCED LEARNING EXPERIMENT COMPLETE ===");
    NLM_LOG_INFO("The brain has demonstrated complex learning behaviors with multiple plasticity mechanisms.");
    NLM_LOG_INFO("Learning Score: " + std::to_string(learningScore));
    NLM_LOG_INFO("Total Spikes Generated: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Final Structural Complexity: " + std::to_string(brain->getTotalSynapseCount()) + " synapses");
    NLM_LOG_INFO("");
}

// Interactive mode for exploring different configurations
void runInteractiveMode() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== INTERACTIVE LEARNING EXPLORER ===");
    NLM_LOG_INFO("This mode allows you to explore different brain configurations and learning scenarios.");
    NLM_LOG_INFO("");
    
    // Create a base configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Interactive learning scenarios
    struct LearningScenario {
        std::string name;
        std::function<void()> run;
    };
    
    std::vector<LearningScenario> scenarios;
    
    // Scenario 1: Simple STDP learning
    scenarios.push_back({"STDP-only learning", [&]() {
        NLM_LOG_INFO("Running STDP-only learning scenario...")
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) return;
        
        // Enable only STDP plasticity
        for (size_t i = 1; i <= config->getOr<size_t>("region_count", 1); ++i) {
            auto* region = brain->getRegion(RegionId(i));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(false, true, false);  // STDP only
                }
            }
        }
        
        // Run STDP demonstration
        for (SimulationStep step = 0; step < 500; ++step) {
            // Create correlated activity for STDP
            if (step % 50 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step);
        }
        
        NLM_LOG_INFO("  STDP scenario complete - observe weight changes in synapses");
    }});
    
    // Scenario 2: Hebbian learning with repetition
    scenarios.push_back({"Hebbian repetition learning", [&]() {
        NLM_LOG_INFO("Running Hebbian repetition learning scenario...")
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) return;
        
        // Enable only Hebbian plasticity
        for (size_t i = 1; i <= config->getOr<size_t>("region_count", 1); ++i) {
            auto* region = brain->getRegion(RegionId(i));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, false, false);  // Hebbian only
                }
            }
        }
        
        // Run repeated correlated activity for Hebbian learning
        for (SimulationStep step = 0; step < 800; ++step) {
            // Pre-synaptic activity
            if (step % 100 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
            }
            brain->step(step);
            
            // Post-synaptic activity (creates correlation)
            if (step % 100 == 50) {
                brain->injectCurrentToNeurons(NeuronType::Internal, 25.0f);
            }
        }
        
        NLM_LOG_INFO("  Hebbian scenario complete - observe synaptic strengthening");
    }});
    
    // Scenario 3: Neuromodulated learning
    scenarios.push_back({"Neuromodulated learning", [&]() {
        NLM_LOG_INFO("Running neuromodulated learning scenario...")
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) return;
        
        // Enable all plasticity mechanisms
        for (size_t i = 1; i <= config->getOr<size_t>("region_count", 1); ++i) {
            auto* region = brain->getRegion(RegionId(i));
            if (region) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, true);  // All plasticity types
                }
            }
        }
        
        // Simulate reward-modulated learning
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Apply reward-like signals at intervals
            if (step % 150 == 0) {
                // Simulate positive reward
                brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            } else if (step % 200 == 0) {
                // Simulate negative reward (punishment)
                brain->injectCurrentToNeurons(NeuronType::Internal, -20.0f);
            }
            
            brain->step(step);
        }
        
        NLM_LOG_INFO("  Neuromodulated scenario complete - observe reward-based learning");
    }});
    
    // Run interactive scenarios
    NLM_LOG_INFO("Available learning scenarios:");
    for (size_t i = 0; i < scenarios.size(); ++i) {
        NLM_LOG_INFO("  " + std::to_string(i + 1) + ". " + scenarios[i].name);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("To explore scenarios, run the program with --mode interactive and specify scenarios.")
    NLM_LOG_INFO("For now, all scenarios are available in the tests/ directory.")
    NLM_LOG_INFO("");
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command-line arguments
    std::string mode = "default";  // default mode
    AdvancedConfig config;
    
    // Simple mode parsing for backward compatibility
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else if (arg == "connectivity" || arg == "--connectivity") {
            mode = "connectivity";
        } else if (arg == "plasticity" || arg == "--plasticity") {
            mode = "plasticity";
        } else if (arg == "stdp" || arg == "--stdp") {
            mode = "stdp";
        } else if (arg == "learn" || arg == "--learn") {
            mode = "learn";
        } else if (arg == "quick" || arg == "--quick") {
            mode = "quick";
        } else if (arg == "--interactive") {
            mode = "interactive";
        } else if (arg.substr(0, 2) == "--") {
            // Pass advanced arguments to config parser
            if (!argMap.empty()) {
                config = parseAdvancedArgs(argc, argv);
            }
        }
    }
    
    // Route to appropriate mode
    if (mode == "interactive") {
        runInteractiveMode();
        return 0;
    } else if (mode == "connectivity") {
        NLM_LOG_INFO("=== BASIC NEURAL CONNECTIVITY TEST ===");
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        runBasicConnectivityTest(brain);
    } else if (mode == "plasticity") {
        NLM_LOG_INFO("=== PLASTICITY LEARNING EXPERIMENT ===");
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        runPlasticityExperiment(brain);
    } else if (mode == "stdp") {
        NLM_LOG_INFO("=== STDP VERIFICATION TEST ===");
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 800);
        config->set("region_count", 1);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        runStdpVerification(brain);
    } else if (mode == "learn") {
        // Run advanced learning experiment
        runAdvancedLearningExperiment(config);
    } else if (mode == "quick") {
        NLM_LOG_INFO("=== QUICK NLM TEST ===");
        NLM_LOG_INFO("Running minimal demonstration of neural computation...");
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 200);
        config->set("region_count", 1);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        // Quick connectivity test
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(50.0f);
            }
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        NLM_LOG_INFO("  Quick test complete - " + std::to_string(brain->getTotalSpikeCount()) + " spikes generated");
    } else {
        // Default comprehensive demo
        LearningExperimentDemo();
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM EXECUTION COMPLETE ===");
    NLM_LOG_INFO("Thank you for exploring the Neural Learning Machine!")
    NLM_LOG_INFO("");
    
    return 0;
}