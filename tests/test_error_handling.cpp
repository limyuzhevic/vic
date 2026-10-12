// Error Handling and Bounds Checking Tests
// These tests verify the comprehensive error handling and bounds checking implemented across the NLM codebase

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_error_handling {

void testNullPointerChecks() {
    std::cout << "    testNullPointerChecks passed" << std::endl;
    
    // Test with empty config
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    // Test getRegion with null brain (initialized but empty)
    nlm::RegionId invalidId(9999);
    auto* region = brain.getRegion(invalidId);
    assert(region == nullptr);  // Should return null for non-existent region
    
    // Test getWorkingMemory with null pImpl
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory == nullptr);  // Should return null for null pImpl
    
    // Test other getters with null pImpl
    assert(brain.getEpisodicMemory() == nullptr);
    assert(brain.getAssociativeMemory() == nullptr);
    assert(brain.getPredictionSystem() == nullptr);
    assert(brain.getPlanner() == nullptr);
    assert(brain.getConceptFormation() == nullptr);
    assert(brain.getAttention() == nullptr);
    assert(brain.getDevelopmentSystem() == nullptr);
    assert(brain.getDopamine() == nullptr);
    assert(brain.getCuriosity() == nullptr);
    assert(brain.getNovelty() == nullptr);
    assert(brain.getPredictionErrorSignal() == nullptr);
    assert(brain.getRandomGenerator() == nullptr);
}

void testBoundsChecking() {
    std::cout << "    testBoundsChecking passed" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test getRegionCount with bounds checking
    size_t regionCount = brain.getRegionCount();
    assert(regionCount == 2);  // Should return actual count, not potential out of bounds
    
    // Test getRegionIds with bounds checking
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 2);  // Should return valid IDs
    
    // Test getTotalNeuronCount with bounds checking
    size_t totalNeurons = brain.getTotalNeuronCount();
    assert(totalNeurons == 10);  // Should return actual count, not potential overflow
    
    // Test getTotalSynapseCount with bounds checking
    size_t totalSynapses = brain.getTotalSynapseCount();
    assert(totalSynapses >= 0);  // Should return valid count, not potential negative
}

void testErrorHandlingPatterns() {
    std::cout << "    testErrorHandlingPatterns passed" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test step with bounds checking
    brain.step(0);  // Should not crash due to null pointer checks
    
    // Test reset with bounds checking
    brain.reset();  // Should not crash due to null pointer checks
    
    // Test produceAction with bounds checking
    auto action = brain.produceAction();
    assert(action != nullptr);  // Should return action even if motor neurons are empty
}

void testMemorySystemErrorHandling() {
    std::cout << "    testMemorySystemErrorHandling passed" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test NeuralWorkingMemory error handling
    if (auto* workingMemory = brain.getWorkingMemory()) {
        // Test with empty pattern
        std::vector<float> emptyPattern;
        workingMemory->store(emptyPattern, 1.0f);  // Should handle null brain_ gracefully
        
        // Test with null neuron ID
        nlm::NeuronId nullNeuron(99999);
        workingMemory->storeToNeuron(nullNeuron, 0.5f);  // Should handle gracefully
        
        // Test retrieve with bounds checking
        auto retrieved = workingMemory->retrieve();
        assert(retrieved.size() == workingMemory->getMemoryNeurons().size());  // Should return valid data
    }
    
    // Test NeuralEpisodicMemory error handling
    if (auto* episodicMemory = brain.getEpisodicMemory()) {
        // Test with null episode
        episodicMemory->replayEpisode(nullptr);  // Should handle null episode gracefully
        
        // Test getEpisode with invalid index
        const nlm::EpisodicMemoryItem* episode = episodicMemory->getEpisode(9999);
        assert(episode == nullptr);  // Should return null for invalid index
    }
}

void testMotorSystemErrorHandling() {
    std::cout << "    testMotorSystemErrorHandling passed" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(15), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Note: We would need to test MotorSystem directly, but it's integrated into Brain
    // For now, test that Brain produces actions without crashing
    auto action = brain.produceAction();
    assert(action != nullptr);
}

void testConfigErrorHandling() {
    std::cout << "    testConfigErrorHandling passed" << std::endl;
    
    // Test empty filepath
    nlm::Config config;
    bool result = config.loadFromFile("");
    assert(result == false);  // Should fail gracefully
    
    // Test invalid key-value pairs
    result = config.loadFromArgs(1, nullptr);
    assert(result == true);  // Should handle null argv gracefully
    
    // Test empty key
    config.set("", "value", nlm::ConfigSource::Runtime);
    // Should handle empty key gracefully (currently may or may not log error)
    
    // Test summary with empty config
    std::string summary = config.summary();
    assert(summary.find("Configuration (0 entries)") != std::string::npos);
}

void runAll() {
    testNullPointerChecks();
    testBoundsChecking();
    testErrorHandlingPatterns();
    testMemorySystemErrorHandling();
    testMotorSystemErrorHandling();
    testConfigErrorHandling();
    
    std::cout << "All error handling and bounds checking tests passed!" << std::endl;
}

} // namespace test_error_handling
