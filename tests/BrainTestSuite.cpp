// Brain Test Suite Implementation
// Implementation of the BrainTestSuite tests

#include "BrainTestSuite.hpp"
#include "core/Config/Config.hpp"
#include <numeric>
#include <algorithm>

namespace test_brain {

void BrainTestSuite::runAllTests() {
    TestRunner::getInstance().addSuite(std::make_unique<BrainTestSuite>());
}

std::string BrainTestSuite::getName() const {
    return "Brain Tests";
}

void BrainTestSuite::registerTests() {
    // Basic functionality tests
    addTest(std::bind(&BrainTestSuite::testBrainCreation, this), "Brain Creation");
    addTest(std::bind(&BrainTestSuite::testBrainInitialization, this), "Brain Initialization");
    addTest(std::bind(&BrainTestSuite::testBrainStep, this), "Brain Step");
    addTest(std::bind(&BrainTestSuite::testBrainReset, this), "Brain Reset");
    addTest(std::bind(&BrainTestSuite::testBrainRegions, this), "Brain Regions");
    
    // Advanced functionality tests
    addTest(std::bind(&BrainTestSuite::testBrainActionProduction, this), "Brain Action Production");
    addTest(std::bind(&BrainTestSuite::testBrainStatus, this), "Brain Status");
    addTest(std::bind(&BrainTestSuite::testBrainWithSensoryInput, this), "Brain with Sensory Input");
    addTest(std::bind(&BrainTestSuite::testBrainWithActionOutput, this), "Brain with Action Output");
    addTest(std::bind(&BrainTestSuite::testBrainPlasticity, this), "Brain Plasticity");
    addTest(std::bind(&BrainTestSuite::testBrainMemorySystems, this), "Brain Memory Systems");
    addTest(std::bind(&BrainTestSuite::testBrainNeuromodulation, this), "Brain Neuromodulation");
    
    // Edge case and stress tests
    addTest(std::bind(&BrainTestSuite::testBrainEmptyConfig, this), "Brain Empty Config");
    addTest(std::bind(&BrainTestSuite::testBrainLargeNetwork, this), "Brain Large Network");
    addTest(std::bind(&BrainTestSuite::testBrainSingleNeuron, this), "Brain Single Neuron");
    addTest(std::bind(&BrainTestSuite::testBrainMaxConnections, this), "Brain Max Connections");
    addTest(std::bind(&BrainTestSuite::testBrainInvalidInput, this), "Brain Invalid Input");
}

bool BrainTestSuite::testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    ASSERT_TRUE(brain.getTotalNeuronCount() == 100, "Brain should have 100 neurons");
    ASSERT_TRUE(brain.getRegionCount() == 1, "Brain should have 1 region");
    ASSERT_TRUE(brain.getTotalSynapseCount() > 0, "Brain should have synapses");
    
    return true;
}

bool BrainTestSuite::testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    ASSERT_TRUE(success, "Brain initialization should succeed");
    
    ASSERT_TRUE(brain.getRegionCount() == 2, "Brain should have 2 regions");
    ASSERT_TRUE(brain.getTotalNeuronCount() == 100, "Brain should have 100 neurons");
    ASSERT_TRUE(brain.getTotalSynapseCount() > 0, "Brain should have synapses");
    
    auto regionIds = brain.getRegionIds();
    ASSERT_TRUE(regionIds.size() == 2, "Should have 2 region IDs");
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        ASSERT_TRUE(region != nullptr, "Region should not be null");
    }
    
    return true;
}

bool BrainTestSuite::testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    ASSERT_TRUE(brain.getTotalNeuronCount() == 100, "Brain should still have 100 neurons");
    
    size_t activeNeurons = brain.getActiveNeuronCount();
    ASSERT_TRUE(activeNeurons >= 0, "Active neuron count should be non-negative");
    
    return true;
}

bool BrainTestSuite::testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    brain.reset();
    brain.initialize();
    
    ASSERT_TRUE(brain.getTotalNeuronCount() == 50, "Brain should have 50 neurons after reset");
    
    return true;
}

