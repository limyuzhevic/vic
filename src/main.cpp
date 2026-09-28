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

void printAdvancedHelp() {
    std::cout << R"(
Advanced NLM Phase 2 Options:

Experiment Options:
  --experiment <name>     Run specific experiment (basic|plasticity|stdp|all)
  --config <file>         Load configuration from file (default: configs/default.cfg)
  --verbose <level>       Set verbosity level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)
  --log-to-file <file>     Log output to file instead of stdout
  --benchmark              Run performance benchmarks
  --record-video          Record simulation video (requires visualization)
  --no-visualization       Disable visualization
  --iterations <n>        Number of simulation iterations for plasticity experiments
  --dt <value>            Custom simulation timestep
  --neurons <n>           Override neuron count
  --regions <n>           Override region count
  --connections <p>       Override connection probability
  --enable-memory         Enable memory systems for experiments
  --enable-prediction      Enable prediction systems for experiments
  --enable-cognition       Enable cognition systems for experiments
  --enable-neuromodulation  Enable neuromodulation systems

Config Override Options:
  --random_seed <n>       Override random seed
  --stdp_ltp_weight <w>    Override STDP LTP weight
  --stdp_ltd_weight <w>    Override STDP LTD weight
  --stdp_time_constant <t> Override STDP time constant
  --synaptogenesis_rate <r> Override synaptogenesis rate
  --pruning_rate <r>       Override pruning rate
  
Learning & Analysis Options:
  --analyze-learning       Analyze learning curves and performance
  --track-memory          Track memory usage during simulation
  --export-data <format>   Export results in specified format (json/csv/plot)
  --export-path <dir>      Set export directory
  --save-checkpoint <file> Save brain state checkpoint
  --load-checkpoint <file> Load brain state checkpoint
  --monte-carlo <n>       Run Monte Carlo simulations with different random seeds
  
Debug & Diagnostic Options:
  --diagnose-neural        Run neural connectivity diagnostics
  --diagnose-plasticity    Run plasticity diagnostics
  --diagnose-memory        Run memory system diagnostics
  --diagnose-cognition     Run cognition system diagnostics
  --diagnose-neuromod       Run neuromodulation diagnostics
  --trace-events          Trace spike and synaptic events
  --memory-profiler        Enable memory profiling
  --performance-profiler   Enable performance profiling
  
Visualization Options:
  --visualization-port <p> Port for visualization server
  --visualization-host <h> Host for visualization server
  --snapshot <file>        Save visualization snapshot
  --record-movies         Record simulation as movie
  
Phase-Specific Options:
  --phase <1|2|3|4>        Specify phase to run (default: 2)
  --enable-all-systems     Enable all available systems
  --disable-all-systems    Disable all optional systems
  --optimize-memory        Optimize memory usage
  --optimize-performance   Optimize for performance
  --research-mode         Enable research features
  )" << std::endl;
}

void printHelp() {
    printBanner();
    std::cout << R"(
USAGE: nlm [OPTIONS]

Basic Options:
  --help                   Show help information
  --advanced-help          Show advanced options and help
  --version                Show version information

Configuration Options:
  --config <file>          Load configuration from file (default: configs/default.cfg)
  --random_seed <n>        Override random seed
  --neurons <n>            Override neuron count
  --regions <n>            Override region count
  --connections <p>        Override connection probability

Experiment Selection:
  --experiment <name>       Run specific experiment (basic|plasticity|stdp|all|learning)
  --phase <1|2|3|4>         Specify phase to run (default: 2)
  --iterations <n>          Number of simulation iterations for plasticity experiments
  --dt <value>              Custom simulation timestep

Output Options:
  --verbose <level>        Set verbosity level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG, 4=TRACE)
  --log-to-file <file>      Log output to file instead of stdout
  --no-color               Disable colored output
  --quiet                  Suppress most output

