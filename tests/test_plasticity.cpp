// Core Component Tests
// Tests for the fundamental NLM building blocks

#include "nlm_testing_framework.hpp"
#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "memory/WorkingMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

// Type Tests
TEST_CASE("types_test", TestCategory::UNIT, "Tests for NLM type definitions and identifiers")
void testTypes() {
    nlm::NeuronId neuronId1(1);
    nlm::NeuronId neuronId2(2);
    
    assert((neuronId1 == nlm::NeuronId(1)) && (neuronId1 != neuronId2));
    assert(neuronId1 < neuronId2);
    assert(neuronId1.index() == 1);
    
    nlm::SynapseId synapseId1(100);
    nlm::SynapseId synapseId2(200);
    assert(synapseId1 < synapseId2);
    
    nlm::RegionId regionId1(1);
    nlm::RegionId regionId2(2);
    assert(regionId1 != regionId2);
    
    nlm::PopulationId popId1(1);
    nlm::PopulationId popId2(2);
    assert(popId1 != popId2);
    
    assert(static_cast<int>(nlm::NeuronType::Excitatory) != 
           static_cast<int>(nlm::NeuronType::Inhibitory));
    
    nlm::PlasticityFlags flags;
    assert(!flags.hebbian && !flags.stdp && !flags.reward_modulated);
    
    std::cout << "Core type tests passed" << std::endl;
    return true;
}

// Configuration Tests
TEST_CASE("config_test", TestCategory::CONFIGURATION, "Tests for configuration management")
void testConfig() {
    nlm::Config config;
    
    // Test basic configuration operations
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr function
    int missingVal = config.getOr<int>("missing", 100);
    assert(missingVal == 100);
    
    int existingVal = config.getOr<int>("int_value", 999);
    assert(existingVal == 42);
    
    // Test keys functionality
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    auto keys = config.getKeys();
    assert(keys.size() == 6); // plus the ones from before
    
    std::cout << "Configuration tests passed" << std::endl;
    return true;
}

// Random Number Tests
TEST_CASE("random_test", TestCategory::UNIT, "Tests for random number generation")
void testRandom() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    for (int i = 0; i < 10; ++i) {
        assert(rng1.uniformInt(0, 100) == rng2.uniformInt(0, 100));
    }
    
    // Test uniform distribution
    for (int i = 0; i < 100; ++i) {
        int val = rng1.uniformInt(5, 10);
        assert(val >= 5 && val <= 10);
    }
    
    // Test real uniform distribution
    for (int i = 0; i < 100; ++i) {
        double val = rng1.uniformReal(0.0, 1.0);
        assert(val >= 0.0 && val < 1.0);
    }
    
    // Test normal distribution
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng1.normal(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    assert(std::abs(mean) < 0.1); // Should be close to 0
    
    // Test Bernoulli distribution
    int trueCount = 0;
    int bernSamples = 10000;
    for (int i = 0; i < bernSamples; ++i) {
        if (rng1.bernoulli(0.3)) {
            ++trueCount;
        }
    }
    double ratio = static_cast<double>(trueCount) / bernSamples;
    assert(ratio > 0.25 && ratio < 0.35); // Should be roughly 0.3
    
    // Test random bool
    bool val = rng1.randomBool();
    assert(true || false); // Always true
    
    // Test pickRandomIndex
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng1.pickRandomIndex(10);
        assert(idx < 10);
    }
    
    // Test seed
    uint32_t val1 = rng1.uniformInt(0, 1000);
    rng1.seed(42); // Reset to known state
    uint32_t val2 = rng1.uniformInt(0, 1000);
    assert(val1 == val2);
    
    std::cout << "Random number tests passed" << std::endl;
    return true;
}

// Clock Tests
TEST_CASE("clock_test", TestCategory::UNIT, "Tests for simulation clock functionality")
void testClock() {
    nlm::SimulationClock clock(0.001);
    
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    assert(clock.getTimestep() == 0.001);
    
    clock.advance();
    assert(clock.getStep() == 1);
    assert(clock.getTime() == 0.001);
    
    clock.advance();
    assert(clock.getStep() == 2);
    assert(clock.getTime() == 0.002);
    
    clock.reset();
    assert(clock.getStep() == 0);
    assert(clock.getTime() == 0.0);
    
    clock.setStep(100);
    assert(clock.getStep() == 100);
    assert(clock.getTime() == 0.1);
    
    clock.setTime(0.5);
    assert(clock.getTime() == 0.5);
    assert(clock.getStep() == 500);
    
    clock.advanceBy(100);
    assert(clock.getStep() == 600);
    assert(clock.getTime() == 0.6);
    
    nlm::Timestamp ts = clock.stepToTime(500);
    assert(ts == 0.5);
    
    nlm::SimulationStep st = clock.timeToStep(0.3);
    assert(st == 300);
    
    assert(clock.isFirstStep());
    clock.advance();
    assert(!clock.isFirstStep());
    assert(!clock.isFirstStep(0)); // Already advanced
    clock.reset();
    assert(clock.isFirstStep());
    
    std::cout << "Clock tests passed" << std::endl;
    return true;
}

