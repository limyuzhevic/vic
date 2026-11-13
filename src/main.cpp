// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace nlm;

using namespace nlm;

// Print help information
    void printHelp() {
        std::cout << "NLM - Neural Learning Machine" << std::endl;
        std::cout << "Phase 6: Integrated Artificial Brain" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "Usage: nlm [OPTIONS] [CONFIG_FILE]" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration Options:" << std::endl;
        std::cout << "  --config file      Load configuration from file (default: configs/default.cfg)" << std::endl;
        std::cout << "  -c file            Alias for --config" << std::endl;
        std::cout << std::endl;
        std::cout << "Runtime Options:" << std::endl;
        std::cout << "  --help             Print this help message" << std::endl;
        std::cout << "  --version          Print version information" << std::endl;
        std::cout << "  --verbose          Enable verbose logging (default)" << std::endl;
        std::cout << "  --quiet            Reduce output to essential information" << std::endl;
        std::cout << "  --quiet-quiet      Minimal output, only errors" << std::endl;
        std::cout << "  --benchmark        Run performance benchmarks" << std::endl;
        std::cout << "  --benchmark-only   Run benchmarks and exit" << std::endl;
        std::cout << "  --debug            Enable debug mode with additional output" << std::endl;
        std::cout << std::endl;
        std::cout << "Simulation Control:" << std::endl;
        std::cout << "  --steps N          Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --timestep T       Simulation timestep in seconds (default: 0.001)" << std::endl;
        std::cout << "  --neurons N        Number of neurons (default: 500)" << std::endl;
        std::cout << "  --regions R        Number of brain regions (default: 1)" << std::endl;
        std::cout << "  --connectivity P   Connection probability (default: 0.15)" << std::endl;
        std::cout << std::endl;
        std::cout << "Plasticity Options:" << std::endl;
        std::cout << "  --hebbian          Enable Hebbian plasticity" << std::endl;
        std::cout << "  --stdp             Enable STDP plasticity" << std::endl;
        std::cout << "  --structural       Enable structural plasticity (synaptogenesis/pruning)" << std::endl;
        std::cout << "  --stdp-ltp-weight W STDP LTP weight (default: 0.02)" << std::endl;
        std::cout << "  --stdp-ltd-weight W STDP LTD weight (default: 0.015)" << std::endl;
        std::cout << "  --stdp-tau T       STDP time constant (default: 20.0)" << std::endl;
        std::cout << "  --synaptogenesis-rate R Synaptogenesis rate (default: 0.0001)" << std::endl;
        std::cout << "  --pruning-rate R   Pruning rate (default: 0.00001)" << std::endl;
        std::cout << std::endl;
        std::cout << "Output Options:" << std::endl;
        std::cout << "  --output file      Save brain state to file at end" << std::endl;
        std::cout << "  --save-interval N  Save checkpoint every N steps (default: 100)" << std::endl;
        std::cout << "  --export-graphviz  Export brain as GraphViz DOT file" << std::endl;
        std::cout << "  --export-neurons file Export neuron state to file" << std::endl;
        std::cout << "  --export-synapses file Export synapse state to file" << std::endl;
        std::cout << std::endl;
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --seed N           Random seed (default: 42)" << std::endl;
        std::cout << "  --max-memory N     Maximum memory usage in MB" << std::endl;
        std::cout << "  --preserve-state   Keep state across resets for longer experiments" << std::endl;
        std::cout << "  --development-mode  Run in developmental stage mode" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm                              Run default experiment" << std::endl;
        std::cout << "  nlm --steps 5000 --neurons 1000     Larger, longer experiment" << std::endl;
        std::cout << "  nlm --benchmark                    Run performance tests" << std::endl;
        std::cout << "  nlm --config my_config.cfg          Use custom configuration" << std::endl;
    }
    
    // Print version information