Advanced Options:
  --enable-memory          Enable memory systems for experiments
  --enable-prediction      Enable prediction systems for experiments
  --enable-cognition       Enable cognition systems for experiments
  --enable-neuromodulation  Enable neuromodulation systems
  --enable-visualization   Enable visualization (default: enabled)
  --benchmark              Run performance benchmarks
  --analyze-learning       Analyze learning curves and performance
  --export-data <format>    Export results in specified format (json/csv/plot)
  --export-path <dir>       Set export directory
  --save-checkpoint <file>  Save brain state checkpoint
  --load-checkpoint <file>  Load brain state checkpoint
  --monte-carlo <n>        Run Monte Carlo simulations with different random seeds
  
Debug Options:
  --diagnose-neural         Run neural connectivity diagnostics
  --diagnose-plasticity     Run plasticity diagnostics
  --diagnose-memory         Run memory system diagnostics
  --diagnose-cognition      Run cognition system diagnostics
  --diagnose-neuromod       Run neuromodulation diagnostics
  --trace-events           Trace spike and synaptic events
  
  )" << std::endl;
    std::cout << "\nFor advanced options and detailed help, use --advanced-help" << std::endl;
}
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

int main(int argc, char** argv) {
    // Parse command line arguments
    bool showHelp = false;
    bool showAdvancedHelp = false;
    bool showVersion = false;
    std::string configFile = "configs/default.cfg";
    std::string experimentName = "all";
    int phase = 2;
    int verbosity = 2;
    bool enableMemory = true;
    bool enablePrediction = true;
    bool enableCognition = true;
    bool enableNeuromodulation = true;
    bool enableVisualization = true;
    bool benchmark = false;
    bool analyzeLearning = false;
    bool enableDiagnostics = false;
    std::string exportFormat = "";
    std::string exportPath = ".";
    std::string saveCheckpoint = "";
    std::string loadCheckpoint = "";
    int monteCarloRuns = 0;
    int customIterations = 0;
    double customDt = 0.0;
    int overrideNeurons = -1;
    int overrideRegions = -1;
    double overrideConnections = -1.0;
    int overrideRandomSeed = -1;
    std::string logToFile = "";
    bool disableColor = false;
    bool quietMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            showHelp = true;
        } else if (arg == "--advanced-help") {
            showAdvancedHelp = true;
        } else if (arg == "--version") {
            showVersion = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--experiment" && i + 1 < argc) {
            experimentName = argv[++i];
        } else if (arg == "--phase" && i + 1 < argc) {
            phase = std::atoi(argv[++i]);
        } else if (arg == "--iterations" && i + 1 < argc) {
            customIterations = std::atoi(argv[++i]);
        } else if (arg == "--dt" && i + 1 < argc) {
            customDt = std::atof(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            overrideNeurons = std::atoi(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            overrideRegions = std::atoi(argv[++i]);
        } else if (arg == "--connections" && i + 1 < argc) {
            overrideConnections = std::atof(argv[++i]);
        } else if (arg == "--verbose" && i + 1 < argc) {
            verbosity = std::atoi(argv[++i]);
        } else if (arg == "--log-to-file" && i + 1 < argc) {
            logToFile = argv[++i];
        } else if (arg == "--enable-memory") {
            enableMemory = true;
        } else if (arg == "--disable-memory") {
            enableMemory = false;
        } else if (arg == "--enable-prediction") {
            enablePrediction = true;
        } else if (arg == "--disable-prediction") {
            enablePrediction = false;
        } else if (arg == "--enable-cognition") {
            enableCognition = true;
        } else if (arg == "--disable-cognition") {
            enableCognition = false;
        } else if (arg == "--enable-neuromodulation") {
            enableNeuromodulation = true;
        } else if (arg == "--disable-neuromodulation") {
            enableNeuromodulation = false;
        } else if (arg == "--enable-visualization") {
            enableVisualization = true;
        } else if (arg == "--disable-visualization") {
            enableVisualization = false;
        } else if (arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "--analyze-learning") {
            analyzeLearning = true;
        } else if (arg == "--export-data" && i + 1 < argc) {
            exportFormat = argv[++i];
        } else if (arg == "--export-path" && i + 1 < argc) {
            exportPath = argv[++i];
        } else if (arg == "--save-checkpoint" && i + 1 < argc) {
            saveCheckpoint = argv[++i];
        } else if (arg == "--load-checkpoint" && i + 1 < argc) {
            loadCheckpoint = argv[++i];
        } else if (arg == "--monte-carlo" && i + 1 < argc) {
            monteCarloRuns = std::atoi(argv[++i]);
        } else if (arg == "--diagnose-neural") {
            enableDiagnostics = true;
        } else if (arg == "--diagnose-plasticity") {
            enableDiagnostics = true;
        } else if (arg == "--diagnose-memory") {
            enableDiagnostics = true;
        } else if (arg == "--diagnose-cognition") {
            enableDiagnostics = true;
        } else if (arg == "--diagnose-neuromod") {
            enableDiagnostics = true;
        } else if (arg == "--no-color") {
            disableColor = true;
        } else if (arg == "--quiet") {
            quietMode = true;
        } else if (arg.find("--random_seed=") != std::string::npos) {
            overrideRandomSeed = std::atoi(arg.substr(arg.find("=") + 1).c_str());
        } else if (arg.find("--stdp_ltp_weight=") != std::string::npos) {
            config->set("stdp_ltp_weight", std::atof(arg.substr(arg.find("=") + 1).c_str()), ConfigSource::CommandLine);
        } else if (arg.find("--stdp_ltd_weight=") != std::string::npos) {
            config->set("stdp_ltd_weight", std::atof(arg.substr(arg.find("=") + 1).c_str()), ConfigSource::CommandLine);
        } else if (arg.find("--stdp_time_constant=") != std::string::npos) {
            config->set("stdp_time_constant", std::atof(arg.substr(arg.find("=") + 1).c_str()), ConfigSource::CommandLine);
        } else if (arg.find("--synaptogenesis_rate=") != std::string::npos) {
            config->set("synaptogenesis_rate", std::atof(arg.substr(arg.find("=") + 1).c_str()), ConfigSource::CommandLine);
        } else if (arg.find("--pruning_rate=") != std::string::npos) {
            config->set("pruning_rate", std::atof(arg.substr(arg.find("=") + 1).c_str()), ConfigSource::CommandLine);
        }
    }
    
    if (showHelp || showVersion || argc < 2) {
        if (showVersion) {
            printBanner();
            std::cout << "NLM - Neural Learning Machine" << std::endl;
            std::cout << "Version: 0.1.0 (Phase 2)" << std::endl;
        } else if (showAdvancedHelp) {
            printAdvancedHelp();
        } else {
            printHelp();
        }
        return 0;
    }
    
    if (showAdvancedHelp) {
        printAdvancedHelp();
    }
    
    printBanner();
    
    std::cout << "NLM Phase " << phase << " Advanced Command Line Interface" << std::endl;
    std::cout << "===========================================" << std::endl << std::endl;
    
    if (!quietMode) {
        std::cout << "Command line arguments parsed:" << std::endl;
        std::cout << "  Config file: " << configFile << std::endl;
        std::cout << "  Experiment: " << experimentName << std::endl;
        std::cout << "  Phase: " << phase << std::endl;
        std::cout << "  Verbosity: " << verbosity << std::endl;
        std::cout << "  Enable memory: " << (enableMemory ? "yes" : "no") << std::endl;
        std::cout << "  Enable prediction: " << (enablePrediction ? "yes" : "no") << std::endl;
        std::cout << "  Enable cognition: " << (enableCognition ? "yes" : "no") << std::endl;
        std::cout << "  Enable neuromodulation: " << (enableNeuromodulation ? "yes" : "no") << std::endl;
        std::cout << "  Enable visualization: " << (enableVisualization ? "yes" : "no") << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "Initializing NLM Phase " << phase << " Advanced Neural Computation..." << std::endl;
    std::cout << "" << std::endl;
    
    // TODO: Implement the rest of the advanced main function logic
    // For now, running the basic functionality
    std::cout << "Running basic functionality... (Advanced features to be implemented)" << std::endl;
    std::cout << "" << std::endl;
    
    return 0;
}
