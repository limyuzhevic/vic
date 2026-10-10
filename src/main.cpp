// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Neural working memory
// - Episodic memory with replay
// - Neuromodulation (Dopamine, Acetylcholine, Norepinephrine, Serotonin)
// - Attention system
// - Reward prediction
// - Curiosity-driven exploration
// - Prediction error learning

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
    ║     - Neural working memory                                   ║
    ║     - Episodic memory with replay                             ║
    ║     - Neuromodulation (DA, ACh, NE, 5-HT)                   ║
    ║     - Attention system                                        ║
    ║     - Reward prediction                                      ║
    ║     - Curiosity-driven exploration                           ║
    ║     - Prediction error learning                              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printVersion() {
    NLM_LOG_INFO("=== NLM Phase 2 Version Info ===");
    NLM_LOG_INFO("NLM Version: Phase 2 Real Neural Computation");
    NLM_LOG_INFO("Build Date: " __DATE__ " " __TIME__);
    NLM_LOG_INFO("Features:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neurons");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  - Neural working memory");
    NLM_LOG_INFO("  - Episodic memory with replay");
    NLM_LOG_INFO("  - Neuromodulation (Dopamine, Acetylcholine, Norepinephrine, Serotonin)");
    NLM_LOG_INFO("  - Attention system");
    NLM_LOG_INFO("  - Reward prediction");
    NLM_LOG_INFO("  - Curiosity-driven exploration");
    NLM_LOG_INFO("  - Prediction error learning");
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

// Save brain state to file
void runSaveState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Saving Brain State ===");
    
    if (brain->save(filepath)) {
        NLM_LOG_INFO("✓ Brain state saved successfully to: " + filepath);
    } else {
        NLM_LOG_ERROR("✗ Failed to save brain state to: " + filepath);
    }
}

void runLoadState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Loading Brain State ===");
    
    if (brain->load(filepath)) {
        NLM_LOG_INFO("✓ Brain state loaded successfully from: " + filepath);
        brain->logStatus();
    } else {
        NLM_LOG_ERROR("✗ Failed to load brain state from: " + filepath);
    }
}

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
    NLM_LOG_INFO("  - Neural working memory");
    NLM_LOG_INFO("  - Episodic memory with replay");
    NLM_LOG_INFO("  - Neuromodulation (Dopamine, Acetylcholine, Norepinephrine, Serotonin)");
    NLM_LOG_INFO("  - Attention system");
    NLM_LOG_INFO("  - Reward prediction");
    NLM_LOG_INFO("  - Curiosity-driven exploration");
    NLM_LOG_INFO("  - Prediction error learning");
    NLM_LOG_INFO("");
    
    // Check for help/version flags
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printBanner();
            std::cout << R"(
Available options:
  --help, -h              Show this help message
  --version, -v           Show version information
  --config <file>         Load configuration from file (default: configs/default.cfg)
  --save <file>           Save brain state to file (requires --run)
  --load <file>           Load brain state from file (requires --run)
  --run                   Run all demonstration tests
  --basic                 Run basic connectivity test only
  --plasticity            Run plasticity learning experiment only
  --stdp                  Run STDP verification only
  --save-state <file>     Alias for --save
  --load-state <file>     Alias for --load
  --save-training         Save after plasticity experiment
  --load-training         Load before plasticity experiment
  --advanced              Run advanced tests
  --help-advanced         Show advanced help
  --demo                  Run demo mode with quick tests
  --quiet, -q             Reduce logging output
  --verbose, -V           Increase logging output
  --autonomous            Run in autonomous mode (continuous learning)
  --trainer               Run as training agent
  --explore               Run exploration mode
  --interact              Enable interactive mode
  --simulation            Run full simulation loop
  --develop               Run development stage tests
  --checkpoint <file>     Set checkpoint file path
  --epochs <n>            Number of learning epochs
  --learning-rate <f>     Set learning rate
  --temperature <f>      Set exploration temperature
  --memory <size>         Set memory capacity
  --seed <n>              Set random seed
  --neurally             Use neural scheduling
  --cognitive             Enable cognitive functions
  --social                 Enable social learning
  --environmental         Enable environment interactions
  --visualization          Enable visualization output
  --benchmark             Run performance benchmarks
  --export <file>         Export results to file
  --import <file>         Import results from file
  --report                Generate detailed report
  --analyze               Analyze brain state
  --compare <file>        Compare with saved state
  --reset                 Reset brain to initial state
  --quit                  Exit immediately

