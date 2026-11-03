// NLM (熙然) - Neural Learning Machine
// Unified CLI and Advanced Features
//
// This provides a comprehensive command-line interface for NLM,
// including multiple phases, benchmarks, configuration tools, and more.

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
#include <algorithm>
#include <map>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <filesystem>

using namespace nlm;
namespace fs = std::filesystem;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Unified CLI and Advanced Features                         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << R"(
USAGE: nlm [OPTIONS] [COMMAND] [ARGS]

COMMANDS:
  phase2               Run Phase 2: Real Neural Computation demo
  phase3               Run Phase 3: World Interaction demo
  phase4               Run Phase 4: Emerging Cognition demo
  phase5               Run Phase 5: Performance and Python bindings
  phase6               Run Phase 6: Final Integration demo
  phase6-text          Run Phase 6: Text/Thought demo
  benchmark [size]     Run performance benchmark (default: 1000 neurons)
  config               Generate configuration files
  checkpoint [file]    Load and display checkpoint information
  analyze [file]       Analyze neural network
  python-test          Test Python bindings
  help                 Show this help message

OPTIONS:
  --help, -h           Show help information
  --version           Show version information
  --verbose, -v       Enable verbose output
  --quiet, -q         Quiet mode (only essential output)
  --timestep T        Set simulation timestep (default: 0.001)
  --steps N            Set number of simulation steps (default: 100)
  --neurons N         Set number of neurons (default: 500)
  --config FILE        Load configuration from file
  --output FILE        Save results to file
  --seed N            Set random seed (default: 42)

EXAMPLES:
  nlm phase2                           # Run Phase 2 demo
  nlm phase6 --steps 5000 --neurons 1000  # Run Phase 6 with more neurons
  nlm benchmark 2000                   # Run benchmark with 2000 neurons
  nlm config generate --type json        # Generate JSON config
  nlm checkpoint my_checkpoint.bin      # Load and display checkpoint info
  nlm python-test                      # Test Python bindings
");
}

void printVersion() {
    std::cout << "NLM Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Built with C++20 and Python bindings" << std::endl;
}

// Parse command line arguments into a map
std::map<std::string, std::string> parseArgs(int argc, char** argv) {
    std::map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) == "--") {
            size_t eq = arg.find('=');
            if (eq != std::string::npos) {
                args[arg.substr(2, eq - 2)] = arg.substr(eq + 1);
            } else {
                args[arg.substr(2)] = "";
            }
        } else if (arg.substr(0, 1) == "-") {
            if (arg.size() > 2) {
                args[arg.substr(1, 1)] = arg.substr(2);
            } else {
                args[arg.substr(1)] = "";
            }
        }
    }
    return args;
}

// Convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Parse integer from string with error handling
int parseInt(const std::string& str, int defaultValue = 0) {
    try {
        return std::stoi(str);
    } catch (...) {
        return defaultValue;
    }
}

// Parse double from string with error handling
double parseDouble(const std::string& str, double defaultValue = 0.0) {
    try {
        return std::stod(str);
    } catch (...) {
        return defaultValue;
    }
}

// Check if file exists
bool fileExists(const std::string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

void runPhase2Demo(const std::map<std::string, std::string>& args) {
    printBanner();
    std::cout << "Running NLM Phase 2: Real Neural Computation" << std::endl;
    std::cout << "===========================================" << std::endl << std::endl;

    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Parse arguments
    int steps = parseInt(args.count("steps") ? args.at("steps") : "", 100);
    int neurons = parseInt(args.count("neurons") ? args.at("neurons") : "", 500);
    double timestep = parseDouble(args.count("timestep") ? args.at("timestep") : "", 0.001);
    int seed = parseInt(args.count("seed") ? args.at("seed") : "", 42);
    bool verbose = args.count("verbose") > 0;

    // Create and configure brain
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);

    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }

    brain->logStatus();

    // Run basic connectivity test
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        std::cerr << "ERROR: No regions found!" << std::endl;
        return;
    }

    auto neuronsVec = region->getAllNeurons();
    if (neuronsVec.empty()) {
        std::cerr << "ERROR: No neurons found!" << std::endl;
        return;
    }

    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();

    // Inject current into first few neurons
    std::cout << "\nTest 1: Basic Neural Connectivity" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Injecting current into " << std::min(size_t(10), neuronsVec.size()) << " neurons..." << std::endl;

    for (size_t i = 0; i < std::min(size_t(10), neuronsVec.size()); ++i) {
        neuronsVec[i]->injectCurrent(50.0f);
    }

    // Run simulation steps
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * timestep);
        if (verbose && step % 50 == 0) {
            std::cout << "  Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing, "
                      << brain->getTotalSpikeCount() - initialSpikes << " total spikes" << std::endl;
        }
    }

    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    std::cout << "\nResults:" << std::endl;
    std::cout << "  Spikes generated: " << spikes << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;

    if (spikes > 0) {
        std::cout << "  ✓ Neural network is functional!" << std::endl;
    } else {
        std::cout << "  ⚠ No spikes detected - neurons may not be responding to input" << std::endl;
    }

    // Reset for plasticity test
    brain->reset();
    brain->initialize();

    std::cout << "\n\nTest 2: Plasticity Learning Experiment" << std::endl;
    std::cout << "=====================================" << std::endl;

    // Enable plasticity
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }

    // Create pattern neurons for learning
    std::vector<NeuronId> patternNeurons;
    if (region) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }

    std::cout << "Applying learning patterns for " << steps << " steps..." << std::endl;

    for (SimulationStep step = 0; step < steps; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }

        brain->step(step, step * timestep);

        if (verbose && step % (steps / 10) == 0) {
            std::cout << "  Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }

    std::cout << "\nPlasticity test completed!" << std::endl;
    std::cout << "Final state: " << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;

    std::cout << "\nPhase 2 demo completed successfully!" << std::endl;
}

