#include "PlasticityExperiment.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace plasticity_experiment {

PlasticityExperiment::Results PlasticityExperiment::run(std::shared_ptr<Brain> brain, uint64_t seed) {
    TestUtils::logSubsection("Test 2: Plasticity Learning Experiment");
    
    // Record initial state
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        throw std::runtime_error("Region 1 not found for plasticity experiment");
    }
    
    Results results;
    results.totalSpikes = brain->getTotalSpikeCount();
    results.initialWeights = TestUtils::extractWeights<float>(*region);
    results.initialMean = TestUtils::calculateMean(results.initialWeights);
    
    // Enable plasticity on synapses
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
    }
    
    // Select pattern neurons (first 20 sensory neurons)
    results.mostActiveNeurons = selectPatternNeurons(brain, 20);
    
    TestUtils::logResult("Pattern neurons selected", std::to_string(results.mostActiveNeurons.size()) + " sensory neurons");
    
    // Apply repeated input pattern
    applyRepeatedInputPattern(brain, results.mostActiveNeurons, 1000);
    
    // Record final state
    results.finalWeights = TestUtils::extractWeights<float>(*region);
    results.finalMean = TestUtils::calculateMean(results.finalWeights);
    
    // Compute statistics
    results.weightChange = results.finalMean - results.initialMean;
    results.strengthened = TestUtils::countSignificantChanges(results.initialWeights, results.finalWeights, 0.01f);
    results.unchanged = results.initialWeights.size() - results.strengthened;
    results.weakened = results.unchanged; // This will be corrected below
    
    results.totalSpikes = brain->getTotalSpikeCount() - results.totalSpikes;
    
    // Count weakened synapses properly
    size_t minSize = std::min(results.initialWeights.size(), results.finalWeights.size());
    results.weakened = 0;
    for (size_t i = 0; i < minSize; ++i) {
        float delta = results.finalWeights[i] - results.initialWeights[i];
        if (delta < -0.01f) {
            results.weakened++;
        }
    }
    
    results.learningOccurred = (std::abs(results.weightChange) > 0.001f) || 
                               (results.strengthened > 0 || results.weakened > 0);
    
    return results;
}

void PlasticityExperiment::logResults(const Results& results) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("Weight Statistics:");
    TestUtils::logMetric("  Initial mean weight", results.initialMean);
    TestUtils::logMetric("  Final mean weight", results.finalMean);
    TestUtils::logMetric("  Weight change", results.weightChange);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    TestUtils::logMetric("  Strengthened", static_cast<double>(results.strengthened));
    TestUtils::logMetric("  Weakened", static_cast<double>(results.weakened));
    TestUtils::logMetric("  Unchanged", static_cast<double>(results.unchanged));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    TestUtils::logMetric("  Total spikes", static_cast<double>(results.totalSpikes));
    TestUtils::logMetric("  Most active neurons", static_cast<double>(results.mostActiveNeurons.size()));
    
    NLM_LOG_INFO("");
    if (results.learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

std::vector<NeuronId> PlasticityExperiment::selectPatternNeurons(std::shared_ptr<Brain> brain, size_t count) {
    std::vector<NeuronId> patternNeurons;
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return patternNeurons;
    
    for (auto* neuron : region->getAllNeurons()) {
        if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < count) {
            patternNeurons.push_back(neuron->getId());
        }
    }
    
    return patternNeurons;
}

void PlasticityExperiment::applyRepeatedInputPattern(std::shared_ptr<Brain> brain, const std::vector<NeuronId>& patternNeurons, size_t steps) {
    TestUtils::logResult("Applying repeated input patterns", std::to_string(steps) + " steps");
    
    for (SimulationStep step = 0; step < steps; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            TestUtils::logProgress(step, static_cast<int>(steps), "Plasticity experiment");
        }
    }
}

} // namespace plasticity_experiment
} // namespace nlm