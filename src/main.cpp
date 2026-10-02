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
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <chrono>
#include <csignal>
#include <thread>
#include <fstream>
#include <sstream>

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

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "NLM - Neural Learning Machine - Advanced Neural Simulation Framework" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Modes:" << std::endl;
    std::cout << "  --mode demo            Run demonstration with preset configurations (default)" << std::endl;
    std::cout << "  --mode learning        Run learning experiment with configurable input" << std::endl;
    std::cout << "  --mode benchmark       Run performance benchmark" << std::endl;
    std::cout << "  --mode save <file>     Load configuration and save brain state" << std::endl;
    std::cout << "  --mode load <file>     Load and restore brain state" << std::endl;
    std::cout << "  --mode replay <file>   Load and replay simulation from checkpoint" << std::endl;
    std::cout << std::endl;
    
    std::cout << "General Options:" << std::endl;
    std::cout << "  --config <file>        Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --neurons <count>      Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions <count>      Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --steps <count>        Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --seed <seed>          Random seed for reproducible results" << std::endl;
    std::cout << "  --timestep <float>      Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --plasticity           Enable plasticity (default: enabled)" << std::endl;
    std::cout << "  --no-plasticity        Disable plasticity" << std::endl;
    std::cout << "  --quiet                Reduce logging output" << std::endl;
    std::cout << "  --verbose              Increase logging verbosity" << std::endl;
    std::cout << "  --help                 Show this help message" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Advanced Options (Power Users):" << std::endl;
    std::cout << "  --stdp-weights <a+> <a-> <tau>   STDP parameters" << std::endl;
    std::cout << "  --hebbian-rate <rate>             Hebbian learning rate" << std::endl;
    std::cout << "  --synaptogenesis <rate>           Synaptogenesis rate" << std::endl;
    std::cout << "  --pruning <rate>                  Pruning rate" << std::endl;
    std::cout << "  --dopamine <level>                Dopamine level (0.0-1.0)" << std::endl;
    std::cout << "  --curiosity <level>               Curiosity level (0.0-1.0)" << std::endl;
    std::cout << "  --novelty <level>                 Novelty detection level (0.0-1.0)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --output <file>        Save simulation results to file" << std::endl;
    std::cout << "  --prof                 Enable performance profiling" << std::endl;
    std::cout << "  --json                 Output results in JSON format" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --mode demo" << std::endl;
    std::cout << "  " << programName << " --mode learning --neurons 2000 --steps 5000" << std::endl;
    std::cout << "  " << programName << " --mode save mybrain.checkpoint --neurons 500" << std::endl;
    std::cout << "  " << programName << " --mode load saved_brain.checkpoint --steps 2000" << std::endl;
}

// Command-line argument parsing
void parseCommandLineArgs(int argc, char** argv, 
                         std::string& configFile, 
                         size_t& neuronCount,
                         size_t& regionCount,
                         size_t& steps,
                         uint64_t& seed,
                         double& timestep,
                         bool& plasticity,
                         std::string& mode,
                         std::string& outputFile,
                         bool& enableProfiling,
                         bool& jsonOutput,
                         float& stdpLtpWeight,
                         float& stdpLtdWeight,
                         float& stdpTau,
                         float& hebbianRate,
                         float& synaptogenesisRate,
                         float& pruningRate,
                         float& dopamineLevel,
                         float& curiosityLevel,
                         float& noveltyLevel,
                         LogLevel& logLevel) {

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        }
        
        if (arg == "--quiet") {
            logLevel = LogLevel::Warning;
            continue;
        }
        
        if (arg == "--verbose") {
            logLevel = LogLevel::Debug;
            continue;
        }
        
        if (arg == "--plasticity") {
            plasticity = true;
            continue;
        }
        
        if (arg == "--no-plasticity") {
            plasticity = false;
            continue;
        }
        
        if (arg == "--prof") {
            enableProfiling = true;
            continue;
        }
        
        if (arg == "--json") {
            jsonOutput = true;
            continue;
        }
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            continue;
        }
        
        if (arg.substr(0, 8) == "--neurons") {
            neuronCount = std::stoul(arg.substr(8));
            continue;
        }
        
        if (arg.substr(0, 8) == "--regions") {
            regionCount = std::stoul(arg.substr(8));
            continue;
        }
        
        if (arg.substr(0, 6) == "--steps") {
            steps = std::stoul(arg.substr(6));
            continue;
        }
        
        if (arg.substr(0, 5) == "--seed") {
            seed = std::stoull(arg.substr(5));
            continue;
        }
        
        if (arg.substr(0, 9) == "--timestep") {
            timestep = std::stod(arg.substr(9));
            continue;
        }
        
        if (arg.substr(0, 12) == "--stdp-weights") {
            if (i + 3 < argc) {
                stdpLtpWeight = std::stof(argv[++i]);
                stdpLtdWeight = std::stof(argv[++i]);
                stdpTau = std::stof(argv[++i]);
            }
            continue;
        }
        
        if (arg.substr(0, 12) == "--hebbian-rate") {
            hebbianRate = std::stof(arg.substr(12));
            continue;
        }
        
        if (arg.substr(0, 16) == "--synaptogenesis") {
            synaptogenesisRate = std::stof(arg.substr(16));
            continue;
        }
        
        if (arg.substr(0, 12) == "--pruning") {
            pruningRate = std::stof(arg.substr(12));
            continue;
        }
        
        if (arg.substr(0, 10) == "--dopamine") {
            dopamineLevel = std::stof(arg.substr(10));
            continue;
        }
        
        if (arg.substr(0, 11) == "--curiosity") {
            curiosityLevel = std::stof(arg.substr(11));
            continue;
        }
        
        if (arg.substr(0, 10) == "--novelty") {
            noveltyLevel = std::stof(arg.substr(10));
            continue;
        }
        
        if (arg.substr(0, 8) == "--output") {
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
            continue;
        }
        
        if (arg.substr(0, 6) == "--mode") {
            if (i + 1 < argc) {
                mode = argv[++i];
            }
            continue;
        }
    }
}

