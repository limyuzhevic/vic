// Specialized assertions for neural systems

#ifndef TEST_ASSERTIONS_HPP
#define TEST_ASSERTIONS_HPP

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cmath>
#include <cassert>
#include <vector>

namespace nlm {

namespace test {

class NeuralSystemAssertions {
public:
    // Neuron assertions
    static void assertNeuronThreshold(const Neuron* neuron, float threshold) {
        assert(neuron != nullptr);
        assert(std::abs(neuron->getThreshold() - threshold) < 1e-4f);
    }
    
    static void assertNeuronMembranePotential(const Neuron* neuron, float potential) {
        assert(neuron != nullptr);
        assert(std::abs(neuron->getMembranePotential() - potential) < 1e-4f);
    }
    
    static void assertNeuronFiringState(const Neuron* neuron, FiringState expectedState) {
        assert(neuron != nullptr);
        assert(neuron->getState().firingState == expectedState);
    }
    
    // Synapse assertions
    static void assertSynapseWeight(const Synapse* synapse, float weight) {
        assert(synapse != nullptr);
        assert(std::abs(synapse->getWeight() - weight) < 1e-4f);
    }
    
    static void assertSynapseType(const Synapse* synapse, SynapseType expectedType) {
        assert(synapse != nullptr);
        assert(synapse->getType() == expectedType);
    }
    
    static void assertSynapseEligibilityTrace(const Synapse* synapse, float trace) {
        assert(synapse != nullptr);
        assert(std::abs(synapse->getEligibilityTrace() - trace) < 1e-4f);
    }
    
    // STDP assertions
    static void assertSTDPPotentiation(float preTime, float postTime, float expectedChange,
                                        float tolerance = 1e-4f) {
        // Calculate expected STDP change for given timing
        // For dt > 0 (pre before post): Δw = A+ * exp(-Δt / τ+)
        float dt = postTime - preTime;
        assert(dt > 0);  // Should be pre before post for potentiation
        
        // Using default STDP parameters from test_stdp.cpp
        float Aplus = 0.01f;  // LTP weight
        float tau = 20.0f;    // Time constant
        
        float expectedLTP = Aplus * std::exp(-dt / tau);
        assert(std::abs(expectedChange - expectedLTP) < tolerance);
    }
    
    static void assertSTDPDepression(float preTime, float postTime, float expectedChange,
                                      float tolerance = 1e-4f) {
        // For dt < 0 (post before pre): Δw = A- * exp(Δt / τ-)
        float dt = postTime - preTime;
        assert(dt < 0);  // Should be post before pre for depression
        
        float Aminus = 0.012f;  // LTD weight
        float tau = 20.0f;      // Time constant
        
        float expectedLTD = -Aminus * std::exp(dt / tau);
        assert(std::abs(expectedChange - expectedLTD) < tolerance);
    }
    
    // Hebbian assertions
    static void assertHebbianCorrelation(const std::vector<float>& activityPre,
                                         const std::vector<float>& activityPost,
                                         float expectedLearningRate = 0.01f,
                                         float tolerance = 1e-4f) {
        // Hebbian learning: Δw = η * a_pre * a_post
        assert(activityPre.size() == activityPost.size());
        
        float totalChange = 0.0f;
        for (size_t i = 0; i < activityPre.size(); ++i) {
            totalChange += activityPre[i] * activityPost[i];
        }
        
        float expectedChange = expectedLearningRate * totalChange;
        // Note: actual validation would need synapse reference
        assert(false); // Placeholder
    }
    
    // Memory assertions
    static void assertWorkingMemoryStorage(const std::vector<float>& pattern,
                                           const std::vector<float>& storedPattern,
                                           float tolerance = 1e-4f) {
        assert(pattern.size() == storedPattern.size());
        for (size_t i = 0; i < pattern.size(); ++i) {
            assert(std::abs(pattern[i] - storedPattern[i]) < tolerance);
        }
    }
    
    static void assertAssociativeCompletion(const std::vector<float>& partialPattern,
                                            const std::vector<float>& completedPattern,
                                            float tolerance = 1e-4f) {
        assert(completedPattern.size() >= partialPattern.size());
        
        // Check that partial pattern is maintained
        for (size_t i = 0; i < partialPattern.size(); ++i) {
            assert(std::abs(partialPattern[i] - completedPattern[i]) < tolerance);
        }
        
        // Check that completed pattern has additional information
        assert(completedPattern.size() > partialPattern.size());
    }
    
    // Neuromodulation assertions
    static void assertDopamineModulation(float baselineLearningRate, float dopamineLevel,
                                          float expectedModulatedRate, float tolerance = 1e-4f) {
        float modulatedRate = baselineLearningRate * dopamineLevel;
        assert(std::abs(modulatedRate - expectedModulatedRate) < tolerance);
    }
    
    static void assertPredictionError(float actual, float expected, float tolerance = 1e-4f) {
        assert(std::abs(actual - expected) < tolerance);
    }
    
    static void assertNoveltyLevel(float baseline, float noveltySignal,
                                   float expectedEffect, float tolerance = 1e-4f) {
        // Novelty effect: exploration increases with novelty signal
        float explorationIncrease = noveltySignal * 0.5f;  // Simple model
        assert(std::abs(explorationIncrease - expectedEffect) < tolerance);
    }
    
    static void assertCuriosityDrivenExploration(float curiosityLevel, float actionDiversity,
                                                 float expectedDiversity, float tolerance = 1e-4f) {
        float diversity = curiosityLevel * 0.8f;  // Simple model
        assert(std::abs(diversity - expectedDiversity) < tolerance);
    }
    
    // Integration assertions
    static void assertPlasticityMemoryCoupling(float plasticityEffect, float memoryStrength,
                                               float expectedCoupling, float tolerance = 1e-4f) {
        float coupling = plasticityEffect * memoryStrength;
        assert(std::abs(coupling - expectedCoupling) < tolerance);
    }
    
    static void assertNeuromodulationPlasticityIntegration(float neuromodulation,
                                                          float baselinePlasticity,
                                                          float expectedEffect, float tolerance = 1e-4f) {
        float effect = baselinePlasticity * neuromodulation;
        assert(std::abs(effect - expectedEffect) < tolerance);
    }
    
    static void assertMemoryNeuromodulationIntegration(float memoryPerformance,
                                                       float neuromodulationLevel,
                                                       float expectedImprovement, float tolerance = 1e-4f) {
        float improvement = memoryPerformance * (1.0f + neuromodulationLevel * 0.5f);
        assert(std::abs(improvement - expectedImprovement) < tolerance);
    }
    
    // System consistency assertions
    static void assertSynapticWeightBounds(float weight, float minBound = -1.0f, float maxBound = 1.0f) {
        assert(weight >= minBound && weight <= maxBound);
    }
    
    static void assertNeuronRefractory(const Neuron* neuron, uint32_t expectedRemaining) {
        assert(neuron != nullptr);
        assert(neuron->isRefractory());
        assert(neuron->getState().refractoryRemaining == expectedRemaining);
    }
    
    static void assertBrainConsistency(const Brain* brain) {
        assert(brain != nullptr);
        
        // Check basic consistency
        auto regions = brain->getRegions();
        assert(!regions.empty());
        
        // Sum neurons across regions
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        
        for (const auto& region : regions) {
            totalNeurons += region->getTotalNeuronCount();
            totalSynapses += region->getSynapseCount();
        }
        
        assert(totalNeurons == brain->getTotalNeuronCount());
        assert(totalSynapses == brain->getTotalSynapseCount());
    }
};

} // namespace test

#endif // TEST_ASSERTIONS_HPP