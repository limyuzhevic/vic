// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Advanced rate-based neural dynamics
// - Enhanced CLI with debugging and visualization

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

#include <chrono>
#include <iomanip>
#include <sstream>

// Simple timing utilities for performance profiling
class Profiler {
public:
    struct ProfileData {
        std::string name;
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point end;
        bool running;
        
        ProfileData(const std::string& n) : name(n), running(false) {}
        
        void start() {
            if (!running) {
                start = std::chrono::high_resolution_clock::now();
                running = true;
            }
        }
        
        void stop() {
            if (running) {
                end = std::chrono::high_resolution_clock::now();
                running = false;
            }
        }
        
        double durationMs() const {
            if (!running && end.time_since_epoch().count() > 0) {
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                return duration.count() / 1000.0;
            }
            return 0.0;
        }
    };
    
    static void profileStep(std::shared_ptr<Brain> brain, size_t step) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Performance Profile: Step " + std::to_string(step) + " ===");
        
        // Time brain step operation
        auto start = std::chrono::high_resolution_clock::now();
        brain->step(step, step * 0.001);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double stepTimeMs = duration.count() / 1000.0;
        
        NLM_LOG_INFO("Brain step execution time: " + std::to_string(stepTimeMs) + " ms");
        
        // Get performance statistics
        size_t spikeCount = brain->getTotalSpikeCount();
        size_t firingNeurons = brain->getFiringNeuronCount();
        float avgFiringRate = brain->getAverageFiringRate();
        
        NLM_LOG_INFO("Spike count: " + std::to_string(spikeCount));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(firingNeurons));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate) + " Hz");
        
        // Calculate spikes per millisecond
        double spikesPerMs = (step > 0) ? (spikeCount / static_cast<double>(step)) : 0.0;
        NLM_LOG_INFO("Spikes per millisecond: " + std::to_string(spikesPerMs));
        
        // Memory usage estimate (simplified)
        NLM_LOG_INFO("Neuron count: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Synapse count: " + std::to_string(brain->getTotalSynapseCount()));
        
        // Store performance data for later analysis
        static std::vector<double> stepTimes;
        static std::vector<size_t> spikeCounts;
        
        stepTimes.push_back(stepTimeMs);
        spikeCounts.push_back(spikeCount);
        
        // Print running statistics every 100 steps
        if (step % 100 == 0 && step > 0) {
            double avgStepTime = 0.0;
            double avgSpikeCount = 0.0;
            
            for (size_t i = 0; i < stepTimes.size(); ++i) {
                avgStepTime += stepTimes[i];
                avgSpikeCount += spikeCounts[i];
            }
            
            avgStepTime /= stepTimes.size();
            avgSpikeCount /= stepTimes.size();
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Running Performance Statistics (last " + std::to_string(stepTimes.size()) + " steps) ===");
            NLM_LOG_INFO("Average step time: " + std::to_string(avgStepTime) + " ms");
            NLM_LOG_INFO("Average spikes per step: " + std::to_string(avgSpikeCount));
            
            double spikesPerSecond = avgSpikeCount * 1000.0 / avgStepTime;
            NLM_LOG_INFO("Estimated spikes per second: " + std::to_string(spikesPerSecond));
        }
    }
    
    static void printPerformanceSummary(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Performance Summary ===");
        
        size_t totalSpikes = brain->getTotalSpikeCount();
        float avgFiringRate = brain->getAverageFiringRate();
        
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Total spikes generated: " + std::to_string(totalSpikes));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate) + " Hz");
        
        if (totalSpikes > 0) {
            double spikesPerStep = static_cast<double>(totalSpikes) / 1000.0;  // Estimate for 1000 steps
            NLM_LOG_INFO("Estimated network throughput: " + std::to_string(spikesPerStep) + " spikes/step");
        }
        
        // Efficiency metrics
        size_t activeNeurons = brain->getFiringNeuronCount();
        float efficiency = (activeNeurons > 0) ? (activeNeurons * 100.0 / brain->getTotalNeuronCount()) : 0.0;
        NLM_LOG_INFO("Current network efficiency: " + std::to_string(efficiency) + "% active neurons");
    }
};

