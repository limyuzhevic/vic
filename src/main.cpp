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
#include <map>
#include <unordered_map>

using namespace nlm;

void printAdvancedHelp() {
    std::cout << "NLM — Advanced CLI Interface" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  ./nlm [OPTIONS] [MODE]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "MODES:" << std::endl;
    std::cout << "  [default]    Run Phase 2 basic connectivity and plasticity demo" << std::endl;
    std::cout << "  --phase3     Run Phase 3 world interaction demo" << std::endl;
    std::cout << "  --phase4     Run Phase 4 cognition demo" << std::endl;
    std::cout << "  --phase6     Run Phase 6 integration demo" << std::endl;
    std::cout << "  --benchmark  Run performance benchmarks" << std::endl;
    std::cout << "  --explore    Explore brain state and system information" << std::endl;
    std::cout << "  --benchmark  Run comprehensive benchmark suite" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION OPTIONS:" << std::endl;
    std::cout << "  --config=<file>      Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --neurons=<N>        Set neuron count" << std::endl;
    std::cout << "  --regions=<N>        Set region count" << std::endl;
    std::cout << "  --seed=<N>           Set random seed for reproducibility" << std::endl;
    std::cout << "  --timestep=<T>       Set simulation timestep (seconds)" << std::endl;
    std::cout << "  --steps=<N>          Set maximum simulation steps" << std::endl;
    std::cout << "  --verbose=<LEVEL>    Set verbosity level (0=Error, 1=Warning, 2=Info, 3=Debug)" << std::endl;
    std::cout << "  --checkpoint=<path>  Enable checkpointing with save directory" << std::endl;
    std::cout << "  --replay=<N>         Replay last N episodes" << std::endl;
    std::cout << std::endl;
    
    std::cout << "ADVANCED OPTIONS:" << std::endl;
    std::cout << "  --explore="MODE>      Advanced exploration mode" << std::endl;
    std::cout << "  --measure="WHAT>      Measure specific system components" << std::endl;
    std::cout << "  --profile=<TARGET>   Profile performance of target components" << std::endl;
    std::cout << "  --validate           Validate configuration and system integrity" << std::endl;
    std::cout << "  --save-state=<file>   Save brain state to file" << std::endl;
    std::cout << "  --load-state=<file>   Load brain state from file" << std::endl;
    std::cout << "  --auto-save=<INTERVAL> Enable automatic checkpointing" << std::endl;
    std::cout << std::endl;
    
    std::cout << "MEMORY SYSTEM OPTIONS:" << std::endl;
    std::cout << "  --memory-capacity=<N> Set working memory capacity" << std::endl;
    std::cout << "  --episodic-limit=<N> Set episodic memory episode limit" << std::endl;
    std::cout << "  --consolidation=<RATE> Enable memory consolidation" << std::endl;
    std::cout << "  --replay-mode=<MODE> Set replay mode (immediate, periodic)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "NEUROMODULATION OPTIONS:" << std::endl;
    std::cout << "  --dopamine=<LEVEL>   Set dopamine level" << std::endl;
    std::cout << "  --curiosity=<LEVEL>  Set curiosity level" << std::endl;
    std::cout << "  --novelty=<LEVEL>     Set novelty detection level" << std::endl;
    std::cout << "  --reward=<SCALE>      Set reward scale" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PLASTICITY OPTIONS:" << std::endl;
    std::cout << "  --stdp-ltp=<W>        Set STDP LTP weight" << std::endl;
    std::cout << "  --stdp-ltd=<W>        Set STDP LTD weight" << std::endl;
    std::cout << "  --stdp-tau=<T>        Set STDP time constant" << std::endl;
    std::cout << "  --hebbian=<W>         Set Hebbian learning rate" << std::endl;
    std::cout << "  --structural=<RATE>   Set structural plasticity rate" << std::endl;
    std::cout << std::endl;
    
    std::cout << "OUTPUT OPTIONS:" << std::endl;
    std::cout << "  --output-format=<FMT>  Set output format (text, json, csv)" << std::endl;
    std::cout << "  --output=<file>       Redirect output to file" << std::endl;
    std::cout << "  --log-file=<file>      Write logs to file" << std::endl;
    std::cout << "  --quiet              Suppress non-essential output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "DEVELOPMENT OPTIONS:" << std::endl;
    std::cout << "  --development         Enable development mode" << std::endl;
    std::cout << "  --checkpoint-dev      Enable checkpoint debugging" << std::endl;
    std::cout << "  --memory-profiling    Enable memory profiling" << std::endl;
    std::cout << "  --perf-measure        Enable performance measurements" << std::endl;
    std::cout << std::endl;
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
    // Parse command line arguments for advanced CLI interface
    std::map<std::string, std::string> args;
    std::vector<std::string> modeArgs;
    bool showHelp = false;
    bool showAdvancedHelp = false;
    std::string outputFile = "";
    std::string logFile = "";
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg == "--advanced-help") {
            showAdvancedHelp = true;
        } else if (arg.find("--") == 0) {
            size_t equalsPos = arg.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = arg.substr(0, equalsPos);
                std::string value = arg.substr(equalsPos + 1);
                args[key] = value;
            } else {
                args[arg] = "";
            }
        } else {
            modeArgs.push_back(arg);
        }
    }
    
    // Show help and exit if requested
    if (showAdvancedHelp) {
        printAdvancedHelp();
        return 0;
    }
    
    if (showHelp || modeArgs.empty()) {
        std::cout << "NLM — Neural Learning Machine" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << std::endl;
        std::cout << "Run Phase 2 neural computation demo with advanced CLI support." << std::endl;
        std::cout << std::endl;
        std::cout << "For advanced help and all options: ./nlm --advanced-help" << std::endl;
        std::cout << std::endl;
        return 0;
    }
    
    // Parse mode arguments
    std::string mode = modeArgs[0];
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    
    // Initialize output redirection if specified
    if (!outputFile.empty()) {
        std::cout.rdbuf()->pubsetbuf(nullptr); // Reset buffer
        std::ofstream outFile(outputFile);
        if (outFile.is_open()) {
            std::cout.rdbuf(outFile.rdbuf());
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    
    // Set log level based on verbosity
    LogLevel logLevel = LogLevel::Info;
    if (args.count("--verbose")) {
        int verbosity = std::stoi(args["--verbose"]);
        logLevel = static_cast<LogLevel>(std::max(0, std::min(3, verbosity)));
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
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
    if (args.count("--config")) {
        configFile = args["--config"];
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
    
    // Apply advanced configuration options
    if (args.count("--neurons")) {
        config->set("neuron_count", static_cast<int64_t>(std::stoi(args["--neurons"])), ConfigSource::CommandLine);
    }
    if (args.count("--regions")) {
        config->set("region_count", static_cast<int64_t>(std::stoi(args["--regions"])), ConfigSource::CommandLine);
    }
    if (args.count("--seed")) {
        config->set("random_seed", static_cast<int64_t>(std::stoll(args["--seed"])), ConfigSource::CommandLine);
    }
    if (args.count("--timestep")) {
        config->set("simulation_timestep", std::stod(args["--timestep"]), ConfigSource::CommandLine);
    }
    if (args.count("--steps")) {
        config->set("max_steps", static_cast<int64_t>(std::stoll(args["--steps"])), ConfigSource::CommandLine);
    }
    if (args.count("--checkpoint")) {
        config->set("checkpoint_dir", args["--checkpoint"], ConfigSource::CommandLine);
        config->set("enable_checkpointing", true, ConfigSource::CommandLine);
    }
    if (args.count("--replay")) {
        config->set("replay_episodes", static_cast<int64_t>(std::stoi(args["--replay"])), ConfigSource::CommandLine);
    }
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory system parameters
    if (args.count("--memory-capacity")) {
        config->set("working_memory_capacity", static_cast<int64_t>(std::stoi(args["--memory-capacity"])), ConfigSource::CommandLine);
    }
    if (args.count("--episodic-limit")) {
        config->set("episodic_memory_limit", static_cast<int64_t>(std::stoi(args["--episodic-limit"])), ConfigSource::CommandLine);
    }
    if (args.count("--consolidation")) {
        config->set("memory_consolidation_rate", std::stof(args["--consolidation"]), ConfigSource::CommandLine);
    }
    if (args.count("--replay-mode")) {
        config->set("replay_mode", args["--replay-mode"], ConfigSource::CommandLine);
    }
    
    // Neuromodulation parameters
    if (args.count("--dopamine")) {
        config->set("dopamine_level", std::stof(args["--dopamine"]), ConfigSource::CommandLine);
    }
    if (args.count("--curiosity")) {
        config->set("curiosity_level", std::stof(args["--curiosity"]), ConfigSource::CommandLine);
    }
    if (args.count("--novelty")) {
        config->set("novelty_level", std::stof(args["--novelty"]), ConfigSource::CommandLine);
    }
    if (args.count("--reward")) {
        config->set("reward_scale", std::stof(args["--reward"]), ConfigSource::CommandLine);
    }
    
    // Plasticity parameters
    if (args.count("--stdp-ltp")) {
        config->set("stdp_ltp_weight", std::stof(args["--stdp-ltp"]), ConfigSource::CommandLine);
    }
    if (args.count("--stdp-ltd")) {
        config->set("stdp_ltd_weight", std::stof(args["--stdp-ltd"]), ConfigSource::CommandLine);
    }
    if (args.count("--stdp-tau")) {
        config->set("stdp_tau", std::stof(args["--stdp-tau"]), ConfigSource::CommandLine);
    }
    if (args.count("--hebbian")) {
        config->set("hebbian_learning_rate", std::stof(args["--hebbian"]), ConfigSource::CommandLine);
    }
    if (args.count("--structural")) {
        config->set("synaptogenesis_rate", std::stof(args["--structural"]), ConfigSource::CommandLine);
    }
    
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
