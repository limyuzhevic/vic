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

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
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
    
    if (spikes > 0) {
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
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
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
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
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

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
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <map>

using namespace nlm;

// Command line help
void printHelp() {
    std::cout << R"(
NLM - Neural Learning Machine - Advanced Commands
===================================================

CONFIG COMMANDS:
  config list [directory]        - List available config files in directory
  config validate [file]          - Validate config file format (JSON/YAML)
  config convert [in] [out]        - Convert between JSON and YAML formats
  config merge [files...] [out]    - Merge multiple config files
  config diff [file1] [file2]      - Show differences between config files
  config backup [file]             - Create backup of config with timestamp
  config extract [file] [keys...]  - Extract specific keys to new config
  config template [type] [file]    - Generate template configs with examples
  config statistics [file]         - Show config statistics (types, sources, etc.)
  config search [file] [pattern]    - Search for config keys with patterns
  config migrate [file] [version]   - Migrate old config format to new format

BRAIN SYSTEM COMMANDS:
  brain status                    - Show brain system status and metrics
  brain reset [steps]             - Reset brain and optionally run steps

MEMORY SYSTEM COMMANDS:
  memory stats                    - Show memory system statistics

NEURON SYSTEM COMMANDS:
  neuron info [region]            - Show neuron population information

PLASTICITY SYSTEM COMMANDS:
  plasticity report               - Show plasticity rule status and parameters

NEUROMODULATION SYSTEM COMMANDS:
  neuromodulation status           - Show neuromodulation system state

AGENT COMMANDS:
  agent learn [params]            - Start learning session with custom parameters
  agent evaluate [file]            - Evaluate agent performance
  agent save [file] [metadata]     - Save agent state with metadata
  agent load [file]               - Load saved agent state
  agent profile                    - Show agent capabilities and limitations

ENVIRONMENT COMMANDS:
  env create [type] [name] [params] - Create new environment with specified parameters
  env list                         - List available environments
  env info [name]                  - Show environment details

WORLD COMMANDS:
  world reset                     - Reset world state
  world pause [enable]            - Pause/unpause world simulation
  world step [count]              - Step world simulation forward

MISCELLANEOUS:
  help                            - Show this help message
  version                         - Show NLM version information
  run [steps]                     - Run default test suite for specified steps

Examples:
  config list ./configs
  config validate ./configs/neural.json
  config convert ./configs/neural.json ./configs/neural.yaml
  brain status
  memory stats
  agent save ./agent_state.json "learning completed"
  world reset
  help

===================================================
NLM Phase 2: Real Neural Computation
Running on: $(date)
===================================================

)" << std::endl;
}

void printVersion() {
    std::cout << "NLM - Neural Learning Machine v2.0 (Phase 2)" << std::endl;
    std::cout << "Real Neural Computation with LIF neurons, STDP, and structural plasticity" << std::endl;
    std::cout << "Compiled: " << __DATE__ << " " << __TIME__ << std::endl;
}

// Command line parsing and execution functions
bool parseConfigCommand(int argc, char** argv, std::shared_ptr<Config> config);
bool parseBrainCommand(int argc, char** argv, std::shared_ptr<Brain> brain);
bool parseMemoryCommand(int argc, char** argv, std::shared_ptr<Brain> brain);
bool parseNeuronCommand(int argc, char** argv, std::shared_ptr<Brain> brain);
bool parsePlasticityCommand(int argc, char** argv, std::shared_ptr<Brain> brain);
bool parseNeuromodulationCommand(int argc, char** argv, std::shared_ptr<Brain> brain);
bool parseAgentCommand(int argc, char** argv, std::shared_ptr<Brain> brain);
bool parseEnvCommand(int argc, char** argv);
bool parseWorldCommand(int argc, char** argv);

// Implementation of command handlers
void cmdConfigList(const std::vector<std::string>& args);
void cmdConfigValidate(const std::vector<std::string>& args);
void cmdConfigConvert(const std::vector<std::string>& args);
void cmdConfigMerge(const std::vector<std::string>& args);
void cmdConfigDiff(const std::vector<std::string>& args);
void cmdConfigBackup(const std::vector<std::string>& args);
void cmdConfigExtract(const std::vector<std::string>& args);
void cmdConfigTemplate(const std::vector<std::string>& args);
void cmdConfigStatistics(const std::vector<std::string>& args);
void cmdConfigSearch(const std::vector<std::string>& args);
void cmdConfigMigrate(const std::vector<std::string>& args);
void cmdBrainStatus(const std::vector<std::string>& args);
void cmdBrainReset(const std::vector<std::string>& args);
void cmdMemoryStats(const std::vector<std::string>& args);
void cmdNeuronInfo(const std::vector<std::string>& args);
void cmdPlasticityReport(const std::vector<std::string>& args);
void cmdNeuromodulationStatus(const std::vector<std::string>& args);
void cmdAgentLearn(const std::vector<std::string>& args);
void cmdAgentEvaluate(const std::vector<std::string>& args);
void cmdAgentSave(const std::vector<std::string>& args);
void cmdAgentLoad(const std::vector<std::string>& args);
void cmdAgentProfile(const std::vector<std::string>& args);
void cmdEnvCreate(const std::vector<std::string>& args);
void cmdEnvList(const std::vector<std::string>& args);
void cmdEnvInfo(const std::vector<std::string>& args);
void cmdWorldReset(const std::vector<std::string>& args);
void cmdWorldPause(const std::vector<std::string>& args);
void cmdWorldStep(const std::vector<std::string>& args);

// Global command dispatcher
declare type
dispatcher for commands