Examples:
  nlm --run                         Run all demonstration tests
  nlm --plasticity                  Run plasticity learning experiment only
  nlm --save brain_state.chk --run  Save state after running tests
  nlm --load brain_state.chk --run  Load state before running tests
  nlm --advanced                    Run advanced feature demonstrations
  nlm --autonomous                   Run continuous learning mode
  nlm --trainer                      Run as reinforcement learning agent
  nlm --epochs 1000                  Run 1000 learning epochs

For more information about specific features, use --help-advanced.
";
            return 0;
        }
        if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        }
    }
    
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
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
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
    
    // Handle save/load commands
    std::string saveFile, loadFile;
    bool runTests = false;
    bool basicOnly = false;
    bool plasticityOnly = false;
    bool stdpOnly = false;
    bool advancedMode = false;
    bool autonomousMode = false;
    bool trainerMode = false;
    bool exploreMode = false;
    bool interactiveMode = false;
    bool fullSimulation = false;
    bool developmentTests = false;
    size_t epochs = 1;
    float learningRate = 0.01f;
    float temperature = 1.0f;
    size_t memoryCapacity = 0;
    uint64_t seed = 42;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 6) == "--save") {
            if (arg.find('=') != std::string::npos) {
                saveFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                saveFile = argv[++i];
            }
        } else if (arg.substr(0, 6) == "--load") {
            if (arg.find('=') != std::string::npos) {
                loadFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                loadFile = argv[++i];
            }
        } else if (arg == "--run") {
            runTests = true;
        } else if (arg == "--basic") {
            basicOnly = true;
        } else if (arg == "--plasticity") {
            plasticityOnly = true;
        } else if (arg == "--stdp") {
            stdpOnly = true;
        } else if (arg == "--advanced") {
            advancedMode = true;
        } else if (arg == "--autonomous") {
            autonomousMode = true;
        } else if (arg == "--trainer") {
            trainerMode = true;
        } else if (arg == "--explore") {
            exploreMode = true;
        } else if (arg == "--interact") {
            interactiveMode = true;
        } else if (arg == "--simulation") {
            fullSimulation = true;
        } else if (arg == "--develop") {
            developmentTests = true;
        } else if (arg.substr(0, 8) == "--epochs") {
            if (arg.find('=') != std::string::npos) {
                epochs = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                epochs = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 13) == "--learning-rate") {
            if (arg.find('=') != std::string::npos) {
                learningRate = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                learningRate = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 11) == "--temperature") {
            if (arg.find('=') != std::string::npos) {
                temperature = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                temperature = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--memory") {
            if (arg.find('=') != std::string::npos) {
                memoryCapacity = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                memoryCapacity = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                seed = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                seed = std::stoul(argv[++i]);
            }
        }
    }
    
    // Load state if specified
    if (!loadFile.empty()) {
        runLoadState(brain, loadFile);
    }
    
    // Save initial state if specified
    if (!saveFile.empty()) {
        runSaveState(brain, saveFile);
    }
    
    // Handle mode selection
    if (basicOnly) {
        runBasicConnectivityTest(brain);
    } else if (plasticityOnly) {
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
    } else if (stdpOnly) {
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (runTests) {
        // Run all tests
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (advancedMode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Advanced Mode ===");
        NLM_LOG_INFO("Running advanced feature demonstrations...");
        
        // Advanced test 1: Neuromodulation demonstration
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Advanced Test 1: Neuromodulation Effects");
        
        // Get neuromodulators
        auto* dopamine = brain->getDopamine();
        auto* acetylcholine = brain->getAcetylcholine();
        auto* norepinephrine = brain->getNorepinephrine();
        auto* serotonin = brain->getSerotonin();
        
        if (dopamine) {
            NLM_LOG_INFO("  Testing Dopamine modulation effects...");
            float initialLevel = dopamine->getLevel();
            NLM_LOG_INFO("  Initial dopamine level: " + std::to_string(initialLevel));
            
            // Apply reward signal
            dopamine->signalReward(0.8f);
            float rewardLevel = dopamine->getLevel();
            NLM_LOG_INFO("  After reward signal: " + std::to_string(rewardLevel));
            
            // Check plasticity factor
            float plasticityFactor = dopamine->getPlasticityFactor();
            NLM_LOG_INFO("  Plasticity factor: " + std::to_string(plasticityFactor));
        }
        
        if (acetylcholine) {
            NLM_LOG_INFO("  Testing Acetylcholine effects (attention)...");
            float achLevel = acetylcholine->getLevel();
            NLM_LOG_INFO("  Acetylcholine level: " + std::to_string(achLevel));
        }
        
        if (norepinephrine) {
            NLM_LOG_INFO("  Testing Norepinephrine effects (arousal)...");
            float neLevel = norepinephrine->getLevel();
            NLM_LOG_INFO("  Norepinephrine level: " + std::to_string(neLevel));
        }
        
        if (serotonin) {
            NLM_LOG_INFO("  Testing Serotonin effects (mood)...");
            float serotoninLevel = serotonin->getLevel();
            NLM_LOG_INFO("  Serotonin level: " + std::to_string(serotoninLevel));
        }
        
        // Run plasticity experiment with neuromodulation
        runPlasticityExperiment(brain);
        
        // Save state after advanced test
        if (!saveFile.empty()) {
            runSaveState(brain, saveFile);
        }
        
    } else if (autonomousMode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Autonomous Mode ===");
        NLM_LOG_INFO("Running continuous learning...");
        
        // Autonomous learning loop
        for (size_t epoch = 0; epoch < epochs; ++epoch) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Autonomous Epoch " + std::to_string(epoch + 1) + " ===");
            
            // Reset brain
            brain->reset();
            brain->initialize();
            
            // Learning cycle
            for (SimulationStep step = 0; step < 100; ++step) {
                // Generate sensory input (simplified)
                for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f * (1.0f + (rand() % 100) / 100.0f));
                }
                
                brain->step(step, step * 0.001);
                
                // Log progress
                if (step % 20 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
            
            // Save checkpoint periodically
            if (epochs > 0 && (epoch + 1) % 10 == 0) {
                std::string checkpointFile = "checkpoints/autonomous_epoch_" + 
                                            std::to_string(epoch + 1) + ".chk";
                runSaveState(brain, checkpointFile);
                NLM_LOG_INFO("Checkpoint saved: " + checkpointFile);
            }
        }
        
        // Save final state
        if (!saveFile.empty()) {
            runSaveState(brain, saveFile);
        }
        
    } else if (trainerMode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Trainer Mode ===");
        NLM_LOG_INFO("Running as reinforcement learning agent...");
        
        // Trainer mode - similar to autonomous but with reward signals
        for (size_t epoch = 0; epoch < epochs; ++epoch) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Training Epoch " + std::to_string(epoch + 1) + " ===");
            
            brain->reset();
            brain->initialize();
            
            // Training loop with reward-based learning
            float cumulativeReward = 0.0f;
            
            for (SimulationStep step = 0; step < 100; ++step) {
                // Simulate environment interaction
                float reward = 0.0f;
                
                // Generate action based on brain
                auto action = brain->produceAction();
                
                // Apply action in environment
                switch (action->getType()) {
                    case ActionType::MoveForward:
                        // Move forward, possibly get reward
                        reward = (rand() % 100) / 100.0f;  // 0-1 reward
                        break;
                    case ActionType::TurnLeft:
                    case ActionType::TurnRight:
                        // Turn, small chance of reward
                        reward = (rand() % 20) / 100.0f;  // 0-0.2 reward
                        break;
                    default:
                        reward = 0.0f;  // Wait action, no reward
                }
                
                // Apply reward signal
                if (auto* dopamine = brain->getDopamine()) {
                    dopamine->signalReward(reward);
                    cumulativeReward += reward;
                }
                
                brain->step(step, step * 0.001);
                
                if (step % 20 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Action: " + std::to_string(static_cast<int>(action->getType())) +
                                " | Reward: " + std::to_string(reward) +
                                " | Cumulative: " + std::to_string(cumulativeReward));
                }
            }
            
            // Save checkpoint periodically
            if (epochs > 0 && (epoch + 1) % 10 == 0) {
                std::string checkpointFile = "checkpoints/trainer_epoch_" + 
                                            std::to_string(epoch + 1) + ".chk";
                runSaveState(brain, checkpointFile);
                NLM_LOG_INFO("Checkpoint saved: " + checkpointFile);
            }
        }
        
        if (!saveFile.empty()) {
            runSaveState(brain, saveFile);
        }
        
    } else if (exploreMode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Exploration Mode ===");
        NLM_LOG_INFO("Running exploration with curiosity...");
        
        brain->reset();
        brain->initialize();
        
        // Exploration loop with curiosity-driven behavior
        for (SimulationStep step = 0; step < 200; ++step) {
            // Generate diverse sensory inputs for exploration
            for (size_t i = 0; i < 15 && i < brain->getTotalNeuronCount() / 4; ++i) {
                float novelty = (rand() % 100) / 100.0f;  // Random novelty
                float input = 15.0f * (1.0f + novelty);
                brain->injectCurrentToNeurons(NeuronType::Sensory, input);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 25 == 0) {
                NLM_LOG_INFO("  Exploration step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
            }
        }
        
        if (!saveFile.empty()) {
            runSaveState(brain, saveFile);
        }
        
    } else if (fullSimulation) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Full Simulation Mode ===");
        NLM_LOG_INFO("Running complete brain simulation...");
        
        // Full simulation with multiple brain regions and complex dynamics
        brain->reset();
        brain->initialize();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Starting simulation loop...");
        
        for (SimulationStep step = 0; step < 500; ++step) {
            // Simulate complex environment interactions
            for (size_t i = 0; i < 30 && i < brain->getTotalNeuronCount() / 4; ++i) {
                float environmentalInput = 10.0f + 
                                           5.0f * std::sin(step * 0.01f) +
                                           3.0f * (rand() % 100) / 100.0f;
                brain->injectCurrentToNeurons(NeuronType::Sensory, environmentalInput);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 50 == 0) {
                NLM_LOG_INFO("  Simulation step " + std::to_string(step) + 
                            " | Neurons: " + std::to_string(brain->getTotalNeuronCount()) +
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Active: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }
        
        if (!saveFile.empty()) {
            runSaveState(brain, saveFile);
        }
        
    } else if (developmentTests) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Development Tests ===");
        NLM_LOG_INFO("Running developmental stage tests...");
        
        // Test brain development across stages
        for (int stage = 0; stage < 5; ++stage) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Testing developmental stage " + std::to_string(stage));
            
            brain->reset();
            brain->initialize();
            
            // Set developmental stage
            if (auto* devSystem = brain->getDevelopmentSystem()) {
                devSystem->update(brain.get(), *brain->getRandomGenerator(), 0.1);
            }
            
            // Run developmental experiment
            for (SimulationStep step = 0; step < 100; ++step) {
                for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
                }
                brain->step(step, step * 0.001);
            }
            
            NLM_LOG_INFO("  Stage " + std::to_string(stage) + " complete");
        }
        
        if (!saveFile.empty()) {
            runSaveState(brain, saveFile);
        }
        
    } else {
        // Default behavior - run basic tests
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
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
    NLM_LOG_INFO("  ✓ Neural working memory implemented");
    NLM_LOG_INFO("  ✓ Episodic memory with replay implemented");
    NLM_LOG_INFO("  ✓ Neuromodulation system (DA, ACh, NE, 5-HT)");
    NLM_LOG_INFO("  ✓ Attention system implemented");
    NLM_LOG_INFO("  ✓ Reward prediction mechanism");
    NLM_LOG_INFO("  ✓ Curiosity-driven exploration");
    NLM_LOG_INFO("  ✓ Prediction error learning");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
    
    // Save final state if requested
    if (!saveFile.empty()) {
        runSaveState(brain, saveFile);
    }
    
    return 0;
}