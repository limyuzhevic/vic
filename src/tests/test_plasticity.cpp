// NLM (熙然) - Neural Learning Machine
// Phase 2: Plasticity Learning Experiment Test
// Extracted test function for improved modularity and test organization

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <numeric>
#include <iomanip>
#include <algorithm>

using namespace nlm;

namespace Tests {

class EnhancedTestRunner {
private:
    std::shared_ptr<Brain> brain;
    bool verbose;
    bool quiet;
    std::chrono::high_resolution_clock::time_point startTime;
    
public:
    EnhancedTestRunner(std::shared_ptr<Brain> b, bool v = false, bool q = false)
        : brain(b), verbose(v), quiet(q) {}
    
    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedTime() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return duration.count() / 1000.0;
    }
    
    void log(const std::string& message, bool important = false) {
        if (quiet && !important) return;
        
        auto elapsed = getElapsedTime();
        std::string timestamp = "[" + std::to_string(elapsed) + "s]";
        
        if (verbose || important) {
            std::cout << timestamp << " " << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }
    
    void runEnhancedPlasticityExperiment() {
        log("=== Test 2: Enhanced Plasticity Learning Experiment ===", true);
        
        struct EnhancedLearningExperiment {
            std::shared_ptr<Brain> brain;
            uint64_t seed;
            size_t initialSynapseCount;
            std::vector<float> initialWeights;
            std::vector<float> finalWeights;
            std::vector<NeuronId> mostActiveNeurons;
            double startTime;
            
            EnhancedLearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
                : brain(b), seed(s), initialSynapseCount(0) {}
            
            void recordInitialState() {
                startTime = brain->getSpikeSystem()->getSimulationTime();
                initialSynapseCount = brain->getTotalSynapseCount();
                initialWeights.clear();
                
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        initialWeights.push_back(syn->getWeight());
                    }
                }
                
                log("  Initial state recorded:");
                log("    Synapses: " + std::to_string(initialSynapseCount));
                if (!initialWeights.empty()) {
                    float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                    float mean = sum / initialWeights.size();
                    log("    Mean weight: " + std::to_string(mean));
                }
            }
            
            void recordFinalState() {
                finalWeights.clear();
                
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (const auto& syn : region->getSynapses()) {
                        finalWeights.push_back(syn->getWeight());
                    }
                }
                
                mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
                
                log("  Final state recorded:");
                log("    Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
                if (!finalWeights.empty()) {
                    float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                    float mean = sum / finalWeights.size();
                    log("    Mean weight: " + std::to_string(mean));
                }
            }
            
            void computeEnhancedStatistics() {
                log("");
                log("=== Enhanced Learning Experiment Results ===");
                log("");
                
                if (initialWeights.empty() || finalWeights.empty()) {
                    log("ERROR: No weights recorded");
                    return;
                }
                
                // Compute weight changes with detailed analysis
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                float meanChange = finalMean - initialMean;
                
                log("Weight Statistics:");
                log("  Initial mean weight: " + std::to_string(initialMean));
                log("  Final mean weight: " + std::to_string(finalMean));
                log("  Mean change: " + std::to_string(meanChange));
                
                // Count synapses that changed significantly with detailed breakdown
                size_t strengthened = 0, weakened = 0, unchanged = 0;
                size_t significantStrengthened = 0, significantWeakened = 0;
                
                size_t minSize = std::min(initialWeights.size(), finalWeights.size());
                for (size_t i = 0; i < minSize; ++i) {
                    float delta = finalWeights[i] - initialWeights[i];
                    if (delta > 0.01f) {
                        ++strengthened;
                        if (delta > 0.05f) ++significantStrengthened;
                    } else if (delta < -0.01f) {
                        ++weakened;
                        if (delta < -0.05f) ++significantWeakened;
                    } else {
                        ++unchanged;
                    }
                }
                
                log("");
                log("Synaptic Changes (detailed):");
                log("  Total strengthened synapses: " + std::to_string(strengthened));
                log("    - Strong: " + std::to_string(significantStrengthened) + " (>5%)");
                log("  Total weakened synapses: " + std::to_string(weakened));
                log("    - Strong: " + std::to_string(significantWeakened) + " (>5%)");
                log("  Unchanged synapses: " + std::to_string(unchanged));
                log("  Total synapses analyzed: " + std::to_string(minSize));
                
                // Calculate average change magnitude
                double avgChange = 0.0;
                size_t count = 0;
                for (size_t i = 0; i < minSize; ++i) {
                    float delta = finalWeights[i] - initialWeights[i];
                    if (std::abs(delta) > 0.001f) {
                        avgChange += std::abs(delta);
                        ++count;
                    }
                }
                if (count > 0) avgChange /= count;
                
                log("  Average change magnitude (significant synapses): " + std::to_string(avgChange));
                
                log("");
                log("Spike Activity Analysis:");
                log("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
                log("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
                
                // Calculate firing rate
                double elapsed = brain->getSpikeSystem()->getSimulationTime() - startTime;
                if (elapsed > 0) {
                    double firingRate = brain->getTotalSpikeCount() / elapsed;
                    log("  Average firing rate: " + std::to_string(firingRate) + " Hz");
                }
                
                // Determine if learning occurred with better thresholds
                bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                        (strengthened > 0) || (weakened > 0);
                
                log("");
                if (learningOccurred) {
                    log("✓ LEARNING DETECTED: Synaptic weights changed through experience");
                    if (meanChange > 0.01) log("  → Network is strengthening overall");
                    else if (meanChange < -0.01) log("  → Network is weakening overall");
                    else log("  → Network balance maintained despite local changes");
                } else {
                    log("✗ NO LEARNING: Weights did not change significantly");
                }
            }
        };
        
        EnhancedLearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern with enhanced feedback
        log("");
        log("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps with more detailed info
            if (step % 100 == 0) {
                log("  Step " + std::to_string(step) + 
                    " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                    " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                    " | Active: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display enhanced statistics
        experiment.computeEnhancedStatistics();
        
        log("  Test 2 completed in " + std::to_string(getElapsedTime()) + "s");
    }
};

} // namespace Tests

int main() {
    std::cout << "=== Enhanced Plasticity Learning Experiment (Modular) ===" << std::endl;
    
    // This is a placeholder for the extracted enhanced plasticity experiment.
    // In a full implementation, this would be integrated with the main CLI system
    // and would include the actual brain simulation and learning analysis.
    
    std::cout << "This demonstrates the modular architecture for plasticity testing." << std::endl;
    std::cout << "Features include:" << std::endl;
    std::cout << "  - Detailed weight tracking and analysis" << std::endl;
    std::cout << "  - Enhanced logging with progress reporting" << std::endl;
    std::cout << "  - Comprehensive statistics and learning detection" << std::endl;
    std::cout << "  - Modular test organization" << std::endl;
    
    return 0;
}