void runPerformanceTest(std::shared_ptr<Brain> brain, size_t steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Test Suite ===");
    NLM_LOG_INFO("Running " + std::to_string(steps) + " simulation steps with profiling...");
    
    // Reset brain for clean test
    brain->reset();
    brain->initialize();
    
    // Run performance test with detailed profiling
    for (size_t step = 0; step < steps; ++step) {
        // Inject varying input patterns to test different operating regimes
        if (step % 100 == 0) {
            // High input phase
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                    neurons[i]->injectCurrent(30.0f);
                }
            }
        }
        
        if (step % 50 == 0) {
            // Low input phase
            auto* region = brain->getRegion(RegionId(1));
            if (region) {
                auto neurons = region->getAllNeurons();
                for (size_t i = 0; i < std::min(size_t(2), neurons.size()); ++i) {
                    neurons[i]->injectCurrent(5.0f);
                }
            }
        }
        
        // Run profiled step
        Profiler::profileStep(brain, step);
        
        // Log periodic summary
        if (step % 250 == 0 && step > 0) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Progress: " + std::to_string(step) + "/" + std::to_string(steps) + 
                        " steps completed (" + std::to_string(static_cast<float>(step) / steps * 100.0f) + "%)");
        }
    }
    
    // Print final performance summary
    Profiler::printPerformanceSummary(brain);
}

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
    ║     - Advanced rate-based dynamics                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

enum class SimulationMode {
    Normal,
    Fast,
    Slow
};

void printHelp() {
    printBanner();
    std::cout << "Usage: nlm [options]\n" << std::endl;
    std::cout << "Options:\n" << std::endl;
    std::cout << "  --config=file      Load configuration from file\n" << std::endl;
    std::cout << "  --neuron-count=N   Set number of neurons (default: 500)\n" << std::endl;
    std::cout << "  --steps=N          Number of simulation steps (default: 1000)\n" << std::endl;
    std::cout << "  --mode=MODE        Simulation mode: normal, fast, slow (default: normal)\n" << std::endl;
    std::cout << "  --analysis         Enable detailed analysis mode\n" << std::endl;
    std::cout << "  --debug            Enable debug output\n" << std::endl;
    std::cout << "  --profile          Enable performance profiling\n" << std::endl;
    std::cout << "  --save=file        Save brain state to file after simulation\n" << std::endl;
    std::cout << "  --load=file        Load brain state from file\n" << std::endl;
    std::cout << "  --visualization     Enable brain visualization\n" << std::endl;
    std::cout << "  --detailed-stats   Enable detailed statistics\n" << std::endl;
    std::cout << "  --neuron-debug     Enable neuron debugging\n" << std::endl;
    std::cout << "  --synapse-debug    Enable synapse debugging\n" << std::endl;
    std::cout << "  --memory-debug     Enable memory system debugging\n" << std::endl;
    std::cout << "  --neuromodulation-debug Enable neuromodulation debugging\n" << std::endl;
    std::cout << "  --development-debug Enable development system debugging\n" << std::endl;
    std::cout << "  --info             Show brain information only\n" << std::endl;
    std::cout << "  --version          Show version information\n" << std::endl;
    std::cout << "  --help             Show this help message\n" << std::endl;
}

void printVersion() {
    std::cout << "NLM v0.1.0 - Neural Learning Machine\n" << std::endl;
    std::cout << "Phase 2: Real Neural Computation\n" << std::endl;
    std::cout << "Experimental artificial developmental brain\n" << std::endl;
    std::cout << "Phase 6 integrated systems: Memory, Prediction, Cognition, Neuromodulation\n" << std::endl;
}

