// Memory System Tests
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_memory {

void testWorkingMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test working memory capacity
    assert(workingMemory->getCapacity() > 0);
    assert(workingMemory->getActiveTraces() >= 0);
    
    std::cout << "    testWorkingMemory passed" << std::endl;
}

void testEpisodicMemory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test episodic memory functionality
    assert(episodicMemory->getCapacity() > 0);
    assert(episodicMemory->getEpisodeCount() >= 0);
    
    std::cout << "    testEpisodicMemory passed" << std::endl;
}

void runAll() {
    testWorkingMemory();
    testEpisodicMemory();
}

} // namespace test_memory
