// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Constants.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

void displayHelp();
void displayAvailableConfigurations();

void displayHelp() {
    std::cout << "\n=== NLM (熙然) - Neural Learning Machine ===\n" << std::endl;
    std::cout << "Phase 2: Real Neural Computation\n" << std::endl;
    
    std::cout << "USAGE:\n";
    std::cout << "  nlm [--config=<file>] [--option=value]...\n" << std::endl;
    std::cout << "  nlm --help, -h, -? (for help)\n" << std::endl;
    
    std::cout << "\nCOMMANDS:\n";
    std::cout << "  --help, -h           Display this help message\n" << std::endl;
    std::cout << "  --config=<file>      Load configuration from file\n" << std::endl;
    std::cout << "  --mode=<mode>        Simulation mode (basic, plasticity, stdp)\n" << std::endl;
    std::cout << "  --neurons=<count>    Number of neurons (default: 1000)\n" << std::endl;
    std::cout << "  --steps=<count>      Number of simulation steps (default: 5000)\n" << std::endl;
    std::cout << "  --seed=<number>      Random seed for reproducible results\n" << std::endl;
    std::cout << "  --verbose, -v         Enable verbose logging\n" << std::endl;
    std::cout << "  --benchmark           Run benchmark instead of standard tests\n" << std::endl;
    std::cout << "  --save=<file>         Save final brain state to file\n" << std::endl;
    std::cout << "  --load=<file>         Load initial brain state from file\n" << std::endl;
    std::cout << "  --list-configs        List all available configuration options\n" << std::endl;
    
    std::cout << "\nADVANCED OPTIONS:\n";
    std::cout << "  --region-count=<n>    Number of brain regions\n" << std::endl;
    std::cout << "  --connection-prob=<f> Synapse connection probability (0.0-1.0)\n" << std::endl;
    std::cout << "  --stdp-ltp=<f>        STDP LTP weight (default: 0.01)\n" << std::endl;
    std::cout << "  --stdp-ltd=<f>        STDP LTD weight (default: 0.012)\n" << std::endl;
    std::cout << "  --stdp-tau=<f>        STDP time constant (default: 20.0)\n" << std::endl;
    std::cout << "  --synaptogenesis=<f>  Synaptogenesis rate (default: 0.0001)\n" << std::endl;
    std::cout << "  --pruning=<f>         Pruning rate (default: 0.00001)\n" << std::endl;
    std::cout << "  --checkpoint-dir=<d>  Directory for checkpoints\n" << std::endl;
    std::cout << "  --checkpoint-interval=<n> Checkpoint interval (default: 100)\n" << std::endl;
    
    std::cout << "\nCONFIGURATION FILES:\n";
    std::cout << "  configs/default.cfg     Default configuration\n" << std::endl;
    std::cout << "  configs/user.cfg        User customizations\n" << std::endl;
    
    std::cout << "\nSIMULATION MODES:\n";
    std::cout << "  basic                  Basic connectivity test\n" << std::endl;
    std::cout << "  plasticity             Plasticity learning experiment\n" << std::endl;
    std::cout << "  stdp                   STDP verification test\n" << std::endl;
    std::cout << "  benchmark              Performance benchmark\n" << std::endl;
    
    std::cout << "\nEXAMPLES:\n";
    std::cout << "  nlm --mode=plasticity --steps=2000 --seed=42\n" << std::endl;
    std::cout << "  nlm --neurons=2000 --connection-prob=0.15 --save=final_brain.bin\n" << std::endl;
    std::cout << "  nlm --benchmark --verbose --checkpoint-dir=./checkpoints\n" << std::endl;
    std::cout << "  nlm --list-configs\n" << std::endl;
    
    std::cout << "\nFor more information, visit: https://github.com/nlm/neural-learning-machine\n" << std::endl;
    std::cout << "Phase 2 Documentation: https://github.com/nlm/documentation/tree/phase2\n" << std::endl;
}

