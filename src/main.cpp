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

void printUsage() {
    printBanner();
    std::cout << "\nUsage: nlm [OPTIONS]\n" << std::endl;
    std::cout << "  Run NLM neural simulation with various options.\n" << std::endl;
    std::cout << "Options:\n" << std::endl;
    std::cout << "  --help, -h              Show this help message\n" << std::endl;
    std::cout << "  --config <file>         Load configuration from file (default: configs/default.cfg)\n" << std::endl;
    std::cout << "  --neurons <N>           Set number of neurons (default: 500)\n" << std::endl;
    std::cout << "  --timestep <T>          Set simulation timestep in seconds (default: 0.001)\n" << std::endl;
    std::cout << "  --steps <N>             Number of simulation steps to run (default: 1000)\n" << std::endl;
    std::cout << "  --test-mode             Run in test mode with detailed connectivity and learning verification\n" << std::endl;
    std::cout << "  --fast                  Run faster with fewer neurons and shorter simulation\n" << std::endl;
    std::cout << "  --save <file>           Save brain state to checkpoint file\n" << std::endl;
    std::cout << "  --load <file>           Load brain state from checkpoint file\n" << std::endl;
    std::cout << "\nAdvanced Options:\n" << std::endl;
    std::cout << "  --visualize              Enable visualization (if supported)\n" << std::endl;
    std::cout << "  --benchmark             Run performance benchmark\n" << std::endl;
    std::cout << "  --plot <file>           Save simulation results to file\n" << std::endl;
    std::cout << "  --stats <N>             Collect statistics every N steps\n" << std::endl;
    std::cout << "  --neuron-stats          Collect neuron statistics\n" << std::endl;
    std::cout << "  --synapse-stats         Collect synapse statistics\n" << std::endl;
    std::cout << "  --plasticity            Enable plasticity simulation\n" << std::endl;
    std::cout << "  --development           Enable developmental processes\n" << std::endl;
    std::cout << "  --neuromodulation        Enable neuromodulation simulation\n" << std::endl;
    std::cout << "  --checkpoint <N>        Save checkpoint every N steps\n" << std::endl;
    std::cout << "  --continue              Continue from last checkpoint\n" << std::endl;
    std::cout << "  --version               Show version information\n" << std::endl;
    std::cout << "  --list-configs          List available configuration presets\n" << std::endl;
    std::cout << "  --demo-mode             Run in demo mode with guided examples\n" << std::endl;
    std::cout << "  --compressive-mode      Run with compressive settings for faster execution\n" << std::endl;
    std::cout << "  --research-mode         Run research-grade simulation with extended duration\n" << std::endl;
    std::cout << "  --neuroplasticity-simulation Enable neuroplasticity simulation\n" << std::endl;
    std::cout << "  --memory-simulation <N>  Run memory simulation with specified capacity\n" << std::endl;
    std::cout << "  --sensory-simulation <S>  Run with specified sensory channels\n" << std::endl;
    std::cout << "  --motor-simulation <M>   Run with specified motor actions\n" << std::endl;
    std::cout << "\nExamples:\n" << std::endl;
    std::cout << "  nlm                           Run Phase 2 with default settings\n" << std::endl;
    std::cout << "  nlm --config my_config.cfg      Load custom configuration\n" << std::endl;
    std::cout << "  nlm --neurons 1000 --steps 500 Run with 1000 neurons for 500 steps\n" << std::endl;
    std::cout << "  nlm --fast                     Quick demonstration with reduced complexity\n" << std::endl;
    std::cout << "  nlm --benchmark              Run performance benchmark\n" << std::endl;
    std::cout << "  nlm --visualize --plot results.txt  Run with visualization and save results\n" << std::endl;
}