void runBenchmark(int neuronCount, int steps) {
    printBanner();
    std::cout << "NLM Performance Benchmark" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << "Testing with " << neuronCount << " neurons and " << steps << " steps" << std::endl << std::endl;

    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);

    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();

    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();

    // Run simulation
    for (size_t step = 0; step < static_cast<size_t>(steps); ++step) {
        // Inject some random input
        for (size_t i = 0; i < static_cast<size_t>(std::min(10, neuronCount / 10)); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }

        brain->step(step, step * 0.001);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();

    // Display results
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Total steps: " << steps << std::endl;
    std::cout << "Total time: " << totalTime << " seconds" << std::endl;
    std::cout << "Steps per second: " << (steps / totalTime) << std::endl;
    std::cout << "Spikes per step: " << (static_cast<double>(brain->getTotalSpikeCount()) / steps) << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Synapses per neuron: " << (static_cast<double>(brain->getTotalSynapseCount()) / brain->getTotalNeuronCount()) << std::endl;

    std::cout << "\nBenchmark completed successfully!" << std::endl;
}

void generateConfigFiles(const std::string& type) {
    printBanner();
    std::cout << "NLM Configuration File Generator" << std::endl;
    std::cout << "================================" << std::endl << std::endl;

    std::cout << "Generating " << type << " configuration files..." << std::endl;

    // Create configs directory if it doesn't exist
    if (!fs::exists("configs")) {
        fs::create_directory("configs");
    }

    // Generate JSON config
    if (toLower(type) == "json") {
        std::string jsonContent = R"({
    "neuron_count": 1000,
    "region_count": 4,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "replay_interval": 100,
    "consolidation_interval": 1000,
    "enable_checkpointing": true,
    "checkpoint_path": "brain_checkpoint.bin"
})";

        std::ofstream jsonFile("configs/default.json");
        if (jsonFile.is_open()) {
            jsonFile << jsonContent;
            jsonFile.close();
            std::cout << "✓ Generated: configs/default.json" << std::endl;
        } else {
            std::cerr << "ERROR: Failed to create configs/default.json" << std::endl;
        }
    }

    // Generate YAML config
    else if (toLower(type) == "yaml" || toLower(type) == "yml") {
        std::string yamlContent = R"(# NLM Configuration
# Generated by NLM configuration tool

# Brain parameters
neuron_count: 1000
region_count: 4
connection_probability: 0.15
simulation_timestep: 0.001
random_seed: 42

# Plasticity parameters
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
stdp_tau: 20.0
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001

# Memory and development
replay_interval: 100
consolidation_interval: 1000

# Checkpointing
enable_checkpointing: true
checkpoint_path: brain_checkpoint.bin
)"";

        std::ofstream yamlFile("configs/default.yaml");
        if (yamlFile.is_open()) {
            yamlFile << yamlContent;
            yamlFile.close();
            std::cout << "✓ Generated: configs/default.yaml" << std::endl;
        } else {
            std::cerr << "ERROR: Failed to create configs/default.yaml" << std::endl;
        }
    }

    // Generate simple text config
    else {
        std::string textContent = R"(# NLM Configuration File
# Simple key=value format for backward compatibility

# Brain parameters
neuron_count = 1000
region_count = 4
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory and development
replay_interval = 100
consolidation_interval = 1000

# Checkpointing
enable_checkpointing = true
checkpoint_path = brain_checkpoint.bin
)"";

        std::ofstream textFile("configs/default.cfg");
        if (textFile.is_open()) {
            textFile << textContent;
            textFile.close();
            std::cout << "✓ Generated: configs/default.cfg" << std::endl;
        } else {
            std::cerr << "ERROR: Failed to create configs/default.cfg" << std::endl;
        }
    }

    std::cout << "\nConfiguration files generated successfully!" << std::endl;
    std::cout << "You can load these configurations using: nlm --config configs/default.json phase2" << std::endl;
}

