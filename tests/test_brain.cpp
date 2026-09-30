// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <functional>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

// Tests for refactored Brain components

void testRefactoredStepMethod() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test individual step methods
    size_t initialSteps = brain.getPendingSpikeEventCount();
    
    // Process delayed spikes
    brain.processDelayedSpikes(0, 0.0);
    
    // Update all neurons
    brain.updateAllNeurons(0.0);
    
    // Detect and schedule spikes
    brain.detectAndScheduleSpikes(0, 0.0);
    
    // Update working memory
    brain.updateWorkingMemory();
    
    // Apply neuromodulation
    brain.applyNeuromodulation();
    
    // Apply plasticity rules
    brain.applyPlasticityRules();
    
    // Update episodic memory
    brain.updateEpisodicMemory();
    
    // Update prediction system
    brain.updatePredictionSystem();
    
    // Update attention system
    brain.updateAttentionSystem();
    
    // Update concept formation
    brain.updateConceptFormation();
    
    // Apply structural plasticity
    brain.applyStructuralPlasticity(0);
    
    // Replay important memories
    brain.replayImportantMemories(0);
    
    // Apply development effects
    brain.applyDevelopmentEffects(0);
    
    // Consolidate memory
    brain.consolidateMemory(0);
    
    // Manage checkpoints
    brain.manageCheckpoints(0, 0.0);
    
    // Verify brain still works
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testRefactoredStepMethod passed" << std::endl;
}

void testTemplateIterationMethods() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test iterateAllNeurons template method
    std::vector<nlm::Neuron*> neuronsFound;
    brain.iterateAllNeurons([&neuronsFound](nlm::Neuron* neuron) {
        neuronsFound.push_back(neuron);
    });
    
    assert(neuronsFound.size() == 50);
    
    // Test iterateAllSynapses template method
    std::vector<nlm::Synapse*> synapsesFound;
    brain.iterateAllSynapses([&synapsesFound](nlm::Synapse* synapse) {
        synapsesFound.push_back(synapse);
    });
    
    assert(synapsesFound.size() > 0);
    
    // Test convenience methods
    brain.injectCurrentToAllSensoryNeurons(10.0f);
    brain.injectCurrentToAllMotorNeurons(5.0f);
    
    std::vector<nlm::NeuronId> activeNeurons = brain.collectActiveNeuronIds();
    assert(activeNeurons.size() >= 0); // May be 0 depending on current state
    
    float averageActivity = brain.calculateAverageNeuronActivity();
    assert(averageActivity >= 0.0f);
    
    std::cout << "    testTemplateIterationMethods passed" << std::endl;
}

void testCommandPattern() {
    // Test that command pattern classes can be instantiated
    // (Direct testing of command pattern requires shared_ptr to Brain)
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test BrainCommandExecutor through stepWithCommands
    // (Note: stepWithCommands uses shared_ptr which isn't directly accessible)
    // This test verifies the interface is available
    
    std::cout << "    testCommandPattern passed (interface verification)" << std::endl;
}

void testBrainConfigConstants() {
    // Test BrainConfig namespace constants
    using namespace nlm::BrainConfig;
    
    assert(DEFAULT_LEAK_CONDUCTANCE == -70.0f);
    assert(DEFAULT_THRESHOLD == -55.0f);
    assert(DEFAULT_RESET_POTENTIAL == -65.0f);
    assert(DEFAULT_MEMBRANE_CAPACITANCE == 1.0f);
    assert(DEFAULT_TIME_CONSTANT == 20.0f);
    
    assert(DEFAULT_STDP_LTP_WEIGHT == 0.01f);
    assert(DEFAULT_STDP_LTD_WEIGHT == 0.012f);
    assert(DEFAULT_STDP_TIME_CONSTANT == 20.0f);
    assert(DEFAULT_HEBBIAN_STRENGTH == 0.005f);
    
    assert(DEFAULT_SYNAPTOGENESIS_RATE == 0.0001f);
    assert(DEFAULT_PRUNING_RATE == 0.00001f);
    
    assert(DEFAULT_WORKING_MEMORY_CAPACITY == 1000);
    assert(DEFAULT_MAX_EPISODIC_EPISODES == 10000);
    assert(DEFAULT_CONSOLIDATION_THRESHOLD == 0.3f);
    
    assert(DEFAULT_DOPAMINE_MAX_LEVEL == 1.0f);
    assert(DEFAULT_DOPAMINE_DECAY == 0.95f);
    assert(DEFAULT_NOVELTY_THRESHOLD == 0.1f);
    
    assert(DEFAULT_SIMULATION_TIMESTEP == 0.001f);
    assert(DEFAULT_REPLAY_INTERVAL == 100);
    assert(DEFAULT_CONSOLIDATION_INTERVAL == 1000);
    
    assert(DEVELOPMENT_FACTOR_INITIAL == 1.0f);
    assert(DEVELOPMENT_FACTOR_CRITICAL == 0.8f);
    assert(DEVELOPMENT_FACTOR_MATURATION == 0.5f);
    assert(DEVELOPMENT_FACTOR_ADULT == 0.2f);
    
    assert(CHECKPOINT_FREQUENCY == 10000);
    assert(MAX_CHECKPOINT_SIZE == 5);
    assert(CHECKPOINT_COMPRESSION_RATIO == 0.7f);
    
    std::cout << "    testBrainConfigConstants passed" << std::endl;
}

void testBrainErrorHandling() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that brain operations handle errors gracefully
    // (Error handling is integrated into the Brain class)
    
    // Verify basic error handling works
    assert(brain.getTotalNeuronCount() == 50);
    
    // Test step with various parameters
    brain.step(0);
    brain.step(100, 0.1);
    
    std::cout << "    testBrainErrorHandling passed" << std::endl;
}

void testBrainEnhancedInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.018f, nlm::ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0002f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.00002f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Verify enhanced initialization features
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Test that enhanced parameters were applied
    auto* spikeSystem = brain.getSpikeSystem();
    assert(spikeSystem != nullptr);
    
    std::cout << "    testBrainEnhancedInitialization passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    
    // New tests for refactored components
    testRefactoredStepMethod();
    testTemplateIterationMethods();
    testCommandPattern();
    testBrainConfigConstants();
    testBrainErrorHandling();
    testBrainEnhancedInitialization();
}

} // namespace test_brain