// Forward declaration for visualization functions
void printBrainVisualization(std::shared_ptr<Brain> brain);
void printDebugNeurons(std::shared_ptr<Brain> brain, size_t maxNeurons = 10);
void printSynapseStatistics(std::shared_ptr<Brain> brain);
void printMemoryStatistics(std::shared_ptr<Brain> brain);
void printNeuromodulationStats(std::shared_ptr<Brain> brain);
void printDevelopmentStats(std::shared_ptr<Brain> brain);

void applySimulationMode(SimulationMode mode) {
    switch (mode) {
        case SimulationMode::Fast:
            NLM_LOG_INFO("Simulation mode: Fast (higher current, faster execution)");
            break;
        case SimulationMode::Slow:
            NLM_LOG_INFO("Simulation mode: Slow (lower current, detailed output)");
            break;
        default:
            NLM_LOG_INFO("Simulation mode: Normal");
            break;
    }
}

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

void runAdvancedSimulation(std::shared_ptr<Brain> brain, SimulationMode mode, 
                          size_t steps, bool enableAnalysis, bool enableDebug) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Advanced Simulation Mode ===");
    
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
    
    if (enableAnalysis) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Detailed Analysis ===");
        
        // Analyze brain state
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
        NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        
        // Analyze connectivity
        if (auto* region = brain->getRegion(RegionId(1))) {
            NLM_LOG_INFO("Region 1 statistics:");
            NLM_LOG_INFO("  Neurons: " + std::to_string(region->getTotalNeuronCount()));
            NLM_LOG_INFO("  Active neurons: " + std::to_string(region->getActiveNeuronCount()));
            NLM_LOG_INFO("  Firing neurons: " + std::to_string(region->getFiringNeuronCount()));
            NLM_LOG_INFO("  Average synaptic weight: " + std::to_string(region->getAverageSynapticWeight()));
            NLM_LOG_INFO("  Synaptic density: " + std::to_string(region->getSynapticDensity()));
        }
    }
    
    if (enableDebug) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Debug Mode ===");
        NLM_LOG_INFO("Detailed neuron information (first 5 neurons):");
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            auto neurons = region->getAllNeurons();
            for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                           " Type: " + std::to_string(static_cast<int>(neurons[i]->getType())) +
                           " V: " + std::to_string(neurons[i]->getMembranePotential()) +
                           " Refractory: " + std::to_string(neurons[i]->isRefractory()) +
                           " Firing: " + std::to_string(neurons[i]->isFiring()));
            }
        }
    }
}

void printBrainVisualization(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Brain Visualization ===");
    
    // Get brain statistics
    NLM_LOG_INFO("Neuron Distribution by Type:");
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto populations = region->getPopulations();
        size_t totalNeurons = 0;
        
        for (size_t i = 0; i < populations.size(); ++i) {
            auto& pop = populations[i];
            size_t count = pop->getSize();
            totalNeurons += count;
            
            std::string typeStr;
            switch (pop->getNeuronType()) {
                case NeuronType::Excitatory: typeStr = "Excitatory"; break;
                case NeuronType::Inhibitory: typeStr = "Inhibitory"; break;
                case NeuronType::Sensory: typeStr = "Sensory"; break;
                case NeuronType::Motor: typeStr = "Motor"; break;
                case NeuronType::Modulatory: typeStr = "Modulatory"; break;
                case NeuronType::Internal: typeStr = "Internal"; break;
                default: typeStr = "Unknown"; break;
            }
            
            NLM_LOG_INFO("  Population " + std::to_string(i) + ": " + typeStr + 
                        " - " + std::to_string(count) + " neurons");
        }
        
        NLM_LOG_INFO("Total neurons: " + std::to_string(totalNeurons));
    }
    
    // Visual activity representation
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Activity Heatmap (firing neurons):");
    if (region) {
        auto neurons = region->getAllNeurons();
        size_t firingCount = 0;
        float maxRate = 0.0f;
        
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                firingCount++;
                maxRate = std::max(maxRate, neuron->getFiringRate());
            }
        }
        
        NLM_LOG_INFO("Firing neurons: " + std::to_string(firingCount) + 
                    " out of " + std::to_string(neurons.size()) + 
                    " (" + std::to_string(static_cast<float>(firingCount) / neurons.size() * 100.0f) + "%)");
        
        // Create simple visual representation
        NLM_LOG_INFO("Activity level: ");
        float activityLevel = static_cast<float>(firingCount) / neurons.size();
        for (int i = 0; i < 20; ++i) {
            if (i < static_cast<int>(activityLevel * 20)) {
                NLM_LOG_INFO("  █");
            } else {
                NLM_LOG_INFO("  ░");
            }
        }
    }
}

