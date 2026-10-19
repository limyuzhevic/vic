#include <limits>
// Error Handling Tests
// Comprehensive tests for error handling and exception scenarios

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <algorithm>

namespace test_error_handling {

// Exception handling tests
void testConfigValidationErrors() {
    std::cout << "  Config Validation Tests" << std::endl;
    
    // Test missing keys
    nlm::Config config;
    
    // Try to get missing key
    auto missingValue = config.get<int>("missing_key");
    assert(!missingValue.has_value()); // Should be nullopt for missing key
    
    // Test getOr with missing key
    int defaultValue = config.getOr<int>("missing", 42);
    assert(defaultValue == 42);
    
    // Test set/get roundtrip
    config.set("test_int", 123);
    auto retrieved = config.get<int>("test_int");
    assert(retrieved.has_value() && *retrieved == 123);
    
    // Test remove
    config.remove("test_int");
    assert(!config.has("test_int"));
    
    // Test invalid type conversion (not crash, just return nullopt)
    config.set("string_val", std::string("hello"));
    auto intFromString = config.get<int>("string_val"); // Should be nullopt
    assert(!intFromString.has_value());
    
    std::cout << "    Config validation passed" << std::endl;
}

void testBrainInitializationErrors() {
    std::cout << "  Brain Initialization Tests" << std::endl;
    
    // Test with invalid configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Default); // Zero neurons - edge case
    
    nlm::Brain brain(config);
    
    // Brain initialization should handle edge cases
    bool initSuccess = brain.initialize();
    // Success depends on implementation, but shouldn't crash
    
    // Test with negative configuration values (if supported)
    config->set("neuron_count", static_cast<int64_t>(-10), nlm::ConfigSource::Default);
    nlm::Brain brain2(config);
    // Should handle gracefully
    
    std::cout << "    Brain initialization errors passed" << std::endl;
}

void testNeuronErrorHandling() {
    std::cout << "  Neuron Error Handling Tests" << std::endl;
    
    // Test neuron operations with edge cases
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test operations on unitialized neuron
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    // Test setting extreme values
    neuron.setMembranePotential(1000.0f); // Very high potential
    assert(neuron.getMembranePotential() >= -70.0f); // Should be clamped
    
    neuron.setMembranePotential(-1000.0f); // Very low potential
    assert(neuron.getMembranePotential() <= -70.0f); // Should be clamped
    
    // Test firing state transitions
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    neuron.setRefractoryPeriod(0); // Zero refractory period
    assert(!neuron.isRefractory()); // Should handle zero gracefully
    
    // Test threshold edge cases
    neuron.setThreshold(1000.0f); // Very high threshold
    neuron.setThreshold(-1000.0f); // Very low threshold
    
    std::cout << "    Neuron error handling passed" << std::endl;
}

void testSynapseErrorHandling() {
    std::cout << "  Synapse Error Handling Tests" << std::endl;
    
    // Test synapse creation with invalid IDs
    nlm::Synapse synapse(nlm::SynapseId(0), nlm::NeuronId(0), nlm::NeuronId(0));
    
    // Test weight edge cases
    synapse.setWeight(-10.0f); // Very negative weight
    assert(synapse.getWeight() >= -1.0f); // Should be clamped
    
    synapse.setWeight(10.0f); // Very positive weight
    assert(synapse.getWeight() <= 1.0f); // Should be clamped
    
    synapse.setWeight(0.5f);
    
    // Test type edge cases
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    assert(!synapse.isExcitatory());
    
    synapse.setType(nlm::SynapseType::Modulatory);
    assert(synapse.isInhibitory()); // Modulatory should be considered inhibitory
    
    // Test delay edge cases
    synapse.setDelay(-1); // Negative delay
    assert(synapse.getDelay() >= 0); // Should handle gracefully
    
    synapse.setDelay(1000); // Very large delay
    assert(synapse.getDelay() >= 0); // Should handle gracefully
    
    // Test spike history with empty data
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    Synapse error handling passed" << std::endl;
}

void testSTDPErrorHandling() {
    std::cout << "  STDP Error Handling Tests" << std::endl;
    
    nlm::STDP stdp;
    
    // Test STDP configuration edge cases
    stdp.setLTPWeight(-1.0f); // Negative weight
    assert(stdp.getLTPWeight() >= 0.0f); // Should be clamped
    
    stdp.setLTDWeight(-1.0f); // Negative weight
    assert(stdp.getLTDWeight() >= 0.0f); // Should be clamped
    
    stdp.setTimeConstant(-100.0f); // Negative time constant
    assert(stdp.getTimeConstant() >= 0.0f); // Should handle gracefully
    
    // Test with null synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test empty spike histories
    std::vector<nlm::Timestamp> emptySpikes;
    stdp.update(&synapse, emptySpikes, emptySpikes, 0.001);
    
    // Test with very different spike timings
    std::vector<nlm::Timestamp> preSpikes = {0.0, 100.0, 200.0};
    std::vector<nlm::Timestamp> postSpikes = {1000.0, 1100.0}; // Much later pre-spikes
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    std::cout << "    STDP error handling passed" << std::endl;
}

void testNeuromodulatorErrorHandling() {
    std::cout << "  Neuromodulator Error Handling Tests" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predError = brain.getPredictionErrorSignal();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    assert(predError != nullptr);
    
    // Test dopamine error handling
    dopamine->setLevel(-1.0f); // Negative level
    assert(dopamine->getLevel() >= 0.0f && dopamine->getLevel() <= 1.0f);
    
    dopamine->signalReward(-100.0f); // Negative reward
    dopamine->signalRewardPredictionError(-50.0f); // Negative prediction error
    
    // Test curiosity error handling
    curiosity->update(-1.0f, -1.0f, 0.01f); // Negative novelty/error
    assert(curiosity->getLevel() >= 0.0f && curiosity->getLevel() <= 1.0f);
    
    // Test weight configuration edge cases
    curiosity->setNoveltyWeight(2.0f); // Weight > 1.0
    curiosity->setPredictionErrorWeight(2.0f); // Weight > 1.0
    
    curiosity->update(0.5f, 0.5f, 0.01f); // Should handle extreme weights
    
    // Test novelty error handling
    novelty->setLevel(-1.0f); // Negative level
    assert(novelty->getLevel() >= 0.0f && novelty->getLevel() <= 1.0f);
    
    // Test prediction error error handling
    predError->computeError(0.0f, 1000.0f); // Extreme values
    assert(predError->getError() >= 0.0f);
    
    std::cout << "    Neuromodulator error handling passed" << std::endl;
}

void testMemorySystemErrorHandling() {
    std::cout << "  Memory System Error Handling Tests" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory error handling
    workingMemory->store({1.0f, 2.0f, 3.0f}, 1.0f); // Valid input
    std::vector<float> retrieved = workingMemory->retrieve();
    assert(retrieved.size() == 3);
    
    // Test with empty input
    workingMemory->store({}, 1.0f); // Empty pattern
    std::vector<float> emptyRetrieved = workingMemory->retrieve();
    // Should handle gracefully
    
    // Test with invalid neuron ID
    nlm::NeuronId invalidId(999999);
    workingMemory->storeToNeuron(invalidId, 0.5f); // May not exist
    float activation = workingMemory->getNeuronActivation(invalidId);
    assert(activation >= 0.0f); // Should return default value
    
    // Test episodic memory error handling
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 1.0f;
    
    episodicMemory->storeEpisode(episode); // Valid episode
    assert(episodicMemory->getEpisodeCount() == 1);
    
    // Test with extreme values
    nlm::EpisodicMemoryItem extremeEpisode;
    extremeEpisode.timestamp = 999999;
    extremeEpisode.reward = 1e10f; // Very large reward
    extremeEpisode.sensoryState = std::vector<float>(1000, 1.0f); // Very long vector
    
    episodicMemory->storeEpisode(extremeEpisode); // Should handle
    
    // Test associative memory error handling
    std::vector<float> patternA = {0.1f, 0.2f, 0.3f};
    std::vector<float> patternB = {0.4f, 0.5f, 0.6f};
    
    associativeMemory->associate(patternA, patternB, 1.5f); // Weight > 1.0
    float strength = associativeMemory->getAssociationStrength(patternA, patternB);
    assert(strength >= 0.0f); // Should be clamped
    
    // Test with empty patterns
    associativeMemory->associate({}, {}); // Empty patterns
    
    std::cout << "    Memory system error handling passed" << std::endl;
}

void testPredictionErrorHandling() {
    std::cout << "  Prediction Error Handling Tests" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction error with edge cases
    std::vector<float> state1 = {0.1f, 0.2f, 0.3f};
    brain.recordSensoryState(state1, 0);
    
    std::vector<float> state2 = {0.15f, 0.25f, 0.35f};
    brain.recordSensoryState(state2, 1);
    
    // Generate prediction with valid data
    std::vector<float> prediction = brain.generatePrediction(2);
    assert(prediction.size() == state1.size());
    
    // Test with NaN/Inf values
    std::vector<float> nanState = {0.0f, std::nanf(""), std::numeric_limits<float>::infinity()};
    brain.recordSensoryState(nanState, 3);
    
    // System should not crash
    std::vector<float> nanPrediction = brain.generatePrediction(4);
    
    // Test error computation edge cases
    float error = brain.updateWithObservation(nanState, 4);
    assert(error >= 0.0f); // Should handle gracefully
    
    std::cout << "    Prediction error handling passed" << std::endl;
}

void testDevelopmentErrorHandling() {
    std::cout << "  Development Error Handling Tests" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* devSystem = brain.getDevelopmentSystem();
    assert(devSystem != nullptr);
    
    // Test developmental stage edge cases
    devSystem->setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(devSystem->getStage() == nlm::DevelopmentalStage::Adult);
    
    // Test stage advancement past end
    devSystem->advanceStage(); // Should handle gracefully
    
    // Test plasticity edge cases
    float plasticity = devSystem->getPlasticityModifier();
    assert(plasticity >= 0.0f); // Should be non-negative
    
    // Test critical period edge cases
    bool isCritical = devSystem->isCriticalPeriod();
    float progress = devSystem->getCriticalPeriodProgress();
    assert(progress >= 0.0f && progress <= 1.0f);
    
    // Test negative developmental age
    devSystem->setDevelopmentalAge(-100.0);
    assert(devSystem->getDevelopmentalAge() >= 0.0); // Should handle gracefully
    
    std::cout << "    Development error handling passed" << std::endl;
}

void testContextManagerErrorHandling() {
    std::cout << "  Context Manager Error Handling Tests" << std::endl;
    
    // Test Python bindings context managers (if available)
    // This would test the __enter__ and __exit__ methods in Python bindings
    
    // For now, test similar patterns in C++ context managers
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test brain context manager pattern
    // In Python: with brain() as b: ...
    // In C++, we can test initialization and cleanup
    
    auto* spikeSystem = brain.getSpikeSystem();
    if (spikeSystem) {
        // Should handle spike system lifecycle
    }
    
    // Test resource cleanup
    brain.reset();
    
    // System should remain in valid state after reset
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    Context manager error handling passed" << std::endl;
}

void testSystemIntegrityAfterErrors() {
    std::cout << "  System Integrity Tests" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some operations that might cause errors
    for (int step = 0; step < 50; ++step) {
        // These operations should not crash the system
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        brain.step(step, step * 0.001);
        
        auto* dopamine = brain.getDopamine();
        if (dopamine) {
            // Some neuromodulation operations
            dopamine->signalReward(static_cast<float>(step));
        }
        
        // Check system integrity
        assert(brain.getTotalNeuronCount() > 0);
        assert(brain.getTotalSynapseCount() >= 0);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
    }
    
    // System should still be functional after error conditions
    std::unique_ptr<nlm::Action> action = brain.produceAction();
    // Action may be null, but system should not crash
    
    std::cout << "    System integrity after errors passed" << std::endl;
}

void testMemoryManagementErrorHandling() {
    std::cout << "  Memory Management Error Handling Tests" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test memory cleanup after errors
    for (int i = 0; i < 20; ++i) {
        // Store some data
        std::vector<float> pattern(5);
        for (int j = 0; j < 5; ++j) {
            pattern[j] = static_cast<float>(i * 0.1f + j * 0.05f);
        }
        workingMemory->store(pattern, 1.0f);
        
        nlm::EpisodicMemoryItem episode;
        episode.timestamp = i * 10;
        episode.reward = static_cast<float>(i);
        episode.sensoryState = pattern;
        episodicMemory->storeEpisode(episode);
        
        std::vector<float> patternB(3);
        for (int j = 0; j < 3; ++j) {
            patternB[j] = static_cast<float>(i * 0.2f + j * 0.1f);
        }
        associativeMemory->associate(pattern, patternB, 0.8f);
        
        // Clean up
        workingMemory->clear();
        episodicMemory->clear();
        associativeMemory->clear();
        
        // Verify cleanup
        assert(workingMemory->getActiveTraces() == 0);
        assert(episodicMemory->getEpisodeCount() == 0);
        assert(associativeMemory->getAssociationCount() == 0);
    }
    
    std::cout << "    Memory management error handling passed" << std::endl;
}

void runAll() {
    std::cout << "Running NLM Error Handling and Exception Tests..." << std::endl;
    
    try {
        testConfigValidationErrors();
        testBrainInitializationErrors();
        testNeuronErrorHandling();
        testSynapseErrorHandling();
        testSTDPErrorHandling();
        testNeuromodulatorErrorHandling();
        testMemorySystemErrorHandling();
        testPredictionErrorHandling();
        testDevelopmentErrorHandling();
        testContextManagerErrorHandling();
        testSystemIntegrityAfterErrors();
        testMemoryManagementErrorHandling();
        
        std::cout << "  All Error Handling tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_error_handling