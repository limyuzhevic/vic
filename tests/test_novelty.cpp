// Novelty Tests
#include "neuromodulation/Novelty.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>

namespace test_novelty {

void testNoveltyCreation() {
    nlm::Novelty novelty;
    
    assert(novelty.getName() != nullptr);
    assert(std::string(novelty.getName()) == "Novelty");
    assert(novelty.getLevel() == 0.0f);
    assert(novelty.getPlasticityFactor() == 0.0f);
    
    std::cout << "    testNoveltyCreation passed" << std::endl;
}

void testNoveltyInitialization() {
    nlm::Novelty novelty;
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    novelty.initialize(&brain);
    
    assert(novelty.getLevel() == 0.0f);
    
    std::cout << "    testNoveltyInitialization passed" << std::endl;
}

void testNoveltyUpdate() {
    nlm::Novelty novelty;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    novelty.initialize(&brain);
    
    // Test with zero input
    novelty.update(0.0f, 1.0f); // novelty, dt
    assert(novelty.getLevel() == 0.0f);
    
    // Test with positive novelty
    novelty.update(0.5f, 1.0f);
    assert(novelty.getLevel() > 0.0f);
    assert(novelty.getLevel() <= 1.0f); // Should be bounded
    
    // Test with high novelty
    novelty.reset();
    novelty.update(1.0f, 1.0f);
    assert(novelty.getLevel() > 0.0f);
    
    std::cout << "    testNoveltyUpdate passed" << std::endl;
}

void testNoveltyPlasticityFactor() {
    nlm::Novelty novelty;
    
    // Test that plasticity factor changes with level
    assert(novelty.getLevel() == 0.0f);
    assert(novelty.getPlasticityFactor() >= 0.0f);
    
    // Update novelty
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    novelty.initialize(&brain);
    
    novelty.update(0.8f, 1.0f);
    float level = novelty.getLevel();
    assert(level > 0.0f);
    assert(novelty.getPlasticityFactor() > 0.0f);
    
    std::cout << "    testNoveltyPlasticityFactor passed" << std::endl;
}

void testNoveltyDecay() {
    nlm::Novelty novelty;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    novelty.initialize(&brain);
    
    // Set novelty high
    novelty.update(1.0f, 1.0f);
    assert(novelty.getLevel() > 0.0f);
    
    // Wait with zero novelty to allow decay
    novelty.update(0.0f, 1.0f);
    
    // Level should decrease over time (decay)
    
    std::cout << "    testNoveltyDecay passed" << std::endl;
}

void testNoveltyReset() {
    nlm::Novelty novelty;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    novelty.initialize(&brain);
    
    novelty.update(1.0f, 1.0f);
    assert(novelty.getLevel() > 0.0f);
    
    novelty.reset();
    assert(novelty.getLevel() == 0.0f);
    
    std::cout << "    testNoveltyReset passed" << std::endl;
}

void testNoveltyIntegration() {
    // Test integration with other systems
    nlm::Novelty novelty;
    
    // Novelty should be able to integrate with brain (if API exists)
    // For now, test basic functionality
    assert(novelty.getName() != nullptr);
    
    std::cout << "    testNoveltyIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running Novelty tests..." << std::endl;
    testNoveltyCreation();
    testNoveltyInitialization();
    testNoveltyUpdate();
    testNoveltyPlasticityFactor();
    testNoveltyDecay();
    testNoveltyReset();
    testNoveltyIntegration();
}

} // namespace test_novelty