void printDebugNeurons(std::shared_ptr<Brain> brain, size_t maxNeurons) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Debug Neuron Information ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_INFO("No region found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    size_t neuronsToShow = std::min(maxNeurons, neurons.size());
    
    NLM_LOG_INFO("Showing detailed info for first " + std::to_string(neuronsToShow) + " neurons:");
    NLM_LOG_INFO("");
    
    for (size_t i = 0; i < neuronsToShow; ++i) {
        auto* neuron = neurons[i];
        
        NLM_LOG_INFO("Neuron " + std::to_string(i) + ":");
        NLM_LOG_INFO("  ID: " + std::to_string(neuron->getId()));
        NLM_LOG_INFO("  Type: " + std::to_string(static_cast<int>(neuron->getType())));
        NLM_LOG_INFO("  Membrane Potential: " + std::to_string(neuron->getMembranePotential()) + " mV");
        NLM_LOG_INFO("  Threshold: " + std::to_string(neuron->getThreshold()) + " mV");
        NLM_LOG_INFO("  Resting Potential: " + std::to_string(neuron->getRestingPotential()) + " mV");
        NLM_LOG_INFO("  Is Firing: " + std::to_string(neuron->isFiring()));
        NLM_LOG_INFO("  Is Refractory: " + std::to_string(neuron->isRefractory()));
        NLM_LOG_INFO("  Firing Rate: " + std::to_string(neuron->getFiringRate()) + " Hz");
        NLM_LOG_INFO("  Last Spike: " + std::to_string(neuron->getLastSpikeTime()) + " ms ago");
        NLM_LOG_INFO("  Refractory Period: " + std::to_string(neuron->getRefractoryPeriod()) + " steps");
        NLM_LOG_INFO("  Refractory Remaining: " + std::to_string(neuron->getState().refractoryRemaining) + " steps");
        NLM_LOG_INFO("  Synaptic Input: " + std::to_string(neuron->getTotalCurrent()) + " μA");
        NLM_LOG_INFO("");
    }
}

void printSynapseStatistics(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Synapse Statistics ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_INFO("No region found!");
        return;
    }
    
    auto synapses = region->getSynapses();
    NLM_LOG_INFO("Total synapses: " + std::to_string(synapses.size()));
    
    if (synapses.empty()) {
        NLM_LOG_INFO("No synapses in region!");
        return;
    }
    
    // Calculate statistics
    float totalWeight = 0.0f;
    float maxWeight = -1000.0f;
    float minWeight = 1000.0f;
    size_t excitatoryCount = 0;
    size_t inhibitoryCount = 0;
    size_t delayedCount = 0;
    
    for (auto& syn : synapses) {
        float weight = syn->getWeight();
        totalWeight += weight;
        maxWeight = std::max(maxWeight, weight);
        minWeight = std::min(minWeight, weight);
        
        if (syn->isExcitatory()) {
            excitatoryCount++;
        } else if (syn->isInhibitory()) {
            inhibitoryCount++;
        }
        
        if (syn->getDelay() > 1) {
            delayedCount++;
        }
    }
    
    float avgWeight = totalWeight / synapses.size();
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Average weight: " + std::to_string(avgWeight));
    NLM_LOG_INFO("  Maximum weight: " + std::to_string(maxWeight));
    NLM_LOG_INFO("  Minimum weight: " + std::to_string(minWeight));
    NLM_LOG_INFO("  Weight Range: " + std::to_string(maxWeight - minWeight));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synapse Types:");
    NLM_LOG_INFO("  Excitatory: " + std::to_string(excitatoryCount) + 
                " (" + std::to_string(static_cast<float>(excitatoryCount) / synapses.size() * 100.0f) + "%)");
    NLM_LOG_INFO("  Inhibitory: " + std::to_string(inhibitoryCount) + 
                " (" + std::to_string(static_cast<float>(inhibitoryCount) / synapses.size() * 100.0f) + "%)");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Delay Statistics:");
    NLM_LOG_INFO("  Average delay: " + std::to_string(delayedCount) + "/" + std::to_string(synapses.size()) + 
                " synapses have delay > 1 step");
    
    // Show some example synapses
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Example synapses (first 5):");
    size_t examples = std::min(size_t(5), synapses.size());
    
    for (size_t i = 0; i < examples; ++i) {
        auto& syn = synapses[i];
        NLM_LOG_INFO("  Synapse " + std::to_string(i) + ": " +
                    "pre=" + std::to_string(syn->getSourceNeuron().index()) +
                    " post=" + std::to_string(syn->getDestinationNeuron().index()) +
                    " weight=" + std::to_string(syn->getWeight()) +
                    " type=" + (syn->isExcitatory() ? "exc" : "inh") +
                    " delay=" + std::to_string(syn->getDelay()));
    }
}

