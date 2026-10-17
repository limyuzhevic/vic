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

// Command-line argument parser and utility functions
namespace cl {
    
    struct CommandLineArgs {
        std::string configFile = "configs/default.cfg";
        bool help = false;
        bool version = false;
        bool verbose = false;
        std::string outputFile = "";
        std::string mode = "phase2";  // "phase2", "phase6", "custom"
        bool validateOnly = false;
        size_t customSteps = 1000;
        size_t customNeurons = 500;
        float customConnectionProb = 0.1f;
        bool enableAllTests = false;
        bool runIndividualTests = false;
        std::vector<std::string> individualTests;
        std::string debugOutput = "";
        bool enableMemoryDump = false;
        std::string memoryDumpFile = "";
        bool enableStepByStep = false;
        bool enableVisualization = false;
        bool enableMetricsExport = false;
        std::string metricsFile = "";
        bool enableNeuroModulationDebug = false;
        bool enablePlasticityDebug = false;
        bool enableDevelopmentDebug = false;
    };
    
    CommandLineArgs parseArgs(int argc, char** argv) {
        CommandLineArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version") {
                args.version = true;
            } else if (arg == "--verbose" || arg == "-v") {
                args.verbose = true;
            } else if (arg == "--validate-only") {
                args.validateOnly = true;
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) args.outputFile = argv[++i];
            } else if (arg == "--mode") {
                if (i + 1 < argc) args.mode = argv[++i];
            } else if (arg == "--mode-phase2") {
                args.mode = "phase2";
            } else if (arg == "--mode-phase6") {
                args.mode = "phase6";
            } else if (arg == "--mode-custom") {
                args.mode = "custom";
            } else if (arg == "--custom-steps") {
                if (i + 1 < argc) args.customSteps = std::stoull(argv[++i]);
            } else if (arg == "--custom-neurons") {
                if (i + 1 < argc) args.customNeurons = std::stoull(argv[++i]);
            } else if (arg == "--custom-conn-prob") {
                if (i + 1 < argc) args.customConnectionProb = std::stof(argv[++i]);
            } else if (arg == "--all-tests") {
                args.enableAllTests = true;
            } else if (arg == "--test") {
                args.runIndividualTests = true;
                if (i + 1 < argc) args.individualTests.push_back(argv[++i]);
            } else if (arg == "--debug-output") {
                if (i + 1 < argc) args.debugOutput = argv[++i];
            } else if (arg == "--memory-dump") {
                args.enableMemoryDump = true;
                if (i + 1 < argc) args.memoryDumpFile = argv[++i];
            } else if (arg == "--step-by-step") {
                args.enableStepByStep = true;
            } else if (arg == "--visualization") {
                args.enableVisualization = true;
            } else if (arg == "--metrics-export") {
                args.enableMetricsExport = true;
                if (i + 1 < argc) args.metricsFile = argv[++i];
            } else if (arg == "--neuromod-debug") {
                args.enableNeuroModulationDebug = true;
            } else if (arg == "--plasticity-debug") {
                args.enablePlasticityDebug = true;
            } else if (arg == "--development-debug") {
                args.enableDevelopmentDebug = true;
            } else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    args.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.configFile = argv[++i];
                }
            } else {
                // Unknown argument, but don't crash - just ignore
                NLM_LOG_WARNING("Unknown argument: " + arg);
            }
        }
        
        return args;
    }
    
    void printHelp() {
        std::cout << R"(
NLM — 熙然 (Neural Learning Machine)

USAGE:
  ./nlm [OPTIONS]

MODES:
  --mode-phase2         Run Phase 2: Real Neural Computation (default)
  --mode-phase6         Run Phase 6: Integration Demo
  --mode-custom         Run custom test with specified parameters

OPTIONS:
  -h, --help            Show this help message
  -v, --verbose         Enable verbose logging
  --version             Show version information
  -o, --output FILE     Save output to file instead of stdout
  --validate-only       Only validate configuration files
  --all-tests           Run all available tests
  --test NAME          Run specific test (connect, plasticity, stdp, all)
  --debug-output FILE   Save detailed debug output to file
  --memory-dump [FILE]  Save brain state to file (default: brain_state.bin)
  --step-by-step        Run step-by-step for manual control
  --visualization       Enable visualization mode
  --metrics-export [FILE] Export metrics to file
  --neuromod-debug      Enable neuromodulation debugging
  --plasticity-debug    Enable plasticity debugging
  --development-debug   Enable development debugging
  --config FILE         Load configuration from file

CUSTOM TEST OPTIONS (when using --mode-custom):
  --custom-steps N     Number of simulation steps (default: 1000)
  --custom-neurons N   Number of neurons (default: 500)
  --custom-conn-prob P Connection probability (default: 0.1)

ADVANCED DEBUGGING OPTIONS:
  --enable-memory-dump [FILE]  Save memory state during execution
  --enable-step-by-step       Interactive step execution
  --enable-neuromod-debug      Debug neuromodulation signals
  --enable-plasticity-debug   Debug synaptic changes
  --enable-development-debug  Debug developmental processes

EXAMPLES:
  ./nlm --mode-phase2                    # Run Phase 2 demo
  ./nlm --mode-phase6                    # Run Phase 6 integration demo
  ./nlm --mode-custom --custom-steps 5000 --verbose  # Custom test with logging
  ./nlm --validate-only --config myconfig.cfg  # Validate config only
  ./nlm --test plasticity                 # Run plasticity test only
  ./nlm --memory-dump brain_state.bin --step-by-step  # Save brain state and run step-by-step
  ./nlm --neuromod-debug --plasticity-debug        # Enable detailed debugging

CONFIGURATION:
  Configuration files can be loaded with --config option.
  Default: configs/default.cfg
  If file doesn't exist, default values are used.
)" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM v1.0.0 (Phase 6)" << std::endl;
        std::cout << "Experimental Artificial Developmental Brain" << std::endl;
        std::cout << "Phase 2: Real Neural Computation (main.cpp)" << std::endl;
        std::cout << "Phase 6: Final Integration Demo" << std::endl;
    }
} // namespace cl

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
    
    // Helper method to record weights from brain state
    void recordWeights(std::vector<float>& weights, const std::string& stateName) {
        weights.clear();
        
        // Record weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                weights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO(stateName + " state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        if (!weights.empty()) {
            float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
            float mean = sum / weights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        recordWeights(initialWeights, "Initial");
    }
    
    void recordFinalState() {
        finalWeights.clear();
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        
        // Reuse the weight recording logic
        recordWeights(finalWeights, "Final");
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

};

