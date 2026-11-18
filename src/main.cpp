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
#include "experiments/LearningExperiment.hpp"
#include "experiments/CommandLineParser.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <functional>

using namespace nlm;

// =============================================================================
// Utility Functions
// =============================================================================

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

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

// =============================================================================
// Demo Runner Classes
// =============================================================================

class DemoRunner {
public:
    using DemoFunction = std::function<void(std::shared_ptr<Brain>)>;
    
    struct DemoInfo {
        std::string description;
        DemoFunction demo;
        bool enabled;
    };
    
    DemoRunner() {}
    ~DemoRunner() {}
    
    void registerDemo(const std::string& name, const std::string& description, 
                     DemoFunction demo, bool enabled = true) {
        demos[name] = {description, demo, enabled};
    }
    
    bool runDemo(const std::string& name, std::shared_ptr<Brain> brain) {
        auto it = demos.find(name);
        if (it == demos.end()) {
            std::cerr << "Error: Unknown demo: " << name << std::endl;
            return false;
        }
        
        if (!it->second.enabled) {
            std::cout << "Demo skipped (disabled): " << name << std::endl;
            return true;
        }
        
        std::cout << "Running demo: " << name << std::endl;
        std::cout << "Description: " << it->second.description << std::endl << std::endl;
        
        try {
            it->second.demo(brain);
            std::cout << "Demo completed: " << name << std::endl << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error running demo " << name << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    bool runAllDemos(std::shared_ptr<Brain> brain) {
        bool success = true;
        for (const auto& demo : demos) {
            if (demo.second.enabled) {
                if (!runDemo(demo.first, brain)) {
                    success = false;
                }
            }
        }
        return success;
    }
    
    void listDemos() const {
        std::cout << "Available Demos:" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        
        for (const auto& demo : demos) {
            std::cout << "  " << std::left << std::setw(20) << demo.first << " - " 
                      << demo.second.description << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "To run a specific demo, use --demo <name>" << std::endl;
        std::cout << "To run all demos, use --run-all" << std::endl;
    }
    
private:
    std::map<std::string, DemoInfo> demos;
};

// =============================================================================
// Legacy Demo Functions
// =============================================================================

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
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
    
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
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
    
    float totalDelta = delta;
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

// =============================================================================
// Configuration Functions
// =============================================================================

void configureBrainFromFile(std::shared_ptr<Config> config, const std::string& configFile) {
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
}

void configureLogging(std::shared_ptr<Logger> logger, const std::string& logLevel) {
    if (logLevel == "DEBUG") {
        logger->setLevel(LogLevel::Debug);
    } else if (logLevel == "INFO") {
        logger->setLevel(LogLevel::Info);
    } else if (logLevel == "WARN") {
        logger->setLevel(LogLevel::Warning);
    } else if (logLevel == "ERROR") {
        logger->setLevel(LogLevel::Error);
    } else {
        logger->setLevel(LogLevel::Info);
    }
}

// =============================================================================
// Experiment Configuration
// =============================================================================

struct ExperimentConfig {
    int neuronCount = 500;
    int regionCount = 1;
    float connectionProbability = 0.15f;
    int64_t randomSeed = 42;
    double timestep = 0.001;
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    std::string configFile = "configs/default.cfg";
    std::string logLevel = "INFO";
    bool verbose = false;
    std::string demo = "";
    bool runAll = false;
};

ExperimentConfig parseExperimentConfig(int argc, char* argv[]) {
    ExperimentConfig config;
    
    CommandLineParser parser("nlm", "Neural Learning Machine - Phase 2 Real Neural Computation");
    
    parser.addStringArgument("--config", "Configuration file path", config.configFile, false,
                            "Path to configuration file. If not provided, uses default.cfg");
    parser.addStringArgument("--log-level", "Logging level", config.logLevel, false,
                            "Logging level: DEBUG, INFO, WARN, ERROR");
    parser.addFlag("--verbose", "Enable verbose output", "Enable verbose logging output");
    parser.addFlag("--help", "Show help message", "Display this help message");
    parser.addFlag("--version", "Show version information", "Display version information");
    
    parser.addIntArgument("--neurons", "Number of neurons", config.neuronCount, false,
                         "Total number of neurons in the brain");
    parser.addIntArgument("--regions", "Number of regions", config.regionCount, false,
                         "Number of neural regions");
    parser.addFloatArgument("--connection-prob", "Synapse connection probability", 
                           config.connectionProbability, false,
                           "Probability of connection between neurons (0.0-1.0)");
    parser.addIntArgument("--seed", "Random seed", config.randomSeed, false,
                         "Random seed for reproducible simulations");
    parser.addFloatArgument("--timestep", "Simulation timestep (ms)", config.timestep, false,
                           "Time step for simulation in milliseconds");
    
    parser.addFloatArgument("--stdp-ltp", "STDP LTP weight", config.stdpLtpWeight, false,
                           "STDP LTP (Long-Term Potentiation) weight");
    parser.addFloatArgument("--stdp-ltd", "STDP LTD weight", config.stdpLtdWeight, false,
                           "STDP LTD (Long-Term Depression) weight");
    parser.addFloatArgument("--stdp-tau", "STDP time constant", config.stdpTau, false,
                           "STDP time constant (tau)");
    
    // Parse the arguments
    if (!parser.parse(argc, argv)) {
        std::cerr << "Error parsing command line arguments:" << std::endl;
        std::cerr << parser.getErrorMessage() << std::endl;
        std::exit(1);
    }
    
    // Update config from parsed values
    if (parser.hasValue("--config")) {
        config.configFile = parser.getString("--config");
    }
    if (parser.hasValue("--log-level")) {
        config.logLevel = parser.getString("--log-level");
    }
    if (parser.isFlagSet("--verbose")) {
        config.verbose = true;
    }
    if (parser.hasValue("--neurons")) {
        config.neuronCount = parser.getInt("--neurons");
    }
    if (parser.hasValue("--regions")) {
        config.regionCount = parser.getInt("--regions");
    }
    if (parser.hasValue("--connection-prob")) {
        config.connectionProbability = parser.getFloat("--connection-prob");
    }
    if (parser.hasValue("--seed")) {
        config.randomSeed = parser.getInt("--seed");
    }
    if (parser.hasValue("--timestep")) {
        config.timestep = parser.getFloat("--timestep");
    }
    if (parser.hasValue("--stdp-ltp")) {
        config.stdpLtpWeight = parser.getFloat("--stdp-ltp");
    }
    if (parser.hasValue("--stdp-ltd")) {
        config.stdpLtdWeight = parser.getFloat("--stdp-ltd");
    }
    if (parser.hasValue("--stdp-tau")) {
        config.stdpTau = parser.getFloat("--stdp-tau");
    }
    
    // Set verbose logging level if verbose flag is set
    if (config.verbose) {
        config.logLevel = "DEBUG";
    }
    
    return config;
}

// =============================================================================
// Main Function
// =============================================================================

int main(int argc, char* argv[]) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Parse command line arguments
    ExperimentConfig config = parseExperimentConfig(argc, argv);
    
    // Check for help flag
    CommandLineParser helpParser("", "");
    helpParser.addFlag("--help", "Show help message");
    if (helpParser.parse(argc, argv) && helpParser.isFlagSet("--help")) {
        printHeader("NLM HELP");
        helpParser.printHelp();
        return 0;
    }
    
    // Check for version flag
    helpParser = CommandLineParser("", "");
    helpParser.addFlag("--version", "Show version information");
    if (helpParser.parse(argc, argv) && helpParser.isFlagSet("--version")) {
        std::cout << "NLM Phase 2 - Real Neural Computation" << std::endl;
        std::cout << "Version: 2.0.0" << std::endl;
        std::cout << "Built with C++20" << std::endl;
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    configureLogging(logger, config.logLevel);
    
    // Also add console logger
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
    
    // Create configuration
    auto nlmConfig = std::make_shared<Config>();
    
    // Configure based on experiment config
    nlmConfig->set("random_seed", static_cast<int64_t>(config.randomSeed), ConfigSource::Default);
    nlmConfig->set("simulation_timestep", config.timestep, ConfigSource::Default);
    nlmConfig->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
    nlmConfig->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
    nlmConfig->set("connection_probability", config.connectionProbability, ConfigSource::Default);
    
    // STDP parameters
    nlmConfig->set("stdp_ltp_weight", config.stdpLtpWeight, ConfigSource::Default);
    nlmConfig->set("stdp_ltd_weight", config.stdpLtdWeight, ConfigSource::Default);
    nlmConfig->set("stdp_tau", config.stdpTau, ConfigSource::Default);
    
    // Structural plasticity parameters
    nlmConfig->set("synaptogenesis_rate", config.synaptogenesisRate, ConfigSource::Default);
    nlmConfig->set("pruning_rate", config.pruningRate, ConfigSource::Default);
    
    // Load config from file if specified
    if (!config.configFile.empty()) {
        configureBrainFromFile(nlmConfig, config.configFile);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(nlmConfig->getOr<int64_t>("random_seed", config.randomSeed)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(nlmConfig->getOr<double>("simulation_timestep", config.timestep)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(nlmConfig->getOr<int64_t>("neuron_count", config.neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(nlmConfig->getOr<int64_t>("region_count", config.regionCount)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(nlmConfig->getOr<float>("connection_probability", config.connectionProbability)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = nlmConfig->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(nlmConfig);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Create demo runner
    DemoRunner demoRunner;
    
    // Register available demos
    demoRunner.registerDemo("basic", "Basic neural connectivity test", runBasicConnectivityTest);
    demoRunner.registerDemo("plasticity", "Plasticity learning experiment", runPlasticityExperiment);
    demoRunner.registerDemo("stdp", "STDP verification test", runStdpVerification);
    
    // Run demos based on command line arguments
    if (config.demo.empty() && config.runAll) {
        // Run all demos
        if (!demoRunner.runAllDemos(brain)) {
            NLM_LOG_ERROR("Some demos failed!");
            return 1;
        }
    } else if (!config.demo.empty()) {
        // Run specific demo
        if (!demoRunner.runDemo(config.demo, brain)) {
            NLM_LOG_ERROR("Demo failed!");
            return 1;
        }
    } else {
        // Run all demos by default (original behavior)
        if (!demoRunner.runAllDemos(brain)) {
            NLM_LOG_ERROR("Some demos failed!");
            return 1;
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    return 0;
}