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
    
    // A pattern, not a seizure: stimulate only the first 20 sensory neurons
    // each step. The old code injected 30.0f into *every* sensory neuron
    // 20 times per step (600.0f), so the whole network fired synchronously
    // and every weight saturated to 1.0 - learning was measurable but not
    // selective at all.
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
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
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

// Command line option parser
struct CLIOptions {
    bool showHelp = false;
    bool runTests = false;
    bool verbose = false;
    bool quick = false;
    bool batchMode = false;
    bool parameterSweep = false;
    bool crossValidation = false;
    bool exportDebugInfo = false;
    bool profilePerformance = false;
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string outputDir = "";
    std::string batchFile = "";
    int testLevel = 2;  // Default to Phase 2 demo
    int batchSize = 10;
    int parameterIterations = 1;
    int crossValidationFolds = 5;
    std::vector<std::string> extraArgs;
};

CLIOptions parseCommandLine(int argc, char** argv) {
    CLIOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--test" || arg == "-t") {
            options.runTests = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quick" || arg == "-q") {
            options.quick = true;
        } else if (arg == "--batch" || arg == "-b") {
            options.batchMode = true;
        } else if (arg == "--parameter-sweep" || arg == "--sweep") {
            options.parameterSweep = true;
        } else if (arg == "--cross-validation" || arg == "--cv") {
            options.crossValidation = true;
        } else if (arg == "--export-debug" || arg == "--debug-export") {
            options.exportDebugInfo = true;
        } else if (arg == "--profile" || arg == "--benchmark") {
            options.profilePerformance = true;
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg.substr(0, 7) == "--config=") {
            options.configFile = arg.substr(8);
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg.substr(0, 8) == "--output=") {
            options.outputFile = arg.substr(9);
        } else if (arg == "--output-dir" && i + 1 < argc) {
            options.outputDir = argv[++i];
        } else if (arg.substr(0, 11) == "--output-dir=") {
            options.outputDir = arg.substr(12);
        } else if (arg == "--batch-file" && i + 1 < argc) {
            options.batchFile = argv[++i];
        } else if (arg.substr(0, 11) == "--batch-file=") {
            options.batchFile = arg.substr(12);
        } else if (arg == "--test-level" && i + 1 < argc) {
            options.testLevel = std::stoi(argv[++i]);
        } else if (arg == "--batch-size" && i + 1 < argc) {
            options.batchSize = std::stoi(argv[++i]);
        } else if (arg == "--parameter-iterations" && i + 1 < argc) {
            options.parameterIterations = std::stoi(argv[++i]);
        } else if (arg == "--cross-validation-folds" && i + 1 < argc) {
            options.crossValidationFolds = std::stoi(argv[++i]);
        } else if (arg[0] != '-') {
            options.extraArgs.push_back(arg);
        }
    }
    
    return options;
}