// Advanced debugging and inspection functions
namespace advanced {

void dumpBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    NLM_LOG_INFO("Dumping brain state to: " + filepath);
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for brain state dump: " + filepath);
        return;
    }
    
    try {
        // Write brain metadata
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        size_t activeNeurons = brain->getActiveNeuronCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        float avgFiringRate = brain->getAverageFiringRate();
        
        file.write(reinterpret_cast<const char*>(&totalNeurons), sizeof(totalNeurons));
        file.write(reinterpret_cast<const char*>(&totalSynapses), sizeof(totalSynapses));
        file.write(reinterpret_cast<const char*>(&activeNeurons), sizeof(activeNeurons));
        file.write(reinterpret_cast<const char*>(&firingNeurons), sizeof(firingNeurons));
        file.write(reinterpret_cast<const char*>(&avgFiringRate), sizeof(avgFiringRate));
        
        // Dump working memory state
        if (auto* wm = brain->getWorkingMemory()) {
            size_t wmTraces = wm->getActiveTraces();
            file.write(reinterpret_cast<const char*>(&wmTraces), sizeof(wmTraces));
        } else {
            size_t wmTraces = 0;
            file.write(reinterpret_cast<const char*>(&wmTraces), sizeof(wmTraces));
        }
        
        // Dump episodic memory state
        if (auto* em = brain->getEpisodicMemory()) {
            size_t emEpisodes = em->getEpisodeCount();
            file.write(reinterpret_cast<const char*>(&emEpisodes), sizeof(emEpisodes));
        } else {
            size_t emEpisodes = 0;
            file.write(reinterpret_cast<const char*>(&emEpisodes), sizeof(emEpisodes));
        }
        
        // Dump neuromodulation state
        if (auto* dopamine = brain->getDopamine()) {
            float dopamineLevel = dopamine->getLevel();
            file.write(reinterpret_cast<const char*>(&dopamineLevel), sizeof(dopamineLevel));
        } else {
            float dopamineLevel = 0.0f;
            file.write(reinterpret_cast<const char*>(&dopamineLevel), sizeof(dopamineLevel));
        }
        
        NLM_LOG_INFO("Brain state dump completed successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain state dump: " + std::string(e.what()));
    }
    
    file.close();
}