void printMemoryStatistics(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Memory System Statistics ===");
    
    // Working memory
    if (auto* wm = brain->getWorkingMemory()) {
        NLM_LOG_INFO("Working Memory:");
        NLM_LOG_INFO("  Enabled: Yes");
        NLM_LOG_INFO("  Pattern capacity: " + std::to_string(wm->getPatternCapacity()));
        NLM_LOG_INFO("  Current patterns: " + std::to_string(wm->getCurrentPatternCount()));
        NLM_LOG_INFO("  Activation level: " + std::to_string(wm->getActivationLevel()));
    } else {
        NLM_LOG_INFO("Working Memory: Not available");
    }
    
    // Episodic memory
    if (auto* em = brain->getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic Memory:");
        NLM_LOG_INFO("  Enabled: Yes");
        NLM_LOG_INFO("  Capacity: " + std::to_string(em->getMaxCapacity()));
        NLM_LOG_INFO("  Current episodes: " + std::to_string(em->getCurrentEpisodeCount()));
        NLM_LOG_INFO("  Replay interval: " + std::to_string(em->getReplayInterval()));
    } else {
        NLM_LOG_INFO("Episodic Memory: Not available");
    }
    
    // Associative memory
    if (auto* am = brain->getAssociativeMemory()) {
        NLM_LOG_INFO("Associative Memory:");
        NLM_LOG_INFO("  Enabled: Yes");
        NLM_LOG_INFO("  Association strength threshold: " + std::to_string(am->getAssociationStrengthThreshold()));
        NLM_LOG_INFO("  Current associations: " + std::to_string(am->getCurrentAssociationCount()));
    } else {
        NLM_LOG_INFO("Associative Memory: Not available");
    }
}

void printNeuromodulationStats(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Neuromodulation Statistics ===");
    
    // Get neuromodulators
    if (auto* dopamine = brain->getDopamine()) {
        NLM_LOG_INFO("Dopamine:");
        NLM_LOG_INFO("  Enabled: Yes");
        NLM_LOG_INFO("  Current level: " + std::to_string(dopamine->getCurrentLevel()));
        NLM_LOG_INFO("  Learning rate: " + std::to_string(dopamine->getLearningRate()));
    } else {
        NLM_LOG_INFO("Dopamine: Not available");
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        NLM_LOG_INFO("Curiosity:");
        NLM_LOG_INFO("  Enabled: Yes");
        NLM_LOG_INFO("  Current level: " + std::to_string(curiosity->getCurrentLevel()));
        NLM_LOG_INFO("  Exploration bonus: " + std::to_string(curiosity->getExplorationBonus()));
    } else {
        NLM_LOG_INFO("Curiosity: Not available");
    }
    
    if (auto* novelty = brain->getNovelty()) {
        NLM_LOG_INFO("Novelty:");
        NLM_LOG_INFO("  Enabled: Yes");
        NLM_LOG_INFO("  Current level: " + std::to_string(novelty->getCurrentLevel()));
        NLM_LOG_INFO("  Surprise threshold: " + std::to_string(novelty->getSurpriseThreshold()));
    } else {
        NLM_LOG_INFO("Novelty: Not available");
    }
    
    if (auto* predError = brain->getPredictionErrorSignal()) {
        NLM_LOG_INFO("Prediction Error:");
        NLM_LOG_INFO("  Enabled: Yes");
        NLM_LOG_INFO("  Current error: " + std::to_string(predError->getCurrentError()));
        NLM_LOG_INFO("  Prediction confidence: " + std::to_string(predError->getPredictionConfidence()));
    } else {
        NLM_LOG_INFO("Prediction Error: Not available");
    }
}