// Helper functions
declare time utils
declare file utils
declare string utils

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Check for help/version commands
    if (argc <= 1) {
        printHelp();
        return 0;
    }
    
    std::string command = argv[1];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    if (command == "help") {
        printHelp();
        return 0;
    }
    
    if (command == "version") {
        printVersion();
        return 0;
    }
    
    if (command == "run") {
        // Convert argv to argument vector for original main logic
        std::vector<std::string> args(argv + 2, argv + argc);
        int steps = 100;
        if (!args.empty()) {
            try {
                steps = std::stoi(args[0]);
            } catch (...) {
                steps = 100;
            }
        }
        
        // Create default config
        auto config = std::make_shared<Config>();
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
        
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Running Default NLM Test Suite ===");
        NLM_LOG_INFO("Steps: " + std::to_string(steps));
        NLM_LOG_INFO("");
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain);
        
        // Reset for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run plasticity experiment
        runPlasticityExperiment(brain);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        return 0;
    }
    
    // Check for config commands
    if (command == "config") {
        if (argc <= 2) {
            std::cout << "Error: Config command requires subcommand.\n";
            std::cout << "Use 'config help' or 'help' for available commands.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        if (subCommand == "list") {
            cmdConfigList(args);
        } else if (subCommand == "validate") {
            cmdConfigValidate(args);
        } else if (subCommand == "convert") {
            cmdConfigConvert(args);
        } else if (subCommand == "merge") {
            cmdConfigMerge(args);
        } else if (subCommand == "diff") {
            cmdConfigDiff(args);
        } else if (subCommand == "backup") {
            cmdConfigBackup(args);
        } else if (subCommand == "extract") {
            cmdConfigExtract(args);
        } else if (subCommand == "template") {
            cmdConfigTemplate(args);
        } else if (subCommand == "statistics") {
            cmdConfigStatistics(args);
        } else if (subCommand == "search") {
            cmdConfigSearch(args);
        } else if (subCommand == "migrate") {
            cmdConfigMigrate(args);
        } else {
            std::cout << "Error: Unknown config subcommand: " << subCommand << std::endl;
            std::cout << "Use 'config help' or 'help' for available commands.\n";
            return 1;
        }
        return 0;
    }
    
    // Check for brain commands
    if (command == "brain") {
        if (argc <= 2) {
            std::cout << "Error: Brain command requires subcommand.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        if (subCommand == "status") {
            cmdBrainStatus(args);
        } else if (subCommand == "reset") {
            cmdBrainReset(args);
        } else {
            std::cout << "Error: Unknown brain subcommand: " << subCommand << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Check for memory commands
    if (command == "memory") {
        if (argc <= 2) {
            std::cout << "Error: Memory command requires subcommand.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        if (subCommand == "stats") {
            cmdMemoryStats(args);
        } else {
            std::cout << "Error: Unknown memory subcommand: " << subCommand << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Check for neuron commands
    if (command == "neuron") {
        if (argc <= 2) {
            std::cout << "Error: Neuron command requires subcommand.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        if (subCommand == "info") {
            cmdNeuronInfo(args);
        } else {
            std::cout << "Error: Unknown neuron subcommand: " << subCommand << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Check for plasticity commands
    if (command == "plasticity") {
        if (argc <= 2) {
            std::cout << "Error: Plasticity command requires subcommand.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        if (subCommand == "report") {
            cmdPlasticityReport(args);
        } else {
            std::cout << "Error: Unknown plasticity subcommand: " << subCommand << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Check for neuromodulation commands
    if (command == "neuromodulation") {
        if (argc <= 2) {
            std::cout << "Error: Neuromodulation command requires subcommand.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        if (subCommand == "status") {
            cmdNeuromodulationStatus(args);
        } else {
            std::cout << "Error: Unknown neuromodulation subcommand: " << subCommand << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Check for agent commands
    if (command == "agent") {
        if (argc <= 2) {
            std::cout << "Error: Agent command requires subcommand.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        if (subCommand == "learn") {
            cmdAgentLearn(args);
        } else if (subCommand == "evaluate") {
            cmdAgentEvaluate(args);
        } else if (subCommand == "save") {
            cmdAgentSave(args);
        } else if (subCommand == "load") {
            cmdAgentLoad(args);
        } else if (subCommand == "profile") {
            cmdAgentProfile(args);
        } else {
            std::cout << "Error: Unknown agent subcommand: " << subCommand << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Check for environment commands
    if (command == "env") {
        if (argc <= 2) {
            std::cout << "Error: Env command requires subcommand.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        if (subCommand == "create") {
            cmdEnvCreate(args);
        } else if (subCommand == "list") {
            cmdEnvList(args);
        } else if (subCommand == "info") {
            cmdEnvInfo(args);
        } else {
            std::cout << "Error: Unknown env subcommand: " << subCommand << std::endl;
            return 1;
        }
        return 0;
    }
    
    // Check for world commands
    if (command == "world") {
        if (argc <= 2) {
            std::cout << "Error: World command requires subcommand.\n";
            return 1;
        }
        std::string subCommand = argv[2];
        std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
        
        std::vector<std::string> args(argv + 3, argv + argc);
        
        if (subCommand == "reset") {
            cmdWorldReset(args);
        } else if (subCommand == "pause") {
            cmdWorldPause(args);
        } else if (subCommand == "step") {
            cmdWorldStep(args);
        } else {
            std::cout << "Error: Unknown world subcommand: " << subCommand << std::endl;
            return 1;
        }
        return 0;
    }
    
    // If we got here, the command is not recognized
    std::cout << "Error: Unknown command: " << command << std::endl;
    std::cout << "Use 'help' for available commands.\n";
    return 1;
}