void exportMetrics(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    NLM_LOG_INFO("Exporting metrics to: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for metrics export: " + filepath);
        return;
    }
    
    file << "NLM Brain Metrics Export" << std::endl;
    file << "=======================" << std::endl << std::endl;
    
    file << "Brain Statistics:" << std::endl;
    file << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    file << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    file << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    file << "  Firing Neurons (this step): " << brain->getFiringNeuronCount() << std::endl;
    file << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    file << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    file << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl << std::endl;
    
    if (auto* wm = brain->getWorkingMemory()) {
        file << "Working Memory:" << std::endl;
        file << "  Active Traces: " << wm->getActiveTraces() << std::endl;
        file << "  Capacity: " << wm->getCapacity() << std::endl << std::endl;
    }
    
    if (auto* em = brain->getEpisodicMemory()) {
        file << "Episodic Memory:" << std::endl;
        file << "  Episodes Stored: " << em->getEpisodeCount() << std::endl;
        file << "  Max Episodes: " << em->getMaxEpisodes() << std::endl << std::endl;
    }
    
    if (auto* dopamine = brain->getDopamine()) {
        file << "Neuromodulation (Dopamine):" << std::endl;
        file << "  Current Level: " << dopamine->getLevel() << std::endl;
        file << "  Plasticity Factor: " << dopamine->getPlasticityFactor() << std::endl << std::endl;
    }
    
    file << "Configuration:" << std::endl;
    if (auto config = brain->getConfig()) {
        file << "  Simulation timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        file << "  Neuron count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
        file << "  Region count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Metrics export completed successfully");
}

void stepByStepExecution(std::shared_ptr<Brain>& brain, size_t maxSteps) {
    NLM_LOG_INFO("=== Step-by-Step Execution ===");
    NLM_LOG_INFO("Press ENTER to advance to next step, or 'q' to quit");
    
    for (SimulationStep step = 0; step < maxSteps; ++step) {
        std::cout << "Step " << step << ": ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input == "q" || input == "quit") {
            NLM_LOG_INFO("Quitting step-by-step execution");
            break;
        }
        
        brain->step(step, static_cast<Timestamp>(step) * 0.001);
        
        NLM_LOG_INFO("  Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()));
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Working Memory Traces: " + std::to_string(
                brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0));
        }
        
        std::cout << " [Ready] " << std::flush;
    }
}

void debugNeuromodulation(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("=== Neuromodulation Debug ===");
    
    if (auto* dopamine = brain->getDopamine()) {
        NLM_LOG_INFO("Dopamine System:");
        NLM_LOG_INFO("  Current Level: " + std::to_string(dopamine->getLevel()));
        NLM_LOG_INFO("  Plasticity Factor: " + std::to_string(dopamine->getPlasticityFactor()));
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        NLM_LOG_INFO("Curiosity System:");
        NLM_LOG_INFO("  Level: " + std::to_string(curiosity->getLevel()));
        NLM_LOG_INFO("  Threshold: " + std::to_string(curiosity->getThreshold()));
    }
    
    if (auto* novelty = brain->getNovelty()) {
        NLM_LOG_INFO("Novelty Detection:");
        NLM_LOG_INFO("  Level: " + std::to_string(novelty->getLevel()));
        NLM_LOG_INFO("  Sensitivity: " + std::to_string(novelty->getSensitivity()));
    }
}

void debugPlasticity(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("=== Plasticity Debug ===");
    
    if (auto* stdp = brain->getSTDP()) {
        NLM_LOG_INFO("STDP System:");
        NLM_LOG_INFO("  LTP Weight: " + std::to_string(stdp->getLTPWeight()));
        NLM_LOG_INFO("  LTD Weight: " + std::to_string(stdp->getLTDWeight()));
        NLM_LOG_INFO("  Time Constant: " + std::to_string(stdp->getTau()));
    }
    
    if (auto* hebbian = brain->getHebbian()) {
        NLM_LOG_INFO("Hebbian System:");
        NLM_LOG_INFO("  Learning Rate: " + std::to_string(hebbian->getLearningRate()));
    }
    
    if (auto* structural = brain->getStructuralPlasticity()) {
        NLM_LOG_INFO("Structural Plasticity:");
        NLM_LOG_INFO("  Synaptogenesis Rate: " + std::to_string(structural->getSynaptogenesisRate()));
        NLM_LOG_INFO("  Pruning Rate: " + std::to_string(structural->getPruningRate()));
    }
}