// Validate configuration parameters
    bool validateConfiguration(const std::shared_ptr<Config>& config) {
        std::stringstream errorMsg;
        
        // Check essential parameters
        bool hasNeuronCount = config->has("neuron_count");
        bool hasRegionCount = config->has("region_count");
        bool hasTimestep = config->has("simulation_timestep");
        bool hasSeed = config->has("random_seed");
        
        if (!hasNeuronCount) {
            errorMsg << "Missing required parameter: neuron_count\n";
        } else {
            int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) {
                errorMsg << "Invalid neuron_count: must be positive, got " << neuronCount << "\n";
            }
        }
        
        if (!hasRegionCount) {
            errorMsg << "Missing required parameter: region_count\n";
        } else {
            int64_t regionCount = config->getOr<int64_t>("region_count", 0);
            if (regionCount <= 0) {
                errorMsg << "Invalid region_count: must be positive, got " << regionCount << "\n";
            }
        }
        
        if (!hasTimestep) {
            errorMsg << "Missing required parameter: simulation_timestep\n";
        } else {
            double timestep = config->getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0) {
                errorMsg << "Invalid simulation_timestep: must be positive, got " << timestep << "\n";
            }
        }
        
        if (!hasSeed) {
            errorMsg << "Missing required parameter: random_seed\n";
        } else {
            int64_t seed = config->getOr<int64_t>("random_seed", 0);
            if (seed < 0) {
                errorMsg << "Invalid random_seed: must be non-negative, got " << seed << "\n";
            }
        }
        
        // Validate plasticity parameters
        if (config->has("stdp_ltp_weight")) {
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.0f);
            if (ltpWeight < 0.0f) {
                errorMsg << "Invalid stdp_ltp_weight: must be non-negative, got " << ltpWeight << "\n";
            }
        }
        
        if (config->has("stdp_ltd_weight")) {
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.0f);
            if (ltdWeight < 0.0f) {
                errorMsg << "Invalid stdp_ltd_weight: must be non-negative, got " << ltdWeight << "\n";
            }
        }
        
        if (config->has("stdp_tau")) {
            float tau = config->getOr<float>("stdp_tau", 0.0f);
            if (tau <= 0.0f) {
                errorMsg << "Invalid stdp_tau: must be positive, got " << tau << "\n";
            }
        }
        
        // Validate plasticity rates
        if (config->has("synaptogenesis_rate")) {
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0f);
            if (synaptogenesisRate < 0.0f || synaptogenesisRate > 1.0f) {
                errorMsg << "Invalid synaptogenesis_rate: must be between 0.0 and 1.0, got " << synaptogenesisRate << "\n";
            }
        }
        
        if (config->has("pruning_rate")) {
            float pruningRate = config->getOr<float>("pruning_rate", 0.0f);
            if (pruningRate < 0.0f || pruningRate > 1.0f) {
                errorMsg << "Invalid pruning_rate: must be between 0.0 and 1.0, got " << pruningRate << "\n";
            }
        }
        
        return errorMsg.str().empty();
    }
    
    // Get validation error message
    std::string getValidationErrorMessage(const std::shared_ptr<Config>& config) {
        std::stringstream errorMsg;
        
        // Check essential parameters
        if (!config->has("neuron_count")) {
            errorMsg << "Missing required parameter: neuron_count\n";
        } else {
            int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) {
                errorMsg << "Invalid neuron_count: must be positive, got " << neuronCount << "\n";
            }
        }
        
        if (!config->has("region_count")) {
            errorMsg << "Missing required parameter: region_count\n";
        } else {
            int64_t regionCount = config->getOr<int64_t>("region_count", 0);
            if (regionCount <= 0) {
                errorMsg << "Invalid region_count: must be positive, got " << regionCount << "\n";
            }
        }
        
        if (!config->has("simulation_timestep")) {
            errorMsg << "Missing required parameter: simulation_timestep\n";
        } else {
            double timestep = config->getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0) {
                errorMsg << "Invalid simulation_timestep: must be positive, got " << timestep << "\n";
            }
        }
        
        return errorMsg.str();
    }
    
    // Log error message with appropriate level
    void logError(const std::string& message, LogLevel level = LogLevel::Error) {
        if (quietQuiet) {
            // Only log errors in quiet-quiet mode
            if (level == LogLevel::Error) {
                std::cerr << "ERROR: " << message << std::endl;
            }
        } else if (quiet) {
            // Log warnings and errors in quiet mode
            if (level == LogLevel::Error || level == LogLevel::Warning) {
                std::cerr << "ERROR: " << message << std::endl;
            }
        } else {
            // Log all messages in verbose mode
            if (level == LogLevel::Error) {
                std::cerr << "ERROR: " << message << std::endl;
            } else if (level == LogLevel::Warning) {
                std::cerr << "WARNING: " << message << std::endl;
            }
        }
    }
    
    // Print network statistics
    void printNetworkStats(std::shared_ptr<Brain> brain) {
        std::cout << "Network Statistics:" << std::endl;
        std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing neurons (this step): " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    // Export network to GraphViz format
    void exportGraphViz(std::shared_ptr<Brain> brain, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "digraph NLM_Brain {" << std::endl;
        file << "  rankdir=LR;" << std::endl;
        file << "  node [shape=circle];" << std::endl;
        
        // Get all neurons
        for (size_t i = 0; i < brain->getTotalNeuronCount(); ++i) {
            file << "  neuron" << i << " [label=\"" << i << "\"]" << std::endl;
        }
        
        // Export synapses
        for (const auto& region : brain->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                uint64_t src = syn->getSourceNeuron().index();
                uint64_t dst = syn->getDestinationNeuron().index();
                float weight = syn->getWeight();
                
                file << "  neuron" << src << " -> neuron" << dst;
                if (weight != 0.0f) {
                    file << " [label=\"" << weight << "\"]";
                }
                file << ";" << std::endl;
            }
        }
        
        file << "}" << std::endl;
        file.close();
        
        std::cout << "Exported network to GraphViz format: " << filename << std::endl;
    }
    
    // Export neuron states
    void exportNeurons(std::shared_ptr<Brain> brain, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "Neuron State Export" << std::endl;
        file << "====================" << std::endl;
        
        size_t neuronIndex = 0;
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    file << "Neuron " << neuronIndex << ":" << std::endl;
                    file << "  Type: " << static_cast<int>(neuron->getType()) << std::endl;
                    file << "  Membrane potential: " << state.membranePotential << std::endl;
                    file << "  Resting potential: " << state.restingPotential << std::endl;
                    file << "  Threshold: " << state.threshold << std::endl;
                    file << "  Reset potential: " << state.resetPotential << std::endl;
                    file << "  Leak conductance: " << state.leakConductance << std::endl;
                    file << "  Firing state: " << static_cast<int>(state.firingState) << std::endl;
                    file << "  Refractory remaining: " << state.refractoryRemaining << std::endl;
                    file << "  Refractory period: " << state.refractoryPeriod << std::endl;
                    file << "  Last spike time: " << state.lastSpikeTime << std::endl;
                    file << "  Firing rate: " << state.firingRate << std::endl;
                    file << "  Is active: " << (neuron->isActive() ? "true" : "false") << std::endl;
                    file << "  Is firing: " << (neuron->isFiring() ? "true" : "false") << std::endl;
                    file << std::endl;
                    neuronIndex++;
                }
            }
        }
        
        file.close();
        std::cout << "Exported " << neuronIndex << " neuron states to: " << filename << std::endl;
    }
    
    // Export synapse states
    void exportSynapses(std::shared_ptr<Brain> brain, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "Synapse State Export" << std::endl;
        file << "=====================" << std::endl;
        
        size_t synapseIndex = 0;
        for (const auto& region : brain->getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                file << "Synapse " << synapseIndex << ":" << std::endl;
                file << "  Source neuron: " << syn->getSourceNeuron().index() << std::endl;
                file << "  Destination neuron: " << syn->getDestinationNeuron().index() << std::endl;
                file << "  Weight: " << syn->getWeight() << std::endl;
                file << "  Delay: " << syn->getDelay() << std::endl;
                file << "  Type: " << static_cast<int>(syn->getType()) << std::endl;
                file << "  Plasticity flags: STDP=" << (syn->getPlasticityFlags().stdp ? "true" : "false")
                      << " Hebbian=" << (syn->getPlasticityFlags().hebbian ? "true" : "false")
                      << " Structural=" << (syn->getPlasticityFlags().structural ? "true" : "false") << std::endl;
                file << "  Eligibility trace: " << syn->getEligibilityTrace() << std::endl;
                file << "  Efficacy: " << syn->getEfficacy() << std::endl;
                file << std::endl;
                synapseIndex++;
            }
        }
        
        file.close();
        std::cout << "Exported " << synapseIndex << " synapse states to: " << filename << std::endl;
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
    
    // Set default verbosity
    bool verbose = true;
    bool quiet = false;
    bool quietQuiet = false;
    bool debug = false;
    bool benchmark = false;
    bool benchmarkOnly = false;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    int simulationSteps = 1000;
    double timestep = 0.001;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    bool hebbian = true;
    bool stdp = true;
    bool structural = true;
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    uint64_t randomSeed = 42;
    int64_t saveInterval = 100;
    bool exportGraphViz = false;
    std::string graphVizFilename = "brain.dot";
    std::string neuronsFilename = "neurons.csv";
    std::string synapsesFilename = "synapses.csv";
    bool exportNeurons = false;
    bool exportSynapses = false;
    int64_t maxMemoryMB = 0;
    bool preserveState = false;
    bool developmentMode = false;
    std::string outputFile = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--quiet") {
            verbose = false;
            quiet = true;
        } else if (arg == "--quiet-quiet" || arg == "--quietquiet") {
            verbose = false;
            quiet = true;
            quietQuiet = true;
        } else if (arg == "--debug") {
            debug = true;
        } else if (arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "--benchmark-only") {
            benchmarkOnly = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 2) == "-c") {
            if (arg.size() > 2) {
                configFile = arg.substr(2);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                simulationSteps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                simulationSteps = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 10) == "--timestep") {
            if (arg.find('=') != std::string::npos) {
                timestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoll(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--regions") {
            if (arg.find('=') != std::string::npos) {
                regionCount = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                regionCount = std::stoll(argv[++i]);
            }
        } else if (arg.substr(0, 12) == "--connectivity") {
            if (arg.find('=') != std::string::npos) {
                connectionProbability = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                connectionProbability = std::stof(argv[++i]);
            }
        } else if (arg == "--hebbian") {
            hebbian = true;
        } else if (arg == "--stdp") {
            stdp = true;
        } else if (arg == "--structural") {
            structural = true;
        } else if (arg.substr(0, 14) == "--stdp-ltp-weight") {
            if (arg.find('=') != std::string::npos) {
                stdpLtpWeight = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                stdpLtpWeight = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 15) == "--stdp-ltd-weight") {
            if (arg.find('=') != std::string::npos) {
                stdpLtdWeight = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                stdpLtdWeight = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 11) == "--stdp-tau") {
            if (arg.find('=') != std::string::npos) {
                stdpTau = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                stdpTau = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 20) == "--synaptogenesis-rate") {
            if (arg.find('=') != std::string::npos) {
                synaptogenesisRate = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                synaptogenesisRate = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 15) == "--pruning-rate") {
            if (arg.find('=') != std::string::npos) {
                pruningRate = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                pruningRate = std::stof(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--seed") {
            if (arg.find('=') != std::string::npos) {
                randomSeed = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                randomSeed = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 14) == "--save-interval") {
            if (arg.find('=') != std::string::npos) {
                saveInterval = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                saveInterval = std::stoll(argv[++i]);
            }
        } else if (arg == "--export-graphviz") {
            exportGraphViz = true;
        } else if (arg.substr(0, 20) == "--graphviz-filename") {
            if (arg.find('=') != std::string::npos) {
                graphVizFilename = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                graphVizFilename = argv[++i];
            }
        } else if (arg == "--export-neurons") {
            exportNeurons = true;
            if (i + 1 < argc) {
                neuronsFilename = argv[++i];
                i++;  // Skip the filename argument
            }
        } else if (arg == "--export-synapses") {
            exportSynapses = true;
            if (i + 1 < argc) {
                synapsesFilename = argv[++i];
                i++;  // Skip the filename argument
            }
        } else if (arg.substr(0, 15) == "--max-memory") {
            if (arg.find('=') != std::string::npos) {
                maxMemoryMB = std::stoll(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                maxMemoryMB = std::stoll(argv[++i]);
            }
        } else if (arg == "--preserve-state") {
            preserveState = true;
        } else if (arg == "--development-mode") {
            developmentMode = true;
        } else if (arg.substr(0, 9) == "--output") {
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else {
            // Unknown argument, could be config file
            if (arg.find('/') == std::string::npos) {
                // Likely a config file
                configFile = arg;
            } else {
                std::cerr << "Error: Unknown option: " << arg << std::endl;
                std::cerr << "Use --help for available options." << std::endl;
                return 1;
            }
        }
    }
    
    // Initialize logger based on verbosity settings
    auto logger = std::make_shared<Logger>();
    LogLevel logLevel = LogLevel::Info;
    if (quietQuiet) {
        logLevel = LogLevel::Error;
    } else if (quiet) {
        logLevel = LogLevel::Warning;
    } else if (debug) {
        logLevel = LogLevel::Debug;
    }
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print startup information
    std::cout << "Starting NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: Phase 6 - Integrated Artificial Brain" << std::endl;
    std::cout << "Random seed: " << randomSeed << std::endl;
    std::cout << "Simulation steps: " << simulationSteps << std::endl;
    std::cout << "Timestep: " << timestep << "s" << std::endl;
    std::cout << "Neurons: " << neuronCount << std::endl;
    std::cout << "Regions: " << regionCount << std::endl;
    std::cout << "Connectivity probability: " << connectionProbability << std::endl;
    
    if (benchmark) {
        std::cout << "Benchmark mode enabled" << std::endl;
    }
    if (developmentMode) {
        std::cout << "Development mode enabled" << std::endl;
    }
    
    // Load configuration
    std::cout << "Loading configuration from: " << configFile << std::endl;
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Override with runtime options
    config->set("random_seed", static_cast<int64_t>(randomSeed), ConfigSource::Runtime);
    config->set("simulation_timestep", timestep, ConfigSource::Runtime);
    config->set("neuron_count", neuronCount, ConfigSource::Runtime);
    config->set("region_count", regionCount, ConfigSource::Runtime);
    config->set("connection_probability", connectionProbability, ConfigSource::Runtime);
    config->set("stdp_ltp_weight", stdpLtpWeight, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", stdpLtdWeight, ConfigSource::Runtime);
    config->set("stdp_tau", stdpTau, ConfigSource::Runtime);
    config->set("synaptogenesis_rate", synaptogenesisRate, ConfigSource::Runtime);
    config->set("pruning_rate", pruningRate, ConfigSource::Runtime);
    
    // Configure plasticity based on command line flags
    PlasticityFlags plasticityFlags;
    plasticityFlags.hebbian = hebbian;
    plasticityFlags.stdp = stdp;
    plasticityFlags.structural = structural;
    
    std::cout << "Plasticity configuration: Hebbian=" << (hebbian ? "true" : "false") 
              << " STDP=" << (stdp ? "true" : "false") 
              << " Structural=" << (structural ? "true" : "false") << std::endl;
    
    // Set max memory if specified
    if (maxMemoryMB > 0) {
        config->set("max_memory_mb", static_cast<int64_t>(maxMemoryMB), ConfigSource::Runtime);
        std::cout << "Maximum memory: " << maxMemoryMB << " MB" << std::endl;
    }
    
    std::cout << "Configuration loaded successfully" << std::endl;
    
    // Initialize brain
    std::cout << "Initializing NLM Brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    brain->logStatus();
    
    if (benchmarkOnly) {
        std::cout << "Benchmark mode - running quick performance tests" << std::endl;
        brain->step(10, 10 * timestep);
        brain->logStatus();
        std::cout << "Benchmark complete" << std::endl;
        return 0;
    }
    
    if (benchmark) {
        std::cout << "Running performance benchmarks..." << std::endl;
        // Run a series of performance tests
        for (int i = 0; i < 10; ++i) {
            brain->step(i * 100, i * 100 * timestep);
        }
        brain->logStatus();
        std::cout << "Benchmark complete" << std::endl;
        brain->reset();
        brain->initialize();
    }
    
    // Run main experiment
    std::cout << "Running main experiment..." << std::endl;
    
    // Test 1: Basic connectivity
    std::cout << "Test 1: Basic neural connectivity" << std::endl;
    runBasicConnectivityTest(brain);
    
    // Reset and test 2: Plasticity learning
    std::cout << "Test 2: Plasticity learning experiment" << std::endl;
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    // Reset and test 3: STDP verification
    std::cout << "Test 3: STDP verification" << std::endl;
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Additional development stages if requested
    if (developmentMode) {
        std::cout << "Development mode - running developmental stages" << std::endl;
        brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        for (int stageStep = 0; stageStep < 500; stageStep += 100) {
            brain->step(stageStep, stageStep * timestep);
        }
        brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
        for (int stageStep = 500; stageStep < 1000; stageStep += 100) {
            brain->step(stageStep, stageStep * timestep);
        }
        brain->setDevelopmentalStage(DevelopmentalStage::Adult);
    }
    
    // Save final state if requested
    if (!outputFile.empty()) {
        std::cout << "Saving final brain state to: " << outputFile << std::endl;
        if (!brain->save(outputFile)) {
            std::cerr << "Warning: Failed to save brain state" << std::endl;
        }
    }
    
    // Checkpoints saving during simulation (simplified)
    std::cout << "Checkpoint simulation..." << std::endl;
    for (int step = 0; step < simulationSteps; step += 100) {
        if (step > 0) {
            brain->step(step, step * timestep);
        }
        if (saveInterval > 0 && step > 0 && step % saveInterval == 0) {
            std::string checkpointFile = "checkpoint_step_" + std::to_string(step) + ".nlm";
            std::cout << "Saving checkpoint: " << checkpointFile << std::endl;
            brain->save(checkpointFile);
        }
    }
    
    // Export data if requested
    if (exportGraphViz) {
        std::cout << "Exporting network to GraphViz format..." << std::endl;
        exportGraphViz(brain, graphVizFilename);
    }
    
    if (exportNeurons) {
        std::cout << "Exporting neuron data..." << std::endl;
        exportNeurons(brain, neuronsFilename);
    }
    
    if (exportSynapses) {
        std::cout << "Exporting synapse data..." << std::endl;
@article{coder2024improving_neural_networks_architecture,
  title={Improving Neural Network Architectures Using NLM},
  author={AI Developer},
  year={2024},
  journal={Neural Network Research},
  doi={10.xxxx/nn.2024.01234}
}
}

    // Print final status
    std::cout << std::endl;
    std::cout << "=== Experiment Complete ===" << std::endl;
    brain->logStatus();
    
    std::cout << std::endl;
    std::cout << "=== Key Results ===" << std::endl;
    printNetworkStats(brain);
    
    std::cout << std::endl;
    std::cout << "Experiment completed successfully!" << std::endl;
    
    return 0;
}