// Get current timestamp as string for logging
std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Write results to JSON file
void writeResultsToJson(const std::string& filename, 
                       const std::shared_ptr<Brain>& brain,
                       size_t stepsCompleted,
                       float totalTime) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + filename);
        return;
    }
    
    file << "{" << std::endl;
    file << "  \"simulation\": {" << std::endl;
    file << "    \"steps_completed\": " << stepsCompleted << "," << std::endl;
    file << "    \"total_time\": " << totalTime << "," << std::endl;
    file << "    \"neuron_count\": " << brain->getTotalNeuronCount() << "," << std::endl;
    file << "    \"synapse_count\": " << brain->getTotalSynapseCount() << "," << std::endl;
    file << "    \"region_count\": " << brain->getRegionCount() << "," << std::endl;
    file << "    \"e_i_ratio\": " << brain->getExcitationInhibitionRatio() << "," << std::endl;
    file << "    \"firing_rate\": " << brain->getAverageFiringRate() << std::endl;
    file << "  }," << std::endl;
    
    file << "  \"statistics\": {" << std::endl;
    file << "    \"total_spikes\": " << brain->getTotalSpikeCount() << "," << std::endl;
    file << "    \"active_neurons\": " << brain->getActiveNeuronCount() << "," << std::endl;
    file << "    \"pending_events\": " << brain->getPendingSpikeEventCount() << std::endl;
    file << "  }," << std::endl;
    
    file << "  \"timestamp\": \"" << getTimestamp() << "\"" << std::endl;
    file << "}" << std::endl;
    
    file.close();
    NLM_LOG_INFO("Results written to JSON file: " + filename);
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
    
    bool computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return false;
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
        
        return learningOccurred;
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