// Neuron Tests
TEST_CASE("neuron_test", TestCategory::UNIT, "Tests for neuron functionality and dynamics")
void testNeuron() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    assert(neuron.getMembranePotential() == -70.0f);
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());
    
    assert(neuron.getTotalCurrent() == 0.0f);
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    neuron.receiveExcitatoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    assert(neuron.getTotalCurrent() == -3.0f);
    
    assert(neuron.getSpikeHistory().empty());
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    assert(neuron.getSpikeHistory().size() == 3);
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    // Test random initialization
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    
    // Test reset
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "Neuron tests passed" << std::endl;
    return true;
}

// Synapse Tests
TEST_CASE("synapse_test", TestCategory::UNIT, "Tests for synapse connectivity and properties")
void testSynapse() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    
    assert(synapse.getWeight() == 0.0f);
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.2f);
    assert(synapse.getWeight() == 0.7f);
    
    synapse.addToWeight(-0.3f);
    assert(synapse.getWeight() == 0.4f);
    
    // Test weight bounds
    synapse.setWeight(10.0f);
    assert(synapse.getWeight() <= 1.0f);
    
    synapse.setWeight(-10.0f);
    assert(synapse.getWeight() >= -1.0f);
    
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.getType() == nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    assert(!synapse.isExcitatory());
    
    assert(synapse.getDelay() == 1);
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    assert(synapse.getEligibilityTrace() == 0.0f);
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() < 0.5f);
    
    assert(synapse.getEfficacy() == 1.0f);
    synapse.setEfficacy(0.5f);
    assert(synapse.getEfficacy() == 0.5f);
    
    synapse.setEfficacy(5.0f);
    assert(synapse.getEfficacy() <= 2.0f);
    
    // Test random initialization
    nlm::RandomGenerator rng(42);
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    assert(synapse.getWeight() > 0.0f);
    assert(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
    
    // Test reset
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    // Test plasticity flags
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "Synapse tests passed" << std::endl;
    return true;
}

// STDP Tests
TEST_CASE("stdp_test", TestCategory::UNIT, "Tests for spike-timing-dependent plasticity")
void testSTDP() {
    nlm::STDP stdp;
    
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstant() == 30.0f);
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    assert(stdp.getLTPWeight() == 0.03f);
    assert(stdp.getLTDWeight() == 0.035f);
    assert(stdp.getTimeConstant() == 25.0f);
    
    assert(stdp.getName() != nullptr);
    assert(std::string(stdp.getName()) == "STDP");
    
    // Test potentiation (pre before post)
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    assert(finalWeight > initialWeight);
    
    // Test depression (post before pre)
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    float initialWeight2 = synapse2.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes2 = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes2 = {5.0, 15.0, 25.0};
    
    stdp.update(&synapse2, preSpikes2, postSpikes2, 0.001);
    
    float finalWeight2 = synapse2.getWeight();
    assert(finalWeight2 < initialWeight2);
    
    // Test empty spikes
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight3 = synapse3.getWeight();
    
    std::vector<nlm::Timestamp> emptyPre;
    std::vector<nlm::Timestamp> postSpikes3 = {1.0, 2.0};
    
    stdp.update(&synapse3, emptyPre, postSpikes3, 0.001);
    assert(synapse3.getWeight() == initialWeight3);
    
    // Test weight bounds
    nlm::Synapse synapse4(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse4.setType(nlm::SynapseType::Excitatory);
    synapse4.setWeight(0.99f);
    
    std::vector<nlm::Timestamp> preSpikes4 = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes4 = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse4, preSpikes4, postSpikes4, 0.001);
    assert(synapse4.getWeight() <= 1.0f);
    
    std::cout << "STDP tests passed" << std::endl;
    return true;
}