void debugDevelopment(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("=== Development Debug ===");
    
    auto stage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("Developmental Stage: " + std::to_string(static_cast<int>(stage)));
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            NLM_LOG_INFO("  Characteristics: High plasticity, rapid learning");
            break;
        case DevelopmentalStage::CriticalPeriod:
            NLM_LOG_INFO("  Characteristics: Sensitive learning window");
            break;
        case DevelopmentalStage::Maturation:
            NLM_LOG_INFO("  Characteristics: Slower learning, refinement");
            break;
        case DevelopmentalStage::Adult:
            NLM_LOG_INFO("  Characteristics: Stable learning rate");
            break;
    }
}

} // namespace advanced

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
    // Parse command-line arguments
    cl::CommandLineArgs args = cl::parseArgs(argc, argv);
    
    // Handle help, version, etc.
    if (args.help) {
        cl::printHelp();
        return 0;
    }
    
    if (args.version) {
        cl::printVersion();
        return 0;
    }
    
    // Set up output stream (file or stdout)
    std::ostream* outputStream = &std::cout;
    std::ofstream outputFile;
    if (!args.outputFile.empty()) {
        outputFile.open(args.outputFile);
        if (outputFile.is_open()) {
            outputStream = &outputFile;
            std::cout.rdbuf(outputFile.rdbuf());
        } else {
            NLM_LOG_ERROR("Failed to open output file: " + args.outputFile);
        }
    }
    
    // Set verbose logging if requested
    if (args.verbose) {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    } else {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
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
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(args.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2 (only if not overridden)
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
    
// Run tests based on command-line arguments
    if (args.enableAllTests) {
        NLM_LOG_INFO("All tests enabled");
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else if (args.runIndividualTests) {
        NLM_LOG_INFO("Running individual tests: ");
        for (const auto& test : args.individualTests) {
            NLM_LOG_INFO("  - " + test);
            if (test == "connect") {
                runBasicConnectivityTest(brain);
            } else if (test == "plasticity" || test == "learning") {
                brain->reset();
                brain->initialize();
                runPlasticityExperiment(brain);
            } else if (test == "stdp" || test == "synapse") {
                brain->reset();
                brain->initialize();
                runStdpVerification(brain);
            } else if (test == "all") {
                runBasicConnectivityTest(brain);
                brain->reset();
                brain->initialize();
                runPlasticityExperiment(brain);
                brain->reset();
                brain->initialize();
                runStdpVerification(brain);
            } else {
                NLM_LOG_WARNING("Unknown test: " + test + ". Skipping.");
            }
        }
    } else {
        // Default behavior - run all standard tests
        NLM_LOG_INFO("Running standard test suite...");
        runBasicConnectivityTest(brain);
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    }
    
    // Handle advanced debugging and inspection features
    if (args.enableMemoryDump) {
        advanced::dumpBrainState(brain, args.memoryDumpFile.empty() ? "brain_state.bin" : args.memoryDumpFile);
    }
    
    if (args.enableStepByStep) {
        NLM_LOG_INFO("Starting step-by-step execution. Press ENTER to advance to next step, 'q' to quit.");
        advanced::stepByStepExecution(brain, 1000);
    }
    
    if (args.enableMetricsExport) {
        advanced::exportMetrics(brain, args.metricsFile.empty() ? "brain_metrics.json" : args.metricsFile);
    }
    
    if (args.enableNeuroModulationDebug) {
        advanced::debugNeuromodulation(brain);
    }
    
    if (args.enablePlasticityDebug) {
        advanced::debugPlasticity(brain);
    }
    
    if (args.enableDevelopmentDebug) {
        advanced::debugDevelopment(brain);
    }
    
    if (args.debugOutput.empty()) {
        std::cout << std::endl;
        std::cout << "=== NLM Execution Summary ===" << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << std::endl;
        std::cout << "Execution completed successfully!" << std::endl;
    }
    
    return 0;
}