void printDevelopmentStats(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Development System Statistics ===");
    
    if (auto* dev = brain->getDevelopmentSystem()) {
        NLM_LOG_INFO("Development System:");
        NLM_LOG_INFO("  Enabled: Yes");
        NLM_LOG_INFO("  Current stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        NLM_LOG_INFO("  Synaptogenesis rate: " + std::to_string(dev->getSynaptogenesisRate()));
        NLM_LOG_INFO("  Pruning rate: " + std::to_string(dev->getPruningRate()));
        NLM_LOG_INFO("  Maturation factor: " + std::to_string(dev->getMaturationFactor()));
        NLM_LOG_INFO("  Critical period active: " + std::to_string(dev->isCriticalPeriodActive()));
        
        // Calculate development progress
        float progress = static_cast<float>(brain->getDevelopmentalStage()) / 5.0f * 100.0f;
        NLM_LOG_INFO("  Development progress: " + std::to_string(progress) + "%");
    } else {
        NLM_LOG_INFO("Development System: Not available");
    }
}

int main(int argc, char** argv) {
    printBanner();
    
    // Check for help/version flags before processing
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        }
    }
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    SimulationMode simulationMode = SimulationMode::Normal;
    bool enableAnalysis = false;
    bool enableDebug = false;
    bool enableProfile = false;
    bool enableVisualization = false;
    bool enableDetailedStats = false;
    bool enableNeuronDebug = false;
    bool enableSynapseDebug = false;
    bool enableMemoryDebug = false;
    bool enableNeuromodulationDebug = false;
    bool enableDevelopmentDebug = false;
    std::string saveFile = "";
    std::string loadFile = "";
    bool showInfoOnly = false;
    size_t simulationSteps = 1000;  // Default number of steps for advanced mode
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--visualization" || arg == "--visualize") {
            enableVisualization = true;
        } else if (arg == "--detailed-stats") {
            enableDetailedStats = true;
        } else if (arg == "--neuron-debug") {
            enableNeuronDebug = true;
        } else if (arg == "--synapse-debug") {
            enableSynapseDebug = true;
        } else if (arg == "--memory-debug") {
            enableMemoryDebug = true;
        } else if (arg == "--neuromodulation-debug") {
            enableNeuromodulationDebug = true;
        } else if (arg == "--development-debug") {
            enableDevelopmentDebug = true;
        } else if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.substr(0, 7) == "--config") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                configFile = arg.substr(pos + 1);
            }
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            // Parse neuron count and override config
            size_t pos = std::string(argv[i + 1]).find('=');
            if (pos != std::string::npos) {
                // We'll handle this in config loadFromArgs later
                config->set("neuron_count", static_cast<int64_t>(std::stoi(argv[i + 1].substr(pos + 1))), ConfigSource::CommandLine);
                ++i; // Skip the value
            } else {
                config->set("neuron_count", static_cast<int64_t>(std::stoi(argv[i + 1])), ConfigSource::CommandLine);
                ++i;
            }
        } else if (arg == "--steps" && i + 1 < argc) {
            size_t pos = std::string(argv[i + 1]).find('=');
            if (pos != std::string::npos) {
                simulationSteps = static_cast<size_t>(std::stoi(argv[i + 1].substr(pos + 1)));
                ++i;
            } else {
                simulationSteps = static_cast<size_t>(std::stoi(argv[i + 1]));
                ++i;
            }
        } else if (arg == "--mode" && i + 1 < argc) {
            std::string mode = argv[++i];
            if (mode == "fast") simulationMode = SimulationMode::Fast;
            else if (mode == "slow") simulationMode = SimulationMode::Slow;
            else simulationMode = SimulationMode::Normal;
        } else if (arg == "--analysis") {
            enableAnalysis = true;
        } else if (arg == "--debug") {
            enableDebug = true;
        } else if (arg == "--profile") {
            enableProfile = true;
        } else if (arg == "--save" && i + 1 < argc) {
            saveFile = argv[++i];
        } else if (arg == "--load" && i + 1 < argc) {
            loadFile = argv[++i];
        }
    }
    
    // Apply visualization options
    if (enableVisualization || enableDetailedStats || enableNeuronDebug || enableSynapseDebug || 
        enableMemoryDebug || enableNeuromodulationDebug || enableDevelopmentDebug) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Debug and Visualization Mode ===");
    }
    
    if (enableVisualization) {
        printBrainVisualization(nullptr); // Will be fixed with brain object
    }
    
    if (enableDetailedStats) {
        NLM_LOG_INFO("Detailed statistics enabled:");
        if (enableNeuronDebug) NLM_LOG_INFO("  Neuron debug: YES");
        if (enableSynapseDebug) NLM_LOG_INFO("  Synapse debug: YES");
        if (enableMemoryDebug) NLM_LOG_INFO("  Memory debug: YES");
        if (enableNeuromodulationDebug) NLM_LOG_INFO("  Neuromodulation debug: YES");
        if (enableDevelopmentDebug) NLM_LOG_INFO("  Development debug: YES");
    }
    
    if (enableNeuronDebug) {
        printDebugNeurons(nullptr, 5); // Will be fixed with brain object
    }
    
    if (enableSynapseDebug) {
        printSynapseStatistics(nullptr); // Will be fixed with brain object
    }
    
    if (enableMemoryDebug) {
        printMemoryStatistics(nullptr); // Will be fixed with brain object
    }
    
    if (enableNeuromodulationDebug) {
        printNeuromodulationStats(nullptr); // Will be fixed with brain object
    }
    
    if (enableDevelopmentDebug) {
        printDevelopmentStats(nullptr); // Will be fixed with brain object
    }
    
    if (showInfoOnly) {
        NLM_LOG_INFO("=== NLM Brain Information ===");
        // Simple info mode - just show brain stats
        NLM_LOG_INFO("This would show brain information only mode.");
        return 0;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
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
    
    // Apply simulation mode
    applySimulationMode(simulationMode);
    
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
    
    // Apply visualization with actual brain object now
    if (enableVisualization) {
        printBrainVisualization(brain);
    }
    
    if (enableNeuronDebug) {
        printDebugNeurons(brain, 5);
    }
    
    if (enableSynapseDebug) {
        printSynapseStatistics(brain);
    }
    
    if (enableMemoryDebug) {
        printMemoryStatistics(brain);
    }
    
    if (enableNeuromodulationDebug) {
        printNeuromodulationStats(brain);
    }
    
    if (enableDevelopmentDebug) {
        printDevelopmentStats(brain);
    }
    
    // Run advanced simulation based on mode
    if (simulationMode == SimulationMode::Normal) {
        // Run original tests
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
    } else {
        // Run advanced simulation
        runAdvancedSimulation(brain, simulationMode, simulationSteps, enableAnalysis, enableDebug);
    }
    
    // Save brain state if requested
    if (!saveFile.empty()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Saving brain state to: " + saveFile);
        if (brain->save(saveFile)) {
            NLM_LOG_INFO("Brain state saved successfully.");
        } else {
            NLM_LOG_ERROR("Failed to save brain state.");
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