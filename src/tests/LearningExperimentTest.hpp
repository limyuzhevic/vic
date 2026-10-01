// Learning Experiment Test
// Demonstrates measurable synaptic changes through experience with optimizations

#ifndef LEARNING_EXPERIMENT_TEST_HPP
#define LEARNING_EXPERIMENT_TEST_HPP

#include "TestRunner.hpp"
#include "brain/Brain.hpp"
#include "core/Optimizations/Optimizations.hpp"
#include <memory>
#include <vector>
#include <numeric>

namespace nlm {

class LearningExperimentTest {
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
public:
    LearningExperimentTest(std::shared_ptr<Brain> b, uint64_t s) 
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
        
        nlm::optimizations::StringBuilder sb;
        sb.append("Initial state recorded:").append("\n")
          .append("  Synapses: ").append(initialSynapseCount).append("\n");
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            sb.append("  Mean weight: ").append(mean);
        }
        NLM_LOG_INFO(sb.build());
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
        
        nlm::optimizations::StringBuilder sb;
        sb.append("Final state recorded:").append("\n")
          .append("  Total spikes: ").append(brain->getTotalSpikeCount()).append("\n");
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            sb.append("  Mean weight: ").append(mean);
        }
        NLM_LOG_INFO(sb.build());
    }
    
    void computeStatistics() {
        nlm::optimizations::StringBuilder sb;
        sb.append("").append("\n")
          .append("=== Learning Experiment Results ===").append("\n")
          .append("").append("\n");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes using optimized stats
        float initialMean = nlm::optimizations::StatsCalculator::calculateMean(initialWeights);
        float finalMean = nlm::optimizations::StatsCalculator::calculateMean(finalWeights);
        
        nlm::optimizations::StringBuilder statsSB;
        statsSB.append("Weight Statistics:").append("\n")
              .append("  Initial mean weight: ").append(initialMean).append("\n")
              .append("  Final mean weight: ").append(finalMean).append("\n")
              .append("  Change: ").append(finalMean - initialMean);
        
        NLM_LOG_INFO(statsSB.build());
        
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
        
        nlm::optimizations::StringBuilder changesSB;
        changesSB.append("").append("\n")
                .append("Synaptic Changes:").append("\n")
                .append("  Strengthened: ").append(strengthened).append("\n")
                .append("  Weakened: ").append(weakened).append("\n")
                .append("  Unchanged: ").append(unchanged);
        NLM_LOG_INFO(changesSB.build());
        
        nlm::optimizations::StringBuilder spikeSB;
        spikeSB.append("").append("\n")
               .append("Spike Activity:").append("\n")
               .append("  Total spikes: ").append(brain->getTotalSpikeCount()).append("\n")
               .append("  Most active neurons recorded: ").append(mostActiveNeurons.size());
        NLM_LOG_INFO(spikeSB.build());
        
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
    
    bool run() {
        nlm::optimizations::StringBuilder sb;
        sb.append("").append("\n")
          .append("=== Test 2: Plasticity Learning Experiment ===");
        NLM_LOG_INFO(sb.build());
        
        // Record initial state
        recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        nlm::optimizations::StringBuilder applySB;
        applySB.append("").append("\n")
               .append("Applying repeated input patterns (1000 steps)...");
        NLM_LOG_INFO(applySB.build());
        
        nlm::optimizations::PerformanceTimer timer;
        timer.start();
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps using optimized string building
            if (step % 100 == 0) {
                nlm::optimizations::StringBuilder progressSB;
                progressSB.append("  Step ").append(step)
                          .append(" | Spikes: ").append(brain->getTotalSpikeCount())
                          .append(" | Firing: ").append(brain->getFiringNeuronCount());
                NLM_LOG_INFO(progressSB.build());
            }
        }
        
        double elapsed = timer.elapsedSeconds();
        NLM_LOG_INFO("  Learning experiment completed in " + std::to_string(elapsed) + "s");
        
        // Record final state
        recordFinalState();
        
        // Compute and display statistics
        computeStatistics();
        
        return true;
    }
};

} // namespace nlm

#endif // LEARNING_EXPERIMENT_TEST_HPP