// Neuromodulation System Tests
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_neuromodulation {

void testDopamine() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test dopamine system
    float baseline = dopamine->getBaselineLevel();
    assert(baseline >= 0.0f);
    
    std::cout << "    testDopamine passed" << std::endl;
}

void testCuriosity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    assert(curiosity != nullptr);
    
    // Test curiosity system
    float curiosityLevel = curiosity->getLevel();
    assert(curiosityLevel >= 0.0f && curiosityLevel <= 1.0f);
    
    std::cout << "    testCuriosity passed" << std::endl;
}

void testNovelty() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* novelty = brain.getNovelty();
    assert(novelty != nullptr);
    
    // Test novelty system
    float noveltyLevel = novelty->getLevel();
    assert(noveltyLevel >= 0.0f);
    
    std::cout << "    testNovelty passed" << std::endl;
}

void runAll() {
    testDopamine();
    testCuriosity();
    testNovelty();
}

} // namespace test_neuromodulation