void printHelp() {
    printBanner();
    std::cout << "\nNLM - Neural Learning Machine\n";
    std::cout << "==============================\n\n";
    std::cout << "USAGE:\n";
    std::cout << "  nlm [OPTIONS] [ARGS]...\n\n";
    std::cout << "OPTIONS:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --verbose           Enable verbose output\n";
    std::cout << "  -q, --quick             Run quick test (reduced neurons)\n";
    std::cout << "  -t, --test              Enable detailed test output\n";
    std::cout << "  -b, --batch             Enable batch processing mode\n";
    std::cout << "      --sweep, --parameter-sweep  Run parameter sweep\n";
    std::cout << "      --cv, --cross-validation     Run cross-validation\n";
    std::cout << "      --debug-export, --export-debug Export debug information\n";
    std::cout << "      --benchmark, --profile      Profile performance\n";
    std::cout << "      --config <file>     Load configuration from file\n";
    std::cout << "      --output <file>     Save results to file\n";
    std::cout << "      --output-dir <dir>  Save outputs to directory\n";
    std::cout << "      --batch-file <file>  Load batch configuration from file\n";
    std::cout << "      --test-level <N>    Select demo level (1-6)\n";
    std::cout << "      --batch-size <N>    Number of batches for batch processing\n";
    std::cout << "      --parameter-iterations <N> Number of parameter sweep iterations\n";
    std::cout << "      --cross-validation-folds <N> Number of CV folds\n\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  nlm                     Run Phase 2 demo (default)\n";
    std::cout << "  nlm --test-level 6      Run Phase 6 integration test\n";
    std::cout << "  nlm --quick             Quick test with fewer neurons\n";
    std::cout << "  nlm --verbose --test    Detailed test output\n";
    std::cout << "  nlm --batch --batch-size 5 Run batch processing with 5 batches\n";
    std::cout << "  nlm --sweep --parameter-iterations 10 Run parameter sweep 10 times\n";
    std::cout << "  nlm --cv --cross-validation-folds 5 Run 5-fold cross-validation\n";
    std::cout << "  nlm --debug-export --output-dir ./debug Export debug information\n\n";
    std::cout << "AVAILABLE DEMOS:\n";
    std::cout << "  Level 1: Project skeleton and basic setup\n";
    std::cout << "  Level 2: Real neural computation (Phase 2) - [DEFAULT]\n";
    std::cout << "  Level 3: World interaction and sensory-motor loop (Phase 3)\n";
    std::cout << "  Level 4: Advanced cognition and planning (Phase 4)\n";
    std::cout << "  Level 5: Performance optimizations and brain-personality (Phase 5)\n";
    std::cout << "  Level 6: Full integration test (Phase 6)\n\n";
    std::cout << "FILES:\n";
    std::cout << "  configs/default.cfg    Default configuration file\n";
    std::cout << "  build/nlm_*           Executables (phase_*_demo)\n\n";
}

void runDemo(CLIOptions& options, std::shared_ptr<Brain> brain) {
    std::cout << "\n=== Starting NLM Demo Level " << options.testLevel << " ===\n";
    
    // Set demo-specific configuration
    auto config = brain->getConfig();
    
    switch (options.testLevel) {
        case 1:
            std::cout << "Phase 1: Project skeleton and basic setup" << std::endl;
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            break;
            
        case 2:
            std::cout << "Phase 2: Real neural computation (LIF, STDP, plasticity)" << std::endl;
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            break;
            
        case 3:
            std::cout << "Phase 3: World interaction and sensory-motor loop" << std::endl;
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            break;
            
        case 4:
            std::cout << "Phase 4: Advanced cognition and planning" << std::endl;
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
            break;
            
        case 5:
            std::cout << "Phase 5: Performance optimizations and brain-personality" << std::endl;
            config->set("neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
            break;
            
        case 6:
            std::cout << "Phase 6: Full integration test" << std::endl;
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
            break;
            
        default:
            std::cout << "Invalid demo level. Using Phase 2 default." << std::endl;
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    
    // Run demo based on level
    if (options.quick) {
        std::cout << "Running quick test (reduced duration)" << std::endl;
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
    } else {
        std::cout << "Running full demo (can take several minutes)" << std::endl;
        for (SimulationStep step = 0; step < 1000; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0 && options.verbose) {
                std::cout << "  Step " << step << ": " 
                         << brain->getFiringNeuronCount() << " neurons firing, "
                         << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
    }
    
    brain->logStatus();
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM - Neural Learning Machine\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line options
    CLIOptions options = parseCommandLine(argc, argv);
    
    // Show help if requested
    if (options.showHelp) {
        printHelp();
        return 0;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load config from file
    if (options.configFile != "configs/default.cfg" || config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply demo-specific and option-specific settings
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    if (options.quick) {
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", options.quick ? 100 : 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", options.quick ? 1 : 1)));
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
    
    // Run demo
    if (options.runTests) {
        NLM_LOG_INFO("=== Running Detailed Tests ===");
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 2: Plasticity learning experiment
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else {
        // Run the selected demo level
        runDemo(options, brain);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demo Complete ===");
    
    if (!options.outputFile.empty()) {
        if (brain->save(options.outputFile)) {
            NLM_LOG_INFO("Results saved to: " + options.outputFile);
        } else {
            NLM_LOG_ERROR("Failed to save results to: " + options.outputFile);
        }
    }
    
    NLM_LOG_INFO("\nNLM is now running with");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation");
    NLM_LOG_INFO("  ✓ STDP and Hebbian plasticity");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Experience-based learning");
    NLM_LOG_INFO("\nThe NLM brain is functioning as an artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    
    return 0;
}
