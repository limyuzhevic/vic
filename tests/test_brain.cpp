// Unit tests for NLM brain core functionality
// This file contains tests for the main brain features and neural computation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <iomanip>

namespace test_brain {

void printTestHeader(const std::string& testName) {
    std::cout << std::setw(60) << std::setfill('=') << "" << std::endl;
    std::cout << "TEST: " << testName << std::endl;
    std::cout << std::setw(60) << std::setfill('=') << "" << std::endl;
}

void testBrainCreation() {
    printTestHeader("Brain Creation");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    ✓ Brain created successfully" << std::endl;
    std::cout << "    - Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "    - Regions: " << brain.getRegionCount() << std::endl;
}

void testBrainInitialization() {
    printTestHeader("Brain Initialization");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    std::cout << "    ✓ Brain initialized successfully" << std::endl;
    std::cout << "    - Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "    - Regions: " << brain.getRegionCount() << std::endl;
    std::cout << "    - Synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
}

void testBrainStep() {
    printTestHeader("Brain Simulation Step");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    std::cout << "    Running " << 10 << " simulation steps..." << std::endl;
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify brain state after steps
    assert(brain.getTotalNeuronCount() == 100);
    std::cout << "    ✓ Brain simulation completed successfully" << std::endl;
    std::cout << "    - Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "    - Active neurons: " << brain.getFiringNeuronCount() << std::endl;
}

void testBrainReset() {
    printTestHeader("Brain Reset");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    std::cout << "    Initial state - Neurons: " << brain.getTotalNeuronCount() << std::endl;
    
    brain.step(0);
    std::cout << "    After one step - Spikes: " << brain.getTotalSpikeCount() << std::endl;
    
    brain.reset();
    std::cout << "    After reset - Neurons: " << brain.getTotalNeuronCount() << std::endl;
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    std::cout << "    ✓ Brain reset successful" << std::endl;
}

void testBrainRegions() {
    printTestHeader("Brain Regions");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    std::cout << "    ✓ Created " << regionIds.size() << " regions" << std::endl;
    
    for (size_t i = 0; i < regionIds.size(); ++i) {
        auto* region = brain.getRegion(regionIds[i]);
        assert(region != nullptr);
        std::cout << "    - Region " << i << ": " << region->getNeuronCount() << " neurons" << std::endl;
    }
}

void testBrainActionProduction() {
    printTestHeader("Action Production");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    ✓ Action produced successfully" << std::endl;
    std::cout << "    - Action type: " << static_cast<int>(action->getType()) << std::endl;
}

void testBrainStatus() {
    printTestHeader("Brain Status");
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    std::cout << "    ✓ Brain statistics verified" << std::endl;
    std::cout << "    - Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "    - Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "    - Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "    - Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "    - E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
}

void runAllTests() {
    std::cout << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "NLM BRAIN UNIT TESTS" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;
    
    testBrainCreation();
    std::cout << std::endl;
    
    testBrainInitialization();
    std::cout << std::endl;
    
    testBrainStep();
    std::cout << std::endl;
    
    testBrainReset();
    std::cout << std::endl;
    
    testBrainRegions();
    std::cout << std::endl;
    
    testBrainActionProduction();
    std::cout << std::endl;
    
    testBrainStatus();
    std::cout << std::endl;
    
    std::cout << "============================================================" << std::endl;
    std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    std::cout << "============================================================" << std::endl;
}

} // namespace test_brain

// Main function for running brain unit tests
int main() {
    try {
        test_brain::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Test failed with unknown exception" << std::endl;
        return 1;
    }
}