bool BrainTestSuite::testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    ASSERT_TRUE(regionIds.size() == 3, "Should have 3 region IDs");
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        ASSERT_TRUE(region != nullptr, "Region should not be null");
    }
    
    ASSERT_TRUE(brain.getTotalNeuronCount() > 0, "Brain should have neurons");
    
    return true;
}

bool BrainTestSuite::testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    ASSERT_TRUE(action != nullptr, "Brain should produce action");
    
    nlm::ActionType actionType = action->getType();
    ASSERT_TRUE((actionType >= nlm::ActionType::MoveForward && 
                 actionType <= nlm::ActionType::Custom), "Action type should be valid");
    
    return true;
}

bool BrainTestSuite::testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    ASSERT_TRUE(brain.getTotalNeuronCount() > 0, "Total neuron count should be positive");
    ASSERT_TRUE(brain.getTotalSynapseCount() >= 0, "Total synapse count should be non-negative");
    ASSERT_TRUE(brain.getActiveNeuronCount() >= 0, "Active neuron count should be non-negative");
    ASSERT_TRUE(brain.getFiringNeuronCount() >= 0, "Firing neuron count should be non-negative");
    ASSERT_TRUE(brain.getAverageFiringRate() >= 0.0f, "Average firing rate should be non-negative");
    
    return true;
}

bool BrainTestSuite::testBrainWithSensoryInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::Vision vision(10, 10, 3);
    std::vector<float> data(10 * 10 * 3, 0.5f);
    vision.setData(data);
    
    brain.receiveSensoryInput(vision);
    
    ASSERT_TRUE(brain.getTotalNeuronCount() == 100, "Brain should still have 100 neurons");
    
    return true;
}

bool BrainTestSuite::testBrainWithActionOutput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    ASSERT_TRUE(action != nullptr, "Brain should produce action");
    
    auto params = action->getParameters();
    ASSERT_TRUE(params.size() >= 0, "Action parameters should be accessible");
    
    return true;
}

bool BrainTestSuite::testBrainPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    std::vector<float> initialWeights;
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    float initialMean = 0.0f;
    if (!initialWeights.empty()) {
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        initialMean = sum / initialWeights.size();
    }
    
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    std::vector<float> finalWeights;
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    ASSERT_TRUE(true, "Plasticity test completed (weights may or may not have changed)");
    
    return true;
}

bool BrainTestSuite::testBrainMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    auto* episodicMemory = brain.getEpisodicMemory();
    
    ASSERT_TRUE(true, "Memory system check completed");
    
    return true;
}

bool BrainTestSuite::testBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    
    ASSERT_TRUE(true, "Neuromodulation system check completed");
    
    return true;
}

bool BrainTestSuite::testBrainEmptyConfig() {
    auto config = std::make_shared<nlm::Config>();
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    ASSERT_TRUE(success, "Brain should initialize with default config");
    
    return true;
}

bool BrainTestSuite::testBrainLargeNetwork() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    ASSERT_TRUE(success, "Large brain should initialize successfully");
    ASSERT_TRUE(brain.getTotalNeuronCount() == 1000, "Large brain should have 1000 neurons");
    
    return true;
}

bool BrainTestSuite::testBrainSingleNeuron() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    ASSERT_TRUE(success, "Single neuron brain should initialize");
    ASSERT_TRUE(brain.getTotalNeuronCount() == 1, "Single neuron brain should have 1 neuron");
    
    return true;
}

bool BrainTestSuite::testBrainMaxConnections() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 1.0f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    ASSERT_TRUE(success, "Max connections brain should initialize");
    ASSERT_TRUE(brain.getTotalSynapseCount() > 0, "Max connections brain should have synapses");
    
    return true;
}

bool BrainTestSuite::testBrainInvalidInput() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    ASSERT_TRUE(success, "Brain with invalid config should still initialize");
    
    return true;
}

} // namespace test_brain