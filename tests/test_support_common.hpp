// Phase 1: Plasticity System Integration Tests

// STDP Integration Tests

// Cross-platform test implementation
#ifndef TEST_SUPPORT_COMMON_HPP
#define TEST_SUPPORT_COMMON_HPP

#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>

namespace nlm {

// Common test utilities
class PlasticityTestFixture {
protected:
    std::unique_ptr<Brain> createTestBrain(size_t neuronCount = 100, size_t regionCount = 1) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_unique<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    std::unique_ptr<Brain> createTestBrainWithMemory() {
        auto brain = createTestBrain(200, 2);
        // Ensure memory systems are properly initialized
        brain->getWorkingMemory()->setCapacity(50);
        brain->getEpisodicMemory()->setMaxEpisodes(100);
        return brain;
    }
    
    std::unique_ptr<Brain> createTestBrainWithNeuromodulation() {
        auto brain = createTestBrain(150, 1);
        // Configure neuromodulators
        if (auto dopamine = brain->getDopamine()) {
            dopamine->configure(0.5f, 1.0f);
        }
        if (auto novelty = brain->getNovelty()) {
            novelty->configure(0.5f, 1.0f);
        }
        if (auto curiosity = brain->getCuriosity()) {
            curiosity->configure(0.5f, 1.0f);
        }
        return brain;
    }
    
    void setupTestBrain(std::unique_ptr<Brain>& brain) {
        assert(brain != nullptr);
        brain->reset();
        
        // Store some test spikes for STDP
        for (size_t i = 0; i < 10; ++i) {
            for (auto* region : brain->getRegions()) {
                for (auto* pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Record some spikes
                        neuron->recordSpike(static_cast<float>(i * 10));
                    }
                }
            }
        }
    }
    
    void verifySTDPResults(Synapse* synapse, float expectedChange, float tolerance = 1e-4f) {
        assert(synapse != nullptr);
        assert(std::abs(synapse->getEligibilityTrace() - expectedChange) < tolerance);
    }
    
    void verifyHebbianResults(Synapse* synapse, float expectedChange, float tolerance = 1e-4f) {
        assert(synapse != nullptr);
        assert(std::abs(synapse->getEligibilityTrace() - expectedChange) < tolerance);
    }
    
    void verifyMemoryResults(const std::vector<float>& actual, const std::vector<float>& expected, 
                            float tolerance = 1e-4f) {
        assert(actual.size() == expected.size());
        for (size_t i = 0; i < actual.size(); ++i) {
            assert(std::abs(actual[i] - expected[i]) < tolerance);
        }
    }
    
    std::vector<float> createTestPattern(int size) {
        std::vector<float> pattern(size);
        for (int i = 0; i < size; ++i) {
            pattern[i] = static_cast<float>(i) / static_cast<float>(size);
        }
        return pattern;
    }
    
    NeuronId createTestNeuron(Brain* brain, NeuronType type) {
        // Create a neuron by setting current
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return NeuronId(-1);
        
        // Find or create a neuron with the specified type
        for (auto* pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                auto neurons = pop->getNeurons();
                if (!neurons.empty()) {
                    return neurons[0]->getId();
                }
            }
        }
        
        // If none found, try to inject current and see if it fires
        for (size_t i = 0; i < brain->getTotalNeuronCount(); ++i) {
            NeuronId id(i + 1);
            brain->injectCurrent(id, 2.0f);  // Threshold is typically -55
            auto* neuron = brain->getNeuron(id);
            if (neuron && neuron->isFiring()) {
                return id;
            }
        }
        
        return NeuronId(-1);
    }
    
    void setupMemoryTestScenario(std::unique_ptr<Brain>& brain) {
        if (!brain) return;
        
        // Store patterns in working memory
        auto workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            auto neurons = workingMemory->getActiveNeurons();
            for (NeuronId id : neurons) {
                workingMemory->storeToNeuron(id, 0.5f);
            }
        }
        
        // Create an episode
        auto episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            // Simple test episode
            NeuralEpisode episode;
            episode.startTime = 0.0f;
            episode.endTime = 100.0f;
            episode.synapticChanges.clear();
            
            // Add some changes
            SynapticChange change;
            change.synapseId = SynapseId(1);
            change.weightChange = 0.1f;
            change.spikePairTimeDiff = 5.0f;
            episode.synapticChanges.push_back(change);
            
            episodicMemory->storeEpisode(episode);
        }
        
        // Create associative memory pairs
        auto associativeMemory = brain->getAssociativeMemory();
        if (associativeMemory) {
            // Simple key-value pair
            auto key = createTestPattern(10);
            auto value = createTestPattern(10);
            associativeMemory->learnAssociation(key, value);
        }
    }
    
    void setNeuromodulationState(float dopamine, float novelty) {
        // Set neuromodulation levels
        // Implementation depends on how neuromodulators are accessed
    }
    
    float getPredictionErrorSignal(Brain* brain) {
        // Get prediction error signal
        if (auto* predictionError = brain->getPredictionErrorSignal()) {
            return predictionError->getLevel();
        }
        return 0.0f;
    }
    
    void simulateReward(float reward) {
        // Simulate reward input
        // Implementation details would depend on reward system
    }
};