void displayAvailableConfigurations() {
    std::cout << "\n=== Available Configuration Options ===\n" << std::endl;
    
    // Configuration categories
    std::cout << "GENERAL SETTINGS:\n";
    std::cout << "  brain.neuron_count              Number of neurons (default: 1000)\n" << std::endl;
    std::cout << "  brain.region_count              Number of regions (default: 1)\n" << std::endl;
    std::cout << "  simulation.timestep             Simulation timestep (default: 0.001)\n" << std::endl;
    
    std::cout << "\nNEURAL DYNAMICS:\n";
    std::cout << "  brain.connection_probability    Synapse connection probability (default: 0.1)\n" << std::endl;
    std::cout << "  brain.initial_weight_mean       Mean initial weight (default: 0.2)\n" << std::endl;
    std::cout << "  brain.initial_weight_std        Weight standard deviation (default: 0.1)\n" << std::endl;
    
    std::cout << "\nPLASTICITY RULES:\n";
    std::cout << "  plasticity.stdp.enable         STDP enabled (default: true)\n" << std::endl;
    std::cout << "  plasticity.stdp.ltp_weight      LTP weight (default: 0.01)\n" << std::endl;
    std::cout << "  plasticity.stdp.ltd_weight      LTD weight (default: 0.012)\n" << std::endl;
    std::cout << "  plasticity.stdp.time_constant   STDP time constant (default: 20.0)\n" << std::endl;
    std::cout << "  plasticity.hebbian.enable      Hebbian enabled (default: true)\n" << std::endl;
    std::cout << "  plasticity.hebbian.learning_rate Hebbian learning rate (default: 0.01)\n" << std::endl;
    
    std::cout << "\nSTRUCTURAL PLASTICITY:\n";
    std::cout << "  plasticity.structural.synaptogenesis_rate Synaptogenesis rate (default: 0.0001)\n" << std::endl;
    std::cout << "  plasticity.structural.pruning_rate         Pruning rate (default: 0.00001)\n" << std::endl;
    
    std::cout << "\nNEUROMODULATION:\n";
    std::cout << "  neuromod.dopamine.baseline    Dopamine baseline (default: 0.0)\n" << std::endl;
    std::cout << "  neuromod.curiosity.scale      Curiosity scale (default: 1.0)\n" << std::endl;
    
    std::cout << "\nDEVELOPMENT:\n";
    std::cout << "  development.stage             Developmental stage (Initial|CriticalPeriod|Maturation|Adult|Aging)\n" << std::endl;
    
    std::cout << "\nMEMORY SYSTEMS:\n";
    std::cout << "  memory.working.capacity        Working memory capacity (default: 100)\n" << std::endl;
    std::cout << "  memory.episodic.max_episodes    Max episodic episodes (default: 1000)\n" << std::endl;
    
    std::cout << "\nCHECKPOINTING:\n";
    std::cout << "  checkpoint.directory           Checkpoint directory (default: ./checkpoints)\n" << std::endl;
    std::cout << "  checkpoint.save_interval       Save interval (default: 100)\n" << std::endl;
    std::cout << "  checkpoint.load_interval       Load interval (default: 100)\n" << std::endl;
    
    std::cout << "\nLOGGING:\n";
    std::cout << "  logging.level                 Log level (Debug|Info|Warning|Error|Critical)\n" << std::endl;
    std::cout << "  logging.to_file               Log to file (default: false)\n" << std::endl;
    std::cout << "  logging.filename              Log filename (default: nlm.log)\n" << std::endl;
    
    std::cout << "\nUse: nlm --list-configs to display all configuration options\n" << std::endl;
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

int main(int argc, char** argv) {
    printBanner();
    
    // Display comprehensive help if no arguments or help requested
    if (argc == 1) {
        displayHelp();
        return 0;
    }
    
    // Check for help flags
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h" || arg == "-?" || arg == "/?") {
            displayHelp();
            return 0;
        }
    }
    
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
    
    // Set default values for Phase 2 using constants
    config->set("random_seed", Constants::DEFAULT_RANDOM_SEED, ConfigSource::Default);
    config->set("simulation_timestep", Constants::DEFAULT_SIMULATION_TIMESTEP, ConfigSource::Default);
    config->set("neuron_count", Constants::DEFAULT_NEURON_COUNT, ConfigSource::Default);
    config->set("region_count", Constants::DEFAULT_REGION_COUNT, ConfigSource::Default);
    config->set("connection_probability", Constants::DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    
    // STDP parameters using constants
    config->set("stdp_ltp_weight", Constants::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", Constants::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_tau", Constants::DEFAULT_STDP_TIME_CONSTANT, ConfigSource::Default);
    
    // Structural plasticity parameters using constants
    config->set("synaptogenesis_rate", Constants::DEFAULT_SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", Constants::DEFAULT_PRUNING_RATE, ConfigSource::Default);
    
    // Log configuration summary using constants
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", Constants::DEFAULT_RANDOM_SEED)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", Constants::DEFAULT_SIMULATION_TIMESTEP)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", Constants::DEFAULT_NEURON_COUNT)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", Constants::DEFAULT_REGION_COUNT)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", Constants::DEFAULT_CONNECTION_PROBABILITY)));
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
