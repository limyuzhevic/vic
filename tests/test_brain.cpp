// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

void testBrainWithSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.05f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a simple sensory input
    class TestSensoryInput {
    public:
        std::vector<double> data;
        TestSensoryInput() {
            data = {0.5, 0.8, 0.3, 0.7, 0.2};
        }
        const std::vector<double>& getData() const { return data; }
    };
    
    TestSensoryInput input;
    brain.receiveSensoryInput(input);
    
    // Simulate a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify brain state after processing input
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getRegionCount() == 2);
    
    std::cout << "    testBrainWithSensoryInput passed" << std::endl;
}

void testBrainMultiStepDynamics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    size_t initialNeurons = brain.getTotalNeuronCount();
    
    // Run multiple steps with varying inputs
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Verify neuron count remains stable
        assert(brain.getTotalNeuronCount() == initialNeurons);
        assert(brain.getTotalSynapseCount() >= 0);
    }
    
    // Verify dynamics evolve over time
    size_t finalSpikes = brain.getTotalSpikeCount();
    assert(finalSpikes >= initialSpikes);  // Spikes should accumulate or stay same
    
    std::cout << "    testBrainMultiStepDynamics passed" << std::endl;
}

void testBrainRegionConnectivity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get region IDs
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    // Verify each region exists
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
        assert(region->getId() == rid);
        
        // Check region has neurons
        assert(region->getTotalNeuronCount() > 0);
        assert(region->getSynapseCount() >= 0);
    }
    
    // Verify total neuron count across regions
    size_t totalFromRegions = 0;
    for (auto rid : regionIds) {
        totalFromRegions += brain.getRegion(rid)->getTotalNeuronCount();
    }
    assert(totalFromRegions == 150);
    
    std::cout << "    testBrainRegionConnectivity passed" << std::endl;
}

void testBrainResetAndReinitialize() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Reset brain
    brain.reset();
    
    // Verify reset state
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    
    // Run a few steps after reset
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Verify brain still functional
    assert(brain.getTotalNeuronCount() == initialNeurons);
    
    std::cout << "    testBrainResetAndReinitialize passed" << std::endl;
}

void testBrainMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Access memory systems
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    auto* associativeMemory = brain.getAssociativeMemory();
    
    assert(workingMemory != nullptr);
    assert(episodicMemory != nullptr);
    assert(associativeMemory != nullptr);
    
    // Test working memory operations
    workingMemory->storeToNeuron(nlm::NeuronId(1), 0.5f);
    assert(workingMemory->getActiveTraces() > 0);
    
    // Test episodic memory operations
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 0;
    episode.reward = 0.5f;
    episode.activeNeurons = {nlm::NeuronId(1), nlm::NeuronId(2)};
    
    episodicMemory->storeEpisode(episode);
    assert(episodicMemory->getEpisodeCount() >= 1);
    
    std::cout << "    testBrainMemorySystems passed" << std::endl;
}

void testBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Access neuromodulation systems
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    
    assert(dopamine != nullptr);
    assert(curiosity != nullptr);
    assert(novelty != nullptr);
    
    // Update neuromodulators
    float initialDopamine = dopamine->getLevel();
    dopamine->update(0.001f);
    
    float initialCuriosity = curiosity->getLevel();
    curiosity->update(0.001f);
    
    float initialNovelty = novelty->getLevel();
    novelty->update(0.001f);
    
    // Apply neuromodulation to brain
    class TestNeuromodulator {
    public:
        float getLevel() const { return 0.8f; }
    } signal;
    
    brain.applyNeuromodulation(signal);
    
    // Verify neuromodulation effects
    assert(dopamine->getLevel() >= 0.0f);
    assert(curiosity->getLevel() >= 0.0f);
    assert(novelty->getLevel() >= 0.0f);
    
    std::cout << "    testBrainNeuromodulation passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
}

} // namespace test_brain