void analyzeCheckpoint(const std::string& filepath) {
    printBanner();
    std::cout << "NLM Checkpoint Analyzer" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "Analyzing checkpoint: " << filepath << std::endl << std::endl;

    if (!fileExists(filepath)) {
        std::cerr << "ERROR: Checkpoint file not found: " << filepath << std::endl;
        return;
    }

    // Try to load checkpoint using Brain
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);

    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain for checkpoint analysis" << std::endl;
        return;
    }

    if (!brain->load(filepath)) {
        std::cerr << "ERROR: Failed to load checkpoint: " << filepath << std::endl;
        std::cout << "This checkpoint may be corrupted or in an incompatible format." << std::endl;
        return;
    }

    // Display checkpoint information
    std::cout << "Checkpoint Analysis Results:" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << "File: " << filepath << std::endl;
    std::cout << "File size: " << fs::file_size(filepath) << " bytes" << std::endl;
    std::cout << std::endl;

    std::cout << "Brain State:" << std::endl;
    std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons (this step): " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;

    // Check for memory systems
    if (brain->getWorkingMemory()) {
        std::cout << "  Working memory traces: " << brain->getWorkingMemory()->getActiveTraces() << std::endl;
    } else {
        std::cout << "  Working memory: Not available" << std::endl;
    }

    if (brain->getEpisodicMemory()) {
        std::cout << "  Episodic memory episodes: " << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
    } else {
        std::cout << "  Episodic memory: Not available" << std::endl;
    }

    // Check for neuromodulation systems
    if (brain->getDopamine()) {
        std::cout << "  Dopamine level: " << brain->getDopamine()->getLevel() << std::endl;
    } else {
        std::cout << "  Dopamine system: Not available" << std::endl;
    }

    if (brain->getCuriosity()) {
        std::cout << "  Curiosity level: " << brain->getCuriosity()->getLevel() << std::endl;
    } else {
        std::cout << "  Curiosity system: Not available" << std::endl;
    }

    if (brain->getPredictionErrorSignal()) {
        std::cout << "  Prediction error: " << brain->getPredictionErrorSignal()->getLevel() << std::endl;
    } else {
        std::cout << "  Prediction error system: Not available" << std::endl;
    }

    std::cout << "\nCheckpoint analysis completed successfully!" << std::endl;
}

void testPythonBindings() {
    printBanner();
    std::cout << "NLM Python Bindings Test" << std::endl;
    std::cout << "=======================" << std::endl << std::endl;

    std::cout << "Testing Python bindings..." << std::endl;
    std::cout << "Note: Python bindings are tested when building with Python support." << std::endl;
    std::cout << "To test bindings manually:" << std::endl;
    std::cout << "  1. Build with Python bindings: cmake -DNLM_BUILD_PYTHON_BINDINGS=ON .." << std::endl;
    std::cout << "  2. Install with pip: pip install ." << std::endl;
    std::cout << "  3. Test in Python: python -c \"import pynlm; print('Bindings work!')\"" << std::endl << std::endl;

    std::cout << "Python bindings test skipped (requires Python build)." << std::endl;
    std::cout << "Check the Python documentation for more details." << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return 0;
    }

    std::map<std::string, std::string> args = parseArgs(argc, argv);

    // Check for global options
    bool verbose = args.count("verbose") > 0;
    bool quiet = args.count("quiet") > 0;
    bool showHelp = args.count("help") > 0;
    bool showVersion = args.count("version") > 0;

    if (showHelp) {
        printHelp();
        return 0;
    }

    if (showVersion) {
        printVersion();
        return 0;
    }

    std::string command = toLower(argv[1]);

    try {
        if (command == "phase2") {
            runPhase2Demo(args);
        } else if (command == "benchmark") {
            int neuronCount = 1000;
            if (args.count("neurons")) {
                neuronCount = parseInt(args.at("neurons"), 1000);
            }
            int steps = 1000;
            if (args.count("steps")) {
                steps = parseInt(args.at("steps"), 1000);
            }
            runBenchmark(neuronCount, steps);
        } else if (command == "config") {
            std::string type = "cfg";
            if (args.count("type")) {
                type = args.at("type");
            }
            generateConfigFiles(type);
        } else if (command == "checkpoint") {
            if (argc < 3) {
                std::cerr << "ERROR: Checkpoint file path required." << std::endl;
                std::cerr << "Usage: nlm checkpoint <filepath>" << std::endl;
                return 1;
            }
            analyzeCheckpoint(argv[2]);
        } else if (command == "python-test" || command == "python_test") {
            testPythonBindings();
        } else if (command == "help") {
            printHelp();
        } else {
            std::cerr << "ERROR: Unknown command: " << command << std::endl;
            std::cerr << "Run 'nlm help' for available commands." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: An unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
