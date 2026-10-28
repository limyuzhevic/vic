// tests/PlasticityTest.hpp
#pragma once

#include "core/TestFramework.hpp"
#include "brain/Brain.hpp"
#include "experiments/Experiment.hpp"

namespace nlm {

class PlasticityTest : public Test {
public:
    const std::string& getName() const override { static const std::string name = "PlasticityTest"; return name; }
    const std::string& getDescription() const override { 
        static const std::string desc = "Tests synaptic plasticity by applying repeated input patterns and measuring weight changes"; 
        return desc; 
    }
    
    bool setup(std::shared_ptr<Brain> brain, const TestConfig& config) override {
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        return true;
    }
    
    TestResult run(std::shared_ptr<Brain> brain, const TestConfig& config) override {
        TestResult result;
        result.testName = getName();
        
        // Record initial weights
        size_t neuronsPerRegion = std::min(static_cast<size_t>(20), brain->getTotalNeuronCount() / 4);
        
        std::vector<float> initialWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (size_t i = 0; i < neuronsPerRegion && i < region->getSynapses().size(); ++i) {
                initialWeights.push_back(region->getSynapses()[i]->getWeight());
            }
        }
        
        // Apply repeated input pattern
        size_t initialSpikes = brain->getTotalSpikeCount();
        for (SimulationStep step = 0; step < config.steps; ++step) {
            // Create input pattern
            for (size_t i = 0; i < neuronsPerRegion && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, config.currentLevel);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO(std::to_string(step) + " | Spikes: " + std::to_string(brain->getTotalSpikeCount() - initialSpikes) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final weights
        std::vector<float> finalWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (size_t i = 0; i < neuronsPerRegion && i < region->getSynapses().size(); ++i) {
                finalWeights.push_back(region->getSynapses()[i]->getWeight());
            }
        }
        
        // Compute statistics
        if (!initialWeights.empty() && !finalWeights.empty()) {
            float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float initialMean = initialSum / initialWeights.size();
            float finalMean = finalSum / finalWeights.size();
            
            result.initialWeightMean = initialMean;
            result.finalWeightMean = finalMean;
            result.weightChange = finalMean - initialMean;
            
            // Count synapses that changed significantly
            size_t strengthened = 0, weakened = 0, unchanged = 0;
            size_t minSize = std::min(initialWeights.size(), finalWeights.size());
            
            for (size_t i = 0; i < minSize; ++i) {
                float delta = finalWeights[i] - initialWeights[i];
                if (delta > 0.01f) ++strengthened;
                else if (delta < -0.01f) ++weakened;
                else ++unchanged;
            }
            
            result.metrics["initial_mean_weight"] = initialMean;
            result.metrics["final_mean_weight"] = finalMean;
            result.metrics["weight_change"] = result.weightChange;
            result.metrics["strengthened"] = static_cast<double>(strengthened);
            result.metrics["weakened"] = static_cast<double>(weakened);
            result.metrics["unchanged"] = static_cast<double>(unchanged);
            
            // Determine if learning occurred
            bool learningOccurred = (std::abs(result.weightChange) > 0.001f) ||
                                   (strengthened > 0 || weakened > 0);
            
            result.passed = learningOccurred;
            if (!learningOccurred) {
                result.errorMessage = "No significant synaptic changes detected";
            }
        } else {
            result.errorMessage = "Could not record weight data";
        }
        
        result.spikesGenerated = brain->getTotalSpikeCount() - initialSpikes;
        return result;
    }
};

} // namespace nlm