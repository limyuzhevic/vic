#include "BasicConnectivityExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <numeric>
#include <algorithm>

namespace nlm {

struct BasicConnectivityExperiment::Impl {
    // Command line interface helper
    void printUsage() const;
    bool parseCommandLine(int argc, char** argv, BasicConnectivityConfig& config);
    void printResults(const BasicConnectivityResult& result) const;
    void printExperimentInfo(const BasicConnectivityConfig& config) const;
};

BasicConnectivityExperiment::BasicConnectivityExperiment() : pImpl(std::make_unique<Impl>()) {}

BasicConnectivityExperiment::~BasicConnectivityExperiment() = default;

BasicConnectivityResult BasicConnectivityExperiment::run(const BasicConnectivityConfig& config, std::shared_ptr<Brain> brain) {
    BasicConnectivityResult result;
    
    // Create brain if not provided
    if (!brain) {
        auto cfg = std::make_shared<Config>();
        cfg->set("random_seed", static_cast<int64_t>(config.seed));
        cfg->set("simulation_timestep", 0.001);
        cfg->set("neuron_count", static_cast<int64_t>(1000));
        cfg->set("region_count", static_cast<int64_t>(1));
        cfg->set("connection_probability", 0.15f);
        
        // STDP parameters
        cfg->set("stdp_ltp_weight", 0.02f);
        cfg->set("stdp_ltd_weight", 0.015f);
        cfg->set("stdp_tau", 20.0f);
        
        // Structural plasticity parameters
        cfg->set("synaptogenesis_rate", 0.0001f);
        cfg->set("pruning_rate", 0.00001f);
        
        brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return result;
        }
    }
    
    NLM_LOG_INFO("=== Basic Connectivity Experiment ===");
    printExperimentInfo(config);
    
    // Test 1: Basic connectivity
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No region found!");
        return result;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("  No neurons found!");
        return result;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(config.currentInjection);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
        result.spikesPropagate = true;
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
        result.spikesPropagate = false;
    }
    
    result.spikesGenerated = spikes;
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity learning experiment
    if (config.enablePlasticity) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        LearningExperiment experiment(brain, config.seed);
        
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
        NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(config.steps) + " steps)...");
        
        for (SimulationStep step = 0; step < config.steps; ++step) {
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
        
        // Extract learning metrics
        result.learningDetected = std::abs(experiment.finalWeights.empty() ? 0 : experiment.finalWeights[0] - 
                                          experiment.initialWeights.empty() ? 0 : experiment.initialWeights[0]) > 0.001f;
        
        if (!experiment.initialWeights.empty()) {
            result.initialMeanWeight = std::accumulate(experiment.initialWeights.begin(), 
                                                     experiment.initialWeights.end(), 0.0f) / 
                                       experiment.initialWeights.size();
        }
        if (!experiment.finalWeights.empty()) {
            result.finalMeanWeight = std::accumulate(experiment.finalWeights.begin(), 
                                                   experiment.finalWeights.end(), 0.0f) / 
                                     experiment.finalWeights.size();
        }
        
        result.meanWeightChange = result.finalMeanWeight - result.initialMeanWeight;
        
        // Count synaptic changes
        if (!experiment.initialWeights.empty() && !experiment.finalWeights.empty()) {
            size_t minSize = std::min(experiment.initialWeights.size(), experiment.finalWeights.size());
            for (size_t i = 0; i < minSize; ++i) {
                float delta = experiment.finalWeights[i] - experiment.initialWeights[i];
                if (delta > 0.01f) ++result.strongenedConnections;
                else if (delta < -0.01f) ++result.weakenedConnections;
                else ++result.unchangedConnections;
            }
        }
    }
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP verification
    if (config.enableStdp) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto& synapses = region->getSynapses();
            if (synapses.size() >= 5) {
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
                float totalDelta = 0.0f;
                for (size_t i = 0; i < 5; ++i) {
                    float delta = synapses[i]->getWeight() - beforeWeights[i];
                    NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                                " after: " + std::to_string(synapses[i]->getWeight()) +
                                " (Δ=" + std::to_string(delta) + ")");
                    totalDelta += delta;
                }
                
                // Check if weights increased (LTP)
                if (totalDelta > 0.001f) {
                    NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
                } else if (totalDelta < -0.001f) {
                    NLM_LOG_INFO("  ! STDP reversed: Check parameters");
                } else {
                    NLM_LOG_INFO("  ! No change: STDP may not be triggering");
                }
            } else {
                NLM_LOG_INFO("  Not enough synapses for STDP test");
            }
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    // Collect final statistics
    result.totalSpikes = brain->getTotalSpikeCount();
    result.firingNeuronCount = brain->getFiringNeuronCount();
    
    // Print formatted results
    printResults(result);
    
    return result;
}

bool BasicConnectivityExperiment::runConnectivityTest(std::shared_ptr<Brain> brain) {
    // Delegate to the main run method
    BasicConnectivityConfig config;
    config.steps = 50;  // Short for connectivity test
    config.currentInjection = 50.0f;
    config.enablePlasticity = false;
    config.enableStdp = false;
    
    BasicConnectivityResult result = run(config, brain);
    return result.spikesPropagate;
}

bool BasicConnectivityExperiment::runPlasticityExperiment(std::shared_ptr<Brain> brain, uint64_t steps) {
    BasicConnectivityConfig config;
    config.steps = steps;
    config.currentInjection = 50.0f;
    config.enablePlasticity = true;
    config.enableStdp = true;
    
    BasicConnectivityResult result = run(config, brain);
    return result.learningDetected;
}

bool BasicConnectivityExperiment::runStdpVerification(std::shared_ptr<Brain> brain) {
    BasicConnectivityConfig config;
    config.steps = 0;  // Not used for STDP
    config.currentInjection = 50.0f;
    config.enablePlasticity = false;
    config.enableStdp = true;
    
    BasicConnectivityResult result = run(config, brain);
    
    // For STDP verification, we need to check if STDP produced changes
    return result.strongenedConnections > 0 || result.weakenedConnections > 0;
}

// Legacy LearningExperiment implementation
void LearningExperiment::recordInitialState() {
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

void LearningExperiment::recordFinalState() {
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

void LearningExperiment::computeStatistics() {
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

} // namespace nlm