class MemoryTestFixture {
protected:
    // Memory-specific test utilities
    std::vector<float> createTestPattern(int size) {
        std::vector<float> pattern(size);
        for (int i = 0; i < size; ++i) {
            pattern[i] = static_cast<float>(i) / static_cast<float>(size);
        }
        return pattern;
    }
    
    NeuronId createTestNeuron(Brain* brain, NeuronType type) {
        // Similar to parent class implementation
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return NeuronId(-1);
        
        for (auto* pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                auto neurons = pop->getNeurons();
                if (!neurons.empty()) {
                    return neurons[0]->getId();
                }
            }
        }
        
        return NeuronId(-1);
    }
    
    void setupMemoryTestScenario(std::unique_ptr<Brain>& brain) {
        if (!brain) return;
        
        auto workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            workingMemory->setCapacity(100);
            workingMemory->clear();
        }
        
        auto episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            episodicMemory->clear();
            episodicMemory->setMaxEpisodes(100);
        }
        
        auto associativeMemory = brain->getAssociativeMemory();
        if (associativeMemory) {
            associativeMemory->clear();
        }
    }
};

class NeuromodulationTestFixture {
protected:
    // Neuromodulation-specific test utilities
    void setNeuromodulationState(float dopamine, float novelty, float curiosity) {
        // Implementation would depend on accessing neuromodulators
    }
    
    float getPredictionErrorSignal(Brain* brain) {
        if (auto* predictionError = brain->getPredictionErrorSignal()) {
            return predictionError->getLevel();
        }
        return 0.0f;
    }
    
    void simulateReward(float reward) {
        // Implementation details
    }
};

// Specialized assertions for neural systems
class NeuralAssertions {
public:
    static void assertWeightWithinBounds(float weight, float min = -1.0f, float max = 1.0f) {
        assert(weight >= min && weight <= max);
    }
    
    static void assertSTDPWeightChange(float initialWeight, const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes, float expectedChange,
                                        float tolerance = 1e-4f) {
        // Verify STDP weight change
        // This would need a reference to STDP instance
        assert(false); // Placeholder
    }
    
    static void assertHebbianLearning(const std::vector<float>& activity1, const std::vector<float>& activity2,
                                      float expectedStrength, float tolerance = 1e-4f) {
        // Verify Hebbian learning
        float actualStrength = 0.0f;
        for (size_t i = 0; i < activity1.size(); ++i) {
            actualStrength += activity1[i] * activity2[i];
        }
        assert(std::abs(actualStrength - expectedStrength) < tolerance);
    }
    
    static void assertMemoryCapacity(std::size_t current, std::size_t capacity, 
                                    bool shouldBeFull = false) {
        if (shouldBeFull) {
            assert(current == capacity);
        } else {
            assert(current <= capacity);
        }
    }
    
    static void assertNeuromodulationEffect(float baseline, float modulation, 
                                           float expectedEffect, float tolerance = 1e-4f) {
        float actualEffect = baseline * modulation;
        assert(std::abs(actualEffect - expectedEffect) < tolerance);
    }
};

// Performance benchmark utilities
class TestPerformanceBenchmark {
public:
    struct BenchmarkResult {
        double elapsedTimeMs;
        size_t operationsCount;
        double operationsPerSecond;
    };
    
    static BenchmarkResult measureOperation(std::function<void()> operation, int iterations = 100) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            operation();
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double elapsedMs = duration.count() / 1000.0;
        double opsPerSecond = (iterations * 1000.0) / elapsedMs;
        
        return {elapsedMs, static_cast<size_t>(iterations), opsPerSecond};
    }
};

} // namespace nlm

#endif // TEST_SUPPORT_COMMON_HPP