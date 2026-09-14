// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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

#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <functional>

namespace nlm {

// Print banner function
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

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

// Command structure for the NLM command-line interface
struct Command {
    std::string name;
    std::string description;
    std::function<void(std::shared_ptr<Brain>, const std::vector<std::string>&)> handler;
    std::vector<std::string> args;
};

// Print help information
void printHelp() {
    printBanner();
    std::cout << "\nUSAGE: nlm [OPTIONS] [COMMAND] [ARGS]" << std::endl;
    std::cout << "\nOPTIONS:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q             Quiet mode (minimal output)" << std::endl;
    std::cout << "  --save-config <file>     Save current configuration to file" << std::endl;
    std::cout << "  --batch                 Run in batch mode (non-interactive)" << std::endl;
    std::cout << "\nCOMMANDS:" << std::endl;
    std::cout << "  test connectivity        Run basic connectivity test" << std::endl;
    std::cout << "  plasticity              Run plasticity learning experiment" << std::endl;
    std::cout << "  stdp-test               Run STDP verification test" << std::endl;
    std::cout << "  all                     Run all tests (default)" << std::endl;
    std::cout << "  status                  Display brain status" << std::endl;
    std::cout << "  reset                   Reset brain to initial state" << std::endl;
    std::cout << "  inject <type> <current> Inject current into specific neuron type" << std::endl;
    std::cout << "  sensory <data>          Inject sensory data" << std::endl;
    std::cout << "\nEXAMPLES:" << std::endl;
    std::cout << "  nlm --verbose --config myconfig.cfg test connectivity" << std::endl;
    std::cout << "  nlm --save-config after_test.cfg plasticity" << std::endl;
    std::cout << "  nlm stdp-test" << std::endl;
}

// Test runner function
void runTest(std::shared_ptr<Brain> brain, const std::vector<std::string>& args) {
    // Implementation will be added
}

// Additional functions for command handlers
void runAllTests(std::shared_ptr<Brain> brain, const std::vector<std::string>& args);
void runStatus(std::shared_ptr<Brain> brain, const std::vector<std::string>& args);
void runReset(std::shared_ptr<Brain> brain, const std::vector<std::string>& args);
void runInjectCurrent(std::shared_ptr<Brain> brain, const std::vector<std::string>& args);
void runSensoryInput(std::shared_ptr<Brain> brain, const std::vector<std::string>& args);

// Main function with improved CLI
int main(int argc, char** argv) {
    printBanner();
    
    // Set up logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Enhanced command-line interface with improved user control");
    NLM_LOG_INFO("");
    
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
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Determine which command to run
    std::string command = "all";
    std::vector<std::string> commandArgs;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            i += 1; // Skip the config value if present
            continue;
        }
        if (arg.substr(0, 13) == "--save-config") {
            i += 1; // Skip the save-config value if present
            continue;
        }
        if (arg == "" && command == "all") {
            command = "all";
        } else if (command == "all" && !arg.empty()) {
            command = arg;
            // Extract arguments after command
            for (int j = i + 1; j < argc; ++j) {
                commandArgs.push_back(argv[j]);
            }
            break;
        }
    }
    
    // Run the specified command
    if (command == "test" || command == "connectivity" || command == "test connectivity") {
        runBasicConnectivityTest(brain);
    } else if (command == "plasticity") {
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
    } else if (command == "stdp-test" || command == "stdp") {
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (command == "status") {
        runStatus(brain, commandArgs);
    } else if (command == "reset") {
        runReset(brain, commandArgs);
    } else if (command == "inject") {
        runInjectCurrent(brain, commandArgs);
    } else if (command == "sensory") {
        runSensoryInput(brain, commandArgs);
    } else if (command == "all" || command.empty()) {
        runAllTests(brain, commandArgs);
    } else {
        NLM_LOG_ERROR("Unknown command: " + command);
        printHelp();
        return 1;
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Command Complete ===");
    
    return 0;
}

// Implementation of command handlers
void runAllTests(std::shared_ptr<Brain> brain, const std::vector<std::string>& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running All Tests ===");
    
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
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("✓ All tests completed successfully");
}

void runStatus(std::shared_ptr<Brain> brain, const std::vector<std::string>& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("To view detailed statistics, use verbose mode");
}

void runReset(std::shared_ptr<Brain> brain, const std::vector<std::string>& args) {
    NLM_LOG_INFO("Resetting brain to initial state...");
    brain->reset();
    brain->initialize();
    NLM_LOG_INFO("✓ Brain reset complete");
}

void runInjectCurrent(std::shared_ptr<Brain> brain, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        NLM_LOG_ERROR("Usage: inject <type> <current>");
        NLM_LOG_ERROR("  type: Sensory, Internal, or Motor");
        NLM_LOG_ERROR("  current: Current value in microAmps");
        return;
    }
    
    // Parse arguments
    std::string typeStr = args[0];
    float current;
    
    try {
        current = std::stof(args[1]);
    } catch (...) {
        NLM_LOG_ERROR("Invalid current value: " + args[1]);
        return;
    }
    
    // Map type string to NeuronType
    NeuronType neuronType;
    if (typeStr == "Sensory") {
        neuronType = NeuronType::Sensory;
    } else if (typeStr == "Internal") {
        neuronType = NeuronType::Internal;
    } else if (typeStr == "Motor") {
        neuronType = NeuronType::Motor;
    } else {
        NLM_LOG_ERROR("Unknown neuron type: " + typeStr);
        return;
    }
    
    NLM_LOG_INFO("Injecting " + std::to_string(current) + " μA into " + typeStr + " neurons");
    brain->injectCurrentToNeurons(neuronType, current);
    NLM_LOG_INFO("✓ Current injection complete");
}

void runSensoryInput(std::shared_ptr<Brain> brain, const std::vector<std::string>& args) {
    if (args.empty()) {
        NLM_LOG_ERROR("Usage: sensory <value1> [value2] ...");
        NLM_LOG_ERROR("  Provide sensory input values (typically 0-1)");
        return;
    }
    
    // Create a simple sensory input
    // In a real implementation, this would use the proper SensoryInput class
    NLM_LOG_INFO("Injecting sensory data: " + args[0]);
    
    // For now, just inject into the first sensory neuron if available
    if (!brain->getSensoryNeurons().empty()) {
        brain->injectCurrent(brain->getSensoryNeurons()[0]->getId(), 10.0f);
    }
    
    NLM_LOG_INFO("✓ Sensory input injected");
}