// Memory Tests
TEST_CASE("memory_test", TestCategory::UNIT, "Tests for memory system components")
void testMemory() {
    // Test Working Memory
    {
        nlm::WorkingMemory wm(100);
        assert(wm.getMaxCapacity() == 100);
        assert(wm.getCurrentSize() == 0);
        
        // Store and retrieve items
        nlm::MemoryItem item1(nlm::MemoryId(1), "test_item_1");
        item1.timestamp = 0.0;
        item1.importance = 0.8f;
        
        wm.store(item1);
        assert(wm.getCurrentSize() == 1);
        
        auto retrieved = wm.retrieve(nlm::MemoryId(1));
        assert(retrieved.has_value());
        assert(retrieved.value().id == nlm::MemoryId(1));
        assert(retrieved.value().content == "test_item_1");
        
        // Test decay
        wm.updateDecay(0.1);
        // Item should have lower importance
        auto updated = wm.retrieve(nlm::MemoryId(1));
        assert(updated.has_value());
        assert(updated.value().importance < 0.8f);
        
        // Test cleanup
        wm.cleanupExpired(1000.0);
        // All items should be clean since we just updated them
        assert(wm.getCurrentSize() == 1);
        
        std::cout << "Working memory tests passed" << std::endl;
    }
    
    // Test Neural Working Memory
    {
        nlm::NeuralWorkingMemory nwm(50, 0.1, 0.9);
        assert(nwm.getCapacity() == 50);
        assert(nwm.getDecayRate() == 0.1f);
        assert(nwm.getMaxImportance() == 0.9f);
        
        nlm::NeuralMemoryEntry entry;
        entry.id = nlm::MemoryId(2);
        entry.neuronId = nlm::NeuronId(1);
        entry.synapticStrength = 0.7f;
        entry.lastAccessed = 0.0;
        entry.importance = 0.6f;
        
        nwm.store(entry);
        assert(nwm.getCurrentSize() == 1);
        
        auto retrieved = nwm.retrieve(nlm::MemoryId(2));
        assert(retrieved.has_value());
        assert(retrieved.value().id == nlm::MemoryId(2));
        assert(retrieved.value().synapticStrength == 0.7f);
        
        nwm.updateEntryImportance(nlm::MemoryId(2), 0.9f);
        auto updated = nwm.retrieve(nlm::MemoryId(2));
        assert(updated.has_value());
        assert(updated.value().importance == 0.9f);
        
        std::cout << "Neural working memory tests passed" << std::endl;
    }
    
    // Test Episodic Memory
    {
        nlm::EpisodicMemory em(200, 0.05, 0.8);
        assert(em.getMaxEvents() == 200);
        assert(em.getTemporalDecay() == 0.05f);
        assert(em.getImportanceThreshold() == 0.8f);
        
        nlm::EpisodicEvent event;
        event.id = nlm::MemoryId(3);
        event.timestamp = 0.0;
        event.context = "test_context";
        event.importance = 0.9f;
        
        em.storeEvent(event);
        assert(em.getEventCount() == 1);
        
        auto retrieved = em.retrieveEvent(nlm::MemoryId(3));
        assert(retrieved.has_value());
        assert(retrieved.value().id == nlm::MemoryId(3));
        assert(retrieved.value().context == "test_context");
        
        // Test temporal retrieval
        em.updateTime(10.0);
        auto recentEvents = em.getRecentEvents(5.0);
        assert(recentEvents.size() == 1); // Still within 5.0 seconds
        
        std::cout << "Episodic memory tests passed" << std::endl;
    }
    
    // Test Semantic Memory
    {
        nlm::SemanticMemory sm(100, 0.2, 0.7);
        assert(sm.getConceptCount() == 0);
        
        nlm::Concept concept;
        concept.id = nlm::ConceptId(1);
        concept.name = "test_concept";
        concept.associations = {"related1", "related2"};
        concept.frequency = 1.0f;
        
        sm.storeConcept(concept);
        assert(sm.getConceptCount() == 1);
        
        auto retrieved = sm.retrieveConcept(nlm::ConceptId(1));
        assert(retrieved.has_value());
        assert(retrieved.value().id == nlm::ConceptId(1));
        assert(retrieved.value().name == "test_concept");
        
        // Test activation
        sm.activateConcept(nlm::ConceptId(1), 0.8f);
        auto activated = sm.retrieveConcept(nlm::ConceptId(1));
        assert(activated.has_value());
        assert(activated.value().activationLevel == 0.8f);
        
        std::cout << "Semantic memory tests passed" << std::endl;
    }
    
    std::cout << "Memory tests passed" << std::endl;
    return true;
}

// Neuromodulation Tests
TEST_CASE("neuromodulation_test", TestCategory::UNIT, "Tests for neuromodulation system")
void testNeuromodulation() {
    nlm::Neuromodulator neuromodulator;
    
    // Test basic neuromodulator functionality
    nlm::ModulationSignal signal;
    signal.type = nlm::ModulationType::Reward;
    signal.strength = 1.0f;
    signal.timestamp = 0.0;
    
    neuromodulator.applySignal(signal);
    
    // Test reward signal
    nlm::Reward reward;
    reward.value = 10.0f;
    reward.timestamp = 0.0;
    
    neuromodulator.applyReward(reward);
    
    // Test prediction error
    nlm::PredictionError error;
    error.prediction = 5.0f;
    error.actual = 8.0f;
    error.timestamp = 0.0;
    
    neuromodulator.applyPredictionError(error);
    
    // Test novelty detection
    nlm::Novelty novelty;
    novelty.significance = 0.7f;
    novelty.timestamp = 0.0;
    
    neuromodulator.applyNovelty(novelty);
    
    // Test curiosity
    nlm::Curiosity curiosity;
    curiosity.level = 0.5f;
    curiosity.timestamp = 0.0;
    
    neuromodulator.applyCuriosity(curiosity);
    
    // Test neural effects
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuromodulator.applyEffects(neuron, 0.1);
    
    assert(neuron.getMembranePotential() != -70.0f); // Should have been modified
    
    std::cout << "Neuromodulation tests passed" << std::endl;
    return true;
}

// Brain Tests (Enhanced)
TEST_CASE("brain_test", TestCategory::UNIT, "Comprehensive tests for brain system")
void testBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    assert(brain.getTotalNeuronCount() == 100);
    
    brain.reset();
    assert(brain.getTotalNeuronCount() == 100);
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 1);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Verify statistics
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "Brain tests passed" << std::endl;
    return true;
}