// Learning Experiment structure for measuring synaptic changes
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

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    int64_t neuronCount = 500;
    double timestep = 0.001;
    int steps = 1000;
    bool testMode = false;
    bool fastMode = false;
    std::string saveFile;
    std::string loadFile;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename" << std::endl;
                return 1;
            }
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                neuronCount = std::stoll(argv[++i]);
            } else {
                std::cerr << "Error: --neurons requires a number" << std::endl;
                return 1;
            }
        } else if (arg == "--timestep") {
            if (i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: --timestep requires a number" << std::endl;
                return 1;
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: --steps requires a number" << std::endl;
                return 1;
            }
        } else if (arg == "--test-mode") {
            testMode = true;
        } else if (arg == "--fast") {
            fastMode = true;
        } else if (arg == "--save") {
            if (i + 1 < argc) {
                saveFile = argv[++i];
            } else {
                std::cerr << "Error: --save requires a filename" << std::endl;
                return 1;
            }
        } else if (arg == "--load") {
            if (i + 1 < argc) {
                loadFile = argv[++i];
            } else {
                std::cerr << "Error: --load requires a filename" << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Warning: Unknown argument \"" << arg << "\" (ignored)" << std::endl;
        }
    }
    
    // Adjust parameters based on mode
    if (fastMode) {
        neuronCount = 100;
        steps = 200;
        timestep = 0.001;
    }
    
    std::cout << "\n=== NLM Advanced Neural Simulation ===" << std::endl;
    std::cout << "Mode: " << (testMode ? "Test Mode" : "Standard Mode") << std::endl;
    std::cout << "Neurons: " << neuronCount << std::endl;
    std::cout << "Timestep: " << timestep << "s" << std::endl;
    std::cout << "Steps: " << steps << std::endl;
    std::cout << "Config file: " << configFile << std::endl;
    
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
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply command-line overrides to configuration
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::CommandLine);
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    config->set("neuron_count", neuronCount, ConfigSource::CommandLine);
    config->set("region_count", 1, ConfigSource::CommandLine); // Fixed for now
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Apply fast mode settings
    if (fastMode) {
        config->set("neuron_count", 100, ConfigSource::CommandLine);
        config->set("connection_probability", 0.1f, ConfigSource::CommandLine);
    }
    
    // Apply test mode settings
    if (testMode) {
        steps = 500; // Run more steps in test mode
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", timestep)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("  simulation_steps: " + std::to_string(steps));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock with configured timestep
    double configuredTimestep = config->getOr<double>("simulation_timestep", timestep);
    SimulationClock clock(configuredTimestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(configuredTimestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Load from checkpoint if specified
    if (!loadFile.empty()) {
        NLM_LOG_INFO("Loading brain state from: " + loadFile);
        if (!brain->load(loadFile)) {
            NLM_LOG_ERROR("Failed to load brain state from: " + loadFile);
            return 1;
        }
    }
    
    // Run main simulation
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Main Simulation ===");
    NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps...");
    
    // Test 1: Basic connectivity (for verification)
    if (testMode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Connectivity Verification ===");
        runBasicConnectivityTest(brain);
    }
    
    // Reset brain for learning experiment
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity learning experiment
    if (testMode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        runPlasticityExperiment(brain);
    }
    
    // Run the main simulation steps
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Main Simulation ===");
    for (int step = 0; step < steps; ++step) {
        brain->step(step, step * configuredTimestep);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Reset and run Test 3: STDP verification (for verification)
    if (testMode) {
        brain->reset();
        brain->initialize();
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        runStdpVerification(brain);
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Save brain state if requested
    if (!saveFile.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + saveFile);
        if (!brain->save(saveFile)) {
            NLM_LOG_ERROR("Failed to save brain state to: " + saveFile);
            return 1;
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Simulation Summary:");
    NLM_LOG_INFO("  Total steps run: " + std::to_string(steps));
    NLM_LOG_INFO("  Final spike count: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Final firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Advanced Neural Simulation Complete!");
    
    return 0;
}

void printVersion() {
    std::cout << "NLM (Neural Learning Machine) - Phase 2" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Implemented: LIF neurons, STDP plasticity, structural plasticity" << std::endl;
}

void listAvailableConfigs() {
    std::cout << "Available configuration presets:" << std::endl;
    std::cout << "  default.cfg - Standard Phase 2 configuration" << std::endl;
    std::cout << "  debug.cfg   - Debug mode with reduced neuron count" << std::endl;
    std::cout << "  fast.cfg    - Fast execution mode" << std::endl;
    std::cout << "  research.cfg - Research-grade settings" << std::endl;
}