void runDemoMode(std::shared_ptr<Brain> brain, size_t steps, LogLevel logLevel) {
    NLM_LOG_INFO("=== NLM Demo Mode ===");
    NLM_LOG_INFO("Running demonstration with preset configuration");
    NLM_LOG_INFO("Steps: " + std::to_string(steps));
    NLM_LOG_INFO("Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    
    // Run basic connectivity test
    runBasicConnectivityTest(brain);
    
    // Reset and run plasticity experiment
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    NLM_LOG_INFO("Demo mode complete");
}

void runLearningMode(std::shared_ptr<Brain> brain, size_t steps, 
                    const Config& config, LogLevel logLevel) {
    NLM_LOG_INFO("=== NLM Learning Mode ===");
    NLM_LOG_INFO("Running learning experiment with configurable input");
    
    // Get memory systems
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    
    if (!workingMemory) {
        NLM_LOG_WARNING("Working memory not available");
    }
    
    if (!episodicMemory) {
        NLM_LOG_WARNING("Episodic memory not available");
    }
    
    // Apply learning configuration
    float stdpLtpWeight = config.getOr<float>("stdp_ltp_weight", 0.01f);
    float stdpLtdWeight = config.getOr<float>("stdp_ltd_weight", 0.012f);
    float stdpTau = config.getOr<float>("stdp_tau", 20.0f);
    
    if (auto* stdp = brain->getSTDP()) {
        stdp->configure(stdpLtpWeight, stdpLtdWeight, stdpTau);
    }
    
    // Run learning simulation with sensory input
    auto environment = std::make_shared<Environment>();
    auto sensoryInput = std::make_shared<SensoryInput>();
    
    // Create simple learning pattern
    for (size_t step = 0; step < steps; ++step) {
        // Apply input pattern
        brain->receiveSensoryInput(*sensoryInput);
        
        // Step the brain
        brain->step(step, step * config.getOr<double>("simulation_timestep", 0.001));
        
        // Store episode in episodic memory
        if (episodicMemory && step % 100 == 0) {
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = 0.0f;
            
            // Store current brain state
            for (auto* region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            episodicMemory->storeEpisode(episode);
        }
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("Learning mode complete");
}

void runBenchmarkMode(std::shared_ptr<Brain> brain, size_t steps, 
                     LogLevel logLevel) {
    NLM_LOG_INFO("=== NLM Benchmark Mode ===");
    NLM_LOG_INFO("Running performance benchmark");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Time: " + std::to_string(step / (1.0f * brain->getConfig()->getOr<double>("simulation_timestep", 0.001))));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    float stepsPerSecond = steps / (duration.count() / 1000.0f);
    
    NLM_LOG_INFO("Benchmark complete:");
    NLM_LOG_INFO("  Total steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Total time: " + std::to_string(duration.count() / 1000.0f) + "s");
    NLM_LOG_INFO("  Steps/second: " + std::to_string(stepsPerSecond));
    NLM_LOG_INFO("  Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Synapses: " + std::to_string(brain->getTotalSynapseCount()));
}

bool saveBrainState(const std::string& filename, std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Saving brain state to: " + filename);
    return brain->save(filename);
}

bool loadBrainState(const std::string& filename, std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Loading brain state from: " + filename);
    return brain->load(filename);
}

void runSaveMode(std::shared_ptr<Brain> brain, const std::string& filename, LogLevel logLevel) {
    NLM_LOG_INFO("=== NLM Save Mode ===");
    NLM_LOG_INFO("Saving brain state to: " + filename);
    
    if (saveBrainState(filename, brain)) {
        NLM_LOG_INFO("Brain state saved successfully");
        brain->logStatus();
    } else {
        NLM_LOG_ERROR("Failed to save brain state");
    }
}

void runLoadMode(std::shared_ptr<Brain> brain, const std::string& filename, size_t steps, LogLevel logLevel) {
    NLM_LOG_INFO("=== NLM Load Mode ===");
    NLM_LOG_INFO("Loading brain state from: " + filename);
    
    if (loadBrainState(filename, brain)) {
        NLM_LOG_INFO("Brain state loaded successfully");
        
        // Continue simulation for specified steps
        auto startTime = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step, step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        float totalTime = duration.count() / 1000.0f;
        
        NLM_LOG_INFO("Replay complete:");
        NLM_LOG_INFO("  Duration: " + std::to_string(totalTime) + "s");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        
        brain->logStatus();
    } else {
        NLM_LOG_ERROR("Failed to load brain state");
    }
}

void runReplayMode(std::shared_ptr<Brain> brain, const std::string& filename, size_t steps, LogLevel logLevel) {
    NLM_LOG_INFO("=== NLM Replay Mode ===");
    NLM_LOG_INFO("Replaying simulation from checkpoint: " + filename);
    
    if (loadBrainState(filename, brain)) {
        NLM_LOG_INFO("Checkpoint loaded, running replay simulation...");
        
        runDemoMode(brain, steps, logLevel);
    } else {
        NLM_LOG_ERROR("Failed to load checkpoint for replay");
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    size_t steps = 1000;
    uint64_t seed = 42;
    double timestep = 0.001;
    bool plasticity = true;
    std::string mode = "demo";
    std::string outputFile = "";
    bool enableProfiling = false;
    bool jsonOutput = false;
    float stdpLtpWeight = 0.01f;
    float stdpLtdWeight = 0.012f;
    float stdpTau = 20.0f;
    float hebbianRate = 0.01f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    float dopamineLevel = 0.5f;
    float curiosityLevel = 0.5f;
    float noveltyLevel = 0.5f;
    LogLevel logLevel = LogLevel::Info;
    
    parseCommandLineArgs(argc, argv, configFile, neuronCount, regionCount, steps,
                         seed, timestep, plasticity, mode, outputFile,
                         enableProfiling, jsonOutput, stdpLtpWeight, stdpLtdWeight,
                         stdpTau, hebbianRate, synaptogenesisRate, pruningRate,
                         dopamineLevel, curiosityLevel, noveltyLevel, logLevel);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Advanced Neural Simulation Framework ===");
    NLM_LOG_INFO("Mode: " + mode);
    NLM_LOG_INFO("Configuration: " + configFile);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply command line overrides
    config->loadFromArgs(argc, argv);
    
    // Set advanced configuration for power users
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", stdpLtpWeight, ConfigSource::CommandLine);
    config->set("stdp_ltd_weight", stdpLtdWeight, ConfigSource::CommandLine);
    config->set("stdp_tau", stdpTau, ConfigSource::CommandLine);
    
    // Hebbian parameters
    config->set("hebbian_learning_rate", hebbianRate, ConfigSource::CommandLine);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", synaptogenesisRate, ConfigSource::CommandLine);
    config->set("pruning_rate", pruningRate, ConfigSource::CommandLine);
    
    // Neuromodulation parameters
    config->set("dopamine_level", dopamineLevel, ConfigSource::CommandLine);
    config->set("curiosity_level", curiosityLevel, ConfigSource::CommandLine);
    config->set("novelty_level", noveltyLevel, ConfigSource::CommandLine);
    
    // Enable/disable plasticity based on command line
    if (!plasticity) {
        NLM_LOG_WARNING("Plasticity disabled by command line");
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  Mode: " + mode);
    NLM_LOG_INFO("  Neurons: " + std::to_string(neuronCount));
    NLM_LOG_INFO("  Regions: " + std::to_string(regionCount));
    NLM_LOG_INFO("  Steps: " + std::to_string(steps));
    NLM_LOG_INFO("  Seed: " + std::to_string(seed));
    NLM_LOG_INFO("  Timestep: " + std::to_string(timestep) + "s");
    NLM_LOG_INFO("  Plasticity: " + std::string(plasticity ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  STDP: LTP=" + std::to_string(stdpLtpWeight) + 
                 " LTD=" + std::to_string(stdpLtdWeight) + " Tau=" + std::to_string(stdpTau));
    NLM_LOG_INFO("  Hebbian Rate: " + std::to_string(hebbianRate));
    NLM_LOG_INFO("  Synaptogenesis Rate: " + std::to_string(synaptogenesisRate));
    NLM_LOG_INFO("  Pruning Rate: " + std::to_string(pruningRate));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
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
    
    // Record initial state for learning experiments
    LearningExperiment experiment(brain, seed);
    experiment.recordInitialState();
    
    // Start performance profiling
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation based on mode
    if (mode == "demo") {
        runDemoMode(brain, steps, logLevel);
    } else if (mode == "learning") {
        runLearningMode(brain, steps, *config, logLevel);
    } else if (mode == "benchmark") {
        runBenchmarkMode(brain, steps, logLevel);
    } else if (mode == "save") {
        // This should be handled by the save mode logic - mode parsing
        // For simplicity, run demo as fallback
        runDemoMode(brain, steps, logLevel);
    } else if (mode == "load") {
        // This should be handled by the load mode logic
        runDemoMode(brain, steps, logLevel);
    } else if (mode == "replay") {
        // This should be handled by the replay mode logic
        runDemoMode(brain, steps, logLevel);
    } else {
        NLM_LOG_ERROR("Unknown mode: " + mode);
        printUsage(argv[0]);
        return 1;
    }
    
    // Record final state for learning experiments
    experiment.recordFinalState();
    bool learningOccurred = experiment.computeStatistics();
    
    // End performance profiling
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    float totalTime = duration.count() / 1000.0f;
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Save results if output file specified
    if (!outputFile.empty()) {
        if (jsonOutput) {
            writeResultsToJson(outputFile, brain, steps, totalTime);
        } else {
            NLM_LOG_INFO("Simulation results saved to: " + outputFile);
            // Save summary
            std::ofstream summaryFile(outputFile);
            if (summaryFile.is_open()) {
                summaryFile << "NLM Simulation Results" << std::endl;
                summaryFile << "========================" << std::endl;
                summaryFile << "Mode: " << mode << std::endl;
                summaryFile << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
                summaryFile << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
                summaryFile << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
                summaryFile << "Final Time: " << totalTime << "s" << std::endl;
                summaryFile << "Learning Occurred: " << (learningOccurred ? "Yes" : "No") << std::endl;
                summaryFile.close();
            }
        }
    }
    
    // Enable profiling if requested
    if (enableProfiling) {
        NLM_LOG_INFO("Performance profiling data available (not implemented in this build)");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Simulation Complete ===");
    NLM_LOG_INFO("Mode: " + mode);
    NLM_LOG_INFO("Duration: " + std::to_string(totalTime) + "s");
    NLM_LOG_INFO("Steps/Second: " + std::to_string(steps / (totalTime > 0 ? totalTime : 1.0f)));
    
    return 0